#pragma once

#include <charconv>
#include <cstdint>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace StringUtils {

static inline auto stringsplit(const std::string &str) -> std::vector<std::string> {
  std::vector<std::string> tokens{};
  std::istringstream strstream{str};
  copy(std::istream_iterator<std::string>(strstream),
       std::istream_iterator<std::string>(), back_inserter(tokens));
  return tokens;
}
static inline auto atoi(const std::string &str) -> std::optional<int64_t> {
  int64_t value;
  const char* cstr = str.c_str();
  auto [p1, ec1] = std::from_chars(cstr, cstr + str.size(), value);
  if (ec1 != std::errc()) {
    return {};
  }
  return value;
}

}; // namespace StringUtils
