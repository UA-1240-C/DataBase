#include <iostream>
#include <fstream>
#include <exception>

#include "MailDB/PgMailDB.h"

using namespace std;
using namespace ISXMailDB;

int main()
{
    PgMailDB pg("host1");
    // std::ifstream file("../src/test.txt");
    // if (!file.is_open()) 
    // {
    //   cerr << "couldn't open a file with db tables\n"; 
    // }
    pg.Connect("postgresql://postgres.qotrdwfvknwbfrompcji:yUf73LWenSqd9Lt4@aws-0-eu-central-1.pooler.supabase.com:6543/postgres?sslmode=require");
    // pg.Connect("dbname=mydsb user=postgres password=password hostaddr=127.0.0.1 port=5432");
   
    try
    {
        pg.Login("user1", "password_hash1");
        
        for (auto &mail : pg.RetrieveEmails("user1", true))
        {
            cout << mail.subject << " " << mail.sender << " " << mail.body << endl;
        }
    }
    catch (const exception &e)
    {
        cout << "exception: " << e.what() << endl;
    }
    // ISXMailDB::PgMailDB mail("host4");

    // mail.Connect("postgresql://postgres.qotrdwfvknwbfrompcji:yUf73LWenSqd9Lt4@aws-0-eu-central-1.pooler.supabase.com:6543/postgres?sslmode=require");

    return 0;
}