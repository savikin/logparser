#pragma once

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>

#include <Time.hpp>
#include <util/ScopeIncrementor.hpp>

struct CoreLogFatalError : public std::runtime_error {
  std::string line;
  int64_t lineno;

  CoreLogFatalError(const std::string &message, int64_t lineno)
      : std::runtime_error(message), lineno(lineno) {}
};

struct Core {
  struct InArrive {
    static constexpr auto TypeID = 1;
    Time when;
    std::string name;
  };
  struct InAttach {
    static constexpr auto TypeID = 2;
    Time when;
    std::string name;
    int64_t tableno;
  };
  struct InAwait {
    static constexpr auto TypeID = 3;
    Time when;
    std::string name;
  };
  struct InDepart {
    static constexpr auto TypeID = 4;
    Time when;
    std::string name;
  };
  struct OutDepart {
    static constexpr auto TypeID = 11;
    std::string name;
    Time time;
  };
  struct OutAttach {
    static constexpr auto TypeID = 12;
    int64_t table;
    std::string name;
    Time time;
  };
  struct OutError {
    static constexpr auto TypeID = 13;
    enum struct ErrorType {
      YOUSHALLNOTPASS,
      NOTOPENYET,
      PLACEISBUSY,
      CLIENTUNKNOWN,
      NOSUCHTABLE,
      ICANWAITNOLONGER,
    } error;

    [[nodiscard]] auto toString() const -> std::string {
      switch (error) {
      case ErrorType::YOUSHALLNOTPASS:
        return "YouShallNotPass";
      case ErrorType::NOTOPENYET:
        return "NotOpenYet";
      case ErrorType::PLACEISBUSY:
        return "PlaceIsBusy";
      case ErrorType::CLIENTUNKNOWN:
        return "ClientUnknown";
      case ErrorType::NOSUCHTABLE:
        return "NoSuchTable";
      case ErrorType::ICANWAITNOLONGER:
        return "ICanWaitNoLonger!";
      }
    }
  };
  // NOTE: usage code of this struct relies on default 
  // construction of optional being empty
  struct TableStats {
    struct Customer{
      std::string name;
      Time start_time;
    };
    std::optional<Customer> customer;
    int64_t income;
    int64_t minutes;

    void account(Time end, int64_t hourly_price) {
      const auto timespent = end - this->customer->start_time;
      const auto hours =
          (timespent % 60 == 0) ? timespent / 60 : (timespent / 60) + 1;
      this->minutes += timespent;
      this->income += hours * hourly_price;
      this->customer.reset();
    }
  };
  enum struct Stage {
    INIT0, // Zero initialized, expect tables
    INIT1, // Expect start and end time
    INIT2, // Expect pricing info
    WORKING
  };

  Stage _stage;
  int64_t tables;
  int64_t tables_taken;
  Time start;
  Time end;
  int64_t hourly_price;
  int64_t lineno;

  // Maps tables to their stats and customers
  std::unordered_map<int64_t, TableStats> data_table;
  // Maps customers to their tables
  std::unordered_map<std::string, std::optional<int64_t>> data_customer;

  // Queue
  int64_t queue_index;
  int64_t queue_end_index;
  std::unordered_map<int64_t, std::string> queue;
  std::unordered_map<std::string, int64_t> queue_position;

  void init0(int64_t number_of_tables) {
    ScopeIncrement scope(lineno);

    if (number_of_tables <= 0) {
      throw CoreLogFatalError{"Количество столов должно быть положительным числом", lineno};
    }

    this->tables = number_of_tables;
    _stage = Stage::INIT1;
  }
  void init1(Time start, Time end) {
    ScopeIncrement scope(lineno);

    this->start = start;
    this->end = end;
    _stage = Stage::INIT2;
  }
  void init2(int64_t hourly_price) {
    ScopeIncrement scope(lineno);

    if (hourly_price <= 0) {
      throw CoreLogFatalError{"Почасовая цена должна быть положительным числом",
                              lineno};
    }

    this->hourly_price = hourly_price;
    _stage = Stage::WORKING;
  }

  [[nodiscard]] auto processArrive(InArrive event) -> std::optional<OutError> {
    ScopeIncrement scope(lineno);

    if (event.when < this->start) {
      return OutError{Core::OutError::ErrorType::NOTOPENYET};
    }

    if (data_customer.contains(event.name)) {
      return OutError{Core::OutError::ErrorType::YOUSHALLNOTPASS};
    }

    data_customer.insert({{event.name}, {}});
    queue.insert({queue_end_index, event.name});
    queue_position.insert({event.name, {queue_end_index}});
    ++queue_end_index;
    return {};
  }
  [[nodiscard]] auto processAttach(InAttach event) -> std::optional<OutError> {
    ScopeIncrement scope(lineno);

    if (!data_customer.contains(event.name)) {
      return OutError{Core::OutError::ErrorType::CLIENTUNKNOWN};
    }

    if (event.tableno >= tables) {
      return OutError{Core::OutError::ErrorType::NOSUCHTABLE};
    }

    // NOTE: relies on default construction of optional being empty
    if (data_table[event.tableno].customer.has_value()) {
      return OutError{Core::OutError::ErrorType::PLACEISBUSY};
    }

    // Accounting on previous table, if applicable
    if (data_customer[event.name].has_value()) {
      const auto tableno = data_customer[event.name].value();
      data_table[tableno].account(event.when, hourly_price);
    } else {
      // Taking previously untaken table
      ++tables_taken;
    };

    // Set new table
    data_customer[event.name] = event.tableno;
    // Record client info in the TableStats
    auto &tablestat = data_table[event.tableno];
    tablestat.customer =
        TableStats::Customer{.name = event.name, .start_time = event.when};

    // Remove client from a queue
    if (queue_position.contains(event.name)) {
      auto position = queue_position[event.name];
      queue_position.erase(event.name);
      queue.erase(position);
    }
    return {};
  }
  [[nodiscard]] auto processAwait(InAwait event)
      -> std::optional<std::variant<OutError, OutDepart>> {
    ScopeIncrement scope(lineno);

    if (!data_customer.contains(event.name)) {
      return OutError{Core::OutError::ErrorType::CLIENTUNKNOWN};
    }

    if (tables_taken < tables) {
      return OutError{Core::OutError::ErrorType::ICANWAITNOLONGER};
    }

    if (queue_position.contains(event.name) && (int64_t)queue.size() > tables) {
      auto pos = queue_position[event.name];
      queue_position.erase(event.name);
      queue.erase(pos);
      return OutDepart{.name = event.name, .time = event.when};
    }

    return {};
  }
  [[nodiscard]] auto processDepart(InDepart event)
      -> std::optional<std::variant<OutError, OutAttach>> {
    ScopeIncrement scope(lineno);

    if (!data_customer.contains(event.name)) {
      return OutError{Core::OutError::ErrorType::CLIENTUNKNOWN};
    }

    auto table = data_customer[event.name];
    if (!table.has_value()) {
      data_customer.erase(event.name);
      return {};
    }

    data_table[table.value()].account(event.when, hourly_price);

    while (queue_index < queue_end_index) {
      if (queue.contains(queue_index)) {
        auto name = queue[queue_index];

        // Set new table
        data_customer[name] = table;
        // Record client info in the TableStats
        auto &tablestat = data_table[table.value()];
        tablestat.customer =
            TableStats::Customer{.name = event.name, .start_time = event.when};

        // Remove client from a queue
        if (queue_position.contains(event.name)) {
          auto position = queue_position[event.name];
          queue_position.erase(event.name);
          queue.erase(position);
        }

        return OutAttach{
            .table = table.value(), .name = name, .time = event.when};
      }

      ++queue_index;
    }

    --tables_taken;
    return {};
  }
};
