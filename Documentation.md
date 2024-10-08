# Documentation

## IMailDB public methods:

+ **IMailDB(const std::string_view host_name)** - set m_host_name to host_name.
  + Exceptions thrown:
    + MailException("Host name couldn't be empty") - if host_name is empty.
   
+ **IMailDB(const IMailDB& other)** - set m_host_name to other.m_host_name.
  + Exceptions thrown:
    + MailException("Host name couldn't be empty") - if other.m_host_name is empty.

+ **bool Connect(const std::string &connection_string)** - establish connection between client and database server.
  + Pass connection_string of one of the following format:
    + local host: `"dbname=<> user=postgres password=<> hostaddr=127.0.0.1 port=5432"`;
    + remote host: `"postgresql://postgres.qotrdwfvknwbfrompcji:yUf73LWenSqd9Lt4@aws-0-eu-central-1.pooler.supabase.com:6543/postgres?sslmode=require"`.
  + Exceptions thrown:
    + MailException(appropriate pqxx error message) - if database doesn't exist or bad credentials passed.

+ **void Disconnect()** - breaks connection between client and database server. On repeat use does nothing.

+ **bool IsConnected()** const - return true, if connection established and exists, otherwise false. 

+ **void SignUp(const std::string_view user_name, const std::string_view password)** - creates user with name and password on host, specified in constructor.
  + Exceptions thrown:
    + MailException("User already exists") - if user exists with the same name on host.
    
+ **void Login(const std::string_view user_name, const std::string_view password)** - checks if passed credentials exist in database.
  + Exceptions thrown:
    + MailException("Invalid user name or password") - if there is no user on host with name = user_name and password = password.

+ **std::vector<User> RetrieveUserInfo(const std::string_view user_name)** - returns list of users either user_name was specified or not(means that was passed empty string literal - ""). If user_name was not specified, returns list of all users.
  + Exceptions thrown:
    + MailException("Connection with database lost or was manually already closed") - if connection with database was lost.

+ **std::vector<std::string> RetrieveEmailContentInfo(const std::string_view content)** - same as **std::vector<User> RetrieveUserInfo(const std::string_view)**, but returns body content of email.
  + Exceptions thrown:
    + MailException("Connection with database lost or was manually already closed") - if connection with database was lost.
   
+ **void MarkEmailsAsReceived(const std::string_view user_name)** - marks **all** emails as received for user with user_name.
  + Exceptions thrown:
    + MailException("User doesn't exist") - If user doesn't exist on host.                            
      
+ **std::vector<Mail> RetrieveEmails(const std::string_view user_name, bool should_retrieve_all = false) const** - if should_retrieve_all = false return mails that haven't been received, otherwise return all emails for user with user_name. The order of mails in vector is the following: newer ones first.
  + Exceptions thrown:
    + MailException("User doesn't exist") - If user doesn't exist on host.
  
+ **bool InsertEmail(const std::string_view sender, const std::string_view receiver,
                            const std::string_view subject, const std::string_view body)** - inserts email with single receiver passed. Also implicitly insert mail content into database.
  + Exceptions thrown:
    + MailException(appropriate pqxx error message) - if transaction failed;
    + MailException("Given value doesn't exist in database.") - if passed either sender or receiver doesn't exist;
    + MailException("Connection with database lost or was manually already closed") - if connection with database was lost.

+ **bool InsertEmail(const std::string_view sender, std::vector<std::string_view> receiver,
                            const std::string_view subject, const std::string_view body)** - inserts email with multiple receivers passed. Also implicitly insert mail content into database.
  + Exceptions thrown:
    + MailException(appropriate pqxx error message) - if transaction failed;
    + MailException("Given value doesn't exist in database.") - if passed either sender or receiver doesn't exist;
    + MailException("Connection with database lost or was manually already closed") - if connection with database was lost.

+ **bool UserExists(const std::string_view user_name)** - returns true if user with user_name exists on host, otherwise return false

+ **bool DeleteEmail(const std::string_view user_name)** - removes all associated mail with user either he was a sender or receiver from email's table.
  + Exceptions thrown:
    + MailException(appropriate pqxx error message) - if transaction failed;
    + MailException("Given value doesn't exist in database.") - if passed user doesn't exist;
    + MailException("Connection with database lost or was manually already closed") - if connection with database was lost.

+ **bool DeleteUser(const std::string_view user_name, const std::string_view password)** - removes user from database and all associated mail with user either he was a sender or receiver.
  + Exceptions thrown:
    + MailException(appropriate pqxx error message) - if transaction failed;
    + MailException("Invalid user name or password") - if bad credentials were passed;
    + MailException("Given value doesn't exist in database.") - if passed user doesn't exist;
    + MailException("Connection with database lost or was manually already closed") - if connection with database was lost.

## Helper structs
```C++
struct User
{
    User(std::string user_name, std::string user_password, std::string host_name)
        : user_name{std::move(user_name)}, user_password{std::move(user_password)}, host_name{std::move(host_name)}
    {
    }

    std::string user_name;
    std::string user_password;
    std::string host_name;
};

struct Mail
{
    Mail(const std::string_view recipient, const std::string_view sender,
         const std::string_view subject, const std::string_view body)
        : recipient(recipient), sender(sender), 
          subject(subject), body(body)
    {
    }
    
    std::string recipient;
    std::string sender;
    std::string subject;
    std::string body;
};
```
