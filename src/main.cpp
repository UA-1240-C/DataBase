#include <iostream>
#include <fstream>
#include <exception>

#include "MailDB/PgMailDB.h"

using namespace std;
using namespace ISXMailDB;

void fn(IMailDB& con)
{
    con.Connect("postgresql://postgres.qotrdwfvknwbfrompcji:yUf73LWenSqd9Lt4@aws-0-eu-central-1.pooler.supabase.com:6543/postgres?sslmode=require");
   
    try
    {
        con.Login("user1", "password_hash1");
        
        for (auto &mail : con.RetrieveEmails("user1", true))
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