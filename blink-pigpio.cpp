#include <iostream>
#include <pigpio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    const int pin = argc > 0 ? std::stoi(argv[1]) : 27;
    std::cout << "Используется PIN " << pin << std::endl;

    if (gpioInitialise() < 0)
    {
        std::cerr << "Ошибка инициализации pigpio!\n";
        return 1;
    }

    gpioSetMode(pin, PI_OUTPUT);

    for (int i = 0; i < 10; i++)
    {
        gpioWrite(pin, 1);
        usleep(500000);
        gpioWrite(pin, 0);
        usleep(500000);
    }

    gpioTerminate();
    return 0;
}
