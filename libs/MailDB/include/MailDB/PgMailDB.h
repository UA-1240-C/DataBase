#pragma once

#include <memory>
#include <pqxx/pqxx>

#include "IMailDB.h"

namespace ISXMailDB
{

class PgMailDB : public IMailDB
{

public:
    PgMailDB(std::string_view host_name);
    PgMailDB(const PgMailDB&);

    virtual ~PgMailDB();

    virtual void Connect(const std::string &connection_string) override;
    virtual void Disconnect() override;
    virtual bool IsConnected() const override;

    virtual void Login(const std::string_view user_name, const std::string_view hash_password) override;


    virtual std::vector<User> RetrieveUserInfo(const std::string_view user_name = "") override;
    virtual std::vector<std::string> RetrieveEmailContentInfo(const std::string_view content = "") override;
    virtual void InsertEmail(const std::string_view sender, const std::string_view receiver,
                                const std::string_view subject, const std::string_view body) override;
    virtual void InsertEmail(const std::string_view sender, const std::vector<std::string_view> receivers,
                                const std::string_view subject, const std::string_view body) override;

    virtual void DeleteEmail(const std::string_view user_name) override;
    virtual void DeleteUser(const std::string_view user_name, const std::string_view hash_password) override;

protected:
    virtual void InsertHost(const std::string_view host_name) override;

    uint32_t InsertEmailContent(const std::string_view content, pqxx::transaction_base& transaction);
    uint32_t RetriveUserId(const std::string_view user_name, pqxx::transaction_base &ntx) const;

    void PerformEmailInsertion(const uint32_t sender_id, const uint32_t receiver_id,
                                const std::string_view subject, const uint32_t body_id, pqxx::transaction_base& transaction);

    std::unique_ptr<pqxx::connection> m_conn;

};

}