#include <gtest/gtest.h>

#include <util/StringUtils.hpp>
#include <Time.hpp>

TEST(Time, TimeParsingFailures) {
  ASSERT_FALSE(Time::parse("0:0").has_value());
  ASSERT_FALSE(Time::parse("00:0").has_value());
  ASSERT_FALSE(Time::parse("0:00").has_value());
  ASSERT_FALSE(Time::parse("").has_value());
  ASSERT_FALSE(Time::parse("abcdf").has_value());
  ASSERT_FALSE(Time::parse("aa:aa").has_value());

  ASSERT_FALSE(Time::parse("24:00").has_value());
  ASSERT_FALSE(Time::parse("00:60").has_value());
  ASSERT_FALSE(Time::parse("24:60").has_value());
}

TEST(Time, TimeParsing) {
  auto time1 = Time::parse("00:00");
  ASSERT_TRUE(time1.has_value());
  ASSERT_EQ(time1->hrs, 0);
  ASSERT_EQ(time1->min, 0);

  auto time2 = Time::parse("17:17");
  ASSERT_TRUE(time2.has_value());
  ASSERT_EQ(time2->hrs, 17);
  ASSERT_EQ(time2->min, 17);

  auto time3 = Time::parse("23:59");
  ASSERT_TRUE(time3.has_value());
  ASSERT_EQ(time3->hrs, 23);
  ASSERT_EQ(time3->min, 59);
}
TEST(Time, TimeComparisons) {
  ASSERT_LT(Time(10, 20), Time(13, 20));
  ASSERT_GT(Time(13, 20), Time(10, 20));

  ASSERT_LT(Time(14, 20) , Time(14, 21));
  ASSERT_GT(Time(14, 20) , Time(14, 19));

  ASSERT_EQ(Time(14, 20), Time(14, 20));
  ASSERT_NE(Time(14, 21), Time(14, 20));
  ASSERT_NE(Time(14, 20), Time(15, 20));
}

TEST(StringUtils, Atoi) {
  ASSERT_FALSE(StringUtils::atoi("").has_value());

  auto one = StringUtils::atoi("1");
  ASSERT_TRUE(one.has_value());
  ASSERT_TRUE(one.value() == 1);

  auto zero = StringUtils::atoi("0");
  ASSERT_TRUE(zero.has_value());
  ASSERT_TRUE(zero.value() == 0);

  auto neg137 = StringUtils::atoi("-137");
  ASSERT_TRUE(neg137.has_value());
  ASSERT_TRUE(neg137.value() == -137);
}

TEST(StringUtils, Split) {
  auto vec1 = StringUtils::stringsplit("");
  ASSERT_TRUE(vec1.size() == 0);

  auto vec2 = StringUtils::stringsplit("1");
  ASSERT_TRUE(vec2.size() == 1);

  auto vec3 = StringUtils::stringsplit("1 1");
  ASSERT_TRUE(vec3.size() == 2);
  ASSERT_TRUE(vec3[0] == "1");
  ASSERT_TRUE(vec3[1] == "1");
}
