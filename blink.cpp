#include <iostream>
#include <chrono>
#include <thread>
#include <gpiod.h>

#define GPIO_CHIP "/dev/gpiochip4"  // На Raspberry Pi 5 основные GPIO находятся в gpiochip4

int main(int argc, char** argv) {

    const int pin = argc > 0 ? std::stoi(argv[1]) : 17;

    // Открываем чип
    gpiod_chip *chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) {
        std::cerr << "Ошибка: не удалось открыть " << GPIO_CHIP << std::endl;
        return 1;
    }

    // Получаем линию (пин)
    gpiod_line *line = gpiod_chip_get_line(chip, pin);
    if (!line) {
        std::cerr << "Ошибка: не удалось получить GPIO " << pin << std::endl;
        gpiod_chip_close(chip);
        return 1;
    }

    // Запрашиваем пин как выход
    if (gpiod_line_request_output(line, "blink", 0) < 0) {
        std::cerr << "Ошибка: не удалось запросить GPIO как выход" << std::endl;
        gpiod_chip_close(chip);
        return 1;
    }

    // Мигание светодиодом
    for (int i = 0; i < 10; i++) {  // Мигаем 10 раз
        gpiod_line_set_value(line, 1);  // Включаем
        std::cout << "LED ON" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        gpiod_line_set_value(line, 0);  // Выключаем
        std::cout << "LED OFF" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Освобождаем GPIO
    gpiod_line_release(line);
    gpiod_chip_close(chip);

    return 0;
}
