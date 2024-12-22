/*
Вариант 4

* Часть 2
  Для своего варианта обеспечить синхронизацию потоков:
  1.  Обеспечить печать имени потока и значения в одну строку без возможных
  разрывов, продемонстрировать два варианта реализации: использование mutex
  и использование блокировки.
  2.  С помощью условной переменной обеспечить, чтобы главный поток дожидался
  завершения дочерних потоков (дочерние потоки перед завершением оповещают
  главный поток, главный поток принимает эти оповещения). Главный поток после
  приема оповещения от каждого дочернего потока печатает об этом событии
  сообщение.
*/

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

std::mutex mtx;
std::condition_variable cv;
bool firstHalfSorted = false;
bool secondHalfSorted = false;

void bubbleSortWithMutex(std::vector<double> &arr,
                         const std::string &threadName, int pre, int pos) {
  for (size_t i = pre; i < pos - 1; ++i) {
    for (size_t j = pre; j < pos - i - 1; ++j) {
      if (arr[j] > arr[j + 1]) {
        std::swap(arr[j], arr[j + 1]);
      }
    }

    {
      std::unique_lock<std::mutex> lock(mtx);
      std::cout << threadName << ": ";
      for (const auto &num : arr) {
        std::cout << num << " ";
      }
      std::cout << std::endl;
    }
  }
}

// Сортировка первой половины массива
void sortFirstHalf(std::vector<double> &arr) {
  bubbleSortWithMutex(arr, "Thread 1", 0, arr.size() / 2);

  mtx.lock(); // Захватываем мьютекс
  firstHalfSorted = true;
  mtx.unlock(); // Освобождаем мьютекс

  cv.notify_one();
}

// Сортировка второй половины массива
void sortSecondHalf(std::vector<double> &arr) {
  bubbleSortWithMutex(arr, "Thread 2", arr.size() / 2, arr.size());

  mtx.lock(); // Захватываем мьютекс
  secondHalfSorted = true;
  mtx.unlock(); // Освобождаем мьютекс

  cv.notify_one();
}

// Полная сортировка массива после завершения первых двух потоков
void sortFullArray(std::vector<double> &arr) {

  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [] { return firstHalfSorted && secondHalfSorted; });

  lock.unlock(); // Разблокируем, чтобы дать возможность захватить снова в
                 // bubbleSortWithMutex

  bubbleSortWithMutex(arr, "Thread 3", 0, arr.size());
}

int main() {
  const int size = 10;
  std::vector<double> arr(size);

  // Заполняем массив случайными числами
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dis(1.0, 100.0);
  for (int i = 0; i < size; ++i) {
    arr[i] = dis(gen);
  }

  // Создание и запуск потоков
  std::thread t1(sortFirstHalf, std::ref(arr));
  std::thread t2(sortSecondHalf, std::ref(arr));
  std::thread t3(sortFullArray, std::ref(arr));

  t1.join();
  t2.join();
  t3.join();

  std::cout << "Главный поток: Работа завершена." << std::endl;

  return 0;
}
