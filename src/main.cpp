#include <iostream>
#include "MailDB/PgMailDB.h"

using namespace std;
using namespace ISXMailDB;

int main() 
{
    PgMailDB pg("myhost");
   // pg.Connect("dbname=mydb user=postgres password=password hostaddr=127.0.0.1 port=5432");

    return 0;
}