#pragma once
#include "CPicture.h"
#include "Constants.h"
#include <list>

using namespace std;

class CAlbum;
typedef list<CAlbum> albums;
typedef albums::iterator albums_iter;
typedef albums::const_iterator albums_const_iter;

class CAlbum
{
private:
	int _id;
	string _name;
	pictures _pictures;
	string _creationDate;
	int _userId;
	
public:
	CAlbum();

	int getId() const;
	void setId(int id);

	string getName() const;
	void setName(string name);

	string getCreationDate() const;
	void setCreationDate(string creationTime);
	void setCreationDateNow();

	int getUserId() const;
	void setUserId(int userId);

	const pictures& getPictures() const;
	pictures& getPicturesRef();
	void addPicture(CPicture& picture);
	void removePicture(int pictureId);
	CPicture* getPicture(int pictureId);
	bool containsPicture(int pictureId);
	void tagUser(int pictureId, int userId);
	void untagUser(int pictureId, int userId);
};
