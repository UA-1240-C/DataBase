#include "PgMailDB.h"

#include <iostream>
#include "MailException.h"

namespace ISXMailDB
{

bool PgMailDB::Connect(const std::string& connection_string)
{   
    try {
        m_conn = std::make_unique<pqxx::connection>(connection_string);
        if (!m_conn->is_open()) 
        {
            throw 1;
        }

        // add host to hosts table if 
        pqxx::work tx(*m_conn);

        try 
        {
            m_host_id = tx.query_value<uint32_t>("SELECT host_id FROM hosts WHERE host_name = " + tx.quote(m_host_name));
        }
        catch (std::exception& e)
        {
            pqxx::result host_id_result = tx.exec_params(
            "INSERT INTO hosts (host_name) VALUES ( $1 ) RETURNING host_id"
            , m_host_name);

            m_host_id = host_id_result[0][0].as<uint32_t>();
        }
        
        tx.commit();

        std::cout << "Connected successfully to " << m_conn->dbname() << "\n Host name : " << m_host_name 
                  <<" Host id : " << m_host_id << "\n";
        return true;
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Connection to database failed: " << e.what() << std::endl;
        throw;
    }
}

void PgMailDB::Disconnect() 
{
    //TODO
    if (m_conn && m_conn->is_open())
    {
        m_conn.reset();
    }
}

bool PgMailDB::IsConnected() const
{
   // TODO
    return true;
}

bool PgMailDB::SignUp(const std::string_view user_name, const std::string_view hash_password)
{
    pqxx::work tx(*m_conn);

    pqxx::result find_user = tx.exec_params("SELECT 1 FROM users WHERE host_id = $1 AND user_name = $2", m_host_id, user_name);
    if (find_user.affected_rows() != 0)
    {
        throw std::logic_error("User already exists");
    }

    tx.exec_params(
        "INSERT INTO users (host_id, user_name, password_hash)"
        "VALUES ($1, $2, $3)"
    , m_host_id, user_name, hash_password);

    tx.commit();

    std::cout << "user signed up\n";
    return true;
}

bool PgMailDB::Login(const std::string_view user_name, const std::string_view hash_password)
{
    pqxx::work tx(*m_conn);

    pqxx::result find_user = tx.exec_params(
        "SELECT 1 FROM users "
        "WHERE host_id = $1 AND user_name = $2 AND password_hash = $3"
        , m_host_id, user_name, hash_password);

    if (find_user.affected_rows() == 0)
    {
        throw std::logic_error("Invalid user name or password");
    }
    std::cout << "users is logged in" << std::endl;

    return true;
}

bool PgMailDB::InsertEmail(const std::string_view sender, const std::string_view receiver, const std::string_view subject, const std::string_view body)
{
    return false;
}
bool PgMailDB::RetrieveEmails(const std::string_view user_name)
{
    return false;
}
bool PgMailDB::RetrieveAllEmails(const std::string_view user_name)
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
std::vector<std::vector<std::string_view>> PgMailDB::ExecuteQuery(const std::string_view &query)
{
    return std::vector<std::vector<std::string_view>>();
}
std::vector<std::vector<std::string_view>> PgMailDB::RetrieveEmails(const std::string_view &criteria)
{
    return std::vector<std::vector<std::string_view>>();
}

}