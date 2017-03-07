#pragma once
#include "CMemoryAccess.h"
#include "CAlbum.h"
#include <map>

using namespace std;

class CAlbumManager
{
private:
	CDataAccess& _dataAccess;
	CAlbum *_openAlbum;

	// albums management
	void createAlbum();
	void openAlbum();
	void closeAlbum();
	void deleteAlbum();
	void listAlbums();
	void listAlbumsOfUser();

	// album management
	void addPictureToAlbum();
	void removePictureFromAlbum();
	void listPicturesInAlbum();
	void openPicture();

	// tags related
	void tagUserInPicture();
	void untagUserInPicture();
	void listUserTags();

	// users management
	void addUser();
	void removeUser();
	void listUsers();
	void userStatistics();

	void topTaggedUser();
	void topTaggedPicture();
	void picturesTaggedUser();

	string readName();
	bool fileExistsOnDisk(const string& filename);

public:
	CAlbumManager(CDataAccess& dataAccess);

	void executeCommand(const string& command);
	void printHelp();
};
