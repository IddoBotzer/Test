#include "CDataAccessTester.h"
#include <ctime>

#define FAIL			return "FAILED"
#define SUCCESS			return "PASSED"

const CAlbum& CDataAccessTester::getRandomAlbum(const albums& albums)
{
	int index = rand() % albums.size();
	albums_const_iter iter = albums.begin();
	for (int i=0; i<index; ++i)
		++iter;

	return *iter;
}

const CUser& CDataAccessTester::getRandomUser(const users& users)
{
	int index = rand() % users.size();
	users_const_iter iter = users.begin();
	for (int i=0; i<index; ++i)
		++iter;

	return *iter;
}

const CPicture& CDataAccessTester::getRandomPicture(const CAlbum& album)
{
	const pictures& pictures = album.getPictures();
	int index = rand() % pictures.size();
	pictures_const_iter iter = pictures.begin();
	for (int i=0; i<index; ++i)
		++iter;

	return *iter;
}

CDataAccessTester::CDataAccessTester(CDataAccess& dataAccess) : _dataAccess(dataAccess)
{
	srand (time(nullptr));
}

void CDataAccessTester::createUser(int i)
{
	CUser user;
	char buff[10];
	user.setName("User" + string(_itoa(i, buff, 10)));
	_dataAccess.addUser(user);
}

bool CDataAccessTester::checkUserExists(int i)
{
	char buff[10];
	return _dataAccess.userExists("User" + string(_itoa(i, buff, 10)));
}

string CDataAccessTester::createFiveUsers()
{
	for (int i=1; i<=5; ++i)
	{
		createUser(i);
		if (checkUserExists(i) == false)
			FAIL;
	}

	SUCCESS;
}

string CDataAccessTester::deleteUser()
{
	int curSize = _dataAccess.getUsers().size();
	_dataAccess.deleteUser("User2");
	--curSize;
	_dataAccess.deleteUser("User3");
	--curSize;

	if (_dataAccess.getUsers().size() != curSize)
		FAIL;

	SUCCESS;
}

string CDataAccessTester::getUser()
{
	const users& users = _dataAccess.getUsers();
	if (users.size() == 0)
		FAIL;

	users_const_iter iter = users.begin();
	CUser* user = _dataAccess.getUser(iter->getId());
	if (user == nullptr)
		FAIL;

	SUCCESS;
}

string CDataAccessTester::testUserExists()
{
	const users& users = _dataAccess.getUsers();
	if (users.size() == 0)
		FAIL;

	users_const_iter iter = users.begin();
	if (_dataAccess.userExists(iter->getName()) == false)
		FAIL;
	if (_dataAccess.userExists(iter->getId()) == false)
		FAIL;

	SUCCESS;
}

void CDataAccessTester::createAlbum(int i)
{
	CAlbum album;
	char buff[10];
	album.setName("Album" + string(_itoa(i, buff, 10)));
	_dataAccess.insertAlbum(album);
}

bool CDataAccessTester::checkAlbumExists(int i)
{
	char buff[10];
	return _dataAccess.albumExists("Album" + string(_itoa(i, buff, 10)));
}

string CDataAccessTester::createFiveAlbums()
{
	for (int i=1; i<=5; ++i)
	{
		createAlbum(i);
		if (checkAlbumExists(i) == false)
			FAIL;
	}

	SUCCESS;
}

string CDataAccessTester::deleteAlbum()
{
	int curSize = _dataAccess.getAlbums().size();
	_dataAccess.deleteAlbum("Album2");
	--curSize;
	_dataAccess.deleteAlbum("Album3");
	--curSize;

	if (_dataAccess.getAlbums().size() != curSize)
		FAIL;

	SUCCESS;
}

string CDataAccessTester::openAlbum()
{
	const albums& albums = _dataAccess.getAlbums();
	if (albums.size() == 0)
		FAIL;

	albums_const_iter iter = albums.begin();
	CAlbum* album = _dataAccess.openAlbum(iter->getName());
	if (album == nullptr)
		FAIL;

	SUCCESS;
}

string CDataAccessTester::testAlbumExists()
{
	const albums& albums = _dataAccess.getAlbums();
	if (albums.size() == 0)
		FAIL;

	albums_const_iter iter = albums.begin();
	if (_dataAccess.albumExists(iter->getName()) == false)
		FAIL;

	SUCCESS;
}

void CDataAccessTester::addPictureToAlbum(int albumId, int i)
{
	CPicture picture;
	char buff[10];
	picture.setName("Picture" + string(_itoa(i, buff, 10)));
	picture.setLocation("C:\\Pictures\\Picture" + string(_itoa(i, buff, 10)) + ".bmp");
	
	_dataAccess.addPictureToAlbum(albumId, picture);
}

string CDataAccessTester::addFivePicture()
{
	const albums& albums = _dataAccess.getAlbums();
	if (albums.size() == 0)
		FAIL;

	albums_const_iter iter = albums.begin();
	string albumName = iter->getName();
	CAlbum* album = _dataAccess.openAlbum(albumName);
	if (album == nullptr)
		FAIL;

	for (int i=1; i<=5; ++i)
		addPictureToAlbum(album->getId(), i);

	album = _dataAccess.openAlbum(albumName);
	if (album == nullptr)
		FAIL;

	if (album->getPictures().size() != 5)
		FAIL;

	SUCCESS;
}

string CDataAccessTester::deleteTwoPictures()
{
	const albums& albums = _dataAccess.getAlbums();
	if (albums.size() == 0)
		FAIL;

	albums_const_iter iter = albums.begin();
	string albumName = iter->getName();
	int albumId = iter->getId();
	CAlbum* album = _dataAccess.openAlbum(albumName);
	if (album == nullptr)
		FAIL;

	pictures& pictures = album->getPicturesRef();
	int curSize = pictures.size();
	if (curSize == 0)
		FAIL;

	pictures_iter picIter = pictures.begin();
	int picId1 = picIter->getId();
	++picIter;
	int picId2 = picIter->getId();
	++picIter;

	_dataAccess.removePictureFromAlbum(albumId, picId1);
	--curSize;
	_dataAccess.removePictureFromAlbum(albumId, picId2);
	--curSize;
	
	album = _dataAccess.openAlbum(albumName);
	if (album == nullptr)
		FAIL;

	if (curSize != album->getPictures().size())
		FAIL;

	SUCCESS;
}

string CDataAccessTester::tagTenUsers()
{
	// make sure we have pictures in all albums
	albums albums = _dataAccess.getAlbums();
	albums_const_iter albumIter = albums.begin();
	int start=50;
	for (; albumIter != albums.end(); ++albumIter)
	{
		if (albumIter->getPictures().size() == 0)
		{
			for (int i=1; i<=5; ++i)
				addPictureToAlbum(albumIter->getId(), start++);
		}
	}

	// do the tags
	int tagsCount = 10;
	users users = _dataAccess.getUsers();
	do
	{
		albums = _dataAccess.getAlbums();
		const CAlbum& album = getRandomAlbum(albums);
		CAlbum* pAlbum = _dataAccess.openAlbum(album.getName());
		const CUser& user = getRandomUser(users);
		const CPicture& picture = getRandomPicture(*pAlbum);
		if (_dataAccess.isUserTaggedInPicture(picture, user.getId()))
			continue;

		CPicture tmpPic;
		tmpPic.setId(picture.getId());
		_dataAccess.tagUserInPicture(tmpPic, user.getId());
		tagsCount--;
	} while (tagsCount != 0);

	// verify we have 10 total tags
	tagsCount = 0;
	albums = _dataAccess.getAlbums();
	albumIter = albums.begin();
	for (; albumIter != albums.end(); ++albumIter)
	{
		CAlbum* pAlbum = _dataAccess.openAlbum(albumIter->getName());
		const pictures& pics = pAlbum->getPictures();
		pictures_const_iter picsIter = pics.begin();
		for (;picsIter != pics.end(); ++picsIter)
			tagsCount += picsIter->getTagsCount();
	}

	if (tagsCount != 10)
		FAIL;

	SUCCESS;
}

string CDataAccessTester::untagTwoUsers()
{
	// do the tags
	int tagsCount = 2;
	albums albums = _dataAccess.getAlbums();
	do
	{
		albums = _dataAccess.getAlbums();
		const CAlbum& album = getRandomAlbum(albums);
		CAlbum* pAlbum = _dataAccess.openAlbum(album.getName());
		const CPicture& picture = getRandomPicture(*pAlbum);
		if (picture.getTagsCount() == 0)
			continue;

		CPicture tmpPic;
		tmpPic.setId(picture.getId());
		_dataAccess.untagUserInPicture(tmpPic, *picture.getUserTags().begin());
		tagsCount--;
	} while (tagsCount != 0);

	// verify we have 8 total tags
	tagsCount = 0;
	albums = _dataAccess.getAlbums();
	albums_const_iter albumIter = albums.begin();
	for (; albumIter != albums.end(); ++albumIter)
	{
		CAlbum* pAlbum = _dataAccess.openAlbum(albumIter->getName());
		const pictures& pics = pAlbum->getPictures();
		pictures_const_iter picsIter = pics.begin();
		for (;picsIter != pics.end(); ++picsIter)
			tagsCount += picsIter->getTagsCount();
	}

	if (tagsCount != 8)
		FAIL;

	SUCCESS;
}
