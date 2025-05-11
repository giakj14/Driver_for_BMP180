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

Dự án này xây dựng driver cho cảm biến áp suất **BMP180**. Driver được thiết kế để chạy trên Raspberry Pi 5.
---

## Tính năng chính
- Bao gồm chức năng đo nhiệt độ
- Không chì (Pb-free), không halogen và tuân thủ tiêu chuẩn RoHS
- Giao tiếp I2C
- Cấp độ nhạy ẩm MSL 1 (Moisture Sensitivity Level 1)

---

## Sơ đồ kết nối



