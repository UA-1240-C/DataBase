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
    pqxx::work tx(*m_conn);

    try
    {
        tx.exec_params0("SELECT 1 FROM users WHERE host_id = $1 AND user_name = $2", m_host_id, user_name);

        tx.exec_params(
            "INSERT INTO users (host_id, user_name, password_hash)"
            "VALUES ($1, $2, $3)",
            m_host_id, user_name, hash_password);
    }
    catch (pqxx::unexpected_rows &e)
    {
        throw MailException("User already exists");
    }
    tx.commit();
}

void PgMailDB::InsertHost(const std::string_view host_name)
{
    pqxx::work tx(*m_conn);
    try
    {
        m_host_id = tx.query_value<uint32_t>("SELECT host_id FROM hosts WHERE host_name = " + tx.quote(m_host_name));
    }
    catch (pqxx::unexpected_rows& e)
    {
        pqxx::result host_id_result = tx.exec_params(
            "INSERT INTO hosts (host_name) VALUES ( $1 ) RETURNING host_id", m_host_name);

        m_host_id = host_id_result[0][0].as<uint32_t>();
    }
    tx.commit();
}

void PgMailDB::Login(const std::string_view user_name, const std::string_view hash_password)
{
    pqxx::nontransaction ntx(*m_conn);

    try
    {
        ntx.exec_params1(
            "SELECT 1 FROM users "
            "WHERE host_id = $1 AND user_name = $2 AND password_hash = $3",
            m_host_id, user_name, hash_password);
    }
    catch (const pqxx::unexpected_rows &e)
    {
        throw MailException("Invalid user name or password");
    }
}

std::string PgMailDB::GetPasswordHash(const std::string_view user_name)
{
    pqxx::nontransaction ntx(*m_conn);
    try
    {
        std::string password_hash = ntx.query_value<std::string>("SELECT password_hash FROM users "
        "WHERE user_name = " + ntx.quote(user_name) 
        +  "AND host_id = " + ntx.quote(m_host_id));
        return password_hash;
    }
    catch(const pqxx::unexpected_rows &e)
    {
        return {};
    }
    
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
    pqxx::nontransaction ntx(*m_conn);

    uint32_t user_id = RetriveUserId(user_name, ntx);

    std::string query =
        "WITH filtered_emails AS ( "
        "    SELECT sender_id, subject, mail_body_id, sent_at "
        "    FROM \"emailMessages\" "
        "    WHERE recipient_id = " +
        ntx.quote(user_id) + 
        (should_retrieve_all ? "": " AND is_received = FALSE")+
        ")"
        "SELECT u.user_name AS sender_name, f.subject, m.body_content "
        "FROM filtered_emails AS f "
        "LEFT JOIN users AS u ON u.user_id = f.sender_id "
        "LEFT JOIN \"mailBodies\" AS m ON m.mail_body_id = f.mail_body_id "
        "ORDER BY f.sent_at DESC; ";

    std::vector<Mail> resutl_mails;

    for (auto& [sender, subject, body] : ntx.query<std::string, std::string, std::string>(query))
    {
        resutl_mails.emplace_back(user_name, sender, subject, body);
    }

    return resutl_mails;
}

void PgMailDB::MarkEmailsAsReceived(const std::string_view user_name)
{

    pqxx::work tx(*m_conn);

    uint32_t user_id = RetriveUserId(user_name, tx);

    tx.exec_params(
        "UPDATE \"emailMessages\" "
        "SET is_received = TRUE "
        "WHERE recipient_id = $1 "
        "AND is_received = FALSE"
        , user_id);

    tx.commit();
}

bool PgMailDB::UserExists(const std::string_view user_name)
{
    pqxx::nontransaction ntx(*m_conn);
    try 
    {
        ntx.exec_params1("SELECT 1 FROM users WHERE host_id = $1 AND user_name = $2", m_host_id, user_name);
        return true;
    }
    catch(pqxx::unexpected_rows &e)
    {
        return false;
    }
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
    try
    {
        return ntx.query_value<uint32_t>("SELECT user_id FROM users WHERE user_name = " + ntx.quote(user_name) 
                                         + "  AND host_id = " + ntx.quote(m_host_id));
    }
    catch (pqxx::unexpected_rows &e)
    {
        throw MailException("User doesn't exist");
    };
}  


void PgMailDB::PerformEmailInsertion(const uint32_t sender_id, const uint32_t receiver_id,
                                const std::string_view subject, const uint32_t body_id, pqxx::transaction_base& transaction)
{
    return;
}
}