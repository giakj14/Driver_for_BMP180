# Viết driver cho cảm biến áp suất BMP180 trên hệ điều hành Linux
## Thành viên nhóm
- **Quách Tuấn Thành** – MSSV: `22146223`  
- **Trần Linh** – MSSV: `22146343`  
- **Huỳnh Võ Gia Kỳ** – MSSV: `22146339`  
- **Nguyễn Thái Khánh** – MSSV: `22146332`  

---
## Mục lục
- [Giới thiệu](#giới-thiệu)
- [Tính năng chính](#tính-năng-chính)
- [Thông số kỹ thuật](#thông-số-kỹ-thuật)
- [Sơ đồ nối chân với Raspberry Pi 5](#sơ-đồ-nối-chân-với-Raspberry-Pi-5)
- [Hướng dẫn sử dụng](#hướng-dẫn-sử-dụng)
  - [Bật chế độ I2C](#bật-chế-độ-i2c)
  - [Kiểm tra địa chỉ I2C của module](#kiểm-tra-địa-chỉ-i2c-của-module)
  - [Cấu hình module](#cấu-hình-module)
  - [Cài đặt](#cài-đặt)
  - [Kiểm tra log kernel và chạy](#kiểm-tra-log-kernel-và-chạy)
  - [Gỡ cài đặt và dọn dẹp](#gỡ-cài-đặt-và-dọn-dẹp)
- [Tính năng driver](#tính-năng-driver)
- [Tài liệu tham khảo](#tài-liệu-tham-khảo)

---
## Giới thiệu
- Cảm biến áp suất khí quyển BMP180 là cảm biến có độ chính xác cao và tiêu thụ điện năng cực thấp, được thiết kế để đo áp suất khí quyển, nhiệt độ và độ cao. Cảm biến này được ứng dụng rộng rãi trong các trạm thời tiết, hệ thống định vị trong nhà và các ứng dụng IoT.
- BMP180 hỗ trợ cả hai giao tiếp SPI và I²C, giúp tương thích linh hoạt với nhiều nền tảng phần cứng khác nhau như Arduino hoặc Rasberry Pi. Cảm biến sử dụng công nghệ cảm biến điện trở biến dạng (piezo-resistive) kết hợp với bộ chuyển đổi tương tự–số (ADC) để đo chính xác áp suất khí quyển. Khi áp suất thay đổi, điện trở của cảm biến cũng thay đổi tương ứng; các thay đổi này sau đó được chuyển thành giá trị số nhờ vào ADC.
- Ngoài ra, cảm biến còn tích hợp một cảm biến nhiệt độ trên chip, cho phép hiệu chỉnh dữ liệu áp suất theo nhiệt độ nhằm tăng độ chính xác. Nguyên lý hoạt động cơ bản bao gồm: cảm biến piezo-resistive phát hiện thay đổi áp suất, dữ liệu áp suất dạng tương tự được chuyển đổi sang dạng số nhờ ADC, kết hợp với dữ liệu nhiệt độ để bù sai số và cải thiện độ chính xác.

---
## Tính năng chính
- Độ chính xác cao
- Tiêu thụ điện năng thấp
- Đa chức năng
- Linh hoạt trong ứng dụng
- Dễ dàng tích hợp giao tiếp

---
## Thông số kỹ thuật
- Điện áp hoạt động: 1.8V đến 3.6V
- Dải đo áp suất: 300 đến 1100 hPa
- Phương pháp cảm biến: Cảm biến điện trở biến dạng (Piezo-resistive)
- Loại đầu ra: tín hiệu số
- Nhiệt độ hoạt động: -40°C đến +85°C

---
## Sơ đồ nối chân với Raspberry Pi 5
- VCC:    Pin 1 hoặc 17 (3.3V)
- GND:    Pin 6, 9, 14, 20, 25, 30, 34, 39
- SDA:    GPIO2 / SDA1 (Pin 3)
- SCL:    GPIO3 / SCL1 (Pin 5)

---
## Hướng dẫn sử dụng
### Bật chế độ I2C
```bash
sudo raspi-config
# Vào Interfacing Options -> I2C -> Enable
```
### Kiểm tra địa chỉ I2C của module
```bash
sudo i2cdetect -y 1
```
Nếu cắm BMP180 vào như phần cứng ở trên mà xuất hiện địa chỉ 0x77 thì kết nối thành công. Nếu địa chỉ hiển thị UU tại vị trí 0x77, lỗi này có thể bị chiếm bởi 1 driver khác.
### Cấu hình module
Bước 1: Raspberry Pi 5
```bash
cd /boot/firmware
```
Bước 2: Tìm đến file có đầu là bcm2712 và đuôi là .dtb
Bước 3: Chuyển đổi file bạn vừa kiểm ở trên từ .dtb sang .dts (ví dụ là Pi5-b, thay tên file bạn kiếm được, tùy loại Pi)
```bash
sudo dtc -I dtb -O dts -o bcm2712-rpi-5-b.dts bcm2712-rpi-5-b.dtb
```
Bước 4:
Truy cập vào file .dts vừa tạo
```bash
sudo geany bcm2712-rpi-5-b.dts
```
Bước 5: Truy cập
```bash
i2c@74000
```
và nhập đoạn code:
```bash
bmp180@77{ 
    compatible = "bosh, bmp180"; 
    reg = <0x77>; 
};
```
Bước 6: Chuyển đổi file bạn vừa kiểm ở trên từ .dts sang .dtb
```bash
sudo dtc -I dts -O dtb -o bcm2712-rpi-5-b.dtb bcm2712-rpi-5-b.dts
```
---
## Cài đặt
Truy cập vào folder lưu các file driver:
```bash
make
```
sau đó:
```bash
sudo insmod bmp180_driver.ko
```
---
## Kiểm tra log kernel và chạy ioctl
Mở giao diện kernel:
```bash
dmesg - w
```
và chạy ioctl:
```bash
gcc bmp180_ioctl -o run
sudo ./run
```
---
## Gỡ cài đặt và dọn dẹp file
```bash
sudo rmmod bmp180_driver
make clean
```
## Tính năng driver
Để dùng các chức năng của driver khi người dùng code ở lớp user thì cần thêm thư viện sau:
```bash
#include <sys/ioctl.h>
```
Tiếp theo, define các chức năng muốn dùng của driver như sau:
- Đọc giá trị nhiệt độ chưa qua xử lý
```bash
#define BMP180_IOCTL_READ_raw_TEMP _IOR(BMP180_IOCTL_MAGIC, 1, int)
```
- Đọc giá trị áp suất chưa qua xử lý
```bash
#define BMP180_IOCTL_READ_raw_PRESS _IOR(BMP180_IOCTL_MAGIC, 2, int)
```
- Đọc giá trị nhiệt độ đã qua xử lý
```bash
#define BMP180_IOCTL_READ_real_TEMP _IOR(BMP180_IOCTL_MAGIC, 3, int)
```
- Đọc giá trị áp suất đã qua xử lý
```bash
#define BMP180_IOCTL_READ_real_PRESS _IOR(BMP180_IOCTL_MAGIC, 4, int)
```
- Ghi giá trị vào các thanh ghi
```bash
#define BMP180_IOCTL_INIT _IOW(BMP180_IOCTL_MAGIC, 5, struct bmp180_config)
```
---
## Tài liệu tham khảo
[Datasheet BMP180](https://cdn-shop.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf)





