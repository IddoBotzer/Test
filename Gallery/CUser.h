#pragma once
#include <string>
#include <list>

using namespace std;

class CUser;
typedef list<CUser> users;
typedef users::iterator users_iter;
typedef users::const_iterator users_const_iter;

class CUser
{
private:
	int _id;
	string _name;

public:
	CUser();

	int getId() const;
	void setId(int id);

	string getName() const;
	void setName(string name);
};
