#pragma once

#include <string>
#include <exception>

namespace ISXMailDB
{
class MailException : public std::exception
{
public:
	MailException(const std::string& message) : m_message(message) {}
	MailException(const char* message) : m_message(message) {}

	const char* what() const noexcept override {
        return m_message.c_str();
    }
private:
	std::string m_message;
};
};