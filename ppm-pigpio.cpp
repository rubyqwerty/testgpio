#include <cstdint>
#include <iostream>
#include <pigpio.h>
#include <vector>

#define FRAME_LENGTH 22500 // 22.5 мс (PPM-кадр)

void sendPPM(uint32_t pin, std::vector<uint32_t> channels)
{
    gpioWaveClear(); // Очищаем буфер

    std::vector<gpioPulse_t> pulses;
    int frameStart = gpioTick();

    for (auto pulseWidth : channels)
    {
        gpioPulse_t high = {1u << pin, 0, pulseWidth};
        gpioPulse_t low = {0, 1u << pin, 400};
        pulses.push_back(high);
        pulses.push_back(low);
    }

    uint32_t remaining = FRAME_LENGTH - (gpioTick() - frameStart);
    if (remaining > 0)
    {
        gpioPulse_t sync = {0, 1u << pin, remaining};
        pulses.push_back(sync);
    }

    gpioWaveAddGeneric(pulses.size(), pulses.data());
    int wave_id = gpioWaveCreate();
    if (wave_id >= 0)
    {
        gpioWaveTxSend(wave_id, 1);
    }
}

int main(int argc, char **argv)
{
    const int pin = argc > 0 ? std::stoi(argv[1]) : 27;
    std::cout << "Используется PIN " << pin << std::endl;

    if (gpioInitialise() < 0)
    {
        std::cerr << "Ошибка pigpio!" << std::endl;
        return -1;
    }

    gpioSetMode(pin, PI_OUTPUT);
    std::vector<uint32_t> channels = {1000, 1500, 2000, 1700, 1300, 1800};

    while (true)
    {
        sendPPM(pin, channels);
        gpioDelay(FRAME_LENGTH);
    }

    gpioTerminate();
    return 0;
}
