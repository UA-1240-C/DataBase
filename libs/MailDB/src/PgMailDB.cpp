#include <iostream>
#include <memory>

#include <pqxx/except>

#include "PgMailDB.h"

#include <iostream>
#include "MailException.h"

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

bool PgMailDB::Connect(const std::string& connection_string)
    {
        try {
            m_conn = std::make_unique<pqxx::connection>(connection_string);
            InsertHost(m_host_name);
        }
        catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        std::cout << "con";
        return true;

    }

void PgMailDB::Disconnect()
{
    try
    {
        if (!IsConnected())
        {
            throw pqxx::broken_connection();
        }
        m_conn->close();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

bool PgMailDB::IsConnected() const
{
    if (m_conn) 
    {
        return m_conn->is_open();
    }
    else
    {
        throw pqxx::broken_connection();
    }
}

bool PgMailDB::SignUp(const std::string_view user_name, const std::string_view hash_password)
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

    return true;
}

void PgMailDB::InsertHost(const std::string_view host_name)
{
    pqxx::work tx(*m_conn);
    try
    {
        m_host_id = tx.query_value<uint32_t>("SELECT host_id FROM hosts WHERE host_name = " + tx.quote(m_host_name));
    }
    catch (std::exception &e)
    {
        pqxx::result host_id_result = tx.exec_params(
            "INSERT INTO hosts (host_name) VALUES ( $1 ) RETURNING host_id", m_host_name);

        m_host_id = host_id_result[0][0].as<uint32_t>();
    }
    tx.commit();
}

bool PgMailDB::Login(const std::string_view user_name, const std::string_view hash_password)
{
    pqxx::nontransaction ntx(*m_conn);

    try
    {
        ntx.exec_params1(
            "SELECT 1 FROM users "
            "WHERE host_id = $1 AND user_name = $2 AND password_hash = $3",
            m_host_id, user_name, hash_password);
    }
    catch (pqxx::unexpected_rows &e)
    {
        throw MailException("Invalid user name or password");
    }

    return true;
}

 std::vector<std::vector<std::string>> PgMailDB::RetrieveUserInfo(const std::string_view user_name)
    {
        try 
        {
            if (!IsConnected())
            {
                throw pqxx::broken_connection();
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return {};
        }


        pqxx::nontransaction nontransaction(*m_conn);
        pqxx::result user_query_result;
        if (user_name.empty())
        {
            user_query_result = nontransaction.exec_params(
                "SELECT * FROM public.\"users\""
            );
        }
        else
        {
            user_query_result = nontransaction.exec_params(
                "SELECT * FROM public.\"users\" "
                "WHERE user_name = $1"
                , nontransaction.quote(user_name)
            );
        }

        std::vector<std::vector<std::string>> info;
        if (!user_query_result.empty())
        {
            WriteQueryResultToStorage(user_query_result, info);
            return info;
        }

        return std::vector<std::vector<std::string>>();
    }

bool PgMailDB::InsertEmailContent(const std::string_view content)
{
    try
    {
        if (!IsConnected())
        {
            throw pqxx::broken_connection();
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    try {
        pqxx::work transaction(*m_conn);
        try 
        {
            uint32_t content_id = transaction.query_value<uint32_t>(
                "SELECT mail_body_id FROM public.\"mailBodies\" WHERE body_content = " + transaction.quote(content)
            );
        }
        catch (const std::exception& e) 
        {
            transaction.exec_params(
                "INSERT INTO public.\"mailBodies\" (body_content)VALUES($1) "
                "ON CONFLICT(body_content) DO NOTHING "
                , content
            );
        }
        transaction.commit();

    }
    catch (const std::exception& e) {
        std::cerr << "Transaction failed: " << e.what() << std::endl;
        return false;
    }

    return true;
}

std::vector<std::vector<std::string>> PgMailDB::RetrieveEmailContentInfo(const std::string_view content)
{
    try
    {
        if (!IsConnected())
        {
            throw pqxx::broken_connection();
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return std::vector<std::vector<std::string>>();
    }

    pqxx::nontransaction nontransaction(*m_conn);
    pqxx::result content_query_result;
    if (content.empty())
    {
        content_query_result = nontransaction.exec_params(
            "SELECT * FROM public.\"mailBodies\""
        );
    }
    else
    {
        content_query_result = nontransaction.exec_params(
            "SELECT * FROM public.\"mailBodies\" "
            "WHERE body_content = $1"
            , nontransaction.quote(content)
        );
    }

    std::vector<std::vector<std::string>> info{};
    if (!content_query_result.empty())
    {
        WriteQueryResultToStorage(content_query_result, info);
        return info;
    }

    return std::vector<std::vector<std::string>>();
}

bool PgMailDB::InsertEmail(const std::string_view sender, const std::string_view receiver,
                                const std::string_view subject, const std::string_view body)
{
    try
    {
        if (!IsConnected())
        {
            throw pqxx::broken_connection();
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    InsertEmailContent(body);

    uint32_t sender_id, receiver_id, body_id;
    {
        pqxx::nontransaction nontransaction(*m_conn);
        try {
            sender_id = nontransaction.query_value<uint32_t>(
                "SELECT user_id FROM public.\"users\" WHERE user_name = " + nontransaction.quote(sender)
            );
            receiver_id = nontransaction.query_value<uint32_t>(
                "SELECT user_id FROM public.\"users\" WHERE user_name = " + nontransaction.quote(receiver)
            );
            body_id = nontransaction.query_value<uint32_t>(
                "SELECT mail_body_id FROM public.\"mailBodies\" WHERE body_content = " + nontransaction.quote(body)
            );
        }
        catch(const pqxx::unexpected_rows& e)
        {
            std::cout << "Given value doesn't exist in database. Aborting operation.\n";
            return false;
        }
    }
    
    try {
        pqxx::work transaction(*m_conn);
        transaction.exec_params(
            "INSERT INTO public.\"emailMessages\" (sender_id, recipient_id, subject, mail_body_id, is_received) "
            "VALUES ($1, $2, $3, $4, false) "
            , sender_id, receiver_id,
            subject, body_id
        );
        transaction.commit();
    }
    catch (const std::exception& e) {
        std::cerr << "Transaction failed: " << e.what() << std::endl;
        return false;
    }

    return true;
}

std::vector<Mail> PgMailDB::RetrieveEmails(const std::string_view user_name, bool should_retrieve_all) const
{
    pqxx::work tx(*m_conn);

    uint32_t user_id = RetriveUserId(user_name, tx);

    std::string additional_condition = "";
    if (!should_retrieve_all)
    {
        additional_condition = " AND is_received = FALSE";
    }

    std::string query =
        "WITH filtered_emails AS ( "
        "    SELECT sender_id, subject, mail_body_id "
        "    FROM emailMessages "
        "    WHERE recipient_id = " +
        tx.quote(user_id) + additional_condition +
        ")"
        "SELECT u.user_name AS sender_name, f.subject, m.body_content "
        "FROM filtered_emails AS f "
        "LEFT JOIN users AS u ON u.user_id = f.sender_id "
        "LEFT JOIN mailBodies AS m ON m.mail_body_id = f.mail_body_id; ";

    std::vector<Mail> resutl_mails;

    for (auto [sender, subject, body] : tx.query<std::string, std::string, std::string>(query))
    {
        resutl_mails.emplace_back(sender, subject, body);
    }

    tx.exec_params(
        "UPDATE emailMessages "
        "SET is_received = TRUE "
        "FROM users "
        "WHERE emailMessages.recipient_id = users.user_id "
        "AND users.user_name = $1; ",
        user_name);

    tx.commit();

    return resutl_mails;
}

bool PgMailDB::DeleteEmail(const std::string_view user_name)
{
    try
    {
        if (!IsConnected())
        {
            throw pqxx::broken_connection();
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    uint32_t user_info;
    {
        pqxx::nontransaction nontransaction(*m_conn);
        try {
            user_info = nontransaction.query_value<uint32_t>(
                "SELECT user_id FROM public.\"users\" WHERE user_name = " + nontransaction.quote(user_name)
            );
        }
        catch (const pqxx::unexpected_rows& e)
        {
            std::cout << "Given value doesn't exist in database. Aborting operation.\n";
            return false;
        }
    }

    try
    {
        pqxx::work transaction(*m_conn);
        transaction.exec_params(
            "DELETE FROM public.\"emailMessages\" "
            "WHERE sender_id = $1 OR recipient_id = $1"
            , user_info
        );
        transaction.commit();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Transaction failed: " << e.what() << std::endl;
        return false;
    }

    return true;
}

bool PgMailDB::DeleteUser(const std::string_view user_name, const std::string_view hash_password)
{
    try
    {
        if (!IsConnected())
        {
            throw pqxx::broken_connection();
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    if (!DeleteEmail(user_name))
    {
        return false;
    }

    try
    {
        pqxx::work transaction(*m_conn);
        transaction.exec_params(
            "DELETE FROM public.\"users\" "
            "WHERE user_name = $1 AND password = $2"
            , transaction.esc(user_name), transaction.esc(hash_password)
        );
        transaction.commit();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Transaction failed: " << e.what() << std::endl;
        return false;
    }

    return true;
} 

uint32_t PgMailDB::RetriveUserId(const std::string_view user_name, pqxx::transaction_base &ntx) const
{
    try
    {
        return ntx.query_value<uint32_t>("SELECT user_id FROM users WHERE user_name = " + ntx.quote(user_name) + "  AND host_id = 1");
    }
    catch (pqxx::unexpected_rows &e)
    {
        throw MailException("User doesn't exist");
    };
}  

void PgMailDB::WriteQueryResultToStorage(const pqxx::result& query_result, std::vector<std::vector<std::string>>& info)
{
    for (auto&& row : query_result)
    {
        info.push_back(std::vector<std::string>());
        for (auto&& column : row)
        {
            info.back().push_back(column.as<std::string>());
        }
    }
}

}