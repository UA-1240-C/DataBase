#include <iostream>
#include <fstream>
#include <exception>

#include "MailDB/PgMailDB.h"

using namespace std;
using namespace ISXMailDB;

int main()
{
    // PgMailDB pg("host1"), pg2("host2");
    // // // pg.Connect("postgresql://postgres.qotrdwfvknwbfrompcji:yUf73LWenSqd9Lt4@aws-0-eu-central-1.pooler.supabase.com:6543/postgres?sslmode=require");
    // pg.Connect("dbname=mydb user=postgres password=password hostaddr=127.0.0.1 port=5432");
    // pg2.Connect("dbname=mydb user=postgres password=password hostaddr=127.0.0.1 port=5432");



    // ofstream output("errors.txt");
    // try
    // {
    //     pg.Login("den", "123");
    //     int i =0;
    //     cin >> i;
    //     for (auto &mail : pg.RetrieveEmails("user1", true))
    //     {
    //         cout << mail.subject << " " << mail.sender << " " << mail.body << endl;
    //     }
    // }
    // catch (const exception &e)
    // {
    //     cout << "exception: " << e.what() << endl;
    //     output << "exception: " << e.what() << endl;
    // }
    // ISXMailDB::PgMailDB mail("host4");

    // mail.Connect("postgresql://postgres.qotrdwfvknwbfrompcji:yUf73LWenSqd9Lt4@aws-0-eu-central-1.pooler.supabase.com:6543/postgres?sslmode=require");

    return 0;
}