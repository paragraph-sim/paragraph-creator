/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of prim nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior
 * written permission.
 *
 * See the NOTICE file distributed with this work for additional information
 * regarding copyright ownership.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <cassert>
#include <memory>
#include <vector>

#include "paragraph/graph/graph.h"
#include "paragraph/graph/instruction.h"
#include "paragraph/graph/subroutine.h"
#include "paragraph/shim/macros.h"
#include "paragraph/shim/statusor.h"
#include "prim/prim.h"
#include "tclap/CmdLine.h"

// This ParaGraph creator models the 27-Point Stencil workload described in
// detail in Section 6.2 here:
// https://www.nicm.dev/pubs/nicmcdonald_hxrouting_sc_2019.pdf
//
// Unlike the paper version, this one does NOT specify the type of all-reduce
// used. Instead it leaves a generic all-reduce instruction in the ParaGraph.
// This allows the user to use ParaGraph translation to further extrapolate
// any algorithm desired for the collective operation.

#define HALO_INSTS 26
#define FACE_XN 0
#define FACE_XP 1
#define FACE_YN 2
#define FACE_YP 3
#define FACE_ZN 4
#define FACE_ZP 5
#define EDGE_XNYN 6
#define EDGE_XNYP 7
#define EDGE_XPYN 8
#define EDGE_XPYP 9
#define EDGE_YNZN 10
#define EDGE_YNZP 11
#define EDGE_YPZN 12
#define EDGE_YPZP 13
#define EDGE_ZNXN 14
#define EDGE_ZNXP 15
#define EDGE_ZPXN 16
#define EDGE_ZPXP 17
#define CORNER_XNYNZN 18
#define CORNER_XNYNZP 19
#define CORNER_XNYPZN 20
#define CORNER_XNYPZP 21
#define CORNER_XPYNZN 22
#define CORNER_XPYNZP 23
#define CORNER_XPYPZN 24
#define CORNER_XPYPZP 25

// This class provides a translation from 3D to 1D
struct Cube {
  u32 xn_;
  u32 yn_;
  u32 zn_;
  Cube(u32 _xn, u32 _yn, u32 _zn) : xn_(_xn), yn_(_yn), zn_(_zn) {}
  u32 id(u32 _x, u32 _y, u32 _z) const {
    assert(_x < xn_);
    assert(_y < yn_);
    assert(_z < zn_);
    u32 i = (((zn_ * yn_) * _z) + (yn_ * _y) + (_x));
    assert(i < (zn_ * yn_ * xn_));
    return i;
  }
};

s32 main(s32 _argc, char** _argv) {
  u32 xn;
  u32 yn;
  u32 zn;
  u32 iterations;
  f64 loop_condition_seconds;
  f64 compute_seconds;
  u32 face_msg_size;
  u32 edge_msg_size;
  u32 corner_msg_size;
  u32 allreduce_msg_size;
  f64 reduction_seconds;
  std::string name;
  std::string output_file;
  u32 verbosity;
  try {
    TCLAP::CmdLine cmd(
        "Make ParaGraph representing a 27-Point Stencil Workloads", ' ', "1.0");
    TCLAP::UnlabeledValueArg<u32> xNodesArg(
        "x_nodes", "Number of nodes in the virtual x dimension", true, 0, "u32",
        cmd);
    TCLAP::UnlabeledValueArg<u32> yNodesArg(
        "y_nodes", "Number of nodes in the virtual y dimension", true, 0, "u32",
        cmd);
    TCLAP::UnlabeledValueArg<u32> zNodesArg(
        "z_nodes", "Number of nodes in the virtual z dimension", true, 0, "u32",
        cmd);
    TCLAP::UnlabeledValueArg<u32> iterationsArg(
        "iterations", "Number of iterations through the stencil algorithm",
        true, 0, "u32", cmd);
    TCLAP::UnlabeledValueArg<f64> loopConditionSecondsArg(
        "loop_condition_seconds", "Amount of seconds each loop condition takes",
        true, 0, "f64", cmd);
    TCLAP::UnlabeledValueArg<f64> computeSecondsArg(
        "compute_seconds", "Amount of seconds each compute step takes", true, 0,
        "f64", cmd);
    TCLAP::UnlabeledValueArg<u32> faceMessageSizeArg(
        "face_msg_size", "Message size of face communications", true, 0, "u32",
        cmd);
    TCLAP::UnlabeledValueArg<u32> edgeMessageSizeArg(
        "edge_msg_size", "Message size of corner communications", true, 0,
        "u32", cmd);
    TCLAP::UnlabeledValueArg<u32> cornerMessageSizeArg(
        "corner_msg_size", "Message size of corner communications", true, 0,
        "u32", cmd);
    TCLAP::UnlabeledValueArg<u32> allreduceMessageSizeArg(
        "allreduce_msg_size", "Message size of all-reduce operation", true, 0,
        "u32", cmd);
    TCLAP::UnlabeledValueArg<f64> reductionSecondsArg(
        "reduction_seconds", "Amount of seconds each reduction operation takes",
        true, 0, "f64", cmd);
    TCLAP::ValueArg<std::string> nameArg("n", "name", "Name of the graph",
                                         false, "27-Point Stencil", "string",
                                         cmd);
    TCLAP::UnlabeledValueArg<std::string> outputFileArg(
        "output_file", "Output graph file", true, "", "filename", cmd);
    TCLAP::ValueArg<u32> verbosityArg("v", "verbosity",
                                      "Configures the verbosity level", false,
                                      0, "u32", cmd);
    cmd.parse(_argc, _argv);
    xn = xNodesArg.getValue();
    yn = yNodesArg.getValue();
    zn = zNodesArg.getValue();
    iterations = iterationsArg.getValue();
    loop_condition_seconds = loopConditionSecondsArg.getValue();
    compute_seconds = computeSecondsArg.getValue();
    face_msg_size = faceMessageSizeArg.getValue();
    edge_msg_size = edgeMessageSizeArg.getValue();
    corner_msg_size = cornerMessageSizeArg.getValue();
    allreduce_msg_size = allreduceMessageSizeArg.getValue();
    reduction_seconds = reductionSecondsArg.getValue();
    output_file = outputFileArg.getValue();
    name = nameArg.getValue();
    verbosity = verbosityArg.getValue();
  } catch (TCLAP::ArgException& e) {
    throw std::runtime_error(e.error().c_str());
  }

  if (verbosity > 0) {
    printf("xn=%u\n", xn);
    printf("yn=%u\n", yn);
    printf("zn=%u\n", zn);
    printf("iterations=%u\n", iterations);
    printf("loop_condition_seconds=%f\n", loop_condition_seconds);
    printf("compute_seconds=%f\n", compute_seconds);
    printf("face_msg_size=%u\n", face_msg_size);
    printf("edge_msg_size=%u\n", edge_msg_size);
    printf("corner_msg_size=%u\n", corner_msg_size);
    printf("allreduce_msg_size=%u\n", allreduce_msg_size);
    printf("reduction_seconds=%f\n", reduction_seconds);
    printf("name=%s\n", name.c_str());
    printf("output_file=%s\n", output_file.c_str());
    printf("\n");
  }
  assert(xn > 0);
  assert(yn > 0);
  assert(zn > 0);
  assert(iterations > 0);
  assert(loop_condition_seconds >= 0.0);
  assert(compute_seconds >= 0.0);
  assert(face_msg_size > 0);
  assert(edge_msg_size > 0);
  assert(corner_msg_size > 0);
  assert(allreduce_msg_size > 0);
  assert(reduction_seconds >= 0.0);
  assert(name != "");
  assert(output_file != "");

  // Creates the graph
  if (verbosity > 0) {
    printf("Creating graph structure\n");
  }
  std::unique_ptr<paragraph::Graph> graph =
      std::make_unique<paragraph::Graph>(name);

  std::unique_ptr<paragraph::Subroutine> main_subr_uniq =
      std::make_unique<paragraph::Subroutine>("main", graph.get());
  paragraph::Subroutine* main_subr = main_subr_uniq.get();
  graph->SetEntrySubroutine(std::move(main_subr_uniq));

  CHECK_OK_AND_ASSIGN(paragraph::Instruction * while_inst,
                      paragraph::Instruction::Create(paragraph::Opcode::kWhile,
                                                     "loop", main_subr, true));

  std::unique_ptr<paragraph::Subroutine> loop_body_subr_uniq =
      std::make_unique<paragraph::Subroutine>("loop_body", graph.get());
  paragraph::Subroutine* loop_body_subr = loop_body_subr_uniq.get();
  while_inst->AppendInnerSubroutine(std::move(loop_body_subr_uniq));

  std::unique_ptr<paragraph::Subroutine> loop_cond_subr_uniq =
      std::make_unique<paragraph::Subroutine>("loop_cond", graph.get());
  paragraph::Subroutine* loop_cond_subr = loop_cond_subr_uniq.get();
  while_inst->AppendInnerSubroutine(std::move(loop_cond_subr_uniq));

  CHECK_OK_AND_ASSIGN(
      paragraph::Instruction * compute_inst,
      paragraph::Instruction::Create(paragraph::Opcode::kDelay, "compute",
                                     loop_body_subr));
  CHECK_OK_AND_ASSIGN(
      paragraph::Instruction * exchange_inst,
      paragraph::Instruction::Create(paragraph::Opcode::kCall, "exchange",
                                     loop_body_subr));
  CHECK_OK_AND_ASSIGN(
      paragraph::Instruction * allreduce_inst,
      paragraph::Instruction::Create(paragraph::Opcode::kAllReduce, "allreduce",
                                     loop_body_subr, true));

  exchange_inst->AddOperand(compute_inst);
  allreduce_inst->AddOperand(exchange_inst);

  std::unique_ptr<paragraph::Subroutine> exchange_subr_uniq =
      std::make_unique<paragraph::Subroutine>("exchange", graph.get());
  paragraph::Subroutine* exchange_subr = exchange_subr_uniq.get();
  exchange_inst->AppendInnerSubroutine(std::move(exchange_subr_uniq));

  std::vector<paragraph::Instruction*> halo_insts;
  paragraph::Instruction* tmp;

  // Face instructions
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "face_xn", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "face_xp", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "face_yn", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "face_yp", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "face_zn", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "face_zp", exchange_subr));
  halo_insts.push_back(tmp);

  // Edge instructions
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "edge_xnyn", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "edge_xnyp", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "edge_xpyn", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "edge_xpyp", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "edge_ynzn", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "edge_ynzp", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "edge_ypzn", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "edge_ypzp", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "edge_znxn", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "edge_znxp", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "edge_zpxn", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "edge_zpxp", exchange_subr));
  halo_insts.push_back(tmp);

  // Corner instructions
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "corner_xnynzn", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "corner_xnynzp", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "corner_xnypzn", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "corner_xnypzp", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "corner_xpynzn", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "corner_xpynzp", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "corner_xpypzn", exchange_subr));
  halo_insts.push_back(tmp);
  CHECK_OK_AND_ASSIGN(
      tmp, paragraph::Instruction::Create(paragraph::Opcode::kSendRecv,
                                          "corner_xpypzp", exchange_subr));
  halo_insts.push_back(tmp);

  assert(halo_insts.size() == HALO_INSTS);

  CHECK_OK_AND_ASSIGN(
      paragraph::Instruction * exchange_root_inst,
      paragraph::Instruction::Create(paragraph::Opcode::kNull, "root",
                                     exchange_subr, true));
  for (paragraph::Instruction* halo_inst : halo_insts) {
    exchange_root_inst->AddOperand(halo_inst);
  }

  std::unique_ptr<paragraph::Subroutine> reduction_subr_uniq =
      std::make_unique<paragraph::Subroutine>("reduction", graph.get());
  paragraph::Subroutine* reduction_subr = reduction_subr_uniq.get();
  allreduce_inst->AppendInnerSubroutine(std::move(reduction_subr_uniq));

  CHECK_OK_AND_ASSIGN(
      paragraph::Instruction * reduction_inst,
      paragraph::Instruction::Create(paragraph::Opcode::kDelay, "reduction",
                                     reduction_subr, true));

  CHECK_OK_AND_ASSIGN(
      paragraph::Instruction * loop_cond_inst,
      paragraph::Instruction::Create(paragraph::Opcode::kDelay,
                                     "loop_condition", loop_cond_subr, true));

  CHECK_OK(graph->ValidateComposite());

  // Configures the communication groups of the exchange operation
  if (verbosity > 0) {
    printf("Configuing communication groups for halo exchange\n");
  }
  Cube cube(xn, yn, zn);
  for (u32 z = 0; z < zn; z++) {
    for (u32 y = 0; y < yn; y++) {
      for (u32 x = 0; x < xn; x++) {
        u32 me = cube.id(x, y, z);
        if (verbosity > 1) {
          printf("Node -> [%u,%u,%u] -> %u\n", x, y, z, me);
        }

        // Face neighbors
        if (x > 0) {
          u32 you = cube.id(x - 1, y, z);
          if (verbosity > 1) {
            printf("  Face -x -> [%u,%u,%u] -> %u\n", x - 1, y, z, you);
          }
          halo_insts.at(FACE_XN)->AppendCommunicationGroup({you, me, you});
        }
        if (x < xn - 1) {
          u32 you = cube.id(x + 1, y, z);
          if (verbosity > 1) {
            printf("  Face +x -> [%u,%u,%u] -> %u\n", x + 1, y, z, you);
          }
          halo_insts.at(FACE_XP)->AppendCommunicationGroup({you, me, you});
        }
        if (y > 0) {
          u32 you = cube.id(x, y - 1, z);
          if (verbosity > 1) {
            printf("  Face -y -> [%u,%u,%u] -> %u\n", x, y - 1, z, you);
          }
          halo_insts.at(FACE_YN)->AppendCommunicationGroup({you, me, you});
        }
        if (y < yn - 1) {
          u32 you = cube.id(x, y + 1, z);
          if (verbosity > 1) {
            printf("  Face +y -> [%u,%u,%u] -> %u\n", x, y + 1, z, you);
          }
          halo_insts.at(FACE_YP)->AppendCommunicationGroup({you, me, you});
        }
        if (z > 0) {
          u32 you = cube.id(x, y, z - 1);
          if (verbosity > 1) {
            printf("  Face -z -> [%u,%u,%u] -> %u\n", x, y, z - 1, you);
          }
          halo_insts.at(FACE_ZN)->AppendCommunicationGroup({you, me, you});
        }
        if (z < zn - 1) {
          u32 you = cube.id(x, y, z + 1);
          if (verbosity > 1) {
            printf("  Face +z -> [%u,%u,%u] -> %u\n", x, y, z + 1, you);
          }
          halo_insts.at(FACE_ZP)->AppendCommunicationGroup({you, me, you});
        }

        // Edge neighbors
        if (x > 0 && y > 0) {
          u32 you = cube.id(x - 1, y - 1, z);
          if (verbosity > 1) {
            printf("  Edge -x,-y -> [%u,%u,%u] -> %u\n", x - 1, y - 1, z, you);
          }
          halo_insts.at(EDGE_XNYN)->AppendCommunicationGroup({you, me, you});
        }
        if (x > 0 && y < yn - 1) {
          u32 you = cube.id(x - 1, y + 1, z);
          if (verbosity > 1) {
            printf("  Edge -x,+y -> [%u,%u,%u] -> %u\n", x - 1, y + 1, z, you);
          }
          halo_insts.at(EDGE_XNYP)->AppendCommunicationGroup({you, me, you});
        }
        if (x < xn - 1 && y > 0) {
          u32 you = cube.id(x + 1, y - 1, z);
          if (verbosity > 1) {
            printf("  Edge +x,-y -> [%u,%u,%u] -> %u\n", x + 1, y - 1, z, you);
          }
          halo_insts.at(EDGE_XPYN)->AppendCommunicationGroup({you, me, you});
        }
        if (x < xn - 1 && y < yn - 1) {
          u32 you = cube.id(x + 1, y + 1, z);
          if (verbosity > 1) {
            printf("  Edge +x,+y -> [%u,%u,%u] -> %u\n", x + 1, y + 1, z, you);
          }
          halo_insts.at(EDGE_XPYP)->AppendCommunicationGroup({you, me, you});
        }
        if (y > 0 && z > 0) {
          u32 you = cube.id(x, y - 1, z - 1);
          if (verbosity > 1) {
            printf("  Edge -y,-z -> [%u,%u,%u] -> %u\n", x, y - 1, z - 1, you);
          }
          halo_insts.at(EDGE_YNZN)->AppendCommunicationGroup({you, me, you});
        }
        if (y > 0 && z < zn - 1) {
          u32 you = cube.id(x, y - 1, z + 1);
          if (verbosity > 1) {
            printf("  Edge -y,+z -> [%u,%u,%u] -> %u\n", x, y - 1, z + 1, you);
          }
          halo_insts.at(EDGE_YNZP)->AppendCommunicationGroup({you, me, you});
        }
        if (y < yn - 1 && z > 0) {
          u32 you = cube.id(x, y + 1, z - 1);
          if (verbosity > 1) {
            printf("  Edge +y,-z -> [%u,%u,%u] -> %u\n", x, y + 1, z - 1, you);
          }
          halo_insts.at(EDGE_YPZN)->AppendCommunicationGroup({you, me, you});
        }
        if (y < yn - 1 && z < zn - 1) {
          u32 you = cube.id(x, y + 1, z + 1);
          if (verbosity > 1) {
            printf("  Edge +y,+z -> [%u,%u,%u] -> %u\n", x, y + 1, z + 1, you);
          }
          halo_insts.at(EDGE_YPZP)->AppendCommunicationGroup({you, me, you});
        }
        if (z > 0 && x > 0) {
          u32 you = cube.id(x - 1, y, z - 1);
          if (verbosity > 1) {
            printf("  Edge -z,-x -> [%u,%u,%u] -> %u\n", x - 1, y, z - 1, you);
          }
          halo_insts.at(EDGE_ZNXN)->AppendCommunicationGroup({you, me, you});
        }
        if (z > 0 && x < xn - 1) {
          u32 you = cube.id(x + 1, y, z - 1);
          if (verbosity > 1) {
            printf("  Edge -z,+x -> [%u,%u,%u] -> %u\n", x + 1, y, z - 1, you);
          }
          halo_insts.at(EDGE_ZNXP)->AppendCommunicationGroup({you, me, you});
        }
        if (z < zn - 1 && x > 0) {
          u32 you = cube.id(x - 1, y, z + 1);
          if (verbosity > 1) {
            printf("  Edge +z,-x -> [%u,%u,%u] -> %u\n", x - 1, y, z + 1, you);
          }
          halo_insts.at(EDGE_ZPXN)->AppendCommunicationGroup({you, me, you});
        }
        if (z < zn - 1 && x < xn - 1) {
          u32 you = cube.id(x + 1, y, z + 1);
          if (verbosity > 1) {
            printf("  Edge +z,+x -> [%u,%u,%u] -> %u\n", x + 1, y, z + 1, you);
          }
          halo_insts.at(EDGE_ZPXP)->AppendCommunicationGroup({you, me, you});
        }

        // Corner neighbors
        if (x > 0 && y > 0 && z > 0) {
          u32 you = cube.id(x - 1, y - 1, z - 1);
          if (verbosity > 1) {
            printf("  Corner -x,-y,-z -> [%u,%u,%u] -> %u\n", x - 1, y - 1,
                   z - 1, you);
          }
          halo_insts.at(CORNER_XNYNZN)
              ->AppendCommunicationGroup({you, me, you});
        }
        if (x > 0 && y > 0 && z < zn - 1) {
          u32 you = cube.id(x - 1, y - 1, z + 1);
          if (verbosity > 1) {
            printf("  Corner -x,-y,+z -> [%u,%u,%u] -> %u\n", x - 1, y - 1,
                   z + 1, you);
          }
          halo_insts.at(CORNER_XNYNZP)
              ->AppendCommunicationGroup({you, me, you});
        }
        if (x > 0 && y < yn - 1 && z > 0) {
          u32 you = cube.id(x - 1, y + 1, z - 1);
          if (verbosity > 1) {
            printf("  Corner -x,+y,-z -> [%u,%u,%u] -> %u\n", x - 1, y + 1,
                   z - 1, you);
          }
          halo_insts.at(CORNER_XNYPZN)
              ->AppendCommunicationGroup({you, me, you});
        }
        if (x > 0 && y < yn - 1 && z < zn - 1) {
          u32 you = cube.id(x - 1, y + 1, z + 1);
          if (verbosity > 1) {
            printf("  Corner -x,+y,+z -> [%u,%u,%u] -> %u\n", x - 1, y + 1,
                   z + 1, you);
          }
          halo_insts.at(CORNER_XNYPZP)
              ->AppendCommunicationGroup({you, me, you});
        }
        if (x < xn - 1 && y > 0 && z > 0) {
          u32 you = cube.id(x + 1, y - 1, z - 1);
          if (verbosity > 1) {
            printf("  Corner -x,-y,-z -> [%u,%u,%u] -> %u\n", x + 1, y - 1,
                   z - 1, you);
          }
          halo_insts.at(CORNER_XPYNZN)
              ->AppendCommunicationGroup({you, me, you});
        }
        if (x < xn - 1 && y > 0 && z < zn - 1) {
          u32 you = cube.id(x + 1, y - 1, z + 1);
          if (verbosity > 1) {
            printf("  Corner -x,-y,+z -> [%u,%u,%u] -> %u\n", x + 1, y - 1,
                   z + 1, you);
          }
          halo_insts.at(CORNER_XPYNZP)
              ->AppendCommunicationGroup({you, me, you});
        }
        if (x < xn - 1 && y < yn - 1 && z > 0) {
          u32 you = cube.id(x + 1, y + 1, z - 1);
          if (verbosity > 1) {
            printf("  Corner -x,+y,-z -> [%u,%u,%u] -> %u\n", x + 1, y + 1,
                   z - 1, you);
          }
          halo_insts.at(CORNER_XPYPZN)
              ->AppendCommunicationGroup({you, me, you});
        }
        if (x < xn - 1 && y < yn - 1 && z < zn - 1) {
          u32 you = cube.id(x + 1, y + 1, z + 1);
          if (verbosity > 1) {
            printf("  Corner -x,+y,+z -> [%u,%u,%u] -> %u\n", x + 1, y + 1,
                   z + 1, you);
          }
          halo_insts.at(CORNER_XPYPZP)
              ->AppendCommunicationGroup({you, me, you});
        }
      }
    }
  }

  if (verbosity > 1) {
    printf("face_xn=%lu\n",
           halo_insts.at(FACE_XN)->GetCommunicationGroupVector().size());
    printf("face_xp=%lu\n",
           halo_insts.at(FACE_XP)->GetCommunicationGroupVector().size());
    printf("face_yn=%lu\n",
           halo_insts.at(FACE_YN)->GetCommunicationGroupVector().size());
    printf("face_yp=%lu\n",
           halo_insts.at(FACE_YP)->GetCommunicationGroupVector().size());
    printf("face_zn=%lu\n",
           halo_insts.at(FACE_ZN)->GetCommunicationGroupVector().size());
    printf("face_zp=%lu\n",
           halo_insts.at(FACE_ZP)->GetCommunicationGroupVector().size());
    printf("edge_xnyn=%lu\n",
           halo_insts.at(EDGE_XNYN)->GetCommunicationGroupVector().size());
    printf("edge_xnyp=%lu\n",
           halo_insts.at(EDGE_XNYP)->GetCommunicationGroupVector().size());
    printf("edge_xpyn=%lu\n",
           halo_insts.at(EDGE_XPYN)->GetCommunicationGroupVector().size());
    printf("edge_xpyp=%lu\n",
           halo_insts.at(EDGE_XPYP)->GetCommunicationGroupVector().size());
    printf("edge_ynzn=%lu\n",
           halo_insts.at(EDGE_YNZN)->GetCommunicationGroupVector().size());
    printf("edge_ynzp=%lu\n",
           halo_insts.at(EDGE_YNZP)->GetCommunicationGroupVector().size());
    printf("edge_ypzn=%lu\n",
           halo_insts.at(EDGE_YPZN)->GetCommunicationGroupVector().size());
    printf("edge_ypzp=%lu\n",
           halo_insts.at(EDGE_YPZP)->GetCommunicationGroupVector().size());
    printf("edge_znxn=%lu\n",
           halo_insts.at(EDGE_ZNXN)->GetCommunicationGroupVector().size());
    printf("edge_znxp=%lu\n",
           halo_insts.at(EDGE_ZNXP)->GetCommunicationGroupVector().size());
    printf("edge_zpxn=%lu\n",
           halo_insts.at(EDGE_ZPXN)->GetCommunicationGroupVector().size());
    printf("edge_zpxp=%lu\n",
           halo_insts.at(EDGE_ZPXP)->GetCommunicationGroupVector().size());
    printf("corner_xnynzn=%lu\n",
           halo_insts.at(CORNER_XNYNZN)->GetCommunicationGroupVector().size());
    printf("corner_xnynzp=%lu\n",
           halo_insts.at(CORNER_XNYNZP)->GetCommunicationGroupVector().size());
    printf("corner_xnypzn=%lu\n",
           halo_insts.at(CORNER_XNYPZN)->GetCommunicationGroupVector().size());
    printf("corner_xnypzp=%lu\n",
           halo_insts.at(CORNER_XNYPZP)->GetCommunicationGroupVector().size());
    printf("corner_xpynzn=%lu\n",
           halo_insts.at(CORNER_XPYNZN)->GetCommunicationGroupVector().size());
    printf("corner_xpynzp=%lu\n",
           halo_insts.at(CORNER_XPYNZP)->GetCommunicationGroupVector().size());
    printf("corner_xpypzn=%lu\n",
           halo_insts.at(CORNER_XPYPZN)->GetCommunicationGroupVector().size());
    printf("corner_xpypzp=%lu\n",
           halo_insts.at(CORNER_XPYPZP)->GetCommunicationGroupVector().size());
  }

  // Configures the communication group of the all-reduce operation
  if (verbosity > 0) {
    printf("Configuring communication group for allreduce\n");
  }
  paragraph::CommunicationGroup allreduce_comm_group;
  u32 nodes = xn * yn * zn;
  for (u32 n = 0; n < nodes; n++) {
    allreduce_comm_group.push_back(n);
  }
  allreduce_inst->AppendCommunicationGroup(allreduce_comm_group);

  // Configures the loop
  if (verbosity > 0) {
    printf("Configuring number of iterations\n");
  }
  loop_body_subr->SetExecutionCount(iterations);
  loop_cond_subr->SetExecutionCount(iterations);
  loop_cond_inst->SetSeconds(loop_condition_seconds);

  // Configures the compute operation
  if (verbosity > 0) {
    printf("Configuring compute operation\n");
  }
  compute_inst->SetSeconds(compute_seconds);

  // Configures the exchange message sizes
  if (verbosity > 0) {
    printf("Configuring exchange message sizes\n");
  }
  for (u32 i = FACE_XN; i < EDGE_XNYN; i++) {
    halo_insts.at(i)->SetBytesIn(face_msg_size);
    halo_insts.at(i)->SetBytesOut(face_msg_size);
  }
  for (u32 i = EDGE_XNYN; i < CORNER_XNYNZN; i++) {
    halo_insts.at(i)->SetBytesIn(edge_msg_size);
    halo_insts.at(i)->SetBytesOut(edge_msg_size);
  }
  for (u32 i = CORNER_XNYNZN; i < HALO_INSTS; i++) {
    halo_insts.at(i)->SetBytesIn(corner_msg_size);
    halo_insts.at(i)->SetBytesOut(corner_msg_size);
  }

  // Configures the allreduce operation
  if (verbosity > 0) {
    printf("Configuring allreduce operations\n");
  }
  allreduce_inst->SetBytesOut(allreduce_msg_size);
  reduction_inst->SetSeconds(reduction_seconds);

  // Validates the final graph structure
  if (verbosity > 0) {
    printf("Validating graph structure\n");
  }
  CHECK_OK(graph->ValidateComposite());
  CHECK_EQ(graph->GetCommunicationSet().size(), nodes);
  CHECK(graph->HasConsecutiveNaturalProcessorIds());

  // Writes the output graph file
  if (verbosity > 0) {
    printf("Writing graph to file: %s\n", output_file.c_str());
  }
  CHECK_OK(graph->WriteToFile(output_file));

  return 0;
}  // NOLINT(readability/fn_size)
