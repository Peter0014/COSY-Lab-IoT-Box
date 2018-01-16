/**
 * <Program>
 * BME280_TempSensor.h
 *
 * <Started>
 * November 2017
 *
 * <Author>
 * Peter Klosowski
 *
 * <Description>
 * Header file for the BME280 Sensor from Bosch. Defines all
 * addresses that are necessarry to read out temperature,
 * humidity and pressure and introduces two structures, one
 * which holds the compensation parameters and another one
 * for raw or real world sensor values.
 *
 * <Sources>
 * Accessed on 11.01.2018 - BME280 Datasheet:
 *      https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-12.pdf
 * Accessed on 11.01.2018
 *      https://github.com/andreiva/raspberry-pi-bme280
 */

#ifndef BME280_TEMPSENSOR_H
#define BME280_TEMPSENSOR_H

#include <inttypes.h>
#include <stdlib.h>
#include "wiringPiI2C.h"

/* --- I2C address --- */
#define ADDRESS       0x76

/* --- Compensation Parameters --- */
/* Temperature */
#define DIG_T1        0x88
#define DIG_T2        0x8A
#define DIG_T3        0x8C

/* Pressure */
#define DIG_P1        0x8E
#define DIG_P2        0x90
#define DIG_P3        0x92
#define DIG_P4        0x94
#define DIG_P5        0x96
#define DIG_P6        0x98
#define DIG_P7        0x9A
#define DIG_P8        0x9C
#define DIG_P9        0x9E

/* Humidity */
#define DIG_H1        0xA1
#define DIG_H2        0xE1
#define DIG_H3        0xE3
#define DIG_H4        0xE4
#define DIG_H5        0xE5
#define DIG_H6        0xE7

/* --- Information Addresses --- */
#define CHIPID        0xD0
#define VERSION       0xD1

/* --- Oversampling Addresses --- */
#define CONTROLHUMID  0xF2
#define CONTROL_MEAS  0xF4

/* --- Sensor Data --- */
#define PRESSUREDATA  0xF7
#define TEMPDATA      0xFA
#define HUMIDDATA     0xFD


/* Used to hold compensation parameters */
typedef struct {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;

    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;

    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
} compParam;

/* Used to hold raw or real world sensor values */
typedef struct {
    int32_t temperature;
    int32_t tempFine;
    uint32_t pressure;
    uint32_t humidity;

} measData;

/* METHODS */

/**
 * Read the Chip ID from the register 0xD0. Always returns
 * 0x60 for the BME280 sensor.
 *
 * @param sensor sensor ID
 */
int8_t readChipID(int sensor);
/**
 * Read the Version from register 0xD1.
 *
 * @param sensor sensor ID
 */
int8_t readVersion(int sensor);
/**
 * To get real world values it is necessary to get the compensation
 * parameters first. These parameters are saved in the non-volatile
 * memory and can not be changed by the user, they are read-only.
 * The parameters are saved in the memory addresses 0x88 to 0xA1
 * and 0xE1 to 0xE7. Parameters taken from BME280 data sheet page 22.
 *
 * @param sensor sensor ID
 */
compParam readCompensationParam(int sensor);
/**
 * Sets the oversampling used by the sensor to de-/activate and
 * the precision of all measurements. Can be set from
 * 'no oversampling' (0) to 'oversampling x 16' (5). The settings
 * mode can be set to sleep (0 - no measurments), forced (1-2 - take meas.
 * just once) or normal mode (3). Further information on oversampling
 * can be find in the BME280 data sheet on page 26f.
 *
 * @param sensor sensor ID
 * @param humOs humidity oversampling value
 * @param tempOs temperature oversampling value
 * @param pressOs pressure oversampling value
 * @param mode setting mode value
 */
void setOversampling(int sensor, int humOs, int tempOs, int pressOs, int mode);
/**
 * Reads the raw pressure value saved in the sensor memory at
 * address 0xF7 to 0xF9. The value needs to be converted to real
 * world values using the compensation parameters (calcPress).
 *
 * @param sensor sensor ID
 */
uint32_t readRawPress(int sensor);
/**
 * Real world pressure calculated using the compensation parameters
 * and temperature fine. Formula used from the BME280 datasheet
 * page 23. Returns the pressure in Q24.8 format in Pa. That means
 * the returned value needs to be divided by 256 to get Pa and
 * additionally divided by 100 to get hPa (i. e. 24674867/256 =
 * 936386.2 Pa /= 100 = 963.862 hPa).
 *
 * @param rawPress raw pressure value
 * @param comp fetched compensation parameters
 * @param tempFine calculated temperature fine
 * @return real world value for pressure
 */
uint32_t calcPress(uint32_t rawPress, compParam comp, int32_t tempFine);
/**
 * Reads the raw temperature value saved in the sensor memory at
 * address 0xFA to 0xFC. The value needs to be converted to real
 * world values using the compensation parameters (calcTemp).
 *
 * @param sensor sensor ID
 */
uint32_t readRawTemp(int sensor);
/**
 * Real world temperature calculated using the compensation parameters.
 * Temperature fine is calculated here. Formula used from the BME280
 * datasheet page 23. Temperature is returned in Celsius and needs to
 * be divided by 100 (i. e. 2148 is 21.48 C).
 *
 * @param rawTemp raw humidity value
 * @param comp fetched compensation parameters
 * @param tempFine calculated temperature fine pointer
 * @return real world value for temperature
 */
int32_t calcTemp(int32_t rawTemp, compParam comp, int32_t *tempFine);
/**
 * Reads the raw humidity value saved in the sensor memory at
 * address 0xFD and 0xFE. The value needs to be converted to real
 * world values using the compensation parameters (calcHumidity).
 *
 * @param sensor sensor ID
 */
uint32_t readRawHum(int sensor);
/**
 * Real world humidity calculated using the compensation parameters
 * and temperature fine. Formula used from the BME280 datasheet
 * page 23f. Returns the humidity in Q22.10 format. That means
 * the returned value needs to be divided by 1024 to get %RH (i. e.
 * 47445/1024 = 46.333 %RH).
 *
 * @param rawHum raw humidity value
 * @param comp fetched compensation parameters
 * @param tempFine calculated temperature fine
 * @return real world value for humidity
 */
uint32_t calcHum(uint32_t rawHum, compParam comp, int32_t tempFine);

#endif //BME280_TEMPSENSOR_H
