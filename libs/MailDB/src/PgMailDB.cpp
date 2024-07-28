#include "PgMailDB.h"

#include <iostream>

namespace ISXMailDB
{

// bool PgMailDB::Connect(const std::string_view connectionString)
// {   
//     try {
//         m_conn = std::make_unique<pqxx::connection>(connectionString);
//         // if (!IsConnected()) {
//         //     throw 1;
//         // }
//         std::cout << "Connected successfully\n";
//         return true;
//     } 
//     catch (const std::exception& e) 
//     {
//         std::cerr << "Connection to database failed: " << e.what() << std::endl;
//         throw;
//     }
// }

// void PgMailDB::Disconnect() 
// {
//     //TODO
// }

// bool PgMailDB::IsConnected() const
// {
//    // TODO
//     return true;
// }

// bool PgMailDB::SignUp(const std::string_view user_name, const std::string_view hash_password)
// {
//     return false;
// }

// bool PgMailDB::Login(const std::string_view user_name, const std::string_view hash_password)
// {
//     return false;
// }

// bool PgMailDB::InsertEmail(const std::string_view sender, const std::string_view receiver, const std::string_view subject, const std::string_view body)
// {
//     return false;
// }
// bool PgMailDB::RetrieveEmails(const std::string_view user_name)
// {
//     return false;
// }
// bool PgMailDB::RetrieveAllEmails(const std::string_view user_name)
// {
//     return false;
// }
// bool PgMailDB::DeleteEmail(const std::string_view user_name)
// {
//     return false;
// }
// bool PgMailDB::DeleteUser(const std::string_view user_name, const std::string_view hash_password)
// {
//     return false;
// }
// std::vector<std::vector<std::string_view>> PgMailDB::ExecuteQuery(const std::string_view &query)
// {
//     return std::vector<std::vector<std::string_view>>();
// }
// std::vector<std::vector<std::string_view>> PgMailDB::RetrieveEmails(const std::string_view &criteria)
// {
//     return std::vector<std::vector<std::string_view>>();
// }

void PgMailDB::method()
{
}

}