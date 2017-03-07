#pragma once
#include <list>
#include "CAlbum.h"
#include "CUser.h"
#include "Constants.h"

using namespace std;

class CDataAccess
{
public:
	virtual ~CDataAccess() {}

	// album related
	virtual const albums& getAlbums()=0;
	virtual const albums& getAlbumsOfUser(int userId) = 0;
	virtual void insertAlbum(CAlbum& album)=0;
	virtual void deleteAlbum(string albumName)=0;
	virtual bool albumExists(string albumName)=0;
	virtual CAlbum* openAlbum(string albumName)=0;
	virtual void closeAlbum(CAlbum *pAlbum)=0;

	// picture related
	virtual void addPictureToAlbum(int albumId, CPicture& picture)=0;
	virtual void removePictureFromAlbum(int albumId, int pictureId)=0;
	virtual void tagUserInPicture(CPicture& picture, int userId)=0;
	virtual void untagUserInPicture(CPicture& picture, int userId)=0;
	virtual bool isUserTaggedInPicture(const CPicture& picture, int userId)=0;

	// user related
	virtual const users& getUsers()=0;
	virtual void addUser(CUser& user)=0;
	virtual void deleteUser(string userName)=0;
	virtual bool userExists(string userName)=0;
	virtual bool userExists(int userId)=0;
	virtual CUser* getUser(int userId)=0;

	// user statistics
	virtual int countAlbumsOwnedOfUser(int userId) = 0;
	virtual int countAlbumsTaggedOfUser(int userId) = 0;
	virtual int countTagsOfUser(int userId) = 0;
	virtual float averageTagsPerAlbumOfUser(int userId)=0;

	// queries
	virtual const CUser* getTopTaggedUser()=0;
	virtual const CPicture* getTopTaggedPicture()=0;
	virtual const pictures& getTaggedPicturesOfUser(int userId) = 0;
	
	virtual bool open()=0;
	virtual void close()=0;
	virtual void clear()=0;
};
