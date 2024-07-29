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
        pg.SignUp("den", "123");
    }
    catch (const exception& e)
    {
        try 
        {
            cout << "exception: " << e.what() << endl;
            output << "exception: " << e.what() << endl;
            pg.Login("den", "1234");
        }
        catch(const exception& e)
        {
            cout << "exception: " << e.what() << endl;
        }

    }
    return 0;
}