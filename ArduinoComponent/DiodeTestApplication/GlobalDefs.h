namespace global
{
    namespace pins
    {
        constexpr auto HALL_SENSOR_A = 6;
        constexpr auto HALL_SENSOR_B = 7;
        constexpr int LIGHT_SENSOR_MAIN = A0;
        constexpr int LIGHT_SENSOR_REF = A1;
    }

    namespace steps
    {
        constexpr auto MOTOR_BASE_MAX = 200;
        constexpr auto MOTOR_DIODE_MAX = 2048;

        constexpr auto MOTOR_BASE_MEASURE_STEP = 8;
        constexpr auto MOTOR_DIODE_MEASURE_STEP = 64;
    }

    namespace time
    {
        constexpr auto DELAY_BETW_MEASURE_STEPS = 100; 
    }
}