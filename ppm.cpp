#include <iostream>
#include <gpiod.h>
#include <chrono>
#include <vector>
#include <thread>

#define GPIO_CHIP "/dev/gpiochip0"

void precise_sleep(long microseconds) {
    struct timespec ts;
    ts.tv_sec = microseconds / 1000000;
    ts.tv_nsec = (microseconds % 1000000) * 1000;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, nullptr);
}

void generatePPM(gpiod_line *line, const std::vector<int> &channels, int frame_length = 20000) {
    int sync_pulse = frame_length;
    for (int ch : channels) {
        sync_pulse -= (ch + 400);
    }

    gpiod_line_set_value(line, 1);
    precise_sleep(300);
    gpiod_line_set_value(line, 0);
    precise_sleep(sync_pulse);

    for (int ch : channels) {
        gpiod_line_set_value(line, 1);
        precise_sleep(400);
        gpiod_line_set_value(line, 0);
        precise_sleep(ch);
    }
}

int main() {
    const auto pin = argc > 1 ? std::stoi(argv[1]) : 21;

    gpiod_chip *chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) {
        std::cerr << "Ошибка: не удалось открыть " << GPIO_CHIP << std::endl;
        return 1;
    }

    gpiod_line *line = gpiod_chip_get_line(chip, pin);
    if (!line) {
        std::cerr << "Ошибка: не удалось получить GPIO " << pin << std::endl;
        gpiod_chip_close(chip);
        return 1;
    }

    if (gpiod_line_request_output(line, "ppm_output", 0) < 0) {
        std::cerr << "Ошибка: не удалось настроить GPIO как выход" << std::endl;
        gpiod_chip_close(chip);
        return 1;
    }

    std::vector<int> channels = {1500, 1200, 1700, 1300, 1800, 1600};

    std::cout << "Генерация PPM-сигнала..." << std::endl;

    while (true) {
        generatePPM(line, channels);
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);
    return 0;
}
