#include <fstream>
#include <iostream>
#include <string>

#include <Core.hpp>
#include <CoreFmt.hpp>


static void logparse(char* filename){
  Core core{};

  std::ifstream file(filename);
  std::string line;

  // init0
  std::getline(file, line);
  CoreFmt::process(core, line);

  // init1
  std::getline(file, line);
  auto start_time = CoreFmt::process(core, line);
  std::cout << start_time.value() << "\n";

  // init2
  std::getline(file, line);
  CoreFmt::process(core, line);

  while (std::getline(file, line)){
    auto res = CoreFmt::process(core, line);
    std::cout << line << "\n";
    if (res.has_value()){
      std::cout << res.value() << "\n";
    }
  }

  std::cout << CoreFmt::close(core) << "\n";
}

auto main(int argc, char **argv) -> int {
  try {
    if (argc != 2) {
      std::cout << "Неверный синтаксис вызова программы\n"
                   "Пример вызова: logparser log.txt\n";
      return 1;
    }

    logparse(argv[1]);

  } catch (const CoreLogFatalError &e) {
    std::cout << "Ошибка формата входного файла\n"
              << e.what()
              << "\n"
                 "Линия "
              << e.lineno << ": " << e.line.value_or("") << '\n';
  }
}
