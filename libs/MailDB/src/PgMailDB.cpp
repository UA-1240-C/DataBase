#include "PgMailDB.h"

#include <iostream>
#include "MailException.h"

namespace ISXMailDB
{

    bool PgMailDB::Connect(const std::string &connection_string)
    {
        try
        {
            m_conn = std::make_unique<pqxx::connection>(connection_string);
            if (!m_conn->is_open())
            {
                // throw MailException("exception");
                throw 1;
            }

            InsertHost(m_host_name);

            return true;
        }
        catch (const std::exception &e)
        {
            throw;
        }
    }

    void PgMailDB::Disconnect()
    {
        // TODO
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
            throw std::logic_error("User already exists");
        }

        tx.commit();

        return true;
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
            throw std::logic_error("Invalid user name or password");
        }

        return true;
    }

    bool PgMailDB::InsertEmail(const std::string_view sender, const std::string_view receiver, const std::string_view subject, const std::string_view body)
    {
        return false;
    }

    std::vector<Mail> PgMailDB::RetrieveEmails(const std::string_view user_name, bool should_retrieve_all) const
    {
        pqxx::work tx(*m_conn);

        // add check if user exist with current host
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
        return false;
    }
    bool PgMailDB::DeleteUser(const std::string_view user_name, const std::string_view hash_password)
    {
        return false;
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

    uint32_t PgMailDB::RetriveUserId(const std::string_view user_name, pqxx::transaction_base &ntx) const
    {
        try
        {
            return ntx.query_value<uint32_t>("SELECT user_id FROM users WHERE user_name = " + ntx.quote(user_name) + "  AND host_id = 1");
        }
        catch (pqxx::unexpected_rows &e)
        {
            throw std::logic_error("user doesn't exist");
        };
    }
}