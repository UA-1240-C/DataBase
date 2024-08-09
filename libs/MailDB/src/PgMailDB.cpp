#include <memory>

#include "MailException.h"
#include "PgMailDB.h"

namespace ISXMailDB
{
PgMailDB::PgMailDB(std::string_view host_name)
    : IMailDB(host_name)
{
}

PgMailDB::PgMailDB(const PgMailDB& other) : IMailDB(other.m_host_name)
{
}

PgMailDB::~PgMailDB()
{   
    Disconnect();
}


void PgMailDB::Connect(const std::string& connection_string)
{
    return;
}

void PgMailDB::Disconnect()
{
    return;

}

bool PgMailDB::IsConnected() const
{
    return false;
}

void PgMailDB::SignUp(const std::string_view user_name, const std::string_view hash_password)
{
    return;

}

void PgMailDB::InsertHost(const std::string_view host_name)
{
        return;
}

void PgMailDB::Login(const std::string_view user_name, const std::string_view hash_password)
{
    return;
}

std::string PgMailDB::GetPasswordHash(const std::string_view user_name)
{
    return std::string();    
}

std::vector<User> PgMailDB::RetrieveUserInfo(const std::string_view user_name)
{
    return std::vector<User>();
}

std::vector<std::string> PgMailDB::RetrieveEmailContentInfo(const std::string_view content)
{
    return std::vector<std::string>();
}

void PgMailDB::InsertEmail(const std::string_view sender, const std::string_view receiver,
                                const std::string_view subject, const std::string_view body)
{
    return;
}

void PgMailDB::InsertEmail(const std::string_view sender, const std::vector<std::string_view> receivers, const std::string_view subject, const std::string_view body)
{
    return;
}

std::vector<Mail> PgMailDB::RetrieveEmails(const std::string_view user_name, bool should_retrieve_all) const
{
    return {};
}

void PgMailDB::MarkEmailsAsReceived(const std::string_view user_name)
{
    return;
}

bool PgMailDB::UserExists(const std::string_view user_name)
{
    return false;
}

void PgMailDB::DeleteEmail(const std::string_view user_name)
{
    return;
}

void PgMailDB::DeleteUser(const std::string_view user_name, const std::string_view hash_password)
{
    return;
} 

uint32_t PgMailDB::InsertEmailContent(const std::string_view content, pqxx::transaction_base& transaction)
{
    return 1;
}

uint32_t PgMailDB::RetriveUserId(const std::string_view user_name, pqxx::transaction_base &ntx) const
{
    return 1;
}  

void PgMailDB::PerformEmailInsertion(const uint32_t sender_id, const uint32_t receiver_id,
                                const std::string_view subject, const uint32_t body_id, pqxx::transaction_base& transaction)
{
    return;
}
}