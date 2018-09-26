#include <hal.h>
#define TOSHIBALED_I2C_ADDRESS 0x55
#define AK09916_I2C_ADDRESS 0x0C

/**
 * TIMINGR register definitions
 */
#define STM32_TIMINGR_PRESC_MASK        (15U << 28)
#define STM32_TIMINGR_PRESC(n)          ((n) << 28)
#define STM32_TIMINGR_SCLDEL_MASK       (15U << 20)
#define STM32_TIMINGR_SCLDEL(n)         ((n) << 20)
#define STM32_TIMINGR_SDADEL_MASK       (15U << 16)
#define STM32_TIMINGR_SDADEL(n)         ((n) << 16)
#define STM32_TIMINGR_SCLH_MASK         (255U << 8)
#define STM32_TIMINGR_SCLH(n)           ((n) << 8)
#define STM32_TIMINGR_SCLL_MASK         (255U << 0)
#define STM32_TIMINGR_SCLL(n)           ((n) << 0)

void boardInit(void) {
    palSetLineMode(BOARD_PAL_LINE_SPI3_SCK, PAL_MODE_ALTERNATE(6) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_PULLDOWN); // SPI3 SCK
    palSetLineMode(BOARD_PAL_LINE_SPI3_MISO, PAL_MODE_ALTERNATE(6) | PAL_STM32_OSPEED_HIGHEST); // SPI3 MISO
    palSetLineMode(BOARD_PAL_LINE_SPI3_MOSI, PAL_MODE_ALTERNATE(6) | PAL_STM32_OSPEED_HIGHEST); // SPI3 MOSI
    palSetLineMode(BOARD_PAL_LINE_SPI3_ICM_CS, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST); // SPI CS
    palSetLineMode(BOARD_PAL_LINE_SPI3_MS5611_CS, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST); // SPI CS
    palSetLineMode(BOARD_PAL_LINE_CAN_RX, PAL_MODE_ALTERNATE(9) | PAL_STM32_OSPEED_HIGHEST);
    palSetLineMode(BOARD_PAL_LINE_CAN_TX, PAL_MODE_ALTERNATE(9) | PAL_STM32_OSPEED_HIGHEST);
    palSetLineMode(BOARD_PAL_LINE_GPS_RX, PAL_MODE_ALTERNATE(7) | PAL_STM32_OSPEED_HIGHEST);    //GPS Rx USART2_Rx
    palSetLineMode(BOARD_PAL_LINE_GPS_TX, PAL_MODE_ALTERNATE(7) | PAL_STM32_OSPEED_HIGHEST);    //GPS Tx USART2_Tx
    palSetLineMode(BOARD_PAL_LINE_I2C_SLAVE_SCL, PAL_MODE_ALTERNATE(4));
    palSetLineMode(BOARD_PAL_LINE_I2C_SLAVE_SDA, PAL_MODE_ALTERNATE(4));

/*
 *
 *    Setup I2C Slave
 *
 */
    rccEnableI2C2(FALSE);
    rccResetI2C2();

    //Disable I2C
    I2C2->CR1 &= ~I2C_CR1_PE;

    //Enable Analog Filter
	I2C2->CR1 &= ~I2C_CR1_ANFOFF;

    //Disable Digital Filter
    I2C2->CR1 &=  ~(I2C_CR1_DNF);

    //Set Prescaler
    I2C2->TIMINGR = (I2C2->TIMINGR & ~STM32_TIMINGR_PRESC_MASK) |
			   (STM32_TIMINGR_PRESC(8));

    //Set Data Setup Time
    I2C2->TIMINGR = (I2C2->TIMINGR & ~STM32_TIMINGR_SCLDEL_MASK) |
			   (STM32_TIMINGR_SCLDEL(9));

    //Set Data Hold Time
    I2C2->TIMINGR = (I2C2->TIMINGR & ~STM32_TIMINGR_SDADEL_MASK) |
			   (STM32_TIMINGR_SDADEL(11));

    //Enable Stretching
	I2C2->CR1 &= ~I2C_CR1_NOSTRETCH;

    //7Bit Address Mode
    I2C2->CR2 &= ~I2C_CR2_ADD10;

    I2C2->OAR1 = (TOSHIBALED_I2C_ADDRESS & 0xFF) << 1; //Emulate Toshiba LED I2C Slave
    I2C2->OAR1 |= (1<<15);
    I2C2->OAR2 = (AK09916_I2C_ADDRESS & 0xFF) << 1; //Emulate AK09916 I2C Slave
    I2C2->OAR2 |= (1<<15);
    //Enable I2C interrupt
    nvicEnableVector(I2C2_EV_IRQn, 3);

    I2C2->CR1 |= (1<<1); // TXIE
    I2C2->CR1 |= (1<<2); // RXIE
    I2C2->CR1 |= (1<<3); // ADDRIE
	I2C2->CR1 |= I2C_CR1_PE; // Enable I2C
}
