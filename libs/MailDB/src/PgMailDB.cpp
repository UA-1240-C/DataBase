#include <iostream>
#include <memory>

#include <pqxx/except>

#include "PgMailDB.h"
#include "DBCredentials.h"

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

            pqxx::work transaction(*m_conn);
            transaction.exec_params(
                "INSERT INTO public.\"hosts\" (host_name)VALUES($1) "
                "ON CONFLICT(host_name) DO NOTHING "
                , m_host_name
            );
            transaction.commit();
        }
        catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return false;
        }

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
        try
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
        catch (...)
        {
            throw pqxx::broken_connection();
        }
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
            transaction.exec_params(
                "INSERT INTO public.\"mailBodies\" (body_content)VALUES($1) "
                "ON CONFLICT(body_content) DO NOTHING "
                , content
            );
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