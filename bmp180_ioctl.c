#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>

#define DEVICE_FILE "/dev/bmp180"

#define BMP180_IOCTL_MAGIC 't'
#define BMP180_IOCTL_READ_raw_TEMP  _IOR(BMP180_IOCTL_MAGIC, 1, int)
#define BMP180_IOCTL_READ_raw_PRESS _IOR(BMP180_IOCTL_MAGIC, 2, int)
#define BMP180_IOCTL_READ_real_TEMP _IOR(BMP180_IOCTL_MAGIC, 3, int)
#define BMP180_IOCTL_READ_real_PRESS _IOR(BMP180_IOCTL_MAGIC, 4, int)
#define BMP180_IOCTL_INIT _IO(BMP180_IOCTL_MAGIC, 5) 

int main() {
    int fd;
    int raw_temp = 0, raw_press = 0, real_temp = 0, real_press = 0;

    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
        return EXIT_FAILURE;
    }

    printf("Device opened successfully.\n");

    if (ioctl(fd, BMP180_IOCTL_INIT) < 0) {
        perror("Failed to configure BMP180");
        close(fd);
        return EXIT_FAILURE;
    }

    while (1) {
        if (ioctl(fd, BMP180_IOCTL_READ_raw_TEMP, &raw_temp) < 0)
            perror("Failed to read raw temp");

        if (ioctl(fd, BMP180_IOCTL_READ_raw_PRESS, &raw_press) < 0)
            perror("Failed to read raw press");

        if (ioctl(fd, BMP180_IOCTL_READ_real_TEMP, &real_temp) < 0)
            perror("Failed to read real temp");

        if (ioctl(fd, BMP180_IOCTL_READ_real_PRESS, &real_press) < 0)
            perror("Failed to read real press");

        printf("Temperature: %.1f °C | Pressure: %.2f hPa\n",
               real_temp / 10.0, real_press / 100.0);

        usleep(500000); // 0.5 second delay
    }

    close(fd);
    return EXIT_SUCCESS;
}
