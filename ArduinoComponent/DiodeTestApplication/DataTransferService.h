#include "DataParameters.h"

using namespace data;

// ======================= DataTransferService declaration

class DataTransferService
{
public:
    DataTransferService();
    void sendMessage(const String & name, const String & value);
    void sendData(const int & motorA, const int & motorB, const StepDirection & dirA, const StepDirection & dirB, const int & light);
    void sendError(const String & what);
};

// ======================= DataTransferService definition

DataTransferService::DataTransferService()
{
    Serial.begin(9600);
}

void DataTransferService::sendMessage(const String & name, const String & value)
{
    String msg = NAME_PREFIX + name + BRACKET_END + VALUE_PREFIX + value + BRACKET_END;
    Serial.println(msg);
}

void DataTransferService::sendData(const int & motorA, const int & motorB, const StepDirection & dirA, const StepDirection & dirB, const int & light)
{
    String value = 
        MOTOR_A + COLON + String(motorA) + SEMI_COLON +
        MOTOR_B + COLON + String(motorB) + SEMI_COLON +
        DIR_A + COLON + String(static_cast<bool>(dirA)) + SEMI_COLON +
        DIR_B + COLON + String(static_cast<bool>(dirB)) + SEMI_COLON +
        LIGHT + COLON + String(light);
    sendMessage(DATA, value);
}

void DataTransferService::sendError(const String & wut)
{
    sendMessage(ERROR, wut);
}