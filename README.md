# Project Driver TCS34725
## Thành viên nhóm
- **Quách Tuấn Thành** – MSSV: `22146`  
- **Trần Linh** – MSSV: `22146`  
- **Huỳnh Võ Gia Kỳ** – MSSV: `22146339`  
- **Nguyễn Thái Khánh** – MSSV: `22146`  

---

## 📚 Mục lục

- [Giới thiệu](#giới-thiệu)
- [Thông số kỹ thuật](#thông-số-kỹ-thuật)
- [Sơ đồ kết nối](#sơ-đồ-kết-nối)
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

Dự án này xây dựng driver cho cảm biến màu **TCS34725**, sử dụng giao tiếp I2C. Driver được thiết kế để chạy trên Raspberry Pi (hoặc các hệ thống Linux tương tự) dưới dạng kernel module.

---

## Thông số kỹ thuật

- Cảm biến màu TCS34725
- Giao tiếp I2C
- Điện áp hoạt động: 3.3V – 5V
- Có tích hợp IR filter

---

## Sơ đồ kết nối



