#include <cstring> //std::memcmp
#include <fstream> //std::ifstream
#include <iostream>
#include <string>
#include <vector>
#include <exception>

// Код домашнего задания
  // Распарсить блоб
  // На вход подается блоб - бинарный файл
  // Требуется:
  //  a. распаковать данные в структуру Blob
  // СТРУКТУРА БЛОБА:
  // version - 2 байта
  // size - 4 байта
  // data - 12 байт
  //  b. написать 3 exception класса
  //    a. обрабатывает значение version = валидные значения [1-7] // version_exception
  //    b. обрабатывает значение size = валидное значение 12 (0xC)
  //    c. обрабатывает значение data = валидное значение "Hello, World" (12 байт без нуль-терминатора)
  //  с. написать обработчик 3 exception классов
  //  d (*). модифицировать распаковку данных в структуру Blob с учетом того, что version и size могут быть big-endian

struct VersionException: std::exception
{
  const char * what() const throw()
  {
    return "Invalid version";
  }
};

void version_exception(std::vector<char> bytes, std::vector<int> ver){
  bool f = false;
  for (int v: ver){
    if (std::memcmp(bytes.data()+16, reinterpret_cast<char *>(&v), 2)){
      f = true;
    }
  }
  if (f){
    std::cout<<"Version is OK\n";
  }
  else {
      throw VersionException();
    }
}

struct SizeException: std::exception
{
  const char * what() const throw()
  {
    return "Invalid size";
  }
};

void size_exception(std::vector<char> bytes, std::uint32_t num){
    if (std::memcmp(bytes.data()+12, reinterpret_cast<char *>(&num), 4)){
      std::cout<<"Size is OK\n";
    }
    else {
      throw SizeException();
    }
}

struct DataException: std::exception
{
  const char * what() const throw()
  {
    return "Invalid data";
  }
};

void data_exception(std::vector<char> bytes, const char* text_data){
    if (std::memcmp(bytes.data(), text_data, 12)){
      std::cout<<"Data is OK\n";
    }
    else {
      throw DataException();
    }
}


void do_fs(const std::string &filename) {
  std::ifstream reader(filename, std::ios::in | std::ios::binary);
  if (reader.is_open()) {
    // read bytes from file to buffer
    std::vector<char> bytes;
    int data_sz = 18;
    bytes.resize(data_sz);
    reader.read(reinterpret_cast<char *>(bytes.data()), data_sz);
    // Place your code here ...
    // Compare version
    version_exception(bytes, std::vector<int>{1, 2, 3, 4, 5, 6, 7});

    // Compare size
    size_exception(bytes, 0xC);

    // Compare data
    data_exception(bytes, "Hello, World");
  }
  else {
    throw std::runtime_error("file does not exist");
  }
}

int main()
{
  try
  {
    do_fs("bin_data_hw03_le.bin");
  }
  catch(const VersionException& ve)
  {
    std::cerr << ve.what() << "\n";
  }
  catch(const SizeException& se)
  {
    std::cerr << se.what() << "\n";
  }
  catch(const DataException& de)
  {
    std::cerr << de.what() << "\n";
  }
  catch(const std::runtime_error& exc)
  {
    std::cerr << exc.what() << "\n";
  }

  return 0;
}
