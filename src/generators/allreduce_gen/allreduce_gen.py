import argparse 
from google.protobuf import text_format
import os

import graph_pb2

def get_all_reduce_textproto():
    graph = '''
name: "all-reduce-generator"
processor_id: -1
entry_subroutine {
  name: "entry_subroutine"
  subroutine_root_id: 5
  execution_probability: 1
  execution_count: 1
  instructions {
    name: "loop"
    opcode: "while"
    instruction_id: 1
    inner_subroutines {
      name: "body_subroutine"
      subroutine_root_id: 3
      execution_probability: 1
      execution_count: 1
      instructions {
        name: "compute_payload"
        opcode: "delay"
        instruction_id: 2
        seconds: 1e-6
      }
      instructions {
        name: "reduction"
        opcode: "all-reduce"
        instruction_id: 3
        bytes_out: 1048576
        operand_ids: 2
      }
    }
    inner_subroutines {
      name: "cond_subroutine"
      subroutine_root_id: 4
      execution_probability: 1
      execution_count: 1
      instructions {
        name: "conditional"
        opcode: "delay"
        instruction_id: 4
      }
    }
  }
    instructions {
    name: "root"
    opcode: "null"
    instruction_id: 5
  }
}
'''
    return graph

def parse():
    parser = argparse.ArgumentParser(
        description="ParaGraph All-Reduce traffic generator")
    parser.add_argument("-n", "--num_proc", 
                        help="Number of processors",
                        required=True, type=int, default=8)
    parser.add_argument("-l","--loop_count",
                        help="Number of All-Reduce iterations",
                        required=True, type=int, default=1)
    parser.add_argument("-s", "--size",
                        help="All-Reduce size, in bytes",
                        required=True, type=float, default=1024*1024)
    parser.add_argument("-t","--time_delay",
                        help="Compute delay for non All-Reduce part",
                        required=True, type=float, default=1e-6)
    parser.add_argument("-d", "--output_dir",
                        help="Otput directiry to save graphs",
                        required=True, default='')
    return parser.parse_args()

def main():
    all_reduce = text_format.Parse(get_all_reduce_textproto(), graph_pb2.GraphProto())
    args = parse()
    print (args)
    
    # get access to instruction protos
    while_instr = all_reduce.entry_subroutine.instructions[0]
    body_subroutine = while_instr.inner_subroutines[0]
    cond_subroutine = while_instr.inner_subroutines[0]
    compute_payload_instr = body_subroutine.instructions[0]
    all_reduce_instr = body_subroutine.instructions[1]

    # Update graph according to traffic gen arguments
    comm_group = graph_pb2.CommunicationGroupProto()
    comm_group.group_ids.extend([i for i in range(args.num_proc)])
    all_reduce_instr.communication_groups.append(comm_group)

    body_subroutine.execution_count = args.loop_count
    cond_subroutine.execution_count = args.loop_count
    all_reduce_instr.bytes_out = args.size
    compute_payload_instr.seconds = args.time_delay

    # Store composite graph
    if not os.path.exists(args.output_dir):
        os.makedirs(args.output_dir)
    filename = 'graph.textproto' 
    f = open(os.path.join(args.output_dir, filename), 'w')
    f.write(text_format.MessageToString(all_reduce))
    f.close()

    # Store individualized graphs
    for id in range(args.num_proc):
        all_reduce.processor_id = id
        filename = 'graph.{0}.textproto'.format(id)
        f = open(os.path.join(args.output_dir, filename), 'w')
        f.write(text_format.MessageToString(all_reduce))
        f.close()
if __name__ == '__main__':
    main()
