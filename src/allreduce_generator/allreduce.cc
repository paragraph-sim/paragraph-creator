/* Copyright 2021 Georgia Tech
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <filesystem>
#include <memory>
#include <string>
#include <utility>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "paragraph/graph/graph.h"
#include "paragraph/shim/macros.h"

ABSL_FLAG(int, num_proc, 8, "Number of processors.");
ABSL_FLAG(int, loop_count, 1, "Number of All-Reduce iterations.");
ABSL_FLAG(double, size, 1024 * 1024, "All-Reduce size, in bytes.");
ABSL_FLAG(double, time_delay, 1e-6,
          "Compute delay for non All-Reduce part, in seconds..");
ABSL_FLAG(bool, comm_overlap, false,
          "Flag that decides whether communication and computation overlaps.");
ABSL_FLAG(std::string, output_graph, "", "Output graph file.");

int32_t main(int32_t argc, char** argv) {
  // Parsing flags
  absl::ParseCommandLine(argc, argv);
  std::filesystem::path output_graph_file = absl::GetFlag(FLAGS_output_graph);
  int num_proc = absl::GetFlag(FLAGS_num_proc);
  int loop_count = absl::GetFlag(FLAGS_loop_count);
  double ar_size = absl::GetFlag(FLAGS_size);
  double time_delay = absl::GetFlag(FLAGS_time_delay);
  bool is_overlapped = absl::GetFlag(FLAGS_comm_overlap);
  CHECK_NE(output_graph_file, "");

  auto graph = std::make_unique<paragraph::Graph>("allreduce_generator");
  auto main_sub = std::make_unique<paragraph::Subroutine>("main", graph.get());
  auto main_sub_ptr = main_sub.get();
  graph->SetEntrySubroutine(std::move(main_sub));

  CHECK_OK_AND_ASSIGN(auto while_instr, paragraph::Instruction::Create(
                                            paragraph::Opcode::kWhile,
                                            "allreduce_loop", main_sub_ptr));
  auto body_sub =
      absl::make_unique<paragraph::Subroutine>("body_subroutine", graph.get());
  auto body_sub_ptr = body_sub.get();
  while_instr->AppendInnerSubroutine(std::move(body_sub));
  body_sub_ptr->SetExecutionCount(loop_count);

  CHECK_OK_AND_ASSIGN(auto compute_inst, paragraph::Instruction::Create(
                                             paragraph::Opcode::kDelay,
                                             "compute_payload", body_sub_ptr));
  compute_inst->SetSeconds(time_delay);

  CHECK_OK_AND_ASSIGN(
      auto allreduce_inst,
      paragraph::Instruction::Create(paragraph::Opcode::kAllReduce, "allreduce",
                                     body_sub_ptr, true));
  if (!is_overlapped) {
    allreduce_inst->AddOperand(compute_inst);
  }
  allreduce_inst->SetBytesOut(ar_size);
  paragraph::CommunicationGroup allreduce_group;
  for (int proc_id = 0; proc_id < num_proc; proc_id++) {
    allreduce_group.push_back(proc_id);
  }
  allreduce_inst->AppendCommunicationGroup(allreduce_group);

  auto reduction_sub = absl::make_unique<paragraph::Subroutine>(
      "reduction_subroutine", graph.get());
  auto reduction_ptr = reduction_sub.get();
  CHECK_OK_AND_ASSIGN(
      auto op1, paragraph::Instruction::Create(paragraph::Opcode::kDelay, "op1",
                                               reduction_ptr));
  op1->SetBytesOut(ar_size);
  CHECK_OK_AND_ASSIGN(
      auto op2, paragraph::Instruction::Create(paragraph::Opcode::kDelay, "op2",
                                               reduction_ptr));
  op2->SetBytesOut(ar_size);
  CHECK_OK_AND_ASSIGN(
      auto sum_op, paragraph::Instruction::Create(paragraph::Opcode::kDelay,
                                                  "sum", reduction_ptr, true));
  sum_op->SetOps(ar_size / 4);
  sum_op->AddOperand(op1);
  sum_op->AddOperand(op2);
  allreduce_inst->AppendInnerSubroutine(std::move(reduction_sub));

  auto cond_sub =
      absl::make_unique<paragraph::Subroutine>("cond_subroutine", graph.get());
  auto cond_sub_ptr = cond_sub.get();
  while_instr->AppendInnerSubroutine(std::move(cond_sub));
  cond_sub_ptr->SetExecutionCount(loop_count);
  CHECK_OK_AND_ASSIGN(auto cond_instr, paragraph::Instruction::Create(
                                           paragraph::Opcode::kDelay,
                                           "conditional", cond_sub_ptr, true));
  cond_instr->SetOps(1);

  CHECK_OK_AND_ASSIGN(auto root_instr, paragraph::Instruction::Create(
                                           paragraph::Opcode::kNull, "root",
                                           main_sub_ptr, true));
  root_instr->SetOps(1);

  // Writing output graph
  CHECK_OK(graph->WriteToFile(output_graph_file));
  return 0;
}
