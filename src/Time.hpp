#pragma once

#include <charconv>
#include <cstdint>
#include <optional>

struct Time {
  int8_t h;
  int8_t m;

  static std::optional<Time> parse(const char *p) {
    // isdigit from libc depends on locale (at least on msvc), hence this
    static constexpr auto digit = [](char c) -> bool {
      return '0' <= c && c <= '9';
    };
    if (!(digit(p[0])) || !(digit(p[1]))) {
      return std::nullopt;
    }
    if (p[2] != ':') {
      return std::nullopt;
    }
    if (!(digit(p[3])) || !(digit(p[4]))) {
      return std::nullopt;
    }

    int8_t h;
    int8_t m;

    // 01234
    // hh:mm
    auto [p1, ec1] = std::from_chars(p + 0, p + 1, h);
    auto [p2, ec2] = std::from_chars(p + 3, p + 4, h);
    if (ec1 != std::errc() || ec2 != std::errc()) {
      return std::nullopt;
    }

    return Time{h, m};
  }
};
