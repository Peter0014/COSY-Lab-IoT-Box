/**
 * <Program>
 * BME280_TempSensor.c
 *
 * <Started>
 * November 2017
 *
 * <Author>
 * Peter Klosowski
 *
 * <Description>
 *  This is the driver class used by the COSY-Lab-IoT-Box
 * to interact with the BME280 Sensor developed by Bosch.
 * It is capable of reading the compensation parameters,
 * read raw temperature, humidity and pressure and
 * calculate the real world values using the compensation
 * params.
 *  Additionally this class holds the methods to communicate
 * with Python software using the CPython library. Although
 * just real world values can be fetched that way.
 *
 * <Sources>
 * Accessed on 11.01.2018 - BME280 Datasheet:
 *      https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-12.pdf
 * Accesses on 11.01.2018 - Extending Python with C
 *      https://docs.python.org/2/extending/extending.html
 * Accessed on 11.01.2018
 *      https://github.com/andreiva/raspberry-pi-bme280
 */

#include <Python.h>
#include "BME280_TempSensor.h"

/**
 * Read the Chip ID from the register 0xD0. Always returns
 * 0x60 for the BME280 sensor.
 *
 * @param sensor sensor ID
 */
int8_t readChipID(int sensor) {
    int8_t chipID = (int8_t) wiringPiI2CReadReg8(sensor, CHIPID);
    return chipID;
}

/**
 * Read the Version from register 0xD1.
 *
 * @param sensor sensor ID
 */
int8_t readVersion(int sensor) {
    int8_t version = (int8_t) wiringPiI2CReadReg8(sensor, VERSION);
    return version;
}

/**
 * To get real world values it is necessary to get the compensation
 * parameters first. These parameters are saved in the non-volatile
 * memory and can not be changed by the user, they are read-only.
 * The parameters are saved in the memory addresses 0x88 to 0xA1
 * and 0xE1 to 0xE7. Parameters taken from BME280 data sheet page 22.
 *
 * @param sensor sensor ID
 */
compParam readCompensationParam(int sensor) {
    compParam data;

    /*
     * Temperature parameters
     */
    data.dig_T1 = (uint16_t) wiringPiI2CReadReg16(sensor, DIG_T1);
    data.dig_T2 = (int16_t) wiringPiI2CReadReg16(sensor, DIG_T2);
    data.dig_T3 = (int16_t) wiringPiI2CReadReg16(sensor, DIG_T3);

    /*
     * Pressure parameters
     */
    data.dig_P1 = (uint16_t) wiringPiI2CReadReg16(sensor, DIG_P1);
    data.dig_P2 = (int16_t) wiringPiI2CReadReg16(sensor, DIG_P2);
    data.dig_P3 = (uint16_t) wiringPiI2CReadReg16(sensor, DIG_P3);
    data.dig_P4 = (uint16_t) wiringPiI2CReadReg16(sensor, DIG_P4);
    data.dig_P5 = (uint16_t) wiringPiI2CReadReg16(sensor, DIG_P5);
    data.dig_P6 = (uint16_t) wiringPiI2CReadReg16(sensor, DIG_P6);
    data.dig_P7 = (uint16_t) wiringPiI2CReadReg16(sensor, DIG_P7);
    data.dig_P8 = (uint16_t) wiringPiI2CReadReg16(sensor, DIG_P8);
    data.dig_P9 = (uint16_t) wiringPiI2CReadReg16(sensor, DIG_P9);

    /*
     * Humidity parameters
     */
    data.dig_H1 = (uint8_t) wiringPiI2CReadReg8(sensor, DIG_H1);
    data.dig_H2 = (int16_t) wiringPiI2CReadReg16(sensor, DIG_H2);
    data.dig_H3 = (uint8_t) wiringPiI2CReadReg8(sensor, DIG_H3);

    /*
     * Each, H4 and H5, are saved in two different memory addresses
     * and it is necessary to bitshift ([3:0], [11:4]) to get the
     * right value.
     */
    data.dig_H4 = wiringPiI2CReadReg8(sensor, DIG_H4) << 4;
    data.dig_H4 = data.dig_H4 | wiringPiI2CReadReg8(sensor, DIG_H4 + 1) & 0xF;

    data.dig_H5 = wiringPiI2CReadReg8(sensor, DIG_H5);
    data.dig_H5 = data.dig_H5 | wiringPiI2CReadReg8(sensor, DIG_H5 + 1) << 4;

    data.dig_H6 = (int8_t) wiringPiI2CReadReg8(sensor, DIG_H6);

    return data;
}

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
void setOversampling(int sensor, int humOs, int tempOs, int pressOs, int mode) {
    wiringPiI2CWriteReg8(sensor, CONTROLHUMID, humOs);

    int controlMeas = tempOs << 5 | pressOs << 2 | mode;
    wiringPiI2CWriteReg8(sensor, CONTROL_MEAS, controlMeas);
}

/**
 * Reads the raw pressure value saved in the sensor memory at
 * address 0xF7 to 0xF9. The value needs to be converted to real
 * world values using the compensation parameters (calcPress).
 *
 * @param sensor sensor ID
 */
uint32_t readRawPress(int sensor) {

    uint32_t rawPress = wiringPiI2CReadReg8(sensor, PRESSUREDATA) << 8;
    rawPress = (rawPress | wiringPiI2CReadReg8(sensor, PRESSUREDATA + 1)) << 8;
    rawPress = (rawPress | wiringPiI2CReadReg8(sensor, PRESSUREDATA + 2)) >> 4;

    return rawPress;
}

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
uint32_t calcPress(uint32_t rawPress, compParam comp, int32_t tempFine) {
    int64_t pressure = 0;

    int64_t var1, var2;

    var1 = ((int64_t) tempFine) - 128000;
    var2 = var1 * var1 * (int64_t) comp.dig_P6;
    var2 = var2 + ((var1 * (int64_t) comp.dig_P5) << 17);
    var2 = var2 + (((int64_t) comp.dig_P4) << 35);

    var1 = (var1 * var1 * ((int64_t) comp.dig_P3) >> 8) + ((var1 * (int64_t) comp.dig_P2 << 12));
    var1 = (((int64_t) 1 << 47) + var1) * ((int64_t) comp.dig_P1) >> 33;

    if (var1 == 0) {
        return 0;
    }

    pressure = 1048576 - rawPress;
    pressure = (((pressure << 31) - var2) * 3125) / var1;

    var1 = (((int64_t) comp.dig_P9) * (pressure >> 13) * (pressure >> 13)) >> 25;
    var2 = (((int64_t) comp.dig_P8) * pressure) >> 19;

    pressure = ((pressure + var1 + var2) >> 8) + (((int64_t) comp.dig_P7) << 4);

    return (uint32_t) pressure;

}

/**
 * Reads the raw temperature value saved in the sensor memory at
 * address 0xFA to 0xFC. The value needs to be converted to real
 * world values using the compensation parameters (calcTemp).
 *
 * @param sensor sensor ID
 */
uint32_t readRawTemp(int sensor) {

    uint32_t rawTemp = wiringPiI2CReadReg8(sensor, TEMPDATA) << 8;
    rawTemp = (rawTemp | wiringPiI2CReadReg8(sensor, TEMPDATA + 1)) << 8;
    rawTemp = (rawTemp | wiringPiI2CReadReg8(sensor, TEMPDATA + 2)) >> 4;

    return rawTemp;
}

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
int32_t calcTemp(int32_t rawTemp, compParam comp, int32_t *tempFine) {
    int32_t temperature = 0;

    int32_t var1, var2;

    var1 = (((rawTemp >> 3) - (comp.dig_T1 << 1)) * (comp.dig_T2)) >> 11;
    var2 = (((((rawTemp >> 4) - comp.dig_T1) * (rawTemp >> 4) - comp.dig_T1) >> 12) * comp.dig_T3) >> 14;

    *tempFine = var1 + var2;

    temperature = ((*tempFine) * 5 + 128) >> 8;

    return temperature;
}

/**
 * Reads the raw humidity value saved in the sensor memory at
 * address 0xFD and 0xFE. The value needs to be converted to real
 * world values using the compensation parameters (calcHumidity).
 *
 * @param sensor sensor ID
 */
uint32_t readRawHum(int sensor) {

    uint32_t rawHum = wiringPiI2CReadReg8(sensor, HUMIDDATA) << 8;
    rawHum = rawHum | wiringPiI2CReadReg8(sensor, HUMIDDATA + 1);

    return rawHum;
}

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
uint32_t calcHum(uint32_t rawHum, compParam comp, int32_t tempFine) {
    int32_t humidity = 0;

    humidity = tempFine - ((int32_t) 76800);
    humidity = ((((rawHum << 14) - (((int32_t) comp.dig_H4) << 20) - (((int32_t) comp.dig_H5) * humidity)) +
                 ((int32_t) 16384)) >> 15) * (((((((humidity * ((int32_t) comp.dig_H6)) >> 10) *
                                                  (((humidity * ((int32_t) comp.dig_H3)) >> 11) + ((int32_t) 32768)))
            >> 10) +
                                                ((int32_t) 2097152)) * ((int32_t) comp.dig_H2) + 8192) >> 14);

    humidity = humidity - (((((humidity >> 15) * (humidity >> 15)) >> 7) * ((int32_t) comp.dig_H1)) >> 4);

    if (humidity < 0) {
        humidity = 0;
    } else if (humidity > 419430400) {
        humidity = 419430400;
    }

    humidity = humidity >> 12;

    return (uint32_t) humidity;
}

/**
 * Setup the sensor, fetch the compensation parameters and
 * read the current temperature from the device.
 *
 * @param self python instance the method is called on
 * @param args passed arguments from calling python method
 * @return real world temperature value
 */
static PyObject *get_temperature(PyObject *self, PyObject *args) {
    int32_t sensor;
    sensor = wiringPiI2CSetup(ADDRESS);
    if (sensor < 0) {
        printf("sensor not found!\n");
        return NULL;
    }

    compParam comp = readCompensationParam(sensor);
    setOversampling(sensor, 1, 1, 1, 3);

    measData rawData;
    rawData.temperature = readRawTemp(sensor);

    measData calcData;
    calcData.temperature = calcTemp(rawData.temperature, comp, &calcData.tempFine);

    return Py_BuildValue("f", (calcData.temperature / 100.0));
}

/**
 * Setup the sensor, fetch the compensation parameters and
 * read the current temperature to get the temperature fine
 * that is used for reading the humidity from the device.
 *
 * @param self python instance the method is called on
 * @param args passed arguments from calling python method
 * @return real world humidity value
 */
static PyObject *get_humidity(PyObject *self, PyObject *args) {
    int32_t sensor;
    sensor = wiringPiI2CSetup(ADDRESS);
    if (sensor < 0) {
        printf("sensor not found!\n");
        return NULL;
    }

    compParam comp = readCompensationParam(sensor);
    setOversampling(sensor, 1, 1, 1, 3);

    measData rawData;
    rawData.temperature = readRawTemp(sensor);
    rawData.humidity = readRawHum(sensor);

    measData calcData;
    /* Calculate and save tempFine by getting the temperature */
    calcTemp(rawData.temperature, comp, &calcData.tempFine);
    calcData.humidity = calcHum(rawData.humidity, comp, calcData.tempFine);

    return Py_BuildValue("f", (calcData.humidity / 1024.0));
}

/**
 * Setup the sensor, fetch the compensation parameters and
 * read the current temperature to get the temperature fine
 * that is used for reading the pressure from the device.
 *
 * @param self python instance the method is called on
 * @param args passed arguments from calling python method
 * @return real world pressure value
 */
static PyObject *get_pressure(PyObject *self, PyObject *args) {
    int32_t sensor;
    sensor = wiringPiI2CSetup(ADDRESS);
    if (sensor < 0) {
        printf("sensor not found!\n");
        return NULL;
    }

    compParam comp = readCompensationParam(sensor);
    setOversampling(sensor, 1, 1, 1, 3);

    measData rawData;
    rawData.pressure = readRawPress(sensor);
    rawData.temperature = readRawTemp(sensor);

    measData calcData;
    /* Calculate and save tempFine by getting the temperature */
    calcTemp(rawData.temperature, comp, &calcData.tempFine);
    calcData.pressure = calcPress(rawData.pressure, comp, calcData.tempFine);

    return Py_BuildValue("f", (calcData.pressure / 256.0 / 100.0));
}

/**
 * Method definitions that are visible in Python afterwards
 */
static PyMethodDef environmentSensor_methods[] = {
        {"get_temperature", get_temperature, METH_VARARGS},
        {"get_humidity",    get_humidity,    METH_VARARGS},
        {"get_pressure",    get_pressure,    METH_VARARGS},
        {NULL, NULL, 0, NULL} /* Sentinel */
};

/**
 * Initializes the module and methods that can be called
 * from python.
 */
void initenvironmentSensor(void) {
    PyImport_AddModule("environmentSensor");
    Py_InitModule("environmentSensor", environmentSensor_methods);
}

int main(int argc, char **argv) {
    /* Initialize the Python interpreter. */
    Py_Initialize();
    /* Initialize methods */
    initenvironmentSensor();

/*
    int sensor = wiringPiI2CSetup(ADDRESS);
    if (sensor < 0) {
        printf("sensor not found!\n");
        return 1;
    }

    int chipID = readChipID(sensor);
    int version = readVersion(sensor);
    printf("CHIP-ID: %i\n"
           "VERSION: %i\n", chipID, version);

    compParam comp =  readCompensationParam(sensor);
    setOversampling(sensor, 1, 1, 1, 3);

    measData rawData;

    rawData.pressure = readRawPress(sensor);
    rawData.temperature = readRawTemp(sensor);
    rawData.humidity = readRawHum(sensor);

    printf("Raw Pressure: %i\n"
           "Raw Temperature: %i\n"
           "Raw Humidity: %i\n", rawData.pressure, rawData.temperature, rawData.humidity);


    measData calcData;
    calcData.temperature = calcTemp(rawData.temperature, comp, &calcData.tempFine );
    calcData.pressure = calcPress(rawData.pressure, comp, calcData.tempFine );
    calcData.humidity = calcHum(rawData.humidity, comp, calcData.tempFine );

    printf("Temperature: %.2f\n"
           "Pressure: %.2f hPa\n"
           "Humidity: %.2f %\n",
           (calcData.temperature / 100.0),
           (calcData.pressure / 256.0 / 100.0),
           (calcData.humidity / 1024.0));

    return 0;
*/
}
