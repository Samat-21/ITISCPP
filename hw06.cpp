// ДЗ - исследование процесса записи данных в файл в синхронном и многопоточном режимах
// 1. Реализовать функцию записи данных в файл в синхронном режиме --> 79 sec
// 2. Реализовать функцию записи данных в файл в многопоточном режиме --> 179 sec
// 3. Измерить время выполнения шага 1 и шага 2
// Примечание:
// 1. Использовать все доступные потоки (кол-во доступных получать через API)
// 2. Размер файла 5ГБ
// 3. Данные записываются блочно, размер блока данных = 0x1000 (4096)
// Вопросы:
// 1. Какая из функций выполнилась быстрее? Почему?
// 2. Как проводилось измерение? Какой API был использован и почему?
// 3. Как изменится время выполнения, если увеличить размер файла (например, 10ГБ)?
// 4. Как изменится время выполнения, если использовать в 2 раза меньше потоков? В 2 раза больше?

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <functional>
#include <ctime>
#include <mutex>

std::mutex mu;

// Функция для записи данных в файл в многопоточном режиме
void write_to_file(const std::string& filename, const int& data, int thread_id)
{
    // mu.lock();
    std::ofstream file(filename, std::ios::binary | std::ios::app);
    unsigned long long iter_num = 167772160; // 5 * 1024 * 1024 * 1024 / 4 / 8;
    for (long long j = 0; j < iter_num; j++){
        file << data;
    }
    file.close();
    // mu.unlock();
}

void static_write_to_file(const std::string& filename, const int& data)
{
    unsigned long long n = 1342177280; // 5 * 1024 * 1024 * 1024 / 4 
    std::ofstream file(filename, std::ios::binary);
    for (unsigned long long i = 0; i < n; i++){
        file << data;
    }
    file.close();
}

int main()
{
    unsigned int start_time =  clock(); // начальное время


    const int num_threads = std::thread::hardware_concurrency();
    std::cout << num_threads << std::endl;
    std::vector<std::thread> threads;
    int block = 0x1000;
    unsigned long long iter_num = 167772160; // 5 * 1024 * 1024 * 1024 / 4 / 8;

    // static write
    //static_write_to_file("output1.bin", block);
    unsigned int end_time1 = clock();
    unsigned int search_time1 = end_time1 - start_time; // искомое время
    std::cout << search_time1 / CLOCKS_PER_SEC << std::endl;




    // Создаем потоки для записи данных в файл
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(write_to_file, "output.bin", block, i);
    }

    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    unsigned int end_time = clock(); // конечное время
    unsigned int search_time = end_time - start_time; // искомое время
    std::cout << search_time / CLOCKS_PER_SEC << std::endl;

    std::cout << "Data has been written to file." << std::endl;

    return 0;
}