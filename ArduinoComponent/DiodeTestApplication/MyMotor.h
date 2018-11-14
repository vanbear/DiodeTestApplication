
// ======================= DEFS
enum class StepMode
{
    FULL_STEP = 0,
    HALF_STEP,
    QUARTER_STEP,
    EIGHT_STEP
};

struct Pins
{
    Pins(int step, int dir, int MS1 = -1, int MS2 = -1) :
        step(step), direction(dir), MS1(MS1), MS2(MS2)
    {}
    int step;
    int direction;
    int MS1;
    int MS2;
};

// ======================= MyMotor declaration

class MyMotor
{
public:
    MyMotor(Pins, unsigned int resolution, bool dir = false, 
        StepMode mode = StepMode::FULL_STEP);
    void doStep();
    void doNSteps(int n);
    void changeDirection(bool dir);
    void toggleDirection();
    void setSpeed(float spd);
    void setStepMode(StepMode);

private:
    unsigned int m_resolution;
    bool m_direction;
    Pins m_pins;
    float m_speed = 100;

    void initPinModes();
    bool isPinSet(int pin) const { return (pin > -1); }
};

// ======================= MyMotor definition

MyMotor::MyMotor(Pins pins, unsigned int res, bool dir, StepMode mode) : m_pins(pins), m_resolution(res)
{
    initPinModes();
    changeDirection(dir);
    setSpeed(m_speed);
    digitalWrite(m_pins.step, LOW);
}

void MyMotor::initPinModes()
{
    pinMode(m_pins.step, OUTPUT);
    pinMode(m_pins.direction, OUTPUT);
    if (isPinSet(m_pins.MS1))
    {
        pinMode(m_pins.MS1, OUTPUT);
    }
    if (isPinSet(m_pins.MS2))
    {
        pinMode(m_pins.MS2, OUTPUT);
    }
}
    
void MyMotor::changeDirection(bool dir)
{
    m_direction = dir;
    digitalWrite(m_pins.direction, (m_direction ? HIGH : LOW));
}

void MyMotor::toggleDirection()
{
    m_direction = !m_direction;
    changeDirection(m_direction);
}

void MyMotor::setSpeed(float spd)
{
    m_speed = spd;
}

void MyMotor::setStepMode(StepMode stepMode)
{
    if (!m_pins.MS1 and !m_pins.MS2)
    {
        return;
    }

    bool MS1 = false;
    bool MS2 = false;
    switch (stepMode)
    {
        case StepMode::FULL_STEP:
        default:
            MS1 = false;
            MS2 = false;
            break;
        case StepMode::HALF_STEP:
            MS1 = true;
            MS2 = false;
            break;
        case StepMode::QUARTER_STEP:
            MS1 = false;
            MS2 = true;
            break;
        case StepMode::EIGHT_STEP:
            MS1 = true;
            MS2 = true;
            break;
    }
    digitalWrite(m_pins.MS1, (MS1 ? HIGH : LOW));
    digitalWrite(m_pins.MS2, (MS2 ? HIGH : LOW));
}

void MyMotor::doStep()
{
    digitalWrite(m_pins.step,HIGH);
    delay(m_speed);
    digitalWrite(m_pins.step,LOW);
    delay(m_speed);
}

void MyMotor::doNSteps(int n)
{
    for (int i = 0; i <= n; i++)
    {
        doStep();
    }
}
