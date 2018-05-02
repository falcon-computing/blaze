// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: msgGamNam.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "msgGamNam.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace blaze {

namespace {

const ::google::protobuf::Descriptor* Gam2NamRequest_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Gam2NamRequest_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* Gam2NamRequest_MsgType_descriptor_ = NULL;
const ::google::protobuf::Descriptor* Accelerator_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Accelerator_reflection_ = NULL;
const ::google::protobuf::Descriptor* Nam2GamAccNames_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Nam2GamAccNames_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_msgGamNam_2eproto() {
  protobuf_AddDesc_msgGamNam_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "msgGamNam.proto");
  GOOGLE_CHECK(file != NULL);
  Gam2NamRequest_descriptor_ = file->message_type(0);
  static const int Gam2NamRequest_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Gam2NamRequest, type_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Gam2NamRequest, pull_),
  };
  Gam2NamRequest_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Gam2NamRequest_descriptor_,
      Gam2NamRequest::default_instance_,
      Gam2NamRequest_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Gam2NamRequest, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Gam2NamRequest, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Gam2NamRequest));
  Gam2NamRequest_MsgType_descriptor_ = Gam2NamRequest_descriptor_->enum_type(0);
  Accelerator_descriptor_ = file->message_type(1);
  static const int Accelerator_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Accelerator, acc_name_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Accelerator, device_name_),
  };
  Accelerator_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Accelerator_descriptor_,
      Accelerator::default_instance_,
      Accelerator_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Accelerator, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Accelerator, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Accelerator));
  Nam2GamAccNames_descriptor_ = file->message_type(2);
  static const int Nam2GamAccNames_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Nam2GamAccNames, isupdated_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Nam2GamAccNames, acc_names_),
  };
  Nam2GamAccNames_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Nam2GamAccNames_descriptor_,
      Nam2GamAccNames::default_instance_,
      Nam2GamAccNames_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Nam2GamAccNames, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Nam2GamAccNames, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Nam2GamAccNames));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_msgGamNam_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Gam2NamRequest_descriptor_, &Gam2NamRequest::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Accelerator_descriptor_, &Accelerator::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Nam2GamAccNames_descriptor_, &Nam2GamAccNames::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_msgGamNam_2eproto() {
  delete Gam2NamRequest::default_instance_;
  delete Gam2NamRequest_reflection_;
  delete Accelerator::default_instance_;
  delete Accelerator_reflection_;
  delete Nam2GamAccNames::default_instance_;
  delete Nam2GamAccNames_reflection_;
}

void protobuf_AddDesc_msgGamNam_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\017msgGamNam.proto\022\005blaze\"z\n\016Gam2NamReque"
    "st\022+\n\004type\030\001 \002(\0162\035.blaze.Gam2NamRequest."
    "MsgType\022\023\n\004pull\030\002 \001(\010:\005false\"&\n\007MsgType\022"
    "\014\n\010ACCNAMES\020\000\022\r\n\tSHARERATE\020\001\"4\n\013Accelera"
    "tor\022\020\n\010acc_name\030\001 \001(\t\022\023\n\013device_name\030\002 \001"
    "(\t\"R\n\017Nam2GamAccNames\022\030\n\tisUpdated\030\001 \001(\010"
    ":\005false\022%\n\tacc_names\030\002 \003(\0132\022.blaze.Accel"
    "eratorB\027\n\025org.apache.hadoop.fcs", 311);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "msgGamNam.proto", &protobuf_RegisterTypes);
  Gam2NamRequest::default_instance_ = new Gam2NamRequest();
  Accelerator::default_instance_ = new Accelerator();
  Nam2GamAccNames::default_instance_ = new Nam2GamAccNames();
  Gam2NamRequest::default_instance_->InitAsDefaultInstance();
  Accelerator::default_instance_->InitAsDefaultInstance();
  Nam2GamAccNames::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_msgGamNam_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_msgGamNam_2eproto {
  StaticDescriptorInitializer_msgGamNam_2eproto() {
    protobuf_AddDesc_msgGamNam_2eproto();
  }
} static_descriptor_initializer_msgGamNam_2eproto_;

// ===================================================================

const ::google::protobuf::EnumDescriptor* Gam2NamRequest_MsgType_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Gam2NamRequest_MsgType_descriptor_;
}
bool Gam2NamRequest_MsgType_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
      return true;
    default:
      return false;
  }
}

#ifndef _MSC_VER
const Gam2NamRequest_MsgType Gam2NamRequest::ACCNAMES;
const Gam2NamRequest_MsgType Gam2NamRequest::SHARERATE;
const Gam2NamRequest_MsgType Gam2NamRequest::MsgType_MIN;
const Gam2NamRequest_MsgType Gam2NamRequest::MsgType_MAX;
const int Gam2NamRequest::MsgType_ARRAYSIZE;
#endif  // _MSC_VER
#ifndef _MSC_VER
const int Gam2NamRequest::kTypeFieldNumber;
const int Gam2NamRequest::kPullFieldNumber;
#endif  // !_MSC_VER

Gam2NamRequest::Gam2NamRequest()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Gam2NamRequest::InitAsDefaultInstance() {
}

Gam2NamRequest::Gam2NamRequest(const Gam2NamRequest& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Gam2NamRequest::SharedCtor() {
  _cached_size_ = 0;
  type_ = 0;
  pull_ = false;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Gam2NamRequest::~Gam2NamRequest() {
  SharedDtor();
}

void Gam2NamRequest::SharedDtor() {
  if (this != default_instance_) {
  }
}

void Gam2NamRequest::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Gam2NamRequest::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Gam2NamRequest_descriptor_;
}

const Gam2NamRequest& Gam2NamRequest::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_msgGamNam_2eproto();
  return *default_instance_;
}

Gam2NamRequest* Gam2NamRequest::default_instance_ = NULL;

Gam2NamRequest* Gam2NamRequest::New() const {
  return new Gam2NamRequest;
}

void Gam2NamRequest::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    type_ = 0;
    pull_ = false;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Gam2NamRequest::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required .blaze.Gam2NamRequest.MsgType type = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::blaze::Gam2NamRequest_MsgType_IsValid(value)) {
            set_type(static_cast< ::blaze::Gam2NamRequest_MsgType >(value));
          } else {
            mutable_unknown_fields()->AddVarint(1, value);
          }
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_pull;
        break;
      }

      // optional bool pull = 2 [default = false];
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_pull:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, &pull_)));
          set_has_pull();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Gam2NamRequest::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required .blaze.Gam2NamRequest.MsgType type = 1;
  if (has_type()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->type(), output);
  }

  // optional bool pull = 2 [default = false];
  if (has_pull()) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(2, this->pull(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Gam2NamRequest::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required .blaze.Gam2NamRequest.MsgType type = 1;
  if (has_type()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      1, this->type(), target);
  }

  // optional bool pull = 2 [default = false];
  if (has_pull()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(2, this->pull(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Gam2NamRequest::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required .blaze.Gam2NamRequest.MsgType type = 1;
    if (has_type()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->type());
    }

    // optional bool pull = 2 [default = false];
    if (has_pull()) {
      total_size += 1 + 1;
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Gam2NamRequest::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Gam2NamRequest* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Gam2NamRequest*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Gam2NamRequest::MergeFrom(const Gam2NamRequest& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_type()) {
      set_type(from.type());
    }
    if (from.has_pull()) {
      set_pull(from.pull());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Gam2NamRequest::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Gam2NamRequest::CopyFrom(const Gam2NamRequest& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Gam2NamRequest::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void Gam2NamRequest::Swap(Gam2NamRequest* other) {
  if (other != this) {
    std::swap(type_, other->type_);
    std::swap(pull_, other->pull_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Gam2NamRequest::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Gam2NamRequest_descriptor_;
  metadata.reflection = Gam2NamRequest_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int Accelerator::kAccNameFieldNumber;
const int Accelerator::kDeviceNameFieldNumber;
#endif  // !_MSC_VER

Accelerator::Accelerator()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Accelerator::InitAsDefaultInstance() {
}

Accelerator::Accelerator(const Accelerator& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Accelerator::SharedCtor() {
  _cached_size_ = 0;
  acc_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  device_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Accelerator::~Accelerator() {
  SharedDtor();
}

void Accelerator::SharedDtor() {
  if (acc_name_ != &::google::protobuf::internal::kEmptyString) {
    delete acc_name_;
  }
  if (device_name_ != &::google::protobuf::internal::kEmptyString) {
    delete device_name_;
  }
  if (this != default_instance_) {
  }
}

void Accelerator::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Accelerator::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Accelerator_descriptor_;
}

const Accelerator& Accelerator::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_msgGamNam_2eproto();
  return *default_instance_;
}

Accelerator* Accelerator::default_instance_ = NULL;

Accelerator* Accelerator::New() const {
  return new Accelerator;
}

void Accelerator::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_acc_name()) {
      if (acc_name_ != &::google::protobuf::internal::kEmptyString) {
        acc_name_->clear();
      }
    }
    if (has_device_name()) {
      if (device_name_ != &::google::protobuf::internal::kEmptyString) {
        device_name_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Accelerator::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional string acc_name = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_acc_name()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->acc_name().data(), this->acc_name().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_device_name;
        break;
      }

      // optional string device_name = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_device_name:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_device_name()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->device_name().data(), this->device_name().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Accelerator::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional string acc_name = 1;
  if (has_acc_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->acc_name().data(), this->acc_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->acc_name(), output);
  }

  // optional string device_name = 2;
  if (has_device_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->device_name().data(), this->device_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->device_name(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Accelerator::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional string acc_name = 1;
  if (has_acc_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->acc_name().data(), this->acc_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->acc_name(), target);
  }

  // optional string device_name = 2;
  if (has_device_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->device_name().data(), this->device_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->device_name(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Accelerator::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional string acc_name = 1;
    if (has_acc_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->acc_name());
    }

    // optional string device_name = 2;
    if (has_device_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->device_name());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Accelerator::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Accelerator* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Accelerator*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Accelerator::MergeFrom(const Accelerator& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_acc_name()) {
      set_acc_name(from.acc_name());
    }
    if (from.has_device_name()) {
      set_device_name(from.device_name());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Accelerator::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Accelerator::CopyFrom(const Accelerator& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Accelerator::IsInitialized() const {

  return true;
}

void Accelerator::Swap(Accelerator* other) {
  if (other != this) {
    std::swap(acc_name_, other->acc_name_);
    std::swap(device_name_, other->device_name_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Accelerator::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Accelerator_descriptor_;
  metadata.reflection = Accelerator_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int Nam2GamAccNames::kIsUpdatedFieldNumber;
const int Nam2GamAccNames::kAccNamesFieldNumber;
#endif  // !_MSC_VER

Nam2GamAccNames::Nam2GamAccNames()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Nam2GamAccNames::InitAsDefaultInstance() {
}

Nam2GamAccNames::Nam2GamAccNames(const Nam2GamAccNames& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Nam2GamAccNames::SharedCtor() {
  _cached_size_ = 0;
  isupdated_ = false;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Nam2GamAccNames::~Nam2GamAccNames() {
  SharedDtor();
}

void Nam2GamAccNames::SharedDtor() {
  if (this != default_instance_) {
  }
}

void Nam2GamAccNames::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Nam2GamAccNames::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Nam2GamAccNames_descriptor_;
}

const Nam2GamAccNames& Nam2GamAccNames::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_msgGamNam_2eproto();
  return *default_instance_;
}

Nam2GamAccNames* Nam2GamAccNames::default_instance_ = NULL;

Nam2GamAccNames* Nam2GamAccNames::New() const {
  return new Nam2GamAccNames;
}

void Nam2GamAccNames::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    isupdated_ = false;
  }
  acc_names_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Nam2GamAccNames::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional bool isUpdated = 1 [default = false];
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, &isupdated_)));
          set_has_isupdated();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_acc_names;
        break;
      }

      // repeated .blaze.Accelerator acc_names = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_acc_names:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_acc_names()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_acc_names;
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Nam2GamAccNames::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional bool isUpdated = 1 [default = false];
  if (has_isupdated()) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(1, this->isupdated(), output);
  }

  // repeated .blaze.Accelerator acc_names = 2;
  for (int i = 0; i < this->acc_names_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      2, this->acc_names(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Nam2GamAccNames::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional bool isUpdated = 1 [default = false];
  if (has_isupdated()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(1, this->isupdated(), target);
  }

  // repeated .blaze.Accelerator acc_names = 2;
  for (int i = 0; i < this->acc_names_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        2, this->acc_names(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Nam2GamAccNames::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional bool isUpdated = 1 [default = false];
    if (has_isupdated()) {
      total_size += 1 + 1;
    }

  }
  // repeated .blaze.Accelerator acc_names = 2;
  total_size += 1 * this->acc_names_size();
  for (int i = 0; i < this->acc_names_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->acc_names(i));
  }

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Nam2GamAccNames::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Nam2GamAccNames* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Nam2GamAccNames*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Nam2GamAccNames::MergeFrom(const Nam2GamAccNames& from) {
  GOOGLE_CHECK_NE(&from, this);
  acc_names_.MergeFrom(from.acc_names_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_isupdated()) {
      set_isupdated(from.isupdated());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Nam2GamAccNames::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Nam2GamAccNames::CopyFrom(const Nam2GamAccNames& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Nam2GamAccNames::IsInitialized() const {

  return true;
}

void Nam2GamAccNames::Swap(Nam2GamAccNames* other) {
  if (other != this) {
    std::swap(isupdated_, other->isupdated_);
    acc_names_.Swap(&other->acc_names_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Nam2GamAccNames::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Nam2GamAccNames_descriptor_;
  metadata.reflection = Nam2GamAccNames_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace blaze

// @@protoc_insertion_point(global_scope)