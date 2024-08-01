# Documentation

## IMailDB public methods:

**IMailDB(const std::string_view host_name)** - set m_host_name to host_name. If host_name is empty, throws a MailException("Host name couldn't be empty").

**bool Connect(const std::string &connection_string)** - todo 

**void Disconnect()** - todo 

**bool IsConnected()** const - todo 

**void SignUp(const std::string_view user_name, const std::string_view hash_password)** - creates user with name and password on host, specified in constructor. If on host exists user with the same name, throws a MailException("User already exists").  

**void Login(const std::string_view user_name, const std::string_view hash_password)** - throws a MailException("Invalid user name or password"), if there is no user on host with name = user_name and password = hash_password.

**std::vector<User> RetrieveUserInfo(const std::string_view user_name = "")** - todo

**bool InsertEmailContent(const std::string_view content)** - todo

**std::vector<std::string> RetrieveEmailContentInfo(const std::string_view content = "")** - todo

**bool InsertEmail(const std::string_view sender, const std::string_view receiver,
                            const std::string_view subject, const std::string_view body)** - todo

**std::vector<Mail> RetrieveEmails(const std::string_view user_name, bool should_retrieve_all = false) const** - todo

**bool UserExists(const std::string_view user_name)** - return true if user with user_name exists on host, otherwise return false

**bool DeleteEmail(const std::string_view user_name)** - todo

**bool DeleteUser(const std::string_view user_name, const std::string_view hash_password)** - todo


## Helper sturcts
```
struct User
{
    User(const std::string& user_name, const std::string& user_password, const std::string& host_name) {}

    std::string user_name;
    std::string user_password;
    std::string host_name;
};

struct Mail
{
    Mail(std::string sender, std::string subject, std::string body) {}

    std::string sender;
    std::string subject;
    std::string body;
};
```
