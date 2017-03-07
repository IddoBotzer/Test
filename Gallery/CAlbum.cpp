#include "stdafx.h"
#include "CAlbum.h"
#include <time.h>

CAlbum::CAlbum() : _id(0)
{
	
}

string CAlbum::getName() const
{
	return _name;
}

void CAlbum::setName(string name)
{
	_name = name;
}

string CAlbum::getCreationDate() const
{
	return _creationDate;
}

void CAlbum::setCreationDate(string creationTime)
{
	_creationDate = creationTime;
}

void CAlbum::setCreationDateNow()
{
	time_t now = time(nullptr);
	tm *pTime = localtime(&now);
	char buff[11];
	strftime(buff, 32, "%d.%m.%Y", pTime);
	_creationDate = buff;
}

int CAlbum::getId() const
{
	return _id;
}

void CAlbum::setId(int id)
{
	_id = id;
}

int CAlbum::getUserId() const {
	return _userId;
}

void CAlbum::setUserId(int userId) {
	_userId = userId;
}

const pictures& CAlbum::getPictures() const
{
	return _pictures;
}

pictures& CAlbum::getPicturesRef()
{
	return _pictures;
}

void CAlbum::addPicture(CPicture& picture)
{
	_pictures.push_back(picture);
}

void CAlbum::removePicture(int pictureId)
{
	pictures_iter iter = _pictures.begin();
	while (iter != _pictures.end())
	{
		if (iter->getId() == pictureId)
			iter = _pictures.erase(iter);
		else
			++iter;
	}
}

CPicture* CAlbum::getPicture(int pictureId)
{
	pictures_iter iter = _pictures.begin();
	while (iter != _pictures.end())
	{
		if (iter->getId() == pictureId)
			return &*iter;
		++iter;
	}

	return nullptr;
}

bool CAlbum::containsPicture(int pictureId)
{
	CPicture *pic = getPicture(pictureId);
	return (pic != nullptr);
}

void CAlbum::tagUser(int pictureId, int userId)
{
	CPicture *pic = getPicture(pictureId);
	if (pic == nullptr)
		return;

	pic->tagUser(userId);
}

void CAlbum::untagUser(int pictureId, int userId)
{
	CPicture *pic = getPicture(pictureId);
	if (pic == nullptr)
		return;

	pic->untagUser(userId);
}
