#include <memory>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "MailDB/PgMailDB.h"
#include "MailDB/MailException.h"


const char*  CONNECTION_STRING = "dbname=testmaildb user=postgres password=password hostaddr=127.0.0.1 port=5432";

using namespace ISXMailDB;

void ExecuteQueryFromFile(pqxx::work& tx, std::string filename)
{
   std::ifstream file(filename);
    if (!file.is_open()) 
    {
      FAIL() << "couldn't open a file with db tables\n"; 
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sql_commands = buffer.str();

    tx.exec(sql_commands);
    tx.commit();
}

bool operator==(const std::vector<Mail>& lhs, const std::vector<Mail>& rhs)
{
  if (lhs.size() != rhs.size())
  {
    return false;
  }
  for (size_t i = 0; i < lhs.size(); i++)
  {
    if(lhs[i].sender != rhs[i].sender 
      || lhs[i].subject != rhs[i].subject 
      || lhs[i].body != rhs[i].body
      || lhs[i].recipient != rhs[i].recipient)
      return false;
  }
  return true;
}


class PgMailDBTest : public testing::Test
{
protected:
  static void SetUpTestSuite() 
  {
    if (!s_connection.is_open())
    {
      FAIL() << "couldn't establish connection with test db\n";
    }

    pqxx::work tx(s_connection);

    ASSERT_NO_FATAL_FAILURE(
      ExecuteQueryFromFile(tx, "../test/db_table_creation.txt")
    );


  }

  static void TearDownTestSuite() 
  {
    pqxx::work tx(s_connection);

    tx.exec("DROP TABLE hosts, users, \"emailMessages\", \"mailBodies\"");
    tx.commit();

    s_connection.close();
  }

  PgMailDBTest() : pg("test host") {}
  ~PgMailDBTest() = default;

  void SetUp() override 
  { 
    try
    {
      pg.Connect(CONNECTION_STRING);
    }
    catch(const std::exception& e)
    {
      FAIL() << "Fail in setup : " << e.what();
    }
  }

  void TearDown() override
  {
    pqxx::work tx(s_connection);

    tx.exec("TRUNCATE TABLE \"emailMessages\" RESTART IDENTITY CASCADE;");
    tx.exec("TRUNCATE TABLE \"mailBodies\" RESTART IDENTITY CASCADE;");
    tx.exec("TRUNCATE TABLE users RESTART IDENTITY CASCADE;");
    tx.exec("TRUNCATE TABLE hosts RESTART IDENTITY CASCADE;");

    tx.commit();
  }

  PgMailDB pg;
  static pqxx::connection s_connection;  
};

pqxx::connection PgMailDBTest::s_connection{CONNECTION_STRING};

TEST_F(PgMailDBTest, GetPasswordHashTest)
{
  std::string result = pg.GetPasswordHash("user1");

  EXPECT_TRUE(result.empty());

  pqxx::work tx(s_connection);
  
  ASSERT_NO_FATAL_FAILURE(
    ExecuteQueryFromFile(tx, "../test/db_insert_dummy_data.txt")
  );
  
  EXPECT_TRUE("password_hash1" == pg.GetPasswordHash("user1"));
  EXPECT_TRUE("password_hash2" == pg.GetPasswordHash("user2"));
  EXPECT_TRUE("password_hash3" == pg.GetPasswordHash("user3"));


  result = pg.GetPasswordHash("user4");
  EXPECT_TRUE(result.empty());
}

TEST_F(PgMailDBTest, SignUpTest)
{
  std::string user_name = "test_user1", hash_password = "hash_password1";
  pg.SignUp(user_name, hash_password);

  pqxx::work tx(s_connection);

  auto [received_name, received_password] = tx.query1<std::string, std::string>(
    "SELECT user_name, password_hash FROM users "
    "WHERE user_name = " + tx.quote(user_name) + " AND password_hash = " + tx.quote(hash_password)
    );

  EXPECT_EQ(user_name, received_name);
  EXPECT_EQ(hash_password, received_password);

  EXPECT_THROW(pg.SignUp(user_name, hash_password), MailException);

  std::vector<std::pair<std::string, std::string>> user_password_pairs =
  {
    {"test_user2", "hash_password2"},
    {"test_user3", "hash_password3"},
    {"test_user4", "hash_password4"},
    {"test_user5", "hash_password5"},
  }; 

  for (auto [name, password] : user_password_pairs)
  {
    pg.SignUp(name, password);
    auto [received_name, received_password] = tx.query1<std::string, std::string>(
    "SELECT user_name, password_hash FROM users "
    "WHERE user_name = " + tx.quote(name) + " AND password_hash = " + tx.quote(password)
    );

    EXPECT_EQ(name, received_name);
    EXPECT_EQ(password, received_password);
  }
}

TEST_F(PgMailDBTest, LoginTest)
{
  ASSERT_THROW(pg.Login("test_user1", "hash_password"), MailException);

  pqxx::work tx(s_connection);
  
  std::vector<std::pair<std::string, std::string>> user_password_pairs =
  {
    {"test_user2", "hash_password2"},
    {"test_user3", "hash_password3"},
    {"test_user4", "hash_password4"},
    {"test_user5", "hash_password5"},
  }; 

  for (auto [name, password] : user_password_pairs)
  {
    tx.exec_params("INSERT INTO users (host_id, user_name, password_hash) "
                   "VALUES (1, $1, $2)"
                   , name, password);
  }
  tx.commit();
  for (auto [name, password] : user_password_pairs)
  {
    EXPECT_NO_THROW(pg.Login(name, password));
  }
  
  ASSERT_THROW(pg.Login("test_user1", "hash_password"), MailException);
}

TEST_F(PgMailDBTest, SignUpWithLoginTest)
{
  std::vector<std::pair<std::string, std::string>> user_password_pairs =
  {
    {"test_user2", "hash_password2"},
    {"test_user3", "hash_password3"},
    {"test_user4", "hash_password4"},
    {"test_user5", "hash_password5"},
  }; 

  for (auto [name, password] : user_password_pairs)
  {
     pg.SignUp(name, password);
     EXPECT_NO_THROW(pg.Login(name,password));
  }
}


TEST_F(PgMailDBTest, MarkEmailsAsReceived)
{
  pqxx::work tx(s_connection);
  
  ASSERT_NO_FATAL_FAILURE(
    ExecuteQueryFromFile(tx, "../test/db_insert_dummy_data.txt")
  );

  EXPECT_THROW(pg.MarkEmailsAsReceived("non-existent user"), MailException);

  pg.MarkEmailsAsReceived("user1");


  pqxx::work w(s_connection);

  auto received_email_count = w.query_value<uint32_t>(
    "SELECT COUNT(*) "
    "FROM \"emailMessages\" "
    "WHERE is_received = true "
    "AND recipient_id = (SELECT user_id FROM users WHERE user_name = 'user1'); "
    );

    EXPECT_EQ(2, received_email_count);
}

TEST_F(PgMailDBTest, RetrieveEmailsTest)
{
  pqxx::work tx(s_connection);
  
  ASSERT_NO_FATAL_FAILURE(
    ExecuteQueryFromFile(tx, "../test/db_insert_dummy_data.txt")
  );

  EXPECT_THROW(pg.RetrieveEmails("non-existent user"), MailException);

  std::vector<Mail> expected_result = {{"user1", "user2", "Subject 2", "This is the body of the second email."}, 
  {"user1", "user3", "Subject 3", "This is the body of the third email."}};

  std::vector<Mail> result = pg.RetrieveEmails("user1");

  EXPECT_TRUE(expected_result==result);

  result = pg.RetrieveEmails("user1", true);

  EXPECT_TRUE(expected_result==result);
}

TEST_F(PgMailDBTest, CheckFunctionsAfterDisconnect)
{
  pg.Disconnect();

  EXPECT_THROW(pg.Login("user1","password"), std::exception);
  EXPECT_THROW(pg.SignUp("user1","password"), std::exception);
  EXPECT_THROW(pg.RetrieveEmails("user1"), std::exception);
  EXPECT_THROW(pg.UserExists("user1"), std::exception);
  EXPECT_THROW(pg.MarkEmailsAsReceived("user1"), std::exception);
}

TEST_F(PgMailDBTest, CheckUserExists)
{
  EXPECT_FALSE(pg.UserExists("user1"));
  EXPECT_FALSE(pg.UserExists("user2"));

  pqxx::work tx(s_connection);
  
  ASSERT_NO_FATAL_FAILURE(
    ExecuteQueryFromFile(tx, "../test/db_insert_dummy_data.txt")
  );

  EXPECT_TRUE(pg.UserExists("user1"));
  EXPECT_TRUE(pg.UserExists("user2"));
  EXPECT_TRUE(pg.UserExists("user3"));

  EXPECT_FALSE(pg.UserExists("user4"));
}

TEST_F(PgMailDBTest, CheckUserExistsWithSignUp)
{
  EXPECT_FALSE(pg.UserExists("user1"));
  EXPECT_FALSE(pg.UserExists("user2"));

  pg.SignUp("user1", "password");
  pg.SignUp("user2", "password");
  pg.SignUp("user3", "password");

  EXPECT_TRUE(pg.UserExists("user1"));
  EXPECT_TRUE(pg.UserExists("user2"));
  EXPECT_TRUE(pg.UserExists("user3"));

  EXPECT_FALSE(pg.UserExists("user4"));
}



int main(int argc, char **argv)
{
 testing::InitGoogleTest(&argc, argv);
 return RUN_ALL_TESTS();
}