#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/delay.h>

#define DRIVER_NAME "bmp180_driver"
#define CLASS_NAME  "bmp180"
#define DEVICE_NAME "bmp180"

//bmp register map
#define BMP180_ADDR 0x77
#define BMP180_REG_CHIP_ID 0xD0
#define BMP180_REG_CONTROL 0xF4
#define BMP180_REG_DATA 0xF6
#define BMP180_REG_CALIBRATION 0xAA

#define TEMP 0x2E
#define PRESS 0x34
#define OSS 0 // Oversampling setting (0 to 3)

// ioctl cmd
#define BMP180_IOCTL_MAGIC 't'
#define BMP180_IOCTL_READ_raw_TEMP _IOR(BMP180_IOCTL_MAGIC, 1, int)
#define BMP180_IOCTL_READ_raw_PRESS _IOR(BMP180_IOCTL_MAGIC, 2, int)
#define BMP180_IOCTL_READ_real_TEMP _IOR(BMP180_IOCTL_MAGIC, 3, int)
#define BMP180_IOCTL_READ_real_PRESS _IOR(BMP180_IOCTL_MAGIC, 4, int)
#define BMP180_IOCTL_INIT _IOW(BMP180_IOCTL_MAGIC, 5, struct bmp180_config)


static struct i2c_client *bmp180_client;
static struct class* bmp180_class = NULL;
static struct device* bmp180_device = NULL;
static int major_number;

int32_t raw_temp,real_temp;
int32_t raw_press,real_press;

struct bmp180_config {
    uint8_t bmp180_chipID;
    uint8_t bmp180_control;
    uint8_t bmp180_data;
    uint8_t bmp180_calibration;
};

struct CalibrationData
{
    int16_t AC1, AC2, AC3;
    uint16_t AC4, AC5, AC6;
    int16_t B1, B2, MB, MC, MD;
} calib_data;

static int bmp180_read_word(u8 reg)
{
    int msb = i2c_smbus_read_byte_data(bmp180_client, reg);
    int lsb = i2c_smbus_read_byte_data(bmp180_client, reg + 1);
    if (msb < 0 || lsb < 0)
        return -1;
    return (msb << 8) | lsb;
}

static int bmp180_read_calibration_data(void)
{   
    calib_data.AC1 = bmp180_read_word(0xAA);
    calib_data.AC2 = bmp180_read_word(0xAC);
    calib_data.AC3 = bmp180_read_word(0xAE);
    calib_data.AC4 = bmp180_read_word(0xB0);
    calib_data.AC5 = bmp180_read_word(0xB2);
    calib_data.AC6 = bmp180_read_word(0xB4);
    calib_data.B1  = bmp180_read_word(0xB6);
    calib_data.B2  = bmp180_read_word(0xB8);
    calib_data.MB  = bmp180_read_word(0xBA);
    calib_data.MC  = bmp180_read_word(0xBC);
    calib_data.MD  = bmp180_read_word(0xBE);
    return 0;
}

static int bmp180_read_raw_temp(void)
{
    i2c_smbus_write_byte_data(bmp180_client, BMP180_REG_CONTROL, TEMP);
    usleep_range(4500, 5000);
    return bmp180_read_word(BMP180_REG_DATA);
}

static int bmp180_get_real_temp(void)
{
    int32_t ut = bmp180_read_raw_temp();
    if (ut < 0)
        return -EIO;

    int32_t x1 = ((ut - calib_data.AC6) * calib_data.AC5) >> 15;
    int32_t x2 = (calib_data.MC << 11) / (x1 + calib_data.MD);
    int32_t b5 = x1 + x2;

    real_temp = (b5 + 8) >> 4; // in 0.1 °C
    return real_temp;
}

static int bmp180_read_raw_pressure(void)
{
    i2c_smbus_write_byte_data(bmp180_client, BMP180_REG_CONTROL, PRESS);
    usleep_range(4500, 5000);

    int msb = i2c_smbus_read_byte_data(bmp180_client, BMP180_REG_DATA);
    int lsb = i2c_smbus_read_byte_data(bmp180_client, BMP180_REG_DATA + 1);
    int xlsb = i2c_smbus_read_byte_data(bmp180_client, BMP180_REG_DATA + 2);
    if (msb < 0 || lsb < 0 || xlsb < 0)
        return -1;

    return ((msb << 16) | (lsb << 8) | xlsb) >> 8; 
}

static int bmp180_get_real_pressure(void)
{
    // First, read temperature to get B5 needed for pressure calculation
    int32_t ut = bmp180_read_raw_temp();
    if (ut < 0)
        return -EIO;

    int32_t x1 = ((ut - calib_data.AC6) * calib_data.AC5) >> 15;
    int32_t x2 = (calib_data.MC << 11) / (x1 + calib_data.MD);
    int32_t b5 = x1 + x2;
    real_temp = (b5 + 8) >> 4; // update global real_temp too

    int32_t up = bmp180_read_raw_pressure();
    if (up < 0)
        return -EIO;

    int32_t b6 = b5 - 4000;
    x1 = (calib_data.B2 * ((b6 * b6) >> 12)) >> 11;
    x2 = (calib_data.AC2 * b6) >> 11;
    int32_t x3 = x1 + x2;
    int32_t b3 = ((((int32_t)calib_data.AC1 * 4 + x3) << OSS) + 2) >> 2;

    x1 = (calib_data.AC3 * b6) >> 13;
    x2 = (calib_data.B1 * ((b6 * b6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    uint32_t b4 = (calib_data.AC4 * (uint32_t)(x3 + 32768)) >> 15;
    uint32_t b7 = ((uint32_t)up - b3) * (50000 >> OSS);

    int32_t p;
    if (b7 < 0x80000000)
        p = (b7 << 1) / b4;
    else
        p = (b7 / b4) << 1;

    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;
    p = p + ((x1 + x2 + 3791) >> 4);

    real_press = p;
    return real_press;
}



static long bmp180_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct bmp180_config config;
    switch (cmd) 
    {
        case BMP180_IOCTL_INIT:
        if (copy_from_user(&config, (struct bmp180_config __user *)arg, sizeof(config)))
            return -EFAULT;

        if (i2c_smbus_write_byte_data(bmp180_client, BMP180_REG_CHIP_ID, config.bmp180_chipID ) < 0) {
            printk(KERN_ERR "Failed to write BMP180_REG_CHIP_ID\n");
            return -EIO;
        }
        if (i2c_smbus_write_byte_data(bmp180_client, BMP180_REG_CONTROL, config.bmp180_control) < 0) {
            printk(KERN_ERR "Failed to write BMP180_REG_CONTROL\n");
            return -EIO;
        }
        if (i2c_smbus_write_byte_data(bmp180_client, BMP180_REG_DATA, config.bmp180_data) < 0) {
            printk(KERN_ERR "Failed to write BMP180_REG_DATA\n");
            return -EIO;
        }
        if (i2c_smbus_write_byte_data(bmp180_client, BMP180_REG_CALIBRATION, config.bmp180_calibration) < 0) {
            printk(KERN_ERR "Failed to write BMP180_REG_CALIBRATION\n");
            return -EIO;
        }
        printk("BMP180 configured successfully\n");
        break;
       
        case BMP180_IOCTL_READ_raw_TEMP:
            raw_temp = bmp180_read_raw_temp();
            if (copy_to_user((int __user *)arg, &raw_temp, sizeof(int)))
                return -EFAULT;
            break;

        case BMP180_IOCTL_READ_raw_PRESS:
            raw_press = bmp180_read_raw_pressure();
            if (copy_to_user((int __user *)arg, &raw_press, sizeof(int)))
                return -EFAULT;
            break;
    
        case BMP180_IOCTL_READ_real_TEMP:
            real_temp = bmp180_get_real_temp();  // °C × 10
            if (copy_to_user((int __user *)arg, &real_temp, sizeof(int)))
                return -EFAULT;
            break;
    
        case BMP180_IOCTL_READ_real_PRESS:
            real_press = bmp180_get_real_pressure();  // Pa
            if (copy_to_user((int __user *)arg, &real_press, sizeof(int)))
                return -EFAULT;
            break;
        

    default:
        return -EINVAL;
    }

    return 0;
}

static int bmp180_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "BMP180 device opened\n");
    return 0;
}

static int bmp180_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "BMP180 device closed\n");
    return 0;
}

static struct file_operations fops = {
    .open = bmp180_open,
    .unlocked_ioctl = bmp180_ioctl,
    .release = bmp180_release,
};

static int bmp180_probe(struct i2c_client *client)
{
    printk("UNLOCKED");
    bmp180_read_calibration_data();

    bmp180_client = client;

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0)
        return major_number;

    bmp180_class = class_create(CLASS_NAME);
    if (IS_ERR(bmp180_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(bmp180_class);
    }

    bmp180_device = device_create(bmp180_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(bmp180_device)) {
        class_destroy(bmp180_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(bmp180_device);
    }

    dev_info(&client->dev, "BMP180 driver installed successfully\n");

    printk(KERN_INFO "BMP180 driver installed successfully\n");
    return 0;
}

static void bmp180_remove(struct i2c_client *client)
{
    device_destroy(bmp180_class, MKDEV(major_number, 0));
    class_destroy(bmp180_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    dev_info(&client->dev, "BMP180 driver uninstalled\n");

    printk(KERN_INFO "BMP180 driver uninstalled\n");
}

static const struct of_device_id bmp180_of_match[] = {
    { .compatible = "bosch,bmp180", },
    { },
};
MODULE_DEVICE_TABLE(of, bmp180_of_match);

static struct i2c_driver bmp180_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = bmp180_of_match,
    },
    .probe = bmp180_probe,
    .remove = bmp180_remove,
};


static int __init bmp180_init(void)
{
    printk(KERN_INFO "Initializing BMP180 driver\n");
    return i2c_add_driver(&bmp180_driver);
}

static void __exit bmp180_exit(void)
{
    printk(KERN_INFO "Exiting BMP180 driver\n");
    i2c_del_driver(&bmp180_driver);
}

module_init(bmp180_init);
module_exit(bmp180_exit);

MODULE_AUTHOR("QUACH TUAN THANH");
MODULE_DESCRIPTION("BMP180 I2C Client Driver");
MODULE_LICENSE("GPL");


