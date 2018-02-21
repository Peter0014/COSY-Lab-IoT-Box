cmake_minimum_required(VERSION 3.9)
project(src C)

set(CMAKE_C_STANDARD 99)

set(PYTHON_EXECUTABLE "C:/Python27")
set(PYTHON_INCLUDE_DIR "C:/Python27/include")
set(PYTHON_LIBRARIES "C:/Python27/Lib")

add_executable(src BME280_TempSensor.c BME280_TempSensor.h SI1145_LightSensor.h SI1145_LightSensor.c CCS811_AirQuality_Wrapper.c)
include_directories(${PYTHON_INCLUDE_DIR})
