#pragma once

#include <cstdlib>
#include <optional>
#include <string>
#include <variant>

#include <Core.hpp>
#include <util/StringUtils.hpp>

template <class... Ts> struct overloads : Ts... {
  using Ts::operator()...;
};

namespace CoreFmt {
static inline auto process(Core &core, const std::string &input)
    -> std::optional<std::string> {
  switch (core._stage) {
  case Core::Stage::INIT0: {
    auto value = StringUtils::atoi(input);
    if (!value.has_value()) {
      throw CoreLogFatalError{
          "Первая строка должна содержать количество столов", core.lineno};
    }
    core.init0(value.value());
  }
    return {};
  case Core::Stage::INIT1: {
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
  }
    return {};
  case Core::Stage::INIT2: {
    auto value = StringUtils::atoi(input);
    if (!value.has_value()) {
      throw CoreLogFatalError{"Третья строка должна содержать стоимость "
                              "часа в компьютерном клубе",
                              core.lineno};
    }
    core.init2(value.value());
    return {};
  }
  case Core::Stage::WORKING: {
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

    // Switch on event type
    switch (type.value()) {
    // Arrival
    case Core::InArrive::TypeID: {
      auto res = core.processArrive(
          Core::InArrive{.when = time.value(), .name = name});
      if (res.has_value()) {
        return tokens[0] + " 13 " + res.value().toString();
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
        return tokens[0] + " 13 " + res.value().toString();
      }
      return {};
    }

    // Await
    case Core::InAwait::TypeID: {
      auto res =
          core.processAwait(Core::InAwait{.when = time.value(), .name = name});
      if (res.has_value()) {
        return std::visit(overloads{
                              [&](const Core::OutError &event) {
                                return tokens[0] + " 13 " + event.toString();
                              },
                              [&](const Core::OutDepart &event) {
                                return tokens[0] + " 11 " + event.name;
                              },
                          },
                          res.value());
      }
      return {};
    }

    // Departing
    case Core::InDepart::TypeID: {
      auto res = core.processDepart(
          Core::InDepart{.when = time.value(), .name = name});
      if (res.has_value()) {
        return std::visit(overloads{
                              [&](const Core::OutError &event) {
                                return tokens[0] + " 13 " + event.toString();
                              },
                              [&](const Core::OutAttach &event) {
                                return tokens[0] + " 12 " + event.name +
                                       std::to_string(event.table);
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
  }
}
}; // namespace CoreFmt
