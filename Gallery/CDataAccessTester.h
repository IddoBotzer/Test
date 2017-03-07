#pragma once
#include "CDataAccess.h"

class CDataAccessTester
{
private:
	CDataAccess& _dataAccess;

	const CAlbum& getRandomAlbum(const albums& albums);
	const CUser& getRandomUser(const users& users);
	const CPicture& getRandomPicture(const CAlbum& album);

public:
	CDataAccessTester(CDataAccess& dataAccess);

	void createUser(int i);
	bool checkUserExists(int i);
	string createFiveUsers();
	string deleteUser();
	string getUser();
	string testUserExists();

	void createAlbum(int i);
	bool checkAlbumExists(int i);
	string createFiveAlbums();
	string deleteAlbum();
	string openAlbum();
	string testAlbumExists();
	
	void addPictureToAlbum(int albumId, int i);
	string addFivePicture();
	string deleteTwoPictures();

	string tagTenUsers();
	string untagTwoUsers();
};
