namespace global
{
    namespace pins
    {
    constexpr auto HALL_SENSOR = 7;
    }

    namespace steps
    {
    constexpr auto MOTOR_BASE_MAX = 200;
    constexpr auto MOTOR_DIODE_MAX = 2048;

    constexpr auto MOTOR_BASE_MEASURE_STEP = 2;
    constexpr auto MOTOR_DIODE_MEASURE_STEP = 32;
    }

    namespace time
    {
        constexpr auto DELAY_BETW_MEASURE_STEPS = 100; 
    }
}