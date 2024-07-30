#pragma once

#include <memory>

#include "IMailDB.h"

namespace ISXMailDB
{

    class PgMailDB : public IMailDB
    {
        public:

            PgMailDB() = default;
            PgMailDB(std::string_view host_name);
            virtual ~PgMailDB();

            // TODO: Viacheslav
            virtual bool Connect(const std::string& connection_string) override;
            virtual void Disconnect() override;
            virtual bool IsConnected() const override;
            virtual bool InsertHost(const std::string& host_name) override;

            // TODO: Denys
            virtual bool SignUp(const std::string_view user_name, const std::string_view hash_password) override {return false;}
            virtual bool Login(const std::string_view user_name, const std::string_view hash_password) override { return false; }

            virtual std::vector<std::vector<std::string>> RetrieveUserInfo(const std::string_view user_name) override;

            virtual bool InsertEmailContent(const std::string_view content) override;
            virtual std::vector<std::vector<std::string>> RetrieveEmailContentInfo(const std::string_view content = "") override;
            // TODO: Viacheslav
            virtual bool InsertEmail(const std::string_view sender, const std::string_view receiver, 
                                     const std::string_view subject, const std::string_view body) override;

            // TODO: Denys
            virtual bool RetrieveEmails(const std::string_view user_name) override { return false; }
            virtual bool RetrieveAllEmails(const std::string_view user_name) override { return false; }

            // TODO: Viacheslav
            virtual bool DeleteEmail(const std::string_view user_name) override;
            virtual bool DeleteUser(const std::string_view user_name, const std::string_view hash_password) override;

        protected:
            // TODO
            virtual std::vector<std::vector<std::string_view>> ExecuteQuery(const std::string_view& query) override { return {}; }

            // ??
            virtual std::vector<std::vector<std::string_view>> RetrieveEmails(const std::string_view& criteria) override { return {}; }

            virtual void WriteQueryResultToStorage(const pqxx::result& query_result, std::vector<std::vector<std::string>>& storage) override;
    };
}