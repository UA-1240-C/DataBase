#pragma once

#include <vector>
#include <string>

#include <pqxx/pqxx>

namespace ISXMailDB
{

class IMailDB
{
public:
    IMailDB(const std::string_view host_name)
    {
        if (host_name.empty())
        {
            throw 1; // change
        }
        m_host_name = host_name; 
    }
    virtual ~IMailDB() = default;


    // TODO: Viacheslav
    virtual bool Connect(const std::string& connection_string) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() const = 0;
    // TODO: Denys
    virtual bool SignUp(const std::string_view user_name, const std::string_view hash_password) = 0;
    virtual bool Login(const std::string_view user_name, const std::string_view hash_password) = 0;

   
    // TODO: Viacheslav
    virtual bool InsertEmail(const std::string_view sender, const std::string_view receiver, 
                             const std::string_view subject, const std::string_view body) = 0;

    // TODO: Denys
    virtual bool RetrieveEmails(const std::string_view user_name) = 0;
    virtual bool RetrieveAllEmails(const std::string_view user_name) = 0;

    // TODO: Viacheslav
    virtual bool DeleteEmail(const std::string_view user_name) = 0;
    virtual bool DeleteUser(const std::string_view user_name, const std::string_view hash_password) = 0;

protected:
    // TODO
    virtual std::vector<std::vector<std::string_view>> ExecuteQuery(const std::string_view& query) = 0;

    // // ??
    virtual std::vector<std::vector<std::string_view>> RetrieveEmails(const std::string_view& criteria) = 0;

    std::string m_host_name;
    uint32_t m_host_id;
};

}
