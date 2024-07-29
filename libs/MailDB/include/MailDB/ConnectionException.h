#pragma once

#include "MailException.h"

namespace ISXMailDB
{
    class ConnectionException : MailException
    {
    public:
        virtual const char* what() const override
        {
            return "Bad credentials or database doesn't exist or database connection was closed previously\n";
        }
    };
}