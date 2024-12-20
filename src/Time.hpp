#pragma once

#include <charconv>
#include <cstdint>
#include <optional>

/* Works with the hh:mm time format */
struct Time {
  int8_t hrs;
  int8_t min;

  static auto parse(const char *ptr) -> std::optional<Time> {
    // isdigit from libc can depend on locale (at least on msvc), hence this
    // (it shouldn't but it can)
    static constexpr auto digit = [](char chr) -> bool {
      return '0' <= chr && chr <= '9';
    };
    if (!(digit(ptr[0])) || !(digit(ptr[1]))) {
      return std::nullopt;
    }
    if (ptr[2] != ':') {
      return std::nullopt;
    }
    if (!(digit(ptr[3])) || !(digit(ptr[4]))) {
      return std::nullopt;
    }

    int8_t hrs;
    int8_t min;

    // 01234
    // hh:mm
    auto [p1, ec1] = std::from_chars(ptr + 0, ptr + 1, hrs);
    auto [p2, ec2] = std::from_chars(ptr + 3, ptr + 4, hrs);
    if (ec1 != std::errc() || ec2 != std::errc()) {
      return std::nullopt;
    }

    return Time{.hrs = hrs, .min = min};
  }
};
