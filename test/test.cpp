#include <memory>
#include <exception>

#include <gtest/gtest.h>

#include "<PATH>/PgMailDB.h"

class DatabaseFixture : public testing::Test
{
  public:
    DatabaseFixture() = default;
    ~DatabaseFixture() = default;

    virtual void SetUp() override
    {
        m_database->SignUp("user1", "pass1");
        m_database->SignUp("user2", "pass2");
        m_database->SignUp("user3", "pass3");

        m_database->InsertEmail("user1", "user2", "subject1", "looooong body1");
        m_database->InsertEmail("user2", "user1", "subject2", "small body2");
        m_database->InsertEmail("user3", "user1", "subject3", "medium body3");
    }

    virtual void TearDown() override
    {
        pqxx::work transaction(*(m_database->GetConnection()));
        transaction.exec(
            "TRUNCATE public.\"emailMessages\", public.\"mailBodies\", public.\"users\" RESTART IDENTITY"
        );
        transaction.commit();
    }
  
    static void SetUpTestCase()
    {
      m_database = std::make_unique<ISXMailDB::PgMailDB>("host");
      m_database->Connect("dbname=<> user=postgres password=<> hostaddr=127.0.0.1 port=5432");
    }

    static void TearDownTestCase()
    {
      m_database->Disconnect();
    }

  protected:
      inline static std::unique_ptr<ISXMailDB::PgMailDB> m_database;
};

TEST(ConnectionTestSuite, Connect_To_Local_Test)
{
    ISXMailDB::PgMailDB database("host");
    bool connection_outcome = database.Connect("dbname=<> user=postgres password=<> hostaddr=127.0.0.1 port=5432");
    EXPECT_TRUE(connection_outcome);

    database.Disconnect();
}

TEST(ConnectionTestSuite, Connect_To_Remote_Test)
{
    ISXMailDB::PgMailDB database("host");
    bool connection_outcome = database.Connect("postgresql://postgres.qotrdwfvknwbfrompcji:yUf73LWenSqd9Lt4@aws-0-eu-central-1.pooler.supabase.com:6543/postgres?sslmode=require");
    EXPECT_TRUE(connection_outcome);

    database.Disconnect();
}

TEST(ConnectionTestSuite, Connect_To_Unexisting_DB_Test)
{
    ISXMailDB::PgMailDB database("host");
    bool connection_outcome = database.Connect("dbname=db user=postgres password=1234 hostaddr=127.0.0.1 port=5432");
    EXPECT_FALSE(connection_outcome);

    database.Disconnect();
}

TEST(ConnectionTestSuite, Connect_Several_Times_Test)
{
    ISXMailDB::PgMailDB database("host");
    bool connection_outcome = database.Connect("postgresql://postgres.qotrdwfvknwbfrompcji:yUf73LWenSqd9Lt4@aws-0-eu-central-1.pooler.supabase.com:6543/postgres?sslmode=require");
    connection_outcome = database.Connect("postgresql://postgres.qotrdwfvknwbfrompcji:yUf73LWenSqd9Lt4@aws-0-eu-central-1.pooler.supabase.com:6543/postgres?sslmode=require");
    connection_outcome = database.Connect("postgresql://postgres.qotrdwfvknwbfrompcji:yUf73LWenSqd9Lt4@aws-0-eu-central-1.pooler.supabase.com:6543/postgres?sslmode=require");
    EXPECT_TRUE(connection_outcome);

    database.Disconnect();
}

TEST(ConnectionTestSuite, Disconnect_When_Connected_To_DB_Test)
{
    ISXMailDB::PgMailDB database("host");
    database.Connect("postgresql://postgres.qotrdwfvknwbfrompcji:yUf73LWenSqd9Lt4@aws-0-eu-central-1.pooler.supabase.com:6543/postgres?sslmode=require");
    EXPECT_TRUE(database.IsConnected());

    database.Disconnect();
    EXPECT_FALSE(database.IsConnected());
}

TEST(ConnectionTestSuite, Disconnect_When_Not_Connected_To_DB_Test)
{
    ISXMailDB::PgMailDB database("host");
    EXPECT_FALSE(database.IsConnected());

    database.Disconnect();
    EXPECT_FALSE(database.IsConnected());
}

TEST(ConnectionTestSuite, Disconnect_Several_Times_Test)
{
    ISXMailDB::PgMailDB database("host");
    EXPECT_FALSE(database.IsConnected());

    database.Disconnect();
    database.Disconnect();
    database.Disconnect();
    database.Disconnect();
    EXPECT_FALSE(database.IsConnected());

}

TEST_F(DatabaseFixture, Retrieve_Existing_User_Test)
{
   EXPECT_EQ(1, m_database->RetrieveUserInfo("user1").size());
}

TEST_F(DatabaseFixture, Retrieve_All_Users_Test)
{
   EXPECT_EQ(3, m_database->RetrieveUserInfo().size());
   EXPECT_EQ(3, m_database->RetrieveUserInfo("").size());
}

TEST_F(DatabaseFixture, Retrieve_Unexisting_Users_Test)
{
   EXPECT_EQ(0, m_database->RetrieveUserInfo("user1234").size());
}

TEST_F(DatabaseFixture, Retrieve_Existing_Body_Content_Test)
{
   EXPECT_EQ(1, m_database->RetrieveEmailContentInfo("medium body3").size());
}

TEST_F(DatabaseFixture, Retrieve_Unexisting_Body_Content_Test)
{
    EXPECT_EQ(0, m_database->RetrieveEmailContentInfo("bodycontent").size());
}

TEST_F(DatabaseFixture, Retrieve_All_Body_Content_Test)
{
    EXPECT_EQ(3, m_database->RetrieveEmailContentInfo().size());
    EXPECT_EQ(3, m_database->RetrieveEmailContentInfo("").size());
}

TEST_F(DatabaseFixture, Insert_Email_With_Existing_Sender_Receiver_Test)
{
    EXPECT_TRUE(m_database->InsertEmail("user3", "user2", "subjectsubject", "body"));
    EXPECT_EQ(2, m_database->RetrieveEmails("user2").size());
}

TEST_F(DatabaseFixture, Insert_Email_With_Unexisting_Sender_Receiver_Test)
{
    EXPECT_FALSE(m_database->InsertEmail("user3242", "user2", "subjectsubject", "body"));
    EXPECT_FALSE(m_database->InsertEmail("user3", "user54235", "subjectsubject", "body"));
    EXPECT_FALSE(m_database->InsertEmail("", "", "subjectsubject", "body"));
}

TEST_F(DatabaseFixture, Insert_Blank_Email_Test)
{
    EXPECT_TRUE(m_database->InsertEmail("user1", "user2", "", ""));
    EXPECT_EQ(2, m_database->RetrieveEmails("user2").size());
}

TEST_F(DatabaseFixture, Insert_Mail_Content_Test)
{
    EXPECT_TRUE(m_database->InsertEmailContent("new body"));
    EXPECT_EQ(1, m_database->RetrieveEmailContentInfo("new body").size());
}

TEST_F(DatabaseFixture, Insert_Existing_Mail_Content_Test)
{
    EXPECT_TRUE(m_database->InsertEmailContent("looooong body1"));
    EXPECT_EQ(1, m_database->RetrieveEmailContentInfo("looooong body1").size());
}

TEST_F(DatabaseFixture, Delete_Email_With_Existing_User_Test)
{
    EXPECT_TRUE(m_database->DeleteEmail("user1"));
    EXPECT_EQ(0, m_database->RetrieveEmails("user1").size());
}

TEST_F(DatabaseFixture, Delete_Email_With_Unexisting_User_Test)
{
    EXPECT_FALSE(m_database->DeleteEmail("user3242"));
}

TEST_F(DatabaseFixture, Delete_Existing_User_Test)
{
    EXPECT_TRUE(m_database->DeleteUser("user1", "pass1"));
    EXPECT_EQ(0, m_database->RetrieveUserInfo("user1").size());
}

TEST_F(DatabaseFixture, Delete_Unexisting_User_Test)
{
    EXPECT_FALSE(m_database->DeleteUser("user1213", "pass1"));
    EXPECT_FALSE(m_database->DeleteUser("user1", "pass1dasdasd"));
}

int main(int argc, char **argv)
{
 testing::InitGoogleTest(&argc, argv);
 return RUN_ALL_TESTS();
}