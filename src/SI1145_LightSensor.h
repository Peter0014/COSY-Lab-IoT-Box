/**
 * <Program>
 * SI1145_LightSensor.h
 *
 * <Started>
 * November 2017
 *
 * <Author>
 * Peter Klosowski
 *
 * <Description>
 * Header file for the SI1145 Sensor from Silicon Labs. Defines
 * all addresses that are necessarry to read out ultraviolet,
 * infrared and visible light and introduces a structure,
 * which holds the readable sensor data values.
 *
 * <Sources>
 * Accessed on 11.01.2018 - SI1145 Datasheet:
 *      https://www.silabs.com/documents/public/data-sheets/Si1145-46-47.pdf
 * Accessed on 11.01.2018
 *      https://github.com/adafruit/Adafruit_SI1145_Library
 */

#ifndef SRC_SI1145_LIGHTSENSOR_H
#define SRC_SI1145_LIGHTSENSOR_H

#include <inttypes.h>
#include <stdlib.h>
#include "wiringPiI2C.h"

/* I2C ADDRESS */
#define ADDRESS       0x60

/* COMMANDS */
#define SI1145_PARAM_SET   0xA0
#define SI1145_RESET       0x01
#define SI1145_PSALS_AUTO  0x0F

/* PARAMETERS */
#define SI1145_PARAM_CHLIST          0x01
#define SI1145_PARAM_CHLIST_ENUV     0x80
#define SI1145_PARAM_CHLIST_ENALSIR  0x20
#define SI1145_PARAM_CHLIST_ENALSVIS 0x10

/* REGISTERS */
#define SI1145_REG_INTCFG         0x03
#define SI1145_REG_INTCFG_INTOE   0x01

#define SI1145_REG_IRQEN 0x04
#define SI1145_REG_IRQEN_ALSEVERYSAMPLE 0x01

#define SI1145_REG_IRQMODE1 0x05
#define SI1145_REG_IRQMODE2 0x06

#define SI1145_REG_HWKEY     0x07
#define SI1145_REG_MEASRATE0 0x08
#define SI1145_REG_MEASRATE1 0x09
#define SI1145_REG_UCOEFF0  0x13
#define SI1145_REG_UCOEFF1  0x14
#define SI1145_REG_UCOEFF2  0x15
#define SI1145_REG_UCOEFF3  0x16
#define SI1145_REG_PARAMWR  0x17
#define SI1145_REG_COMMAND  0x18
#define SI1145_REG_RESPONSE 0x20
#define SI1145_REG_IRQSTAT  0x21

/* DEFAULT VALUES */
#define SI1145_DEF_UCOEFF0 0x7B
#define SI1145_DEF_UCOEFF1 0x6B
#define SI1145_DEF_UCOEFF2 0x01
#define SI1145_DEF_UCOEFF3 0x00

/* SENSOR DATA */
#define UVDATA  0x2C
#define VISDATA 0x22
#define IRDATA  0x24

typedef struct {
    uint16_t uv;
    uint16_t ir;
    uint16_t vis;
} measData;

#endif //SRC_SI1145_LIGHTSENSOR_H
