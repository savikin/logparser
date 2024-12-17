#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <Core.hpp>
#include <string>

struct LogError : public std::runtime_error {
  int64_t lineno;
};

static void logparse(char* filename){
  Core core{};


  std::ifstream f(filename);
  std::string line;
  while (std::getline(f, line)){
    core.process(line);
  }
}

int main(int argc, char **argv) {
  try {
    if (argc != 2) {
      std::cout << "Неверный синтаксис вызова программы\n"
                   "Пример вызова: logparser log.txt\n";
      return 1;
    }
  } catch (const LogError &e) {
    std::cout << "Ошибка формата входного файла, линия " << e.lineno << '\n';
  }
}
