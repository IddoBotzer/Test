#pragma once
#include <string>

class CTag
{
private:
	int _userId;
	std::string _userName;

public:
	CTag(int userId);

	void SetUserName(std::string userName);
	std::string GetUserName();
};
