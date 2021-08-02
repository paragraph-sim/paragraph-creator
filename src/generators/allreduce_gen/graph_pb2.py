# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: graph.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='graph.proto',
  package='paragraph',
  syntax='proto3',
  serialized_options=b'\370\001\001',
  create_key=_descriptor._internal_create_key,
  serialized_pb=b'\n\x0bgraph.proto\x12\tparagraph\",\n\x17\x43ommunicationGroupProto\x12\x11\n\tgroup_ids\x18\x01 \x03(\x03\"\xec\x02\n\x10InstructionProto\x12\x0c\n\x04name\x18\x01 \x01(\t\x12\x0e\n\x06opcode\x18\x02 \x01(\t\x12\x16\n\x0einstruction_id\x18\x03 \x01(\x03\x12\x0b\n\x03ops\x18\x04 \x01(\x01\x12\x10\n\x08\x62ytes_in\x18\x05 \x01(\x01\x12\x11\n\tbytes_out\x18\x06 \x01(\x01\x12\x17\n\x0ftranscendentals\x18\x07 \x01(\x01\x12\x0f\n\x07seconds\x18\x08 \x01(\x01\x12\x1d\n\x15\x62onded_instruction_id\x18\t \x01(\x03\x12\x19\n\x11\x63ommunication_tag\x18\n \x01(\x04\x12@\n\x14\x63ommunication_groups\x18\x0b \x03(\x0b\x32\".paragraph.CommunicationGroupProto\x12\x13\n\x0boperand_ids\x18\x0c \x03(\x03\x12\x35\n\x11inner_subroutines\x18\r \x03(\x0b\x32\x1a.paragraph.SubroutineProto\"\xa6\x01\n\x0fSubroutineProto\x12\x0c\n\x04name\x18\x01 \x01(\t\x12\x1a\n\x12subroutine_root_id\x18\x02 \x01(\x03\x12\x1d\n\x15\x65xecution_probability\x18\x03 \x01(\x01\x12\x17\n\x0f\x65xecution_count\x18\x04 \x01(\x03\x12\x31\n\x0cinstructions\x18\x05 \x03(\x0b\x32\x1b.paragraph.InstructionProto\"f\n\nGraphProto\x12\x0c\n\x04name\x18\x01 \x01(\t\x12\x14\n\x0cprocessor_id\x18\x02 \x01(\x03\x12\x34\n\x10\x65ntry_subroutine\x18\x03 \x01(\x0b\x32\x1a.paragraph.SubroutineProtoB\x03\xf8\x01\x01\x62\x06proto3'
)




_COMMUNICATIONGROUPPROTO = _descriptor.Descriptor(
  name='CommunicationGroupProto',
  full_name='paragraph.CommunicationGroupProto',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='group_ids', full_name='paragraph.CommunicationGroupProto.group_ids', index=0,
      number=1, type=3, cpp_type=2, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=26,
  serialized_end=70,
)


_INSTRUCTIONPROTO = _descriptor.Descriptor(
  name='InstructionProto',
  full_name='paragraph.InstructionProto',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='name', full_name='paragraph.InstructionProto.name', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='opcode', full_name='paragraph.InstructionProto.opcode', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='instruction_id', full_name='paragraph.InstructionProto.instruction_id', index=2,
      number=3, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='ops', full_name='paragraph.InstructionProto.ops', index=3,
      number=4, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='bytes_in', full_name='paragraph.InstructionProto.bytes_in', index=4,
      number=5, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='bytes_out', full_name='paragraph.InstructionProto.bytes_out', index=5,
      number=6, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='transcendentals', full_name='paragraph.InstructionProto.transcendentals', index=6,
      number=7, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='seconds', full_name='paragraph.InstructionProto.seconds', index=7,
      number=8, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='bonded_instruction_id', full_name='paragraph.InstructionProto.bonded_instruction_id', index=8,
      number=9, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='communication_tag', full_name='paragraph.InstructionProto.communication_tag', index=9,
      number=10, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='communication_groups', full_name='paragraph.InstructionProto.communication_groups', index=10,
      number=11, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='operand_ids', full_name='paragraph.InstructionProto.operand_ids', index=11,
      number=12, type=3, cpp_type=2, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='inner_subroutines', full_name='paragraph.InstructionProto.inner_subroutines', index=12,
      number=13, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=73,
  serialized_end=437,
)


_SUBROUTINEPROTO = _descriptor.Descriptor(
  name='SubroutineProto',
  full_name='paragraph.SubroutineProto',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='name', full_name='paragraph.SubroutineProto.name', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='subroutine_root_id', full_name='paragraph.SubroutineProto.subroutine_root_id', index=1,
      number=2, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='execution_probability', full_name='paragraph.SubroutineProto.execution_probability', index=2,
      number=3, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='execution_count', full_name='paragraph.SubroutineProto.execution_count', index=3,
      number=4, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='instructions', full_name='paragraph.SubroutineProto.instructions', index=4,
      number=5, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=440,
  serialized_end=606,
)


_GRAPHPROTO = _descriptor.Descriptor(
  name='GraphProto',
  full_name='paragraph.GraphProto',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='name', full_name='paragraph.GraphProto.name', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='processor_id', full_name='paragraph.GraphProto.processor_id', index=1,
      number=2, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='entry_subroutine', full_name='paragraph.GraphProto.entry_subroutine', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=608,
  serialized_end=710,
)

_INSTRUCTIONPROTO.fields_by_name['communication_groups'].message_type = _COMMUNICATIONGROUPPROTO
_INSTRUCTIONPROTO.fields_by_name['inner_subroutines'].message_type = _SUBROUTINEPROTO
_SUBROUTINEPROTO.fields_by_name['instructions'].message_type = _INSTRUCTIONPROTO
_GRAPHPROTO.fields_by_name['entry_subroutine'].message_type = _SUBROUTINEPROTO
DESCRIPTOR.message_types_by_name['CommunicationGroupProto'] = _COMMUNICATIONGROUPPROTO
DESCRIPTOR.message_types_by_name['InstructionProto'] = _INSTRUCTIONPROTO
DESCRIPTOR.message_types_by_name['SubroutineProto'] = _SUBROUTINEPROTO
DESCRIPTOR.message_types_by_name['GraphProto'] = _GRAPHPROTO
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

CommunicationGroupProto = _reflection.GeneratedProtocolMessageType('CommunicationGroupProto', (_message.Message,), {
  'DESCRIPTOR' : _COMMUNICATIONGROUPPROTO,
  '__module__' : 'graph_pb2'
  # @@protoc_insertion_point(class_scope:paragraph.CommunicationGroupProto)
  })
_sym_db.RegisterMessage(CommunicationGroupProto)

InstructionProto = _reflection.GeneratedProtocolMessageType('InstructionProto', (_message.Message,), {
  'DESCRIPTOR' : _INSTRUCTIONPROTO,
  '__module__' : 'graph_pb2'
  # @@protoc_insertion_point(class_scope:paragraph.InstructionProto)
  })
_sym_db.RegisterMessage(InstructionProto)

SubroutineProto = _reflection.GeneratedProtocolMessageType('SubroutineProto', (_message.Message,), {
  'DESCRIPTOR' : _SUBROUTINEPROTO,
  '__module__' : 'graph_pb2'
  # @@protoc_insertion_point(class_scope:paragraph.SubroutineProto)
  })
_sym_db.RegisterMessage(SubroutineProto)

GraphProto = _reflection.GeneratedProtocolMessageType('GraphProto', (_message.Message,), {
  'DESCRIPTOR' : _GRAPHPROTO,
  '__module__' : 'graph_pb2'
  # @@protoc_insertion_point(class_scope:paragraph.GraphProto)
  })
_sym_db.RegisterMessage(GraphProto)


DESCRIPTOR._options = None
# @@protoc_insertion_point(module_scope)