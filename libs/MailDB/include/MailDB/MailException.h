#pragma once

#include <exception>

namespace ISXMailDB
{
	class MailException : public std::exception
	{
	public:
		virtual const char* what() const = 0;
	};
};