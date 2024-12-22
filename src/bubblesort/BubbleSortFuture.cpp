/*
Вариант 4

* Часть 1
  Реализовать программу, в которой кроме главного создается три
  отдельных потока:
  - первый поток сортирует первую половину вещественного массива,
  - второй поток сортирует вторую половину вещественного массива,
  - третий поток запускается после завершение первых двух, он
    сортирует массив полностью (уже частично отсортированный).

  Каждый поток имеет свое имя (например, thread1, thread2, thread3),
  и печатает отсортированный массив, перед печатью каждого значения
  элемента массива поток должен напечатать с новой строки свое имя.

  После завершения дочерних потоков главный поток выдает сообщение
  об окончании работы. Имена потоков и массив передаются в потоковую
  функцию через ее параметры, использовать одну потоковую функцию
  для всех трех потоков. Массив предварительно до запуска потоков
  заполняются числами с использованием ГПСЧ. Для сортировки
  использовать метод прямого обмена (метод «пузырька»).

  1.  Запустить программу несколько раз при одних и тех же исходных
  данных, посмотреть, как меняются результаты вывода. Сделать выводы.
  2.	Между печатью имени потока и значением установить небольшую
  задержку, например, 10 мс. Посмотреть, как меняются результаты вывода.
  Сделать выводы.
*/

#include <algorithm>
#include <future>
#include <iostream>
#include <random>
#include <vector>

void bubbleSort(std::vector<double> &arr, const std::string &threadName,
                int pre, int pos) {
  for (size_t i = pre; i < pos - 1; ++i) {
    for (size_t j = pre; j < pos - i - 1; ++j) {
      if (arr[j] > arr[j + 1]) {
        std::swap(arr[j], arr[j + 1]);
      }
    }
    std::cout << threadName << ": ";
    for (const auto &num : arr) {
      std::cout << num << " ";
    }
    std::cout << std::endl;
  }
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

  // Используем std::async для запуска асинхронных задач
  auto future1 = std::async(std::launch::async, bubbleSort, std::ref(arr),
                            "Thread 1", 0, arr.size() / 2);
  auto future2 = std::async(std::launch::async, bubbleSort, std::ref(arr),
                            "Thread 2", arr.size() / 2, arr.size());

  // Ждем завершения первых двух задач
  future1.get();
  future2.get();

  auto future3 = std::async(std::launch::async, bubbleSort, std::ref(arr),
                            "Thread 3", 0, arr.size());
  future3.get();

  std::cout << "Главный поток: Работа завершена." << std::endl;

  return 0;
}
