#pragma once

namespace gd
{
    namespace pins
    {
        constexpr int HALL_SENSOR_A = 6;
        constexpr int HALL_SENSOR_B = 7;
        constexpr int LIGHT_SENSOR_MAIN = A0;
        constexpr int LIGHT_SENSOR_REF = A1;
        constexpr int LED = 13;
        constexpr int MOTOR_SLEEP = 12; 
    }

    namespace steps
    {
        constexpr int MOTOR_BASE_MAX = 100;
        constexpr int MOTOR_DIODE_MAX = 2048;

        constexpr int MOTOR_BASE_MEASURE_STEP = 4;
        constexpr int MOTOR_DIODE_MEASURE_STEP = 64;
    }

    namespace time
    {
        constexpr int DELAY_BETWEEN_MEASURE_STEPS = 100; 
    }
}