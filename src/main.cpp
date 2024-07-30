#include <iostream>
#include <pqxx/pqxx>

#include "MailDB/PgMailDB.h"


int main() 
{
    ISXMailDB::PgMailDB mail("host4");

    mail.Connect("postgresql://postgres.qotrdwfvknwbfrompcji:yUf73LWenSqd9Lt4@aws-0-eu-central-1.pooler.supabase.com:6543/postgres?sslmode=require");

    return 0;
}