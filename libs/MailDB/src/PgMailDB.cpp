#include <iostream>
#include <memory>

#include "PgMailDB.h"
#include "DBCredentials.h"
#include "ConnectionException.h"

namespace ISXMailDB
{
    PgMailDB::PgMailDB(std::string_view host_name)
        : IMailDB(host_name)
    {
    }

    PgMailDB::~PgMailDB()
    {   
        Disconnect();
    }

    bool PgMailDB::Connect(const Credentials& credentials)
    {
        try {
            std::string connect_string = "dbname=" + credentials.m_db_name + 
                                         " user=postgres password=" + credentials.m_db_password +
                                         " hostaddr=127.0.0.1 port=5432";
            m_conn = std::make_unique<pqxx::connection>(connect_string);

            if (!IsConnected()) 
            {
                throw ConnectionException();
            }
            return true;

        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
    }

    void PgMailDB::Disconnect()
    {
        try
        {
            if(!IsConnected())
            {
               throw ConnectionException(); 
            }

            m_conn->close();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    bool PgMailDB::IsConnected() const
    {
        return m_conn->is_open();
    }

    bool PgMailDB::InsertEmail(const std::string_view sender, const std::string_view receiver, 
                                 const std::string_view subject, const std::string_view body)
    {
        return false;
    }

    bool PgMailDB::DeleteEmail(const std::string_view user_name)
    {
        return false;
    }

    bool PgMailDB::DeleteUser(const std::string_view user_name, const std::string_view hash_password)
    {
        return false;
    }
}