/**
 * <Program>
 * SI1145_LightSensor.c
 *
 * <Started>
 * November 2017
 *
 * <Author>
 * Peter Klosowski
 *
 * <Description>
 *  This is the driver class used by the COSY-Lab-IoT-Box
 * to interact with the SI1145 Sensor developed by Silicon
 * Labs. The breakout sensor from Adafruit was used. It is
 * capable of reading the ultraviolet, infrared and visible
 * light.
 *  Additionally this class holds the methods to communicate
 * with Python software using the CPython library.
 *
 * <Sources>
 * Accessed on 11.01.2018 - SI1145 Datasheet:
 *      https://www.silabs.com/documents/public/data-sheets/Si1145-46-47.pdf
 * Accesses on 11.01.2018 - Extending Python with C
 *      https://docs.python.org/2/extending/extending.html
 * Accessed on 11.01.2018
 *      https://github.com/adafruit/Adafruit_SI1145_Library
 */

#include <Python.h>
#include <unistd.h>
#include "SI1145_LightSensor.h"

/**
 * It is recommended to reset the command register to 0x00
 * before writing to it and check if the response register
 * changed accordingly (see p. 22).
 *
 * @param sensor sensor ID
 * @param data data that will be written to the command register
 */
void writeToCommand(int sensor, int data) {
    int response = 0x00;
    while (response == 0x00) {
        wiringPiI2CWriteReg8(sensor, SI1145_REG_COMMAND, 0x00);
        response = wiringPiI2CReadReg8(sensor, SI1145_REG_RESPONSE);
        if (response == 0x00) {
            wiringPiI2CWriteReg8(sensor, SI1145_REG_COMMAND, data);
            usleep(25);
            response = wiringPiI2CReadReg8(sensor, SI1145_REG_RESPONSE);
        } else {
            response = 0x00;
        }
    }
}

/**
 * The sensor needs to be reset before any values can be written
 * or read from the I2C connection. The I2C-Broadcast-Reset is
 * sent to all necessary registers. See page 17 of the SI1145
 * datasheet for further information. Reset values start on page
 * 31.
 *
 * @param sensor sensor ID
 */
void resetSensor(int sensor) {
    wiringPiI2CWriteReg8(sensor, SI1145_REG_INTCFG, 0x00);
    wiringPiI2CWriteReg8(sensor, SI1145_REG_IRQEN, 0x00);
    wiringPiI2CWriteReg8(sensor, SI1145_REG_IRQMODE1, 0x00);
    wiringPiI2CWriteReg8(sensor, SI1145_REG_IRQMODE2, 0x00);
    wiringPiI2CWriteReg8(sensor, SI1145_REG_MEASRATE0, 0x00);
    wiringPiI2CWriteReg8(sensor, SI1145_REG_MEASRATE1, 0x00);
    /* Set every status to 1 (p. 39) */
    wiringPiI2CWriteReg8(sensor, SI1145_REG_IRQSTAT, 0xFF);

    wiringPiI2CWriteReg8(sensor, SI1145_REG_COMMAND, SI1145_RESET);
    usleep(10); /* wait 10ms to let sensor reset */
    /* Write 0x17 for proper operation (p. 34) */
    wiringPiI2CWriteReg8(sensor, SI1145_REG_HWKEY, 0x17);
    usleep(10);
}

/**
 * To enable UV reading, it is necessary to configure UCOEF
 * to default values (see p. 16).
 *
 * @param sensor sensor ID
 */
void calibrateUV(int sensor){
    wiringPiI2CWriteReg8(sensor, SI1145_REG_UCOEFF0, SI1145_DEF_UCOEFF0);
    wiringPiI2CWriteReg8(sensor, SI1145_REG_UCOEFF1, SI1145_DEF_UCOEFF1);
    wiringPiI2CWriteReg8(sensor, SI1145_REG_UCOEFF2, SI1145_DEF_UCOEFF2);
    wiringPiI2CWriteReg8(sensor, SI1145_REG_UCOEFF3, SI1145_DEF_UCOEFF3);
}

/**
 * Set the EN_UV, EN_ALS_IR and EN_ALS_VIS bits in CHLIST
 * (see p. 47) to enable UV (ultraviolet), IR (infrared)
 * and VIS (visible light).
 *
 * @param sensor sensor ID
 */
void enableMeas(int sensor) {
    wiringPiI2CWriteReg8(sensor, SI1145_REG_PARAMWR, SI1145_PARAM_CHLIST_ENUV |
                                                     SI1145_PARAM_CHLIST_ENALSIR |
                                                     SI1145_PARAM_CHLIST_ENALSVIS);
    writeToCommand(sensor, SI1145_PARAM_CHLIST | SI1145_PARAM_SET);

    /* Enable interrupt Pin whenever measurements are ready */
    wiringPiI2CWriteReg8(sensor, SI1145_REG_INTCFG, SI1145_REG_INTCFG_INTOE);
    wiringPiI2CWriteReg8(sensor, SI1145_REG_IRQEN, SI1145_REG_IRQEN_ALSEVERYSAMPLE);

    /* measurement rate for auto */
    wiringPiI2CWriteReg8(sensor, SI1145_REG_MEASRATE0, 0xFF); // 255 * 31.25uS = 8ms

    /* auto run */
    writeToCommand(sensor, SI1145_PSALS_AUTO);
}

/**
 * Initialize the sensor to work properly. In this case it
 * resets the device to default values, calibrates the UV
 * reading and enables measurments.
 *
 * @param sensor sensor ID
 */
void initSensor(int sensor) {
    /* Reset device before any register is accessed */
    resetSensor(sensor);
    calibrateUV(sensor);
    enableMeas(sensor);
}

/**
 * Reads the UV value out of the register 0x2C (see p. 30). The value
 * needs to be divided by 100 to represented the real UV index.
 *
 * @param sensor sensor ID
 * @return UV index * 100
 */
uint16_t getUV(int sensor) {
    uint16_t uv = (uint16_t) wiringPiI2CReadReg16(sensor, UVDATA);
    return uv;
}

/**
 * Reads the VIS value out of the register 0x26 (see p. 29f).
 *
 * @param sensor sensor ID
 * @return visible light value
 */
uint16_t getVIS(int sensor) {
    uint16_t vis = (uint16_t) wiringPiI2CReadReg16(sensor, VISDATA);
    return vis;
}

/**
 * Reads the IR value out of the register 0x24 (see p. 30).
 *
 * @param sensor sensor ID
 * @return infrared light value
 */
uint16_t getIR(int sensor) {
    uint16_t ir = (uint16_t) wiringPiI2CReadReg16(sensor, IRDATA);
    return ir;
}

/**
 * Setup the sensor and read the current UV index from the device.
 *
 * @param self python instance the method is called on
 * @param args passed arguments from calling python method
 * @return UV index
 */
static PyObject *get_UV(PyObject *self, PyObject *args) {
    int32_t sensor;
    sensor = wiringPiI2CSetup(ADDRESS);
    if (sensor < 0) {
        printf("sensor not found!\n");
        return NULL;
    }

    initSensor(sensor);
    measData data;
    data.uv = getUV(sensor);

    return Py_BuildValue("f", data.uv / 100.0);
}

/**
 * Setup the sensor and read the current IR value from the device.
 *
 * @param self python instance the method is called on
 * @param args passed arguments from calling python method
 * @return infrared light value
 */
static PyObject *get_IR(PyObject *self, PyObject *args) {
    int32_t sensor;
    sensor = wiringPiI2CSetup(ADDRESS);
    if (sensor < 0) {
        printf("sensor not found!\n");
        return NULL;
    }

    initSensor(sensor);
    measData data;
    data.ir = getIR(sensor);

    return Py_BuildValue("i", data.ir);
}

/**
 * Setup the sensor and read the current VIS value from the device.
 *
 * @param self python instance the method is called on
 * @param args passed arguments from calling python method
 * @return visible light value
 */
static PyObject *get_VIS(PyObject *self, PyObject *args) {
    int32_t sensor;
    sensor = wiringPiI2CSetup(ADDRESS);
    if (sensor < 0) {
        printf("sensor not found!\n");
        return NULL;
    }

    initSensor(sensor);
    measData data;
    data.vis = getVIS(sensor);

    return Py_BuildValue("i", data.vis);
}

/**
 * Method definitions that are visible in Python afterwards
 */
static PyMethodDef lightSensor_methods[] = {
        {"get_UV", get_UV, METH_VARARGS},
        {"get_IR", get_IR, METH_VARARGS},
        {"get_VIS", get_VIS, METH_VARARGS},
        {NULL, NULL, 0, NULL} /* Sentinel */
};

/**
 * Initializes the module and methods that can be called
 * from python.
 */
void initlightSensor(void) {
    PyImport_AddModule("lightSensor");
    Py_InitModule("lightSensor", lightSensor_methods);
}

int main(int argc, char **argv) {
    /* Initialize the Python interpreter. */
    Py_Initialize();
    /* Initialize methods */
    initlightSensor();

/*
    int sensor = wiringPiI2CSetup(ADDRESS);
    if (sensor < 0) {
        printf("sensor not found!\n");
        return 1;
    }

    initSensor(sensor);

    measData data;
    data.uv = getUV(sensor);
    data.ir = getIR(sensor);
    data.vis = getVIS(sensor);

    printf("UV: %.2f\n"
           "IR: %i\n"
           "VIS: %i\n", data.uv / 100.0, data.ir, data.vis);
*/

}
