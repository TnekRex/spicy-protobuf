#include <cmath>
#include <gtest/gtest.h>
#include <hilti/rt/libhilti.h>
#include <hilti/rt/types/reference.h>
#include <math.h>
#include <spicy/rt/libspicy.h>
#include <spicy_protobuf.h>
#include <test-data/i32/protobuf_i32.h>
#include <test-data/i64/protobuf_i64.h>
#include <test-data/varint/protobuf_150.h>
#include <test-data/varint/protobuf_1500.h>
#include <test-data/varint/protobuf_1500000.h>
#include <test-data/varint/protobuf_bools.h>
#include <test-data/varint/protobuf_large_field_num.h>
#include <test-data/varint/protobuf_max_uint.h>
#include <test-data/varint/protobuf_neg_1.h>
#include <test-data/varint/protobuf_neg_150.h>
#include <test-data/varint/protobuf_neg_2.h>
#include <test-data/varint/protobuf_zigzag_150.h>
#include <test-data/groups/protobuf_groups.h>

namespace { // anonymous namespace

using ::hilti::rt::ValueReference;
using ::__hlt::protobuf::Message;

class SpicyProtobufTest : public ::testing::Test {
protected:

  static auto SetUpTestSuite() -> void;
  static auto TearDownTestSuite() -> void;

  // Flags to pass to the parseMessage function.
  // All values should be powers of 2.
  enum ParseFlag {
    NONE         = 0,
    CHECK_GROUPS = 1, // Check that sgroup and egroup are false
  };

  static auto parseFlagIsSet(uint32_t flags, ParseFlag flag_to_check) -> bool;
  static auto parseMessage(const char* data, uint64_t size, uint32_t flags = ParseFlag::CHECK_GROUPS) -> ValueReference<Message>;
};

auto SpicyProtobufTest::SetUpTestSuite() -> void {
  // Initialize runtime libraries
  ::hilti::rt::init();
  ::spicy::rt::init();
}

auto SpicyProtobufTest::TearDownTestSuite() -> void {
  // Tear down runtime libraries
  ::spicy::rt::done();
  ::hilti::rt::done();
}

auto SpicyProtobufTest::parseFlagIsSet(uint32_t const flags, ParseFlag const flag_to_check) -> bool {
  return (flags & flag_to_check) > 0;
}

auto SpicyProtobufTest::parseMessage(const char* p_data, uint64_t const size, uint32_t const flags) -> ValueReference<Message> {
  using namespace ::hilti::rt::reference;
  using ::hilti::rt::Stream;

  auto p_input_stream = make_value<Stream>(p_data, size);
  p_input_stream->freeze();
  auto p_msg = make_value<Message>();
  ::hlt::protobuf::Message::parse2(p_msg, p_input_stream, {}, {});
  if (parseFlagIsSet(flags, ParseFlag::CHECK_GROUPS)) {
    for (auto const &tag_and_val: *p_msg->message) {
      EXPECT_FALSE(tag_and_val.value.value()->sgroup);
      EXPECT_FALSE(tag_and_val.value.value()->egroup);
    }
  }
  return p_msg;
}

TEST_F(SpicyProtobufTest, TestVarIntPositive) {
  using namespace __hlt::protobuf;

  { // protobuf_150
    auto p_data = parseMessage(reinterpret_cast<const char *>(varint_protobuf_150_binpb), varint_protobuf_150_binpb_len);
    ASSERT_EQ(p_data->message->size(), 1);
    auto tag_and_val = p_data->message->at(0);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 1);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
    ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 150);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, 150);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, 150);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, 75);
  }

  { // protobuf_1500
    auto p_data = parseMessage(reinterpret_cast<const char *>(varint_protobuf_1500_binpb), varint_protobuf_1500_binpb_len);
    ASSERT_EQ(p_data->message->size(), 1);
    auto tag_and_val = p_data->message->at(0);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 1);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
    ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 1500);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, 1500);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, 1500);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, 750);
  }

  { // protobuf_1500000
    auto p_data = parseMessage(reinterpret_cast<const char *>(varint_protobuf_1500000_binpb), varint_protobuf_1500000_binpb_len);
    ASSERT_EQ(p_data->message->size(), 1);
    auto tag_and_val = p_data->message->at(0);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 1);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
    ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 1500000);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, 1500000);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, 1500000);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, 750000);
  }

}

TEST_F(SpicyProtobufTest, TestVarIntBool) {
  using namespace __hlt::protobuf;

  { // protobuf_bools
    auto p_data = parseMessage(reinterpret_cast<const char *>(varint_protobuf_bools_binpb), varint_protobuf_bools_binpb_len);
    ASSERT_EQ(p_data->message->size(), 2);
    { // First TagAndValue
      auto tag_and_val = p_data->message->at(0);
      EXPECT_EQ(tag_and_val.tag.value()->field_num, 1);
      EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
      ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 1);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, 1);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, 1);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, -1);
    }

    { // Second TagAndValue
      auto tag_and_val = p_data->message->at(1);
      EXPECT_EQ(tag_and_val.tag.value()->field_num, 2);
      EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
      ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 0);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, 0);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, 0);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, 0);
    }
  }
}

TEST_F(SpicyProtobufTest, TestVarIntLargeFieldNum) {
  using namespace __hlt::protobuf;

  { // protobuf_large_field_num
    auto p_data = parseMessage(reinterpret_cast<const char *>(varint_protobuf_large_field_num_binpb), varint_protobuf_large_field_num_binpb_len);
    ASSERT_EQ(p_data->message->size(), 2);
    { // First TagAndValue
      auto tag_and_val = p_data->message->at(0);
      EXPECT_EQ(tag_and_val.tag.value()->field_num, 63);
      EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
      ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 1);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, 1);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, 1);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, -1);
    }

    { // Second TagAndValue
      auto tag_and_val = p_data->message->at(1);
      EXPECT_EQ(tag_and_val.tag.value()->field_num, 1000);
      EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
      ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 1);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, 1);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, 1);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, -1);
    }
  }
}

TEST_F(SpicyProtobufTest, TestVarIntMaxUint) {
  using namespace __hlt::protobuf;

  { // protobuf_max_uint
    auto p_data = parseMessage(reinterpret_cast<const char *>(varint_protobuf_max_uint_binpb), varint_protobuf_max_uint_binpb_len);
    ASSERT_EQ(p_data->message->size(), 1);
    auto tag_and_val = p_data->message->at(0);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 1);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
    ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 18446744073709551615ull);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, -1);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, -1);
    // The signed zigzag value can't be represented in 64 bits, so skipping it.
  }
}

TEST_F(SpicyProtobufTest, TestVarIntNegative) {
  using namespace __hlt::protobuf;

  { // protobuf_neg_1
    auto p_data = parseMessage(reinterpret_cast<const char *>(varint_protobuf_neg_1_binpb), varint_protobuf_neg_1_binpb_len);
    ASSERT_EQ(p_data->message->size(), 3);
    { // First TagAndValue
      auto tag_and_val = p_data->message->at(0);
      EXPECT_EQ(tag_and_val.tag.value()->field_num, 1);
      EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
      ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 4294967295);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, -1);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, 4294967295);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, -2147483648);
    }

    { // Second TagAndValue
      auto tag_and_val = p_data->message->at(1);
      EXPECT_EQ(tag_and_val.tag.value()->field_num, 2);
      EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
      ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 18446744073709551615ull);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, -1);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, -1);
      // ZigZag too long to be represented in 64 bits
    }

    { // Third TagAndValue
      auto tag_and_val = p_data->message->at(2);
      EXPECT_EQ(tag_and_val.tag.value()->field_num, 3);
      EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
      ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 18446744073709551615ull);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, -1);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, -1);
      // ZigZag too long to be represented in 64 bits
    }
  }

  { // protobuf_neg_2
    auto p_data = parseMessage(reinterpret_cast<const char *>(varint_protobuf_neg_2_binpb), varint_protobuf_neg_2_binpb_len);
    ASSERT_EQ(p_data->message->size(), 2);
    { // First TagAndvalue
      auto tag_and_val = p_data->message->at(0);
      EXPECT_EQ(tag_and_val.tag.value()->field_num, 1);
      EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
      ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 18446744073709551614ull);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, -2);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, -2);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, 9223372036854775807ull);
    }

    { // Second TagAndvalue
      auto tag_and_val = p_data->message->at(1);
      EXPECT_EQ(tag_and_val.tag.value()->field_num, 1);
      EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
      ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 3);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, 3);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, 3);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, -2);
    }
  }

  { // protobuf_neg_150
    auto p_data = parseMessage(reinterpret_cast<const char *>(varint_protobuf_neg_150_binpb), varint_protobuf_neg_150_binpb_len);
    ASSERT_EQ(p_data->message->size(), 1);
    auto tag_and_val = p_data->message->at(0);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 1);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
    ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 18446744073709551466ull);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, -150);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, -150);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, 9223372036854775733ull);
  }
}

TEST_F(SpicyProtobufTest, TestVarIntZigZag) {
  using namespace __hlt::protobuf;

  { // protobuf_zigzag_150
    auto p_data = parseMessage(reinterpret_cast<const char *>(varint_protobuf_zigzag_150_binpb), varint_protobuf_zigzag_150_binpb_len);
    ASSERT_EQ(p_data->message->size(), 2);
    { // First TagAndvalue
      auto tag_and_val = p_data->message->at(0);
      EXPECT_EQ(tag_and_val.tag.value()->field_num, 1);
      EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
      ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 299);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, 299);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, 299);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, -150);
    }

    { // Second TagAndvalue
      auto tag_and_val = p_data->message->at(1);
      EXPECT_EQ(tag_and_val.tag.value()->field_num, 2);
      EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
      ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 300);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, 300);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, 300);
      EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, 150);
    }
  }
}

TEST_F(SpicyProtobufTest, TestI32) {
  using namespace __hlt::protobuf;

  // protobuf_i32
  auto p_data = parseMessage(reinterpret_cast<const char *>(i32_protobuf_i32_binpb), i32_protobuf_i32_binpb_len);
  auto p_msg = p_data->message;
  ASSERT_EQ(p_msg->size(), 28);

  { // field 1
    auto tag_and_val = p_data->message->at(0);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 1);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 0);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 0);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_float, 0.0);
  }

  { // field 2
    auto tag_and_val = p_data->message->at(1);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 2);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 1);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 1);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, 1.4013e-45);
  }

  { // field 3
    auto tag_and_val = p_data->message->at(2);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 3);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 4294967295);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, -1);
    EXPECT_TRUE(std::isnan(tag_and_val.value.value()->i32.value()->as_float));
    EXPECT_TRUE(std::signbit(tag_and_val.value.value()->i32.value()->as_float));
  }

  { // field 4
    auto tag_and_val = p_data->message->at(3);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 4);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 4294967295);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, -1);
    EXPECT_TRUE(std::isnan(tag_and_val.value.value()->i32.value()->as_float));
    EXPECT_TRUE(std::signbit(tag_and_val.value.value()->i32.value()->as_float));
  }

  { // field 5
    auto tag_and_val = p_data->message->at(4);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 5);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 1);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 1);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, 1.4013e-45);
  }

  { // field 6
    auto tag_and_val = p_data->message->at(5);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 6);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 8388607);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 8388607);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, 1.1754942107e-38);
  }

  { // field 7
    auto tag_and_val = p_data->message->at(6);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 7);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 8388608);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 8388608);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, 1.1754943508e-38);
  }

  { // field 8
    auto tag_and_val = p_data->message->at(7);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 8);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 2139095039);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 2139095039);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, 3.4028234664e+38);
  }

  { // field 9
    auto tag_and_val = p_data->message->at(8);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 9);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 1065353215);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 1065353215);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, 1.0);
  }

  { // field 10
    auto tag_and_val = p_data->message->at(9);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 10);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 1065353216);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 1065353216);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, 1.0);
  }

  { // field 11
    auto tag_and_val = p_data->message->at(10);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 11);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 1065353217);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 1065353217);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, 1.0);
  }

  { // field 12
    auto tag_and_val = p_data->message->at(11);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 12);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 3221225472);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, -1073741824);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, -2.0);
  }

  { // field 13
    auto tag_and_val = p_data->message->at(12);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 13);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 0);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 0);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, 0.0);
  }

  { // field 14
    auto tag_and_val = p_data->message->at(13);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 14);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 2147483648);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, -2147483648);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, -0.0);
  }

  { // field 15
    auto tag_and_val = p_data->message->at(14);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 15);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 2139095040);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 2139095040);
    EXPECT_TRUE(std::isinf(tag_and_val.value.value()->i32.value()->as_float));
    EXPECT_FALSE(std::signbit(tag_and_val.value.value()->i32.value()->as_float));
  }

  { // field 16
    auto tag_and_val = p_data->message->at(15);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 16);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 4286578688);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, -8388608);
    EXPECT_TRUE(std::isinf(tag_and_val.value.value()->i32.value()->as_float));
    EXPECT_TRUE(std::signbit(tag_and_val.value.value()->i32.value()->as_float));
  }

  { // field 17
    auto tag_and_val = p_data->message->at(16);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 17);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 1078530011);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 1078530011);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, 3.14159265358979323846);
  }

  { // field 18
    auto tag_and_val = p_data->message->at(17);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 18);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 1051372203);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 1051372203);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, 1.0/3.0);
  }

  { // field 19
    auto tag_and_val = p_data->message->at(18);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 19);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 4290772993);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, -4194303);
    EXPECT_TRUE(std::isnan(tag_and_val.value.value()->i32.value()->as_float));
  }

  { // field 20
    auto tag_and_val = p_data->message->at(19);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 20);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 4286578689);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, -8388607);
    EXPECT_TRUE(std::isnan(tag_and_val.value.value()->i32.value()->as_float));
  }

  { // field 21
    auto tag_and_val = p_data->message->at(20);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 21);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 0);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 0);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, 0.0);
  }

  { // field 22
    auto tag_and_val = p_data->message->at(21);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 22);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 2147483648);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, -2147483648);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, -0.0);
  }

  { // field 23
    auto tag_and_val = p_data->message->at(22);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 23);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 2139095040);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 2139095040);
    EXPECT_TRUE(std::isinf(tag_and_val.value.value()->i32.value()->as_float));
    EXPECT_FALSE(std::signbit(tag_and_val.value.value()->i32.value()->as_float));
  }

  { // field 24
    auto tag_and_val = p_data->message->at(23);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 24);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 4286578688);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, -8388608);
    EXPECT_TRUE(std::isinf(tag_and_val.value.value()->i32.value()->as_float));
    EXPECT_TRUE(std::signbit(tag_and_val.value.value()->i32.value()->as_float));
  }

  { // field 25
    auto tag_and_val = p_data->message->at(24);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 25);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 150);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 150);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, 2.10195e-43);
  }

  { // field 26
    auto tag_and_val = p_data->message->at(25);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 26);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 4294967146);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, -150);
    EXPECT_TRUE(std::isnan(tag_and_val.value.value()->i32.value()->as_float));
    EXPECT_TRUE(std::signbit(tag_and_val.value.value()->i32.value()->as_float));
  }

  { // field 27
    auto tag_and_val = p_data->message->at(26);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 27);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 1125521818);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 1125521818);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, 150.1);
  }

  { // field 28
    auto tag_and_val = p_data->message->at(27);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 28);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 3273005466);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, -1021961830);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, -150.1);
  }

}

TEST_F(SpicyProtobufTest, TestI64) {
  using namespace __hlt::protobuf;

  // protobuf_i64
  auto p_data = parseMessage(reinterpret_cast<const char *>(i64_protobuf_i64_binpb), i64_protobuf_i64_binpb_len);
  auto p_msg = p_data->message;
  ASSERT_EQ(p_msg->size(), 24);

  { // field 1
    auto tag_and_val = p_data->message->at(0);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 1);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 0);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 0);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_double, 0.0);
  }

  { // field 2
    auto tag_and_val = p_data->message->at(1);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 2);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 1);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 1);
    EXPECT_DOUBLE_EQ(tag_and_val.value.value()->i64.value()->as_double, 4.94066e-324);
  }

  { // field 3
    auto tag_and_val = p_data->message->at(2);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 3);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 18446744073709551615u);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, -1);
    EXPECT_TRUE(std::isnan(tag_and_val.value.value()->i64.value()->as_double));
    EXPECT_TRUE(std::signbit(tag_and_val.value.value()->i64.value()->as_double));
  }

  { // field 4
    auto tag_and_val = p_data->message->at(3);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 4);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 18446744073709551615u);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, -1);
    EXPECT_TRUE(std::isnan(tag_and_val.value.value()->i64.value()->as_double));
    EXPECT_TRUE(std::signbit(tag_and_val.value.value()->i64.value()->as_double));
  }

  { // field 5
    auto tag_and_val = p_data->message->at(4);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 5);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 0);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 0);
    EXPECT_DOUBLE_EQ(tag_and_val.value.value()->i64.value()->as_double, 0.0);
  }

  { // field 6
    auto tag_and_val = p_data->message->at(5);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 6);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 9223372036854775808u);
    EXPECT_DOUBLE_EQ(tag_and_val.value.value()->i64.value()->as_double, -0.0);
  }

  { // field 7
    auto tag_and_val = p_data->message->at(6);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 7);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 0);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 0);
    EXPECT_DOUBLE_EQ(tag_and_val.value.value()->i64.value()->as_double, 0.0);
  }

  { // field 8
    auto tag_and_val = p_data->message->at(7);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 8);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 9218868437227405312);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 9218868437227405312);
    EXPECT_TRUE(std::isinf(tag_and_val.value.value()->i64.value()->as_double));
    EXPECT_FALSE(std::signbit(tag_and_val.value.value()->i64.value()->as_double));
  }

  { // field 9
    auto tag_and_val = p_data->message->at(8);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 9);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 18442240474082181120u);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, -4503599627370496);
    EXPECT_TRUE(std::isinf(tag_and_val.value.value()->i64.value()->as_double));
    EXPECT_TRUE(std::signbit(tag_and_val.value.value()->i64.value()->as_double));
  }

  { // field 10
    auto tag_and_val = p_data->message->at(9);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 10);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 150);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 150);
    EXPECT_DOUBLE_EQ(tag_and_val.value.value()->i64.value()->as_double, 7.41098e-322);
  }

  { // field 11
    auto tag_and_val = p_data->message->at(10);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 11);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 18446744073709551466u);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, -150);
    EXPECT_TRUE(std::isnan(tag_and_val.value.value()->i64.value()->as_double));
    EXPECT_TRUE(std::signbit(tag_and_val.value.value()->i64.value()->as_double));
  }

  { // field 12
    auto tag_and_val = p_data->message->at(11);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 12);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 4639485190814774067);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 4639485190814774067);
    EXPECT_DOUBLE_EQ(tag_and_val.value.value()->i64.value()->as_double, 150.1);
  }

  { // field 13
    auto tag_and_val = p_data->message->at(12);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 13);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 13862857227669549875u);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, -4583886846040001741);
    EXPECT_DOUBLE_EQ(tag_and_val.value.value()->i64.value()->as_double, -150.1);
  }

  { // field 14
    auto tag_and_val = p_data->message->at(13);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 14);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 4607182418800017409);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 4607182418800017409);
    EXPECT_DOUBLE_EQ(tag_and_val.value.value()->i64.value()->as_double, 1.0);
  }

  { // field 15
    auto tag_and_val = p_data->message->at(14);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 15);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 4577909021222109184);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 4577909021222109184);
    EXPECT_DOUBLE_EQ(tag_and_val.value.value()->i64.value()->as_double, 3.0 / 256.0);
  }

  { // field 16
    auto tag_and_val = p_data->message->at(15);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 16);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 1);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 1);
    EXPECT_DOUBLE_EQ(tag_and_val.value.value()->i64.value()->as_double, 4.9406564584124654e-324);
  }

  { // field 17
    auto tag_and_val = p_data->message->at(16);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 17);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 4503599627370495);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 4503599627370495);
    EXPECT_DOUBLE_EQ(tag_and_val.value.value()->i64.value()->as_double, 2.2250738585072009e-308);
  }

  { // field 18
    auto tag_and_val = p_data->message->at(17);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 18);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 4503599627370496);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 4503599627370496);
    EXPECT_DOUBLE_EQ(tag_and_val.value.value()->i64.value()->as_double, 2.2250738585072014e-308);
  }

  { // field 19
    auto tag_and_val = p_data->message->at(18);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 19);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 9218868437227405311);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 9218868437227405311);
    EXPECT_DOUBLE_EQ(tag_and_val.value.value()->i64.value()->as_double, 1.7976931348623157e308);
  }

  { // field 20
    auto tag_and_val = p_data->message->at(19);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 20);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 9218868437227405313);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 9218868437227405313);
    EXPECT_TRUE(std::isnan(tag_and_val.value.value()->i64.value()->as_double));
    EXPECT_FALSE(std::signbit(tag_and_val.value.value()->i64.value()->as_double));
  }

  { // field 21
    auto tag_and_val = p_data->message->at(20);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 21);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 9221120237041090561);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 9221120237041090561);
    EXPECT_TRUE(std::isnan(tag_and_val.value.value()->i64.value()->as_double));
    EXPECT_FALSE(std::signbit(tag_and_val.value.value()->i64.value()->as_double));
  }

  { // field 22
    auto tag_and_val = p_data->message->at(21);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 22);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 9223372036854775807);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 9223372036854775807);
    EXPECT_TRUE(std::isnan(tag_and_val.value.value()->i64.value()->as_double));
    EXPECT_FALSE(std::signbit(tag_and_val.value.value()->i64.value()->as_double));
  }

  { // field 23
    auto tag_and_val = p_data->message->at(22);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 23);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 4599676419421066581);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 4599676419421066581);
    EXPECT_DOUBLE_EQ(tag_and_val.value.value()->i64.value()->as_double, 1.0 / 3.0);
  }

  { // field 24
    auto tag_and_val = p_data->message->at(23);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 24);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I64);
    ASSERT_FALSE(tag_and_val.value.value()->i64->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_unsigned, 4614256656748904448);
    EXPECT_EQ(tag_and_val.value.value()->i64.value()->as_twos_compliment, 4614256656748904448);
    EXPECT_DOUBLE_EQ(tag_and_val.value.value()->i64.value()->as_double, 3.14159274101257324);
  }

}

TEST_F(SpicyProtobufTest, TestGroups) {
  using namespace __hlt::protobuf;

  // protobuf_groups
  auto p_data = parseMessage(reinterpret_cast<const char *>(groups_protobuf_groups_binpb), groups_protobuf_groups_binpb_len, ParseFlag::NONE);
  auto p_msg = p_data->message;
  ASSERT_EQ(p_msg->size(), 6);

  { // Field 1
    auto tag_and_val = p_data->message->at(0);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 1);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
    EXPECT_FALSE(tag_and_val.value.value()->sgroup);
    EXPECT_FALSE(tag_and_val.value.value()->egroup);
    ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 1);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, -1);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, 1);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, 1);
    EXPECT_FALSE(tag_and_val.group.has_value());
  }

  int32_t group_num = -1;
  { // Field 2, SGROUP
    auto tag_and_val = p_data->message->at(1);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 2);
    group_num = tag_and_val.tag.value()->field_num;
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::SGROUP);
    EXPECT_TRUE(tag_and_val.value.value()->sgroup);
    EXPECT_FALSE(tag_and_val.value.value()->egroup);
    EXPECT_EQ(tag_and_val.group, group_num);
  }

  { // Field 3
    auto tag_and_val = p_data->message->at(2);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 3);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
    EXPECT_FALSE(tag_and_val.value.value()->sgroup);
    EXPECT_FALSE(tag_and_val.value.value()->egroup);
    ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 2);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, 1);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, 2);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, 2);
    EXPECT_EQ(tag_and_val.group, group_num);
  }

  { // Field 4
    auto tag_and_val = p_data->message->at(3);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 4);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::I32);
    EXPECT_FALSE(tag_and_val.value.value()->sgroup);
    EXPECT_FALSE(tag_and_val.value.value()->egroup);
    ASSERT_FALSE(tag_and_val.value.value()->i32->isNull());
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_unsigned, 1078530011);
    EXPECT_EQ(tag_and_val.value.value()->i32.value()->as_twos_compliment, 1078530011);
    EXPECT_FLOAT_EQ(tag_and_val.value.value()->i32.value()->as_float, 3.1415927);
    EXPECT_EQ(tag_and_val.group, group_num);
  }

  { // Field 2, EGROUP
    auto tag_and_val = p_data->message->at(4);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, group_num);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::EGROUP);
    EXPECT_FALSE(tag_and_val.value.value()->sgroup);
    EXPECT_TRUE(tag_and_val.value.value()->egroup);
    EXPECT_EQ(tag_and_val.group, group_num);
  }

  { // Field 5
    auto tag_and_val = p_data->message->at(5);
    EXPECT_EQ(tag_and_val.tag.value()->field_num, 5);
    EXPECT_EQ(tag_and_val.tag.value()->wire_type.value(), WireType::VARINT);
    EXPECT_FALSE(tag_and_val.value.value()->sgroup);
    EXPECT_FALSE(tag_and_val.value.value()->egroup);
    ASSERT_FALSE(tag_and_val.value.value()->varint->isNull());
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_unsigned, 3);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_zigzag, -2);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment64, 3);
    EXPECT_EQ(tag_and_val.value.value()->varint.value()->as_twos_compliment32, 3);
    EXPECT_FALSE(tag_and_val.group.has_value());
  }
}

} // anonymous namespace
