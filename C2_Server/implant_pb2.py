# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: implant.proto
# Protobuf Python Version: 5.26.1
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\rimplant.proto\"U\n\x0fRegisterImplant\x12\x10\n\x08Password\x18\x01 \x01(\t\x12\x0c\n\x04GUID\x18\x02 \x01(\t\x12\x10\n\x08Username\x18\x03 \x01(\t\x12\x10\n\x08Hostname\x18\x04 \x01(\t\"=\n\x0bTaskRequest\x12\x10\n\x08TaskGuid\x18\x01 \x01(\t\x12\x0e\n\x06Opcode\x18\x02 \x01(\x05\x12\x0c\n\x04\x41rgs\x18\x03 \x01(\t\"2\n\x0cTaskResponse\x12\x10\n\x08TaskGuid\x18\x01 \x01(\t\x12\x10\n\x08Response\x18\x03 \x01(\x0c\x62\x06proto3')

_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'implant_pb2', _globals)
if not _descriptor._USE_C_DESCRIPTORS:
  DESCRIPTOR._loaded_options = None
  _globals['_REGISTERIMPLANT']._serialized_start=17
  _globals['_REGISTERIMPLANT']._serialized_end=102
  _globals['_TASKREQUEST']._serialized_start=104
  _globals['_TASKREQUEST']._serialized_end=165
  _globals['_TASKRESPONSE']._serialized_start=167
  _globals['_TASKRESPONSE']._serialized_end=217
# @@protoc_insertion_point(module_scope)