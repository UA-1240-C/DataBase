#include <iostream>
#include <memory>
#include <ctime>

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

            m_transaction = std::make_unique<pqxx::work>(*m_conn);

            m_transaction->exec_params(
                "INSERT INTO public.\"hosts\" (host_name)VALUES($1) "
                "ON CONFLICT(host_name) DO NOTHING "
                , m_host_name);

            m_transaction->commit();

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

    std::vector<std::vector<std::string>> PgMailDB::RetrieveUserInfo(const std::string_view user_name)
    {
        StartTransaction();

        pqxx::result user_query_result;
        if (user_name.empty())
        {
            user_query_result = m_transaction->exec_params(
                "SELECT * FROM public.\"users\""
            );
        }
        else
        {
            user_query_result = m_transaction->exec_params(
                "SELECT * FROM public.\"users\" "
                "WHERE user_name = $1"
                , m_transaction->esc(user_name)
            );
        }

        CommitTransaction();

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
        StartTransaction();
        m_transaction->exec_params(
            "INSERT INTO public.\"mailBodies\" (body_content)VALUES($1) "
            "ON CONFLICT(body_content) DO NOTHING "
            , content
        );
        CommitTransaction();

        return true;
    }

    std::vector<std::vector<std::string>> PgMailDB::RetrieveEmailContentInfo(const std::string_view content)
    {
        StartTransaction();

        pqxx::result content_query_result;
        if (content.empty())
        {
            content_query_result = m_transaction->exec_params(
                "SELECT * FROM public.\"mailBodies\""
            );
        }
        else
        {
            content_query_result = m_transaction->exec_params(
                "SELECT * FROM public.\"mailBodies\" "
                "WHERE body_content = $1"
                , m_transaction->esc(content)
            );
        }

        CommitTransaction();

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
        InsertEmailContent(body);

        std::vector<std::vector<std::string>> sender_info = RetrieveUserInfo(sender);
        std::vector<std::vector<std::string>> receiver_info = RetrieveUserInfo(receiver);
        std::vector<std::vector<std::string>> body_info = RetrieveEmailContentInfo(body);
        
        StartTransaction();
        m_transaction->exec_params(
            "INSERT INTO public.\"emailMessages\" (sender_id, recipient_id, subject, mail_body_id, is_received) "
            "VALUES ($1, $2, $3, $4, false) "
            , sender_info[0][0], receiver_info[0][0],
            subject, body_info[0][0]
        );
        CommitTransaction();

        return true;
    }

    bool PgMailDB::DeleteEmail(const std::string_view user_name)
    {
        std::vector<std::vector<std::string>> user_info = RetrieveUserInfo(user_name);

        StartTransaction();
        m_transaction->exec_params(
            "DELETE FROM public.\"emailMessages\" "
            "WHERE sender_id = $1 OR recipient_id = $1"
            , user_info[0][0]
        );
        CommitTransaction();

        return true;
    }

    bool PgMailDB::DeleteUser(const std::string_view user_name, const std::string_view hash_password)
    {
        DeleteEmail(user_name);

        StartTransaction();
        m_transaction->exec_params(
            "DELETE FROM public.\"users\" "
            "WHERE user_name = $1 AND password = $2"
            , user_name, hash_password
        );
        CommitTransaction();

        return true;
    }

    void PgMailDB::StartTransaction()
    {
        m_transaction = std::make_unique<pqxx::work>(*m_conn);
    }

    void PgMailDB::CommitTransaction()
    {
        if (m_transaction) {
            m_transaction->commit();
            m_transaction.reset();
        }
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