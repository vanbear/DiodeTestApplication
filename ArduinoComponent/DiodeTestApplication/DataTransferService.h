#pragma once

#include "DataParameters.h"

using namespace data;

// ======================= DataTransferService declaration

class DataTransferService
{
public:
    void sendMessage(const String & name, const String & value);
    void sendData(const int & motorA, const int & motorB, const StepDirection & dirA, const StepDirection & dirB, const int & light);
    void sendError(const String & msg);
};

// ======================= DataTransferService definition

void DataTransferService::sendMessage(const String & name, const String & value)
{
    String msg = name + COLON + value;
    Serial.println(msg);
}

void DataTransferService::sendData(const int & motorA, const int & motorB, const StepDirection & dirA, const StepDirection & dirB, const int & light)
{
    String value = 
        String(motorA) + SEMI_COLON +
        String(motorB) + SEMI_COLON +
        String(static_cast<bool>(dirA)) + SEMI_COLON +
        String(static_cast<bool>(dirB)) + SEMI_COLON +
        String(light);
    sendMessage(DATA, value);
}

void DataTransferService::sendError(const String & msg)
{
    sendMessage(ERROR, msg);
}