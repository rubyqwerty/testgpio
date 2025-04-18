#include "crossfire.h"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <vector>

#define UART_PORT "/dev/ttyAMA0"
#define TIMEOUT 200

int uart_init()
{
    int fd = open(UART_PORT, O_RDWR | O_NOCTTY);
    if (fd == -1)
    {
        std::cerr << "Ошибка открытия порта " << UART_PORT << std::endl;
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;
    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

void uart_send(int fd, uint8_t *data, size_t length) { write(fd, data, length); }

static std::vector<int16_t> channels{
    -0x400, // 1 can
    -0x400, // 2 can
    -0x400, // 3 can
    -0x400, // 4 can
    -0x400, // 5 can ARM
    -0x400, // 6 can
    -0x400, // 7 can
    -0x400, // 8 can
    -0x400, // 9 can
    -0x400, // 10 can
    -0x400, // 11 can
    -0x400, // 12 can
    -0x400, // 13 can
    -0x400, // 14 can
    -0x400, // 15 can
    -0x400, // 16 can
};

std::string bytesToHex(const uint8_t *data, size_t length, bool withSpaces = true)
{
    std::ostringstream oss;
    for (size_t i = 0; i < length; ++i)
    {
        oss << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(data[i]);
        if (withSpaces && i < length - 1)
            oss << ' ';
    }
    return oss.str();
}

void SendControllFrame(int fd)
{
    auto frame = new uint8_t[256];

    const auto frame_size = createCrossfireChannelsFrame(0, frame, channels.data());

    //const auto line = bytesToHex(frame, frame_size);

    //std::cout << "Отправил данные: " << line << std::endl;

    uart_send(fd, frame, frame_size);

    delete[] frame;
}


int main()
{
    int fd = uart_init();
    if (fd == -1)
        return EXIT_FAILURE;

    
            while (1)
            {
                SendControllFrame(fd);

                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
      
    close(fd); // Закрываем UART
    return EXIT_SUCCESS;
}