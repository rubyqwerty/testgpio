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
#define TIMEOUT 2

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
    cfsetispeed(&options, __MAX_BAUD);
    cfsetospeed(&options, __MAX_BAUD);
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

int uart_receive(int fd, uint8_t *buffer, size_t length)
{
    int bytes_read = 0;
    fd_set read_fds;
    struct timeval timeout;

    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;

    int ret = select(fd + 1, &read_fds, NULL, NULL, &timeout);
    if (ret > 0)
    {
        bytes_read = read(fd, buffer, length);
    }

    return bytes_read;
}

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

    const auto line = bytesToHex(frame, frame_size);

    std::cout << "Отправил данные: " << line << std::endl;

    uart_send(fd, frame, frame_size);

    delete[] frame;
}

std::atomic<bool> isBindResponseReceive = false;

int main()
{
    int fd = uart_init();
    if (fd == -1)
        return EXIT_FAILURE;

    while (1)
    {
        SendControllFrame(fd);

        uint8_t buffer[256];
        int bytes_read = uart_receive(fd, buffer, sizeof(buffer));
        if (bytes_read > 0)
        {
            std::cout << "Получен ответ: " << bytesToHex(buffer, bytes_read) << std::endl;
        }
        else
        {
            std::cout << "Ответ не получен в течение " << TIMEOUT << " секунд." << std::endl;
        }
    }

    close(fd); // Закрываем UART
    return EXIT_SUCCESS;
}