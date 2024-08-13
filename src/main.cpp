#include <iostream>
#include <fstream>
#include <exception>
#include <vector>

#include "MailDB/PgMailDB.h"

using namespace std;
using namespace ISXMailDB;

int main()
{
    using test = tuple<string, string, string>;
    vector<test> vec, vec2;
    vec.push_back(test("user2", "subject", "body"));
    // = {
    //     test("user2", "subject", "body")
    // };
    std::vector<Mail> resutl_mails;
    for (auto& [sender, subject, body] : vec)
    {
        resutl_mails.emplace_back("user1" ,sender, subject, body);
        cout << sender << subject << body << endl;
    }
    for (auto& [sender, subject, body] : vec)
    {
        cout << sender << subject << body << endl;
    }
    for (const auto& mail : resutl_mails)
    {
        cout << mail << endl;
    }


}