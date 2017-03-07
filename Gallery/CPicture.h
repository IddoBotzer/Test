#pragma once
#include <set>
#include <iostream>
#include <string>
#include <list>

using namespace std;

class CPicture;
typedef list<CPicture> pictures;
typedef pictures::iterator pictures_iter;
typedef pictures::const_iterator pictures_const_iter;

class CPicture
{
private:
	int _id;
	string _name;
	string _locationOnDisk;
	string _creationDate;
	set<int> _usersTags;
	
public:
	CPicture();

	int getId() const;
	void setId(int id);

	string getName() const;
	void setName(string name);

	string getLocation() const;
	void setLocation(string location);

	string getCreationDate() const;
	void setCreationDate(string creationTime);
	void setCreationDateNow();

	void tagUser(int userId);
	void untagUser(int userId);
	int getTagsCount() const;
	const set<int>& getUserTags() const;
	set<int>& getUserTagsRef();
};
