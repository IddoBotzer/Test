#include "stdafx.h"
#include "CMemoryAccess.h"
#include <map>

CMemoryAccess::CMemoryAccess() : _nextAlbumId(100), _nextPictureId(200), _nextUserId(300)
{
	
}

CMemoryAccess::~CMemoryAccess()
{
	close();
}

bool CMemoryAccess::open()
{
	// create some dummy albums
	for (int i=0; i<10; ++i) {
		_albums.push_back(CreateDummyAlbum(i));
	}

	// create some dummy users
	for (int i=0; i<=10; ++i) {
		string name("User");
		char buff[10];
		_itoa_s(_nextUserId, buff, 10, 10);
		name += string(buff);

		CUser user;
		user.setName(name);
		addUser(user);
	}

	return true;
}

void CMemoryAccess::close()
{
	
}

void CMemoryAccess::clear()
{
	_users.clear();
	_albums.clear();
	_nextAlbumId = 1;
	_nextPictureId = 1;
	_nextUserId = 1;
}

CAlbum CMemoryAccess::CreateDummyAlbum(int index)
{
	string name("Dummy");
	char buff[10];
	_itoa_s(index, buff, 10, 10);
	name += string(buff);

	CAlbum album;
	album.setId(_nextAlbumId++);
	album.setName(name);

	for (int i=1; i<6; ++i)
	{
		string picName = "Picture";
		_itoa_s(_nextPictureId, buff, 10, 10);
		picName += string(buff);

		CPicture pic;
		pic.setId(_nextPictureId++);
		pic.setName(picName);
		pic.setLocation("C:\\Pictures\\" + picName + ".bmp");
		album.addPicture(pic);
	}

	return album;
}

const albums& CMemoryAccess::getAlbums()
{
	return _albums;
}

const albums& CMemoryAccess::getAlbumsOfUser(int userId)
{	
	_albumsOfUser.clear();

	albums_iter iter = _albums.begin();
	while (iter != _albums.end())
	{
		if (iter->getUserId() == userId)
			_albumsOfUser.push_back(*iter);
		++iter;
	}

	return _albumsOfUser;
}

void CMemoryAccess::insertAlbum(CAlbum& album)
{
	album.setId(_nextAlbumId++);
	album.setCreationDateNow();

	_albums.push_back(album);
}

void CMemoryAccess::deleteAlbum(string albumName)
{
	albums_iter iter = _albums.begin();
	while (iter != _albums.end())
	{
		if (iter->getName().compare(albumName) == 0)
			iter = _albums.erase(iter);
		else
			++iter;
	}
}

bool CMemoryAccess::albumExists(string albumName)
{
	albums_iter iter = _albums.begin();
	while (iter != _albums.end())
	{
		if (iter->getName().compare(albumName) == 0)
			return true;
		++iter;
	}

	return false;
}

CAlbum* CMemoryAccess::openAlbum(string albumName)
{
	albums_iter iter = _albums.begin();
	while (iter != _albums.end())
	{
		if (iter->getName().compare(albumName) == 0)
			return &*iter;
		++iter;
	}

	return nullptr;
}

void CMemoryAccess::closeAlbum(CAlbum* pAlbum)
{
	// basically here we would like to delete the allocated memory we got from openAlbum
	// 
}

void CMemoryAccess::tagUserInPicture(CPicture& picture, int userId)
{
	albums_iter albumIter = _albums.begin();
	while (albumIter != _albums.end())
	{
		if (albumIter->containsPicture(picture.getId()) == false) {
			++albumIter;
			continue;
		}

		albumIter->tagUser(picture.getId(), userId);
		break;
	}
}

void CMemoryAccess::untagUserInPicture(CPicture& picture, int userId)
{
	albums_iter albumIter = _albums.begin();
	while (albumIter != _albums.end())
	{
		if (albumIter->containsPicture(picture.getId()) == false) {
			++albumIter;
			continue;
		}

		albumIter->untagUser(picture.getId(), userId);
		break;
	}
}

bool CMemoryAccess::isUserTaggedInPicture(const CPicture& picture, int userId)
{
	return picture.getUserTags().find(userId) != picture.getUserTags().end();
}

void CMemoryAccess::removePictureFromAlbum(int albumId, int pictureId)
{
	albums_iter iter = _albums.begin();
	while (iter != _albums.end())
	{
		if (iter->getId() == albumId)
			iter->removePicture(pictureId);
		++iter;
	}
}

void CMemoryAccess::addPictureToAlbum(int albumId, CPicture& picture)
{
	picture.setId(_nextPictureId++);
	picture.setCreationDateNow();

	albums_iter iter = _albums.begin();
	while (iter != _albums.end())
	{
		if (iter->getId() == albumId)
			iter->addPicture(picture);
		++iter;
	}
}

const users& CMemoryAccess::getUsers()
{
	return _users;
}

void CMemoryAccess::addUser(CUser& user)
{
	user.setId(_nextUserId++);
	_users.push_back(user);
}

void CMemoryAccess::deleteUser(string userName)
{
	users_iter iter = _users.begin();
	while (iter != _users.end())
	{
		if (iter->getName().compare(userName) == 0)
			iter = _users.erase(iter);
		else
			++iter;
	}
}

bool CMemoryAccess::userExists(string userName)
{
	users_iter iter = _users.begin();
	while (iter != _users.end())
	{
		if (iter->getName().compare(userName) == 0)
			return true;
		++iter;
	}

	return false;
}

bool CMemoryAccess::userExists(int userId)
{
	users_iter iter = _users.begin();
	while (iter != _users.end())
	{
		if (iter->getId() == userId)
			return true;
		++iter;
	}

	return false;
}

CUser* CMemoryAccess::getUser(int userId)
{
	users_iter iter = _users.begin();
	while (iter != _users.end())
	{
		if (iter->getId() == userId)
			return &*iter;
		++iter;
	}

	return nullptr;
}

// user statistics
int CMemoryAccess::countAlbumsOwnedOfUser(int userId)
{
	int albumsCount = 0;

	albums_const_iter albumsIter = _albums.begin();
	for (; albumsIter != _albums.end(); ++albumsIter)
	{
		if (albumsIter->getUserId() == userId)
			++albumsCount;
	}

	return albumsCount;
}

int CMemoryAccess::countAlbumsTaggedOfUser(int userId)
{
	int albumsCount = 0;

	albums_const_iter albumsIter = _albums.begin();
	for (; albumsIter != _albums.end(); ++albumsIter)
	{
		const pictures& pics = albumsIter->getPictures();
		pictures_const_iter picsIter = pics.begin();
		for (; picsIter != pics.end(); ++picsIter)
		{
			if (isUserTaggedInPicture(*picsIter, userId))
			{
				albumsCount++;
				break;
			}
		}
	}

	return albumsCount;
}

int CMemoryAccess::countTagsOfUser(int userId)
{
	int tagsCount = 0;

	albums_const_iter albumsIter = _albums.begin();
	for (; albumsIter != _albums.end(); ++albumsIter)
	{
		const pictures& pics = albumsIter->getPictures();
		pictures_const_iter picsIter = pics.begin();
		for (; picsIter != pics.end(); ++picsIter)
		{
			if (isUserTaggedInPicture(*picsIter, userId))
			{
				tagsCount++;
			}
		}
	}

	return tagsCount;
}

float CMemoryAccess::averageTagsPerAlbumOfUser(int userId)
{
	int albumsTaggedCount = countAlbumsTaggedOfUser(userId);

	if (albumsTaggedCount == 0)
		return 0;

	return (float)countTagsOfUser(userId) / (float)albumsTaggedCount;
}

const CUser* CMemoryAccess::getTopTaggedUser()
{
	map<int, int> userTagsCountMap;

	albums_const_iter albumsIter = _albums.begin();
	for (; albumsIter != _albums.end(); ++albumsIter)
	{
		const pictures& pics = albumsIter->getPictures();
		pictures_const_iter picsIter = pics.begin();
		for (; picsIter != pics.end(); ++picsIter)
		{
			const set<int>& userTags = picsIter->getUserTags();
			set<int>::const_iterator tagsIter = userTags.begin();
			for (; tagsIter != userTags.end(); ++tagsIter)
			{
				if (userTagsCountMap.find(*tagsIter) == userTagsCountMap.end())
					userTagsCountMap.insert(map<int, int>::value_type(*tagsIter, 0));
				userTagsCountMap[*tagsIter]++;
			}
		}
	}

	int topTaggedUserId = -1;
	int currentMax = -1;
	map<int, int>::const_iterator countIter = userTagsCountMap.begin();
	for (; countIter != userTagsCountMap.end(); ++countIter)
	{
		if (countIter->second < currentMax)
			continue;

		topTaggedUserId = countIter->first;
		currentMax = countIter->second;
	}

	if (topTaggedUserId == -1)
		return nullptr;

	return getUser(topTaggedUserId);
}

const CPicture* CMemoryAccess::getTopTaggedPicture()
{
	int currentMax = -1;
	const CPicture* mostTaggedPic = nullptr;
	albums_const_iter albumsIter = _albums.begin();
	for (; albumsIter != _albums.end(); ++albumsIter)
	{
		const pictures& pics = albumsIter->getPictures();
		pictures_const_iter picsIter = pics.begin();
		for (; picsIter != pics.end(); ++picsIter)
		{
			int tagsCount = picsIter->getTagsCount();
			if (tagsCount == 0)
				continue;

			if (tagsCount <= currentMax)
				continue;

			mostTaggedPic = &*picsIter;
			currentMax = tagsCount;
		}
	}

	return mostTaggedPic;
}

const pictures& CMemoryAccess::getTaggedPicturesOfUser(int userId)
{
	_picturesOfUser.clear();

	albums_const_iter albumsIter = _albums.begin();
	for (; albumsIter != _albums.end(); ++albumsIter)
	{
		const pictures& pics = albumsIter->getPictures();
		pictures_const_iter picsIter = pics.begin();
		for (; picsIter != pics.end(); ++picsIter)
		{
			if (isUserTaggedInPicture(*picsIter, userId))
			{
				_picturesOfUser.push_back(*picsIter);
			}
		}
	}

	return _picturesOfUser;
}
