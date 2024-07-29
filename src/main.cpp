#include <iostream>
#include <fstream>
#include <exception>

#include "MailDB/PgMailDB.h"

using namespace std;
using namespace ISXMailDB;

int main() 
{
    PgMailDB pg("myhost");
    pg.Connect("dbname=mydb user=postgres password=password hostaddr=127.0.0.1 port=5432");

    ofstream output("errors.txt");
    try
    {
        //pg.SignUp("den", "123");
        //pg.Login("den", "123");
        for (auto& mail : pg.RetrieveEmails("den", true))
        {
            cout << mail.subject << " " << mail.sender << " " << mail.body << endl;
        }
    }
    catch (const exception& e)
    {
        cout << "exception: " << e.what() << endl;
        output << "exception: " << e.what() << endl;
    }
    return 0;
}