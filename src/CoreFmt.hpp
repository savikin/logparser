#pragma once

#include <cstdlib>
#include <format>
#include <optional>
#include <string>
#include <variant>

#include <Core.hpp>
#include <util/StringUtils.hpp>

template <class... Ts> struct overloads : Ts... {
  using Ts::operator()...;
};

namespace CoreFmt {
namespace internal {
static inline void init0(Core &core, const std::string &input) {
  auto value = StringUtils::atoi(input);
  if (!value.has_value()) {
    throw CoreLogFatalError{"Первая строка должна содержать количество столов",
                            core.lineno};
  }
  core.init0(value.value());
};

static inline auto init1(Core &core, const std::string &input) -> std::string {
  if (input.size() < 11) {
    throw CoreLogFatalError{"Вторая строка должна содержать время "
                            "начала и конца работы заведения",
                            core.lineno};
  }
  // 0123456789
  // hh:mm hh:mm
  auto start = Time::parse(input.c_str());
  auto end = Time::parse(input.c_str() + 6);
  if (!(start.has_value() && end.has_value())) {
    throw CoreLogFatalError{"Ошибка чтения времени", core.lineno};
  }
  core.init1(start.value(), end.value());
  return start->print();
}
static inline void init2(Core &core, const std::string &input) {
    auto value = StringUtils::atoi(input);
    if (!value.has_value()) {
      throw CoreLogFatalError{"Третья строка должна содержать стоимость "
                              "часа в компьютерном клубе",
                              core.lineno};
    }
    core.init2(value.value());
}
static inline auto process_working(Core &core, const std::string &input)
    -> std::optional<std::string> {
  // Preliminary stage
  auto tokens = StringUtils::stringsplit(input);
  if (tokens.size() < 3) {
    throw CoreLogFatalError{
        "Минимальное возможное событие - <время> <тип> <имя клиента>",
        core.lineno};
  }
  auto time = Time::parse(tokens[0]);
  if (!time.has_value()) {
    throw CoreLogFatalError{"Ошибка парсинга времени события", core.lineno};
  }
  auto type = StringUtils::atoi(tokens[1]);
  if (!type.has_value()) {
    throw CoreLogFatalError{"Ошибка парсинга типа события", core.lineno};
  }
  auto name = tokens[2];
  for (char chr : name) {
    switch (chr) {
    case 'a' ... 'z':
    case '0' ... '9':
    case '_':
      break;

    default:
      throw CoreLogFatalError{"Встречено невалидное имя", core.lineno};
    };
  }

  // Switch on event type
  switch (type.value()) {
  // Arrival
  case Core::InArrive::TypeID: {
    auto res =
        core.processArrive(Core::InArrive{.when = time.value(), .name = name});
    if (res.has_value()) {
      return std::format("{} 13 {}", tokens[0], res.value().toString());
    }
    return {};
  }

  // Attachment
  case Core::InAttach::TypeID: {
    if (tokens.size() < 4) {
      throw CoreLogFatalError{"Для данного типа требуется номер стола",
                              core.lineno};
    }
    auto tableno = StringUtils::atoi(tokens[3]);
    if (!tableno.has_value()) {
      throw CoreLogFatalError{"Ошибка парсинга номера стола", core.lineno};
    }

    auto res = core.processAttach(Core::InAttach{
        .when = time.value(), .name = name, .tableno = tableno.value()});
    if (res.has_value()) {
      return std::format("{} 13 {}", tokens[0], res.value().toString());
    }
    return {};
  }

  // Await
  case Core::InAwait::TypeID: {
    auto res =
        core.processAwait(Core::InAwait{.when = time.value(), .name = name});
    if (res.has_value()) {
      return std::visit(
          overloads{
              [&](const Core::OutError &event) {
                return std::format("{} 13 {}", tokens[0], event.toString());
              },
              [&](const Core::OutDepart &event) {
                return std::format("{} 11 {}", tokens[0], event.name);
              },
          },
          res.value());
    }
    return {};
  }

  // Departing
  case Core::InDepart::TypeID: {
    auto res =
        core.processDepart(Core::InDepart{.when = time.value(), .name = name});
    if (res.has_value()) {
      return std::visit(overloads{
                            [&](const Core::OutError &event) {
                              return std::format("{} 13 {}", tokens[0],
                                                 event.toString());
                            },
                            [&](const Core::OutAttach &event) {
                              return std::format("{} 12 {} {}", tokens[0],
                                                 event.name, event.table);
                            },
                        },
                        res.value());
    }
    return {};
  }

  default:
    throw CoreLogFatalError{"Неизвестный тип события", core.lineno};
  }
};
};

static inline auto process(Core &core, const std::string &input)
    -> std::optional<std::string> {
  switch (core._stage) {
  case Core::Stage::INIT0: {
    internal::init0(core, input);
    return {};
  }
  case Core::Stage::INIT1: {
    return internal::init1(core, input);
  }
  case Core::Stage::INIT2: {
    internal::init2(core, input);
    return {};
  }
  case Core::Stage::WORKING: {
    return internal::process_working(core, input);
  }
  default: {
    throw CoreLogFatalError{"Ошибка внутреннего состояния", core.lineno};
  }
  }
}

static inline auto close(Core &core) -> std::string {
  auto ret = core.close();
  std::string result{};

  // Departures
  for (const auto&client: ret.departures){ 
    result += std::format("{} 11 {}\n", client.time.print() , client.name);
  }
  // Close time
  result += ret.close_time.print() + "\n";

  // Table stats;
  for (const auto &table : ret.tables) {
    result += std::format("{} {} {}\n", table.number, table.income,
                          Time::from_minutes(table.minutes).print());
  }

  return result;
}
}; // namespace CoreFmt 
