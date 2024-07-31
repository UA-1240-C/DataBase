#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "MailDB/PgMailDB.h"

const char*  CONNECTION_STRING = "dbname=testmaildb user=postgres password=password hostaddr=127.0.0.1 port=5432";

using namespace ISXMailDB;

class PgMailDBTest : public testing::Test
{
protected:
  static void SetUpTestSuite() 
  {
    if (!s_connection.is_open())
    {
      FAIL() << "couldn't establish connection with test db\n";
    }

    std::ifstream file("db_table_creation.txt");
    if (!file.is_open()) 
    {
      FAIL() << "couldn't open a file with db tables\n"; 
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sql_commands = buffer.str();

    pqxx::work tx(s_connection);

    tx.exec(sql_commands);
    tx.commit();

  }

  static void TearDownTestSuite() 
  {
    pqxx::work tx(s_connection);

    tx.exec("DROP TABLE hosts, users, emailMessages, mailBodies");
    tx.commit();

    s_connection.close();
  }

  PgMailDBTest() : pg("test host") {}
  ~PgMailDBTest() = default;

  // Some expensive resource shared by all tests.
  static pqxx::connection s_connection;

  PgMailDB pg;
};

pqxx::connection PgMailDBTest::s_connection{CONNECTION_STRING};

TEST(TestSuiteSample, TestSample)
{
  ASSERT_EQ(6, 6);
}

int main(int argc, char **argv)
{
 testing::InitGoogleTest(&argc, argv);
 return RUN_ALL_TESTS();
}