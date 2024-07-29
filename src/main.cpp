#include <iostream>
#include "MailDB/PgMailDB.h"
#include "MailDB/DBCredentials.h"
#include <pqxx/pqxx>

using namespace std;

int main() 
{
    
    ISXMailDB::PgMailDB mail("host");

    ISXMailDB::Credentials credentials("", "");

    mail.Connect(credentials);

    return 0;
}