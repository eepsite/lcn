#include "NetworkInfo.h"

const char* NetworkInfo::GetAddress() const
{
    return nAddress;
}

int NetworkInfo::GetCommunicationsPort() const
{
    return nCommunicationsPort;
}

int NetworkInfo::GetRegistrationPort() const
{
    return nRegistrationPort;
}

int NetworkInfo::GetStreamPort() const
{
    return nStreamPort;
}
