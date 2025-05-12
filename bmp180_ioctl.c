#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>

#define DEVICE_FILE "/dev/bmp180"

#define BMP180_IOCTL_MAGIC 't'
#define BMP180_IOCTL_READ_raw_TEMP _IOR(BMP180_IOCTL_MAGIC, 1, int)
#define BMP180_IOCTL_READ_raw_PRESS _IOR(BMP180_IOCTL_MAGIC, 2, int)
#define BMP180_IOCTL_READ_real_TEMP _IOR(BMP180_IOCTL_MAGIC, 3, int)
#define BMP180_IOCTL_READ_real_PRESS _IOR(BMP180_IOCTL_MAGIC, 4, int)
#define BMP180_IOCTL_INIT _IOW(BMP180_IOCTL_MAGIC, 5, struct bmp180_config)

struct bmp180_config {
    uint8_t bmp180_chipID;
    uint8_t bmp180_control;
    uint8_t bmp180_data;
    uint8_t bmp180_calibration;
};

int main() {
    int fd;
    int raw_temp = 0, raw_press = 0, real_temp = 0, real_press = 0;

    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
        return EXIT_FAILURE;
    }

    printf("Device opened successfully.\n");

    struct bmp180_config config = {
        .bmp180_chipID = 0xD0,
        .bmp180_control = 0xF4,
        .bmp180_data = 0xF6,
        .bmp180_calibration = 0xAA,
    };

    if (ioctl(fd, BMP180_IOCTL_INIT, &config) < 0) {
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

        usleep(500000);
    }

    close(fd);
    return EXIT_SUCCESS;
}
