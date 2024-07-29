#pragma once

#include <string>
#include <initializer_list>

namespace ISXMailDB
{
    struct Credentials
    {
        Credentials(const std::string& db_name, const std::string& db_password)
            : m_db_name{db_name}, m_db_password{db_password}
        {
        }
        
        std::string m_db_name{};
        std::string m_db_password{};
    };  

};