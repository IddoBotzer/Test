#include "stdafx.h"
#include "sqlite3.h"
#include "CDatabaseAccess.h"

#define RETURN_IF_INVALID				if (_sqldb == nullptr) return;
#define RETURN_RES_IF_INVALID(res)		if (_sqldb == nullptr) return res;


static int sqlExecCallback(void* param, int argc, char** argv, char** azColName)
{
	CDatabaseAccess *dbAccess = (CDatabaseAccess *)param;
	dbAccess->resetLastId();

	int i;
	for(i=0; i<argc; i++) {
		if (string(azColName[i]).compare("ID") == 0) {
			dbAccess->setLastId(argv[i]);
		}
	}
	return 0;
}

static int sqlTopId(void* param, int argc, char** argv, char** azColName)
{
	pair<int, int> *topId = (pair<int, int> *)param;
	if (argc != 2)
		return 0;

	int curCount = atoi(argv[1]);
	if (curCount < topId->second)
		return 0;

	topId->first = atoi(argv[0]);
	topId->second = curCount;

	return 0;
}

static int sqlInt(void* params, int argc, char** argv, char** azColName)
{
	int *num = (int*)params;

	if (argc != 1)
		return 0;

	*num = atoi(argv[0]);

	return 0;
}

static int listUsersCallback(void* param, int argc, char** argv, char** azColName)
{
	users *usersList = static_cast<users *>(param);

	CUser currentUser;

	for(int i=0; i<argc; i++) {
		if (string(azColName[i]).compare("ID") == 0) {
			currentUser.setId(atoi(argv[i]));
		}
		else if (string(azColName[i]).compare("NAME") == 0) {
			currentUser.setName(argv[i]);
		}
	}
	usersList->push_back(currentUser);

	return 0;
}

static int listAlbumsCallback(void* param, int argc, char** argv, char** azColName)
{
	albums *usersList = static_cast<albums *>(param);

	CAlbum currentAlbum;

	for(int i=0; i<argc; i++) {
		if (string(azColName[i]).compare("ID") == 0) {
			currentAlbum.setId(atoi(argv[i]));
		}
		else if (string(azColName[i]).compare("NAME") == 0) {
			currentAlbum.setName(argv[i]);
		}
		else if (string(azColName[i]).compare("CREATION_DATE") == 0)
		{
			currentAlbum.setCreationDate(argv[i]);
		}
		else if (string(azColName[i]).compare("USER_ID") == 0)
		{
			currentAlbum.setUserId(atoi(argv[i]));
		}
	}
	usersList->push_back(currentAlbum);

	return 0;
}

static int listPicturesCallback(void* param, int argc, char** argv, char** azColName)
{
	pictures *picsList = static_cast<pictures *>(param);

	CPicture currentPicture;

	for(int i=0; i<argc; i++) {
		if (string(azColName[i]).compare("ID") == 0) {
			currentPicture.setId(atoi(argv[i]));
		}
		else if (string(azColName[i]).compare("NAME") == 0) {
			currentPicture.setName(argv[i]);
		}
		else if (string(azColName[i]).compare("LOCATION") == 0) {
			currentPicture.setLocation(argv[i]);
		}
		else if (string(azColName[i]).compare("CREATION_DATE") == 0)
		{
			currentPicture.setCreationDate(argv[i]);
		}
	}
	picsList->push_back(currentPicture);

	return 0;
}

static int listUserTagsCallback(void* param, int argc, char** argv, char** azColName)
{
	set<int> *usersList = (set<int> *)param;

	for(int i=0; i<argc; i++) {
		if (string(azColName[i]).compare("USER_ID") == 0) {
			usersList->insert(atoi(argv[i]));
		}
	}

	return 0;
}

CDatabaseAccess::CDatabaseAccess() : _sqldb(nullptr)
{
}

CDatabaseAccess::~CDatabaseAccess()
{
	close();
}

const albums& CDatabaseAccess::getAlbums()
{
	_albums.clear();
	RETURN_RES_IF_INVALID(_albums);

	string sqlStatement;
	char *errMessage = nullptr;

	sqlStatement = "SELECT * FROM ALBUMS;";

	resetLastId();
	sqlite3_exec(_sqldb, sqlStatement.c_str(), listAlbumsCallback, &_albums, &errMessage);

	return _albums;
}

const albums& CDatabaseAccess::getAlbumsOfUser(int userId)
{
	_albumsOfUser.clear();
	RETURN_RES_IF_INVALID(_albums);

	string sqlStatement;
	char *errMessage = nullptr;

	char buff[10];
	sqlStatement = "SELECT * FROM ALBUMS WHERE USER_ID=" + string(_itoa(userId, buff, 10)) + ";";

	resetLastId();
	sqlite3_exec(_sqldb, sqlStatement.c_str(), listAlbumsCallback, &_albumsOfUser, &errMessage);

	return _albumsOfUser;
}


void CDatabaseAccess::insertAlbum(CAlbum& album)
{
	RETURN_IF_INVALID;

	string sqlStatement;
	char *errMessage = nullptr;
	int res;

	album.setCreationDateNow();

	char buff[10];

	sqlStatement = "INSERT INTO ALBUMS (NAME, CREATION_DATE, USER_ID) "	\
        "VALUES ('" + album.getName() + "', '" + album.getCreationDate() + "', " + string(_itoa(album.getUserId(), buff, 10)) + "); "	\
		"SELECT * FROM ALBUMS WHERE NAME='" + album.getName() + "';";

	res = sqlite3_exec(_sqldb, sqlStatement.c_str(), sqlExecCallback, this, &errMessage);
	if (res != SQLITE_OK)
		return;

	if (_lastId == -1)
		return;

	album.setId(_lastId);
}

void CDatabaseAccess::deleteAlbum(string albumName)
{
	RETURN_IF_INVALID;

	string sqlStatement;
	char *errMessage = nullptr;

	sqlStatement = "DELETE FROM ALBUMS WHERE NAME='" + albumName + "';";

	sqlite3_exec(_sqldb, sqlStatement.c_str(), nullptr, this, &errMessage);
}

bool CDatabaseAccess::albumExists(string albumName)
{
	RETURN_RES_IF_INVALID(false);

	string sqlStatement;
	char *errMessage = nullptr;
	int res;

	sqlStatement = "SELECT * FROM ALBUMS WHERE NAME='" + albumName + "';";

	resetLastId();
	res = sqlite3_exec(_sqldb, sqlStatement.c_str(), sqlExecCallback, this, &errMessage);
	if (res != SQLITE_OK)
		return false;

	return _lastId != -1;
}

CAlbum* CDatabaseAccess::openAlbum(string albumName)
{
	RETURN_RES_IF_INVALID(nullptr);

	getAlbums();
	if (_albums.size() == 0)
		return nullptr;

	albums_iter iter = _albums.begin();
	while (iter != _albums.end())
	{
		if (iter->getName().compare(albumName) == 0)
			break;
		++iter;
	}

	if (iter == _albums.end())
		return nullptr;

	string sqlStatement;
	char *errMessage = nullptr;

	char buff[10];
	sqlStatement = "SELECT * FROM PICTURES WHERE ALBUM_ID=" + string(_itoa(iter->getId(), buff, 10)) + ";";
	sqlite3_exec(_sqldb, sqlStatement.c_str(), listPicturesCallback, &iter->getPicturesRef(), &errMessage);

	pictures& pictures = iter->getPicturesRef();
	pictures_iter picIter = pictures.begin();
	for (; picIter != pictures.end(); ++picIter)
	{
		sqlStatement = "SELECT * FROM TAGS WHERE PICTURE_ID=" + string(_itoa(picIter->getId(), buff, 10)) + ";";
		sqlite3_exec(_sqldb, sqlStatement.c_str(), listUserTagsCallback, &picIter->getUserTagsRef(), &errMessage);
	}

	return &*iter;
}

void CDatabaseAccess::closeAlbum(CAlbum* pAlbum)
{
}

void CDatabaseAccess::addPictureToAlbum(int albumId, CPicture& picture)
{
	RETURN_IF_INVALID;

	string sqlStatement;
	char *errMessage = nullptr;
	int res;

	picture.setCreationDateNow();

	char buff[10];
	sqlStatement = "INSERT INTO PICTURES (NAME, LOCATION, CREATION_DATE, ALBUM_ID) VALUES ("	\
        "'" + picture.getName() + "', "					\
        "'" + picture.getLocation() + "', "				\
        "'" + picture.getCreationDate() + "', "			\
        "" + string(_itoa(albumId, buff, 10)) + ");"	\
		"SELECT * FROM PICTURES WHERE NAME='" + picture.getName() + "';";

	res = sqlite3_exec(_sqldb, sqlStatement.c_str(), sqlExecCallback, this, &errMessage);
	if (res != SQLITE_OK)
		return;

	if (_lastId == -1)
		return;

	picture.setId(_lastId);

	albums_iter iter = _albums.begin();
	while (iter != _albums.end())
	{
		if (iter->getId() == albumId)
			iter->addPicture(picture);
		++iter;
	}
}

void CDatabaseAccess::removePictureFromAlbum(int albumId, int pictureId)
{
	RETURN_IF_INVALID;

	string sqlStatement;
	char *errMessage = nullptr;
	char buff[10];
	sqlStatement = "DELETE FROM PICTURES WHERE ID=" + string(_itoa(pictureId, buff, 10)) + ";";
	sqlite3_exec(_sqldb, sqlStatement.c_str(), nullptr, this, &errMessage);

	sqlStatement = "DELETE FROM TAGS WHERE PICTURE_ID=" + string(_itoa(pictureId, buff, 10)) + ";";
	sqlite3_exec(_sqldb, sqlStatement.c_str(), nullptr, this, &errMessage);
}

void CDatabaseAccess::tagUserInPicture(CPicture& picture, int userId)
{
	RETURN_IF_INVALID;

	string sqlStatement;
	char *errMessage = nullptr;
	char buff[10];
	string picIdStr = string(_itoa(picture.getId(), buff, 10));
	string userIdStr = string(_itoa(userId, buff, 10));

	sqlStatement = "INSERT INTO TAGS (PICTURE_ID, USER_ID) "	\
        "VALUES (" + picIdStr + ", " + userIdStr + ");";

	sqlite3_exec(_sqldb, sqlStatement.c_str(), nullptr, this, &errMessage);

	picture.tagUser(userId);
}

void CDatabaseAccess::untagUserInPicture(CPicture& picture, int userId)
{
	RETURN_IF_INVALID;

	string sqlStatement;
	char *errMessage = nullptr;
	char buff[10];
	string picIdStr = string(_itoa(picture.getId(), buff, 10));
	string userIdStr = string(_itoa(userId, buff, 10));

	sqlStatement = "DELETE FROM TAGS WHERE PICTURE_ID=" + picIdStr + " AND USER_ID=" + userIdStr + ";";

	sqlite3_exec(_sqldb, sqlStatement.c_str(), sqlExecCallback, this, &errMessage);

	picture.untagUser(userId);
}

bool CDatabaseAccess::isUserTaggedInPicture(const CPicture& picture, int userId)
{
	RETURN_RES_IF_INVALID(false);

	string sqlStatement;
	char *errMessage = nullptr;
	char buff[10];

	resetLastId();
	sqlStatement = "SELECT * FROM TAGS WHERE PICTURE_ID=" + string(_itoa(picture.getId(), buff, 10)) + " AND USER_ID=" + string(_itoa(userId, buff, 10)) + ";";
	sqlite3_exec(_sqldb, sqlStatement.c_str(), sqlExecCallback, this, &errMessage);

	return _lastId != -1;
}

const users& CDatabaseAccess::getUsers()
{
	_users.clear();
	RETURN_RES_IF_INVALID(_users);

	string sqlStatement;
	char *errMessage = nullptr;

	sqlStatement = "SELECT * FROM USERS;";

	resetLastId();
	sqlite3_exec(_sqldb, sqlStatement.c_str(), listUsersCallback, &_users, &errMessage);

	return _users;
}

void CDatabaseAccess::addUser(CUser& user)
{
	RETURN_IF_INVALID;

	string sqlStatement;
	char *errMessage = nullptr;
	int res;

	sqlStatement = "INSERT INTO USERS (NAME) "	\
        "VALUES ('" + user.getName() + "');"	\
		"SELECT * FROM USERS WHERE NAME='" + user.getName() + "';";

	res = sqlite3_exec(_sqldb, sqlStatement.c_str(), sqlExecCallback, this, &errMessage);
	if (res != SQLITE_OK)
		return;

	if (_lastId == -1)
		return;

	user.setId(_lastId);
}

void CDatabaseAccess::deleteUser(string userName)
{
	RETURN_IF_INVALID;

	string sqlStatement;
	char *errMessage = nullptr;

	sqlStatement = "DELETE FROM USERS WHERE NAME='" + userName + "';";

	sqlite3_exec(_sqldb, sqlStatement.c_str(), sqlExecCallback, this, &errMessage);
}

bool CDatabaseAccess::userExists(string userName)
{
	RETURN_RES_IF_INVALID(false);

	string sqlStatement;
	char *errMessage = nullptr;
	int res;

	sqlStatement = "SELECT * FROM USERS WHERE NAME='" + userName + "';";

	resetLastId();
	res = sqlite3_exec(_sqldb, sqlStatement.c_str(), sqlExecCallback, this, &errMessage);
	if (res != SQLITE_OK)
		return false;

	return _lastId != -1;
}

bool CDatabaseAccess::userExists(int userId)
{
	RETURN_RES_IF_INVALID(false);

	string sqlStatement;
	char *errMessage = nullptr;
	int res;

	char buff[10];
	sqlStatement = "SELECT * FROM USERS WHERE ID=" + string(_itoa(userId, buff, 10)) + ";";

	resetLastId();
	res = sqlite3_exec(_sqldb, sqlStatement.c_str(), sqlExecCallback, this, &errMessage);
	if (res != SQLITE_OK)
		return false;

	return _lastId != -1;
}

CUser* CDatabaseAccess::getUser(int userId)
{
	RETURN_RES_IF_INVALID(nullptr);

	getUsers();
	if (_users.size() == 0)
		return nullptr;

	users_iter iter = _users.begin();
	while (iter != _users.end())
	{
		if (iter->getId() == userId)
			return &*iter;
		++iter;
	}

	return nullptr;
}

int CDatabaseAccess::countAlbumsOwnedOfUser(int userId) {
	RETURN_RES_IF_INVALID(-1);

	string sqlStatement;
	char *errMessage = nullptr;
	int res;
	char buff[10];

	sqlStatement = "SELECT COUNT(*) FROM ALBUMS WHERE USER_ID=" + string(_itoa(userId, buff, 10)) + ";";

	int albumsCount;

	res = sqlite3_exec(_sqldb, sqlStatement.c_str(), sqlInt, &albumsCount, &errMessage);
	if (res != SQLITE_OK)
		return -1;

	return albumsCount;
}

int CDatabaseAccess::countAlbumsTaggedOfUser(int userId)
{
	RETURN_RES_IF_INVALID(-1);

	string sqlStatement;
	char *errMessage = nullptr;
	int res;
	char buff[10];

	sqlStatement = "SELECT COUNT(DISTINCT ALBUM_ID) FROM ALBUMS "                    \
		           "INNER JOIN PICTURES ON PICTURES.ALBUM_ID = ALBUMS.ID "			 \
				   "LEFT JOIN TAGS ON PICTURES.ID = TAGS.PICTURE_ID "				 \
				   "WHERE TAGS.USER_ID =" + string(_itoa(userId, buff, 10)) + ";";

	int albumsCount;

	res = sqlite3_exec(_sqldb, sqlStatement.c_str(), sqlInt, &albumsCount, &errMessage);
	if (res != SQLITE_OK)
		return -1;

	return albumsCount;
}

int CDatabaseAccess::countTagsOfUser(int userId)
{
	RETURN_RES_IF_INVALID(-1);

	string sqlStatement;
	char *errMessage = nullptr;
	int res;
	char buff[10];

	sqlStatement = "SELECT COUNT(*) FROM TAGS WHERE USER_ID=" + string(_itoa(userId, buff, 10)) + ";";

	int albumsCount;

	res = sqlite3_exec(_sqldb, sqlStatement.c_str(), sqlInt, &albumsCount, &errMessage);
	if (res != SQLITE_OK)
		return -1;

	return albumsCount;
}

float CDatabaseAccess::averageTagsPerAlbumOfUser(int userId)
{
	float albumsOfTaggedUser = static_cast<float>(countAlbumsTaggedOfUser(userId));
	if (albumsOfTaggedUser == 0)
		return albumsOfTaggedUser;

	return static_cast<float>(countTagsOfUser(userId)) / albumsOfTaggedUser;
}

const CUser* CDatabaseAccess::getTopTaggedUser()
{
	RETURN_RES_IF_INVALID(nullptr);

	string sqlStatement;
	char *errMessage = nullptr;
	int res;

	sqlStatement = "select USER_ID, count(*) from tags group by USER_ID;";

	pair<int, int> tagsPair;
	tagsPair.first = -1;
	tagsPair.second = -1;
	res = sqlite3_exec(_sqldb, sqlStatement.c_str(), sqlTopId, &tagsPair, &errMessage);
	if (res != SQLITE_OK)
		return nullptr;

	if (tagsPair.first == -1)
		return nullptr;

	return getUser(tagsPair.first);
}

const CPicture* CDatabaseAccess::getTopTaggedPicture()
{
	RETURN_RES_IF_INVALID(nullptr);

	string sqlStatement;
	char *errMessage = nullptr;
	int res;

	sqlStatement = "select PICTURE_ID, count(*) from tags group by PICTURE_ID;";

	pair<int, int> tagsPair;
	tagsPair.first = -1;
	tagsPair.second = -1;
	res = sqlite3_exec(_sqldb, sqlStatement.c_str(), sqlTopId, &tagsPair, &errMessage);
	if (res != SQLITE_OK)
		return nullptr;

	if (tagsPair.first == -1)
		return nullptr;

	char buff[10];
	pictures picturesList;
	sqlStatement = "SELECT * FROM PICTURES WHERE ID=" + string(_itoa(tagsPair.first, buff, 10)) + ";";
	sqlite3_exec(_sqldb, sqlStatement.c_str(), listPicturesCallback, &picturesList, &errMessage);
	if (res != SQLITE_OK)
		return nullptr;

	if (picturesList.size() == 0)
		return nullptr;

	pictures_iter iter = picturesList.begin();
	while (iter != picturesList.end())
	{
		if (iter->getId() == tagsPair.first)
		{
			_topTaggedPicture = *iter;
			return &_topTaggedPicture;
		}
		++iter;
	}

	return nullptr;
}

const pictures& CDatabaseAccess::getTaggedPicturesOfUser(int userId)
{
	_picturesOfUser.clear();
	RETURN_RES_IF_INVALID(_picturesOfUser);

	string sqlStatement;
	char *errMessage = nullptr;

	char buff[10];
	sqlStatement = "SELECT PICTURES.* FROM PICTURES "                                 \
			       "LEFT JOIN TAGS ON PICTURES.ID = TAGS.PICTURE_ID "				 \
		           "WHERE TAGS.USER_ID =" + string(_itoa(userId, buff, 10)) + ";";

	int res = sqlite3_exec(_sqldb, sqlStatement.c_str(), listPicturesCallback, &_picturesOfUser, &errMessage);

	return _picturesOfUser;
}

void CDatabaseAccess::setLastId(char* lastId)
{
	_lastId = atoi(lastId);
}

void CDatabaseAccess::resetLastId()
{
	_lastId = -1;
}

bool CDatabaseAccess::open()
{
	// check if file exists
	string filename = "galleryDB.sqlite";
	bool fileExisted = fileExistsOnDisk(filename);

	// try to open the database
	int res = sqlite3_open(filename.c_str(), &_sqldb);
	if (res != SQLITE_OK) {
		_sqldb = nullptr;
		return false;
	}

	// db is open, check if we need to init it
	if (fileExisted)
		return true;

	// need to init, lets create the tables and some other stuff...
	return initDatabase();
}

void CDatabaseAccess::close()
{
	RETURN_IF_INVALID;

	sqlite3_close(_sqldb);
	_sqldb = nullptr;
}

void CDatabaseAccess::clear()
{
	RETURN_IF_INVALID;

	char *sqlStatement;
	char *errMessage = nullptr;

	sqlStatement = "DELETE FROM ALBUMS;"	\
		"DELETE FROM PICTURES;"				\
		"DELETE FROM USERS;"				\
		"DELETE FROM TAGS;";

	sqlite3_exec(_sqldb, sqlStatement, nullptr, nullptr, &errMessage);
}

bool CDatabaseAccess::fileExistsOnDisk(const string& filename)
{
	struct stat buffer;   
	return (stat(filename.c_str(), &buffer) == 0); 
}

bool CDatabaseAccess::initDatabase()
{
	char *sqlStatement;
	char *errMessage = nullptr;
	int res;

	// create Users table
	sqlStatement = "CREATE TABLE USERS("				\
		"ID    INTEGER   PRIMARY KEY   AUTOINCREMENT,"	\
		"NAME  TEXT					NOT NULL);";

	res = sqlite3_exec(_sqldb, sqlStatement, nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
		return false;

	// create Albums table
	sqlStatement = "CREATE TABLE ALBUMS("							\
		"ID			     INTEGER   PRIMARY KEY	AUTOINCREMENT,"   	\
		"NAME			 TEXT					NOT NULL,"			\
		"CREATION_DATE   TEXT					NOT NULL,"          \
		"USER_ID         INT                    NOT NULL,"          \
		"FOREIGN KEY(USER_ID)      REFERENCES   USER(ID));";

	res = sqlite3_exec(_sqldb, sqlStatement, nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
		return false;



	// create Pictures table
	sqlStatement = "CREATE TABLE PICTURES("							\
         "ID			 INTEGER   PRIMARY KEY		AUTOINCREMENT,"	\
         "NAME			 TEXT						NOT NULL,"		\
         "LOCATION		 TEXT						NOT NULL,"		\
		 "CREATION_DATE  TEXT						NOT NULL,"		\
         "ALBUM_ID		 INT						NOT NULL,"      \
		 "FOREIGN KEY(ALBUM_ID)    REFERENCES       ALBUM(ID));";

	res = sqlite3_exec(_sqldb, sqlStatement, nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
		return false;

	// create Tags table
	sqlStatement = "CREATE TABLE TAGS("						     	\
         "ID			INTEGER      PRIMARY KEY   AUTOINCREMENT,"	\
         "PICTURE_ID	INT						   NOT NULL,"		\
         "USER_ID		INT						   NOT NULL,"       \
		 "FOREIGN KEY(PICTURE_ID)    REFERENCES    PICTURES(ID),"    \
	     "FOREIGN KEY(USER_ID)       REFERENCES    USERS(ID));";

	res = sqlite3_exec(_sqldb, sqlStatement, nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
		return false;

	return true;
}
