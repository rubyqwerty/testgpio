#include <iostream>
#include <unistd.h>
#include <vector>
#include <wiringPi.h>

#define FRAME_LENGTH 22500 // Длина PPM-кадра (22.5 мс)

void sendPPM(std::vector<int> channels, int pin)
{
    int frameStart = micros();

    for (int pulseWidth : channels)
    {
        digitalWrite(pin, HIGH);
        delayMicroseconds(pulseWidth); // Длина импульса канала
        digitalWrite(pin, LOW);
        delayMicroseconds(400); // Интервал между импульсами
    }

    int remaining = FRAME_LENGTH - (micros() - frameStart);
    if (remaining > 0)
    {
        delayMicroseconds(remaining); // Синхронизация кадра
    }
}

int main(int argc, char **argv)
{
    const int pin = argc > 0 ? std::stoi(argv[1]) : 27;
    std::cout << "Используется PIN " << pin << std::endl;

    wiringPiSetup();
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);

    std::vector<int> channels = {1000, 1500, 2000, 1700, 1300, 1800};

    while (true)
    {
        sendPPM(channels, pin);
    }

    return 0;
}
