#include "stdafx.h"
#include "CPicture.h"
#include <ctime>

CPicture::CPicture() : _id(0)
{
}

int CPicture::getId() const
{
	return _id;
}

void CPicture::setId(int id)
{
	_id = id;
}

string CPicture::getName() const
{
	return _name;
}

void CPicture::setName(string name)
{
	_name = name;
}

string CPicture::getLocation() const
{
	return _locationOnDisk;
}

void CPicture::setLocation(string location)
{
	_locationOnDisk = location;
}

string CPicture::getCreationDate() const
{
	return _creationDate;
}

void CPicture::setCreationDate(string creationTime)
{
	_creationDate = creationTime;
}

void CPicture::setCreationDateNow()
{
	time_t now = time(nullptr);
	tm *pTime = localtime(&now);
	char buff[11];
	strftime(buff, 32, "%d.%m.%Y", pTime);
	_creationDate = buff;
}

void CPicture::tagUser(int userId)
{
	_usersTags.insert(userId);
}

void CPicture::untagUser(int userId)
{
	_usersTags.erase(userId);
}

int CPicture::getTagsCount() const
{
	return _usersTags.size();
}

const set<int>& CPicture::getUserTags() const
{
	return _usersTags;
}

set<int>& CPicture::getUserTagsRef()
{
	return _usersTags;
}
