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

    mail.InsertEmail("m3@host.com", "m2@host.com", "mail subject hear", "mail content hereeeeee");

    mail.DeleteUser("m2@host.com", "pass");
    return 0;
}