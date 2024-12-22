#pragma once

#include <charconv>
#include <cstdint>
#include <optional>
#include <string>

/* Works with the hh:mm time format */
struct Time {
  int8_t hrs;
  int8_t min;

  auto operator<=>(const Time &) const = default;
  friend auto operator-(Time lhs, Time rhs) -> int64_t {
    return (lhs.hrs * 60LL + lhs.min) - (rhs.hrs * 60LL + rhs.min);
  }

  [[nodiscard]] auto print() const -> std::string {
    char buf[5]{'0', '0', ':', '0', '0'};
    // hh:mm
    // 01234
    if (hrs < 10) {
      std::to_chars(&buf[1], &buf[1] + 1, hrs);
    } else {
      std::to_chars(&buf[0], &buf[1] + 1, hrs);
    }
    if (min < 10) {
      std::to_chars(&buf[4], &buf[4] + 1, min);
    } else {
      std::to_chars(&buf[3], &buf[4] + 1, min);
    }

    return {buf, sizeof(buf)};
  };

  static auto parse(const std::string &str) -> std::optional<Time> {
    return parse(str.c_str());
  }
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
    auto [p1, ec1] = std::from_chars(ptr + 0, ptr + 1 + 1, hrs);
    auto [p2, ec2] = std::from_chars(ptr + 3, ptr + 4 + 1, min);
    if (ec1 != std::errc() || ec2 != std::errc()) {
      return std::nullopt;
    }
    if (!(0 <= hrs && hrs <= 23)) { // NOLINT (it's more readable this way)
      return std::nullopt;
    }
    if (!(0 <= min && min <= 59)) { // NOLINT (it's more readable this way)
      return std::nullopt;
    }

    return Time{.hrs = hrs, .min = min};
  }
};
