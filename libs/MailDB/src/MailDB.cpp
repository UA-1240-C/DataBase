#include "MailDB.h"
#include <pqxx/pqxx>

void connect()
{
    try {
        // Connect to the database
        pqxx::connection conn("dbname=mydb user=postgres password=password hostaddr=127.0.0.1 port=5432");
        if (conn.is_open()) {
            std::cout << "Connected to " << conn.dbname() << std::endl;
        } else {
            std::cout << "Can't open the database" << std::endl;
        }

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}