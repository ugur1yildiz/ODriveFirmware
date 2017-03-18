#include "balance.h"
#include "i2c.h"
#include "low_level.h"

void tx(uint16_t MemAddress, uint8_t *pData, uint16_t size) {
  HAL_I2C_Mem_Write(&hi2c1, MPU6050_DEFAULT_ADDRESS, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, size, 999999999);
}

void rx(uint16_t MemAddress, uint8_t *pData, uint16_t size) {
  HAL_I2C_Mem_Read(&hi2c1, MPU6050_DEFAULT_ADDRESS, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, size, 999999999);
}

void I2C_writeBit(uint8_t regAddr, uint8_t bitNum, uint8_t data) {
    uint8_t b;
    rx(regAddr, &b, 1);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    tx(regAddr, &b, 1);
}

void I2C_writeBits(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b;
    rx(regAddr, &b, 1);
    
	 uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
	 data <<= (bitStart - length + 1); // shift data into correct position
	 data &= mask; // zero all non-important bits in data
	 b &= ~(mask); // zero all important bits in existing byte
	 b |= data; // combine data with existing byte
	 tx(regAddr, &b, 1);
        
}

uint8_t MPU6050_getDeviceID() {
    uint8_t buffer;
    //I2C_readBits(MPU6050_RA_WHO_AM_I, MPU6050_WHO_AM_I_BIT, MPU6050_WHO_AM_I_LENGTH, &buffer);
    return buffer;
}

void MPU6050_setClockSource(uint8_t source) {
    I2C_writeBits(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}

void MPU6050_setSleepEnabled(bool enabled) {
    I2C_writeBit(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}

void MPU6050_setFullScaleGyroRange(uint8_t range) {
    I2C_writeBits(MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

void MPU6050_setFullScaleAccelRange(uint8_t range) {
    I2C_writeBits(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}

void balance_main(void const * argument) {


    MPU6050_setClockSource(MPU6050_CLOCK_PLL_XGYRO);

    MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_250);
    MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
    MPU6050_setSleepEnabled(false); // thanks to Jack Elston for pointing this one out!
    
    // should return true, else error.
    //safe_assert(MPU6050_getDeviceID() == 0x34);

    while (true) {    
      uint8_t buffer[14];
      rx(MPU6050_RA_ACCEL_XOUT_H, buffer, 14);
    
    }


}
