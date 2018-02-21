//
// Created by peter on 06.02.2018.
//

#include <Python.h>
#include <unistd.h>
#include "CCS811_AirQuality.c"

/* I2C */
#define ADDRESS       0x5A
#define CHANNEL       1

int isInit = 0;

void initSensor() {
    if (isInit == 0) {
        int sensor;
        sensor = ccs811Init(CHANNEL, ADDRESS);
        if (sensor < 0) {
            printf("sensor not found!\n");
        } else {
            isInit = 1;
        }
    }
}

/**
 * Setup the sensor and read the current IR value from the device.
 *
 * @param self python instance the method is called on
 * @param args passed arguments from calling python method
 * @return infrared light value
 */
static PyObject *get_eCO2(PyObject *self, PyObject *args) {
    initSensor();
    if (isInit == 0) {
        return Py_BuildValue("i", -1);
    }

    int eCO2, TVOC;
    if (!ccs811ReadValues(&eCO2, &TVOC))
    {
        isInit = 0;
        initSensor();
        return Py_BuildValue("i", -1);
    }

    return Py_BuildValue("i", eCO2);
}

/**
 * Setup the sensor and read the current IR value from the device.
 *
 * @param self python instance the method is called on
 * @param args passed arguments from calling python method
 * @return infrared light value
 */
static PyObject *get_TVOC(PyObject *self, PyObject *args) {
    initSensor();
    if (isInit == 0) {
        return Py_BuildValue("i", -1);
    }

    int eCO2, TVOC;
    if (!ccs811ReadValues(&eCO2, &TVOC))
    {
        isInit = 0;
        initSensor();
        return Py_BuildValue("i", -1);
    }

    return Py_BuildValue("i", TVOC);
}

/**
 * Method definitions that are visible in Python afterwards
 */
static PyMethodDef airSensor_methods[] = {
        {"get_eCO2", get_eCO2, METH_VARARGS},
        {"get_TVOC", get_TVOC, METH_VARARGS},
        {NULL, NULL, 0, NULL} /* Sentinel */
};

/**
 * Initializes the module and methods that can be called
 * from python.
 */
void initairSensor(void) {
    PyImport_AddModule("airSensor");
    Py_InitModule("airSensor", airSensor_methods);
}

int main(int argc, char **argv) {
    /* Initialize the Python interpreter. */
    Py_Initialize();
    /* Initialize methods */
    initairSensor();

    //test();
}