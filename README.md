# Viết driver cho cảm biến áp suất BMP180 trên hệ điều hành Linux
## Thành viên nhóm
- **Quách Tuấn Thành** – MSSV: `22146223`  
- **Trần Linh** – MSSV: `22146343`  
- **Huỳnh Võ Gia Kỳ** – MSSV: `22146339`  
- **Nguyễn Thái Khánh** – MSSV: `22146332`  

---

## 📚 Mục lục

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
- [Khuyến nghị khi sử dụng cảm biến](#khuyến-nghị-khi-sử-dụng-cảm-biến)
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
- VCC     Pin 1 hoặc 17
- GND	    Pin 6, 9, 14, 20, 25, 30, 34, 39
- SDA	    GPIO2 / SDA1 (Pin 3)
- SCL	    GPIO3 / SCL1 (Pin 5)

---

## Hướng dẫn sử dụng
### Bật chế độ I2C
```bash
sudo raspi-config
# Vào Interfacing Options -> I2C -> Enable
```
### Kiểm tra địa chỉ I2C của module

### Cấu hình module

---
## Khuyến nghị khi sử dụng cảm biến

---
## Tài liệu tham khảo





