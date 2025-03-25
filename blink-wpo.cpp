#include <iostream>
#include <unistd.h> // Для usleep()
#include <wiringPi.h>

int main(int argc, char **argv)
{
    const int pin = argc > 0 ? std::stoi(argv[1]) : 27;
    std::cout << "Используется PIN " << pin << std::endl;

    // Инициализация WiringOP
    if (wiringPiSetup() == -1)
    {
        std::cerr << "Ошибка инициализации WiringOP!" << std::endl;
        return 1;
    }

    pinMode(pin, OUTPUT); // Устанавливаем пин как выход
    std::cout << "Моргание светодиодом. Нажмите Ctrl+C для выхода." << std::endl;

    while (true)
    {
        digitalWrite(pin, HIGH); // Включаем светодиод
        usleep(500000);          // Ждём 500 мс (0.5 секунды)

        digitalWrite(pin, LOW); // Выключаем светодиод
        usleep(500000);         // Ждём 500 мс (0.5 секунды)
    }

    return 0;
}
