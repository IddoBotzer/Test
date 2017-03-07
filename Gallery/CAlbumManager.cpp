#include "stdafx.h"
#include "CAlbumManager.h"
#include <iostream>
#include "Constants.h"


CAlbumManager::CAlbumManager(CDataAccess& dataAccess) : _dataAccess(dataAccess), _openAlbum(nullptr)
{
	
}

void CAlbumManager::executeCommand(const string& command)
{
	if (command.compare(CREATE_ALBUM_COMMAD) == 0) {
		createAlbum();
	}
	else if (command.compare(OPEN_ALBUM_COMMAD) == 0) {
		openAlbum();
	}
	else if (command.compare(CLOSE_ALBUM_COMMAD) == 0) {
		closeAlbum();
	}
	else if (command.compare(DELETE_ALBUM_COMMAD) == 0) {
		deleteAlbum();
	}
	else if (command.compare(LIST_ALBUMS_COMMAD) == 0) {
		listAlbums();
	}
	else if (command.compare(LIST_ALBUMS_OF_USER_COMMAND) == 0) {
		listAlbumsOfUser();
	}
	else if (command.compare(ADD_PICTURE_COMMAD) == 0) {
		addPictureToAlbum();
	}
	else if (command.compare(REMOVE_PICTURE_COMMAD) == 0) {
		removePictureFromAlbum();
	}
	else if (command.compare(LIST_PICTURES_COMMAD) == 0) {
		listPicturesInAlbum();
	}
	else if (command.compare(OPEN_PICTURE_COMMAD) == 0) {
		openPicture();
	}
	else if (command.compare(TAG_USER_COMMAD) == 0) {
		tagUserInPicture();
	}
	else if (command.compare(UNTAG_USER_COMMAD) == 0) {
		untagUserInPicture();
	}
	else if (command.compare(LIST_TAGS_COMMAND) == 0) {
		listUserTags();
	}
	else if (command.compare(ADD_USER_COMMAD) == 0) {
		addUser();
	}
	else if (command.compare(REMOVE_USER_COMMAD) == 0) {
		removeUser();
	}
	else if (command.compare(LIST_USERS_COMMAD) == 0) {
		listUsers();
	}
	else if (command.compare(USER_STATISTICS_COMMAND) == 0) {
		userStatistics();
	}
	else if (command.compare(TOP_TAGGED_USER_COMMAND) == 0) {
		topTaggedUser();
	}
	else if (command.compare(TOP_TAGGED_PICTURE_COMMAND) == 0) {
		topTaggedPicture();
	}
	else if (command.compare(PICTURES_TAGGED_USER) == 0) {
		picturesTaggedUser();
	}
	else {
		cout << "Error: Invalid command [" << command << "]\n";
	}
}

void CAlbumManager::printHelp()
{
	cout << "\n";
	cout << "Supported Album commands:\n";
	cout << CREATE_ALBUM_COMMAD << '\n';
	cout << OPEN_ALBUM_COMMAD << '\n';
	cout << CLOSE_ALBUM_COMMAD << '\n';
	cout << DELETE_ALBUM_COMMAD << '\n';
	cout << LIST_ALBUMS_COMMAD << '\n';
	cout << LIST_ALBUMS_OF_USER_COMMAND << '\n';
	cout << "\n";
	
	cout << "Supported Album commads (when specific album is open):" << '\n';
	cout << ADD_PICTURE_COMMAD << '\n';
	cout << REMOVE_PICTURE_COMMAD << '\n';
	cout << LIST_PICTURES_COMMAD << '\n';
	cout << OPEN_PICTURE_COMMAD << '\n';
	cout << TAG_USER_COMMAD << '\n';
	cout << UNTAG_USER_COMMAD << '\n';
	cout << LIST_TAGS_COMMAND << '\n';
	cout << "\n";
	
	cout << "Supported User commads:" << '\n';
	cout << ADD_USER_COMMAD << '\n';
	cout << REMOVE_USER_COMMAD << '\n';
	cout << LIST_USERS_COMMAD << '\n';
	cout << USER_STATISTICS_COMMAND << '\n';
	cout << "\n";
	
	cout << "Supported Queries:" << '\n';
	cout << TOP_TAGGED_USER_COMMAND << '\n';
	cout << TOP_TAGGED_PICTURE_COMMAND << '\n';
	cout << PICTURES_TAGGED_USER << '\n';
	cout << "\n";
}

void CAlbumManager::createAlbum()
{
	cout << "Album - ";
	string name = readName();
	if (_dataAccess.albumExists(name) == true) {
		cout << "Error: Failed to create album, album with the same name already exists\n";
		return;
	}

	string userIdStr;
	cout << "Enter user id: ";
	getline(cin, userIdStr);

	int userId = stoi(userIdStr);
	if (userId == -1)
	{
		cout << "Aborted...\n";
		return;
	}

	if (_dataAccess.userExists(userId) == false) {
		cout << "Error: Can't create album since there is no user with id [" << userIdStr << "]\n";
		return;
	}

	CAlbum newAlbum;
	newAlbum.setName(name);
	newAlbum.setUserId(userId);

	_dataAccess.insertAlbum(newAlbum);
	cout << "Album [" << name << "] " << "@" << userId << " created successfully\n";
}

void CAlbumManager::openAlbum()
{
	// close open album
	if (_openAlbum != nullptr) {
		closeAlbum();
	}

	// try to open a new one
	string name = readName();
	_openAlbum = _dataAccess.openAlbum(name);
	if (_openAlbum == nullptr) {
		cout << "Error: Failed to open album since it does not exists\n";
		return;
	}

	// success
	cout << "Album [" << name << "] opened successfully\n";
}

void CAlbumManager::closeAlbum()
{
	if (_openAlbum == nullptr) {
		cout << "Error: There is no open album to close.\n";
		return;
	}

	cout << "Album [" << _openAlbum->getName() << "] closed successfully\n";
	_dataAccess.closeAlbum(_openAlbum);
	_openAlbum = nullptr;
}

void CAlbumManager::deleteAlbum()
{
	// get album name
	string name = readName();

	// check if exists
	if (_dataAccess.albumExists(name) == false) {
		cout << "Error: Failed to delete album since it does not exists\n";
		return;
	}

	// album exist, close album if it is opened
	if (_openAlbum != nullptr && _openAlbum->getName().compare(name) == 0) {
		closeAlbum();
	}

	_dataAccess.deleteAlbum(name);
	cout << "Album [" << name << "] deleted successfully\n";
}

void CAlbumManager::listAlbums()
{
	const albums& albums = _dataAccess.getAlbums();
	cout << "Albums list:\n";
	for (albums_const_iter iter = albums.begin(); iter != albums.end(); ++iter) {
		cout << "   + Album [" << iter->getId() << "] " << "- " << 
			    iter->getName() << " @" << iter->getUserId() << "\n";
	}
	cout << "\n";
}

void CAlbumManager::listAlbumsOfUser()
{

	string userIdStr;
	cout << "Enter user id: ";
	getline(cin, userIdStr);

	int userId = stoi(userIdStr);
	if (userId == -1)
	{
		cout << "Aborted...\n";
		return;
	}

	if (_dataAccess.userExists(userId) == false) {
		cout << "Error: Can't tag user in picture since there is no user with id [" << userIdStr << "] in album\n";
		return;
	}

	const albums& albums = _dataAccess.getAlbumsOfUser(userId);

	cout << "Albums list @" << userId << ":\n";
	for (albums_const_iter iter = albums.begin(); iter != albums.end(); ++iter) {
		cout << "   + Album [" << iter->getId() << "] " << "- " <<
			iter->getName() <<
			", creation date: " << iter->getCreationDate() << "\n";
	}
}

void CAlbumManager::addPictureToAlbum()
{
	if (_openAlbum == nullptr) {
		cout << "Error: Can't perform operation, you need to open an album first\n";
		return;
	}

	string picName;
	cout << "Enter picture name: ";
	getline(cin, picName);

	string picLocation;
	cout << "Enter picture location: ";
	getline(cin, picLocation);

	CPicture picture;
	picture.setName(picName);
	picture.setLocation(picLocation);
	_dataAccess.addPictureToAlbum(_openAlbum->getId(), picture);

	cout << "Picture [" << picture.getId() << "] successfully added to Album [" << _openAlbum->getName() << "]\n";
}

void CAlbumManager::removePictureFromAlbum()
{
	if (_openAlbum == nullptr) {
		cout << "Error: Can't perform operation, you need to open an album first\n";
		return;
	}

	string picIdStr;

	cout << "Enter picture id: ";
	getline(cin, picIdStr);

	int picId = stoi(picIdStr);
	if (picId == -1)
	{
		cout << "Aborted...\n";
		return;
	}

	_dataAccess.removePictureFromAlbum(_openAlbum->getId(), picId);
	cout << "Picture [" << picId << "] successfully removed from Album [" << _openAlbum->getName() << "]\n";
}

void CAlbumManager::listPicturesInAlbum()
{
	if (_openAlbum == nullptr) {
		cout << "Error: Can't perform operation, you need to open an album first\n";
		return;
	}

	cout << "List of pictures in Album [" << _openAlbum->getName() << "] @" << _openAlbum->getUserId() << " :\n";
	const pictures& albumPictures = _openAlbum->getPictures();
	for (pictures_const_iter iter = albumPictures.begin(); iter != albumPictures.end(); ++iter) {
		cout << "   + Picture [" << iter->getId() << "] - " << iter->getName() << 
			"\tLocation: [" << iter->getLocation() << "]\tCreation Date: [" <<
				iter->getCreationDate() << "]\tTags: [" << iter->getTagsCount() << "]\n";
	}
	cout << "\n";
}

void CAlbumManager::openPicture()
{
	if (_openAlbum == nullptr) {
		cout << "Error: Can't perform operation, you need to open an album first\n";
		return;
	}

	string picIdStr;

	cout << "Enter picture id: ";
	getline(cin, picIdStr);

	int picId = stoi(picIdStr);
	if (picId == -1)
	{
		cout << "Aborted...\n";
		return;
	}

	const CPicture* pic = _openAlbum->getPicture(picId);
	if (pic == nullptr)
	{
		cout << "Error: Can't open picture since there is no picture with id [" << picIdStr << "] in album\n";
		return;
	}

	if (fileExistsOnDisk(pic->getLocation()) == false)
	{
		cout << "Error: Can't open picture since it doesnt exists on disk [" << picIdStr << "]\n";
		return;
	}

	system(pic->getLocation().c_str());
}

void CAlbumManager::tagUserInPicture()
{
	if (_openAlbum == nullptr) {
		cout << "Error: Can't perform operation, you need to open an album first\n";
		return;
	}

	string picIdStr;
	cout << "Enter picture id: ";
	getline(cin, picIdStr);

	int picId = stoi(picIdStr);
	if (picId == -1)
	{
		cout << "Aborted...\n";
		return;
	}

	CPicture* pic = _openAlbum->getPicture(picId);
	if (pic == nullptr)
	{
		cout << "Error: Can't tag user in picture since there is no picture with id [" << picIdStr << "] in album\n";
		return;
	}

	string userIdStr;
	cout << "Enter user id: ";
	getline(cin, userIdStr);

	int userId = stoi(userIdStr);
	if (userId == -1)
	{
		cout << "Aborted...\n";
		return;
	}

	if (_dataAccess.userExists(userId) == false) {
		cout << "Error: Can't tag user in picture since there is no user with id [" << userIdStr << "] in album\n";
		return;
	}

	_dataAccess.tagUserInPicture(*pic, userId);
	cout << "User [" << userIdStr << "] successfulkly tagged in picture [" << pic->getName() << "] in album [" << _openAlbum->getName() << "]\n";
}

void CAlbumManager::untagUserInPicture()
{
	if (_openAlbum == nullptr) {
		cout << "Error: Can't perform operation, you need to open an album first\n";
		return;
	}

	string picIdStr;
	cout << "Enter picture id: ";
	getline(cin, picIdStr);

	int picId = stoi(picIdStr);
	if (picId == -1)
	{
		cout << "Aborted...\n";
		return;
	}

	CPicture* pic = _openAlbum->getPicture(picId);
	if (pic == nullptr)
	{
		cout << "Error: Can't untag user from picture since there is no picture with id [" << picIdStr << "] in album\n";
		return;
	}

	string userIdStr;
	cout << "Enter user id: ";
	getline(cin, userIdStr);

	int userId = stoi(userIdStr);
	if (userId == -1)
	{
		cout << "Aborted...\n";
		return;
	}

	_dataAccess.untagUserInPicture(*pic, userId);
	cout << "User [" << userIdStr << "] successfulkly untagged in picture [" << pic->getName() << "] in album [" << _openAlbum->getName() << "]\n";
}

void CAlbumManager::listUserTags()
{
	if (_openAlbum == nullptr) {
		cout << "Error: Can't perform operation, you need to open an album first\n";
		return;
	}

	string picIdStr;
	cout << "Enter picture id: ";
	getline(cin, picIdStr);

	int picId = stoi(picIdStr);
	if (picId == -1)
	{
		cout << "Aborted...\n";
		return;
	}

	const CPicture* pic = _openAlbum->getPicture(picId);
	if (pic == nullptr)
	{
		cout << "Error: Can't list user tags in picture since there is no picture with id [" << picIdStr << "] in album\n";
		return;
	}

	const set<int>& users = pic->getUserTags();
	cout << "Tagget users in picture [" << picIdStr << "]:\n";
	for (set<int>::const_iterator iter = users.begin(); iter != users.end(); ++iter) {
		CUser* user = _dataAccess.getUser(*iter);
		cout << "   + User [" << user->getId() << "] - " << user->getName() << "\n";
	}
	cout << "\n";

}

void CAlbumManager::addUser()
{
	string name = readName();
	if (_dataAccess.userExists(name) == true) {
		cout << "Error: Failed to create user, user with the same name already exists\n";
		return;
	}

	CUser user;
	user.setName(name);
	_dataAccess.addUser(user);
	cout << "User [" << name << "] with id [" << user.getId() << "] created successfully\n";
}

void CAlbumManager::removeUser()
{
	// get user name
	string name = readName();

	// check if exists
	if (_dataAccess.userExists(name) == false) {
		cout << "Error: Failed to delete user since it does not exists\n";
		return;
	}

	_dataAccess.deleteUser(name);
	cout << "User [" << name << "] deleted successfully\n";
}

void CAlbumManager::listUsers()
{
	cout << "Users list:\n";
	const users& users = _dataAccess.getUsers();
	for (users_const_iter iter = users.begin(); iter != users.end(); ++iter) {
		cout << "   + User [" << iter->getId() << "] - " << iter->getName() << "\n";
	}
	cout << "\n";
}

void CAlbumManager::userStatistics()
{
	string userIdStr;
	cout << "Enter user id: ";
	getline(cin, userIdStr);

	int userId = stoi(userIdStr);
	if (userId == -1)
	{
		cout << "Aborted...\n";
		return;
	}

	if (_dataAccess.userExists(userId) == false) {
		cout << "Error: Can't generate statistics since there is no user with id [" << userIdStr << "] in album\n";
		return;
	}
	cout << "@" << userId << " Statistics:\n" <<
		"Count of Albums Tagged: " << _dataAccess.countAlbumsTaggedOfUser(userId) << "\n" <<
		"Count of Tags: " << _dataAccess.countTagsOfUser(userId) << "\n" <<
		"Avarage Tags per Alboum: " << _dataAccess.averageTagsPerAlbumOfUser(userId) << "\n";


}

void CAlbumManager::topTaggedUser()
{
	const CUser *user = _dataAccess.getTopTaggedUser();
	if (user == nullptr)
	{
		cout << "Error: failed to retreive top tagged user\n";
		return;
	}

	cout << "The top tagged user is: " << user->getName() << "\n";
}

void CAlbumManager::topTaggedPicture()
{
	const CPicture *picture = _dataAccess.getTopTaggedPicture();
	if (picture == nullptr)
	{
		cout << "Error: failed to retreive top tagged picture\n";
		return;
	}

	cout << "The top tagged picture is: " << picture->getName() << "\n";
}


void CAlbumManager::picturesTaggedUser()
{
	string userIdStr;
	cout << "Enter user id: ";
	getline(cin, userIdStr);

	int userId = stoi(userIdStr);
	if (userId == -1)
	{
		cout << "Aborted...\n";
		return;
	}

	if (_dataAccess.userExists(userId) == false) {
		cout << "Error: Can't get pictures since there is no user with id [" << userIdStr << "] in album\n";
		return;
	}

	CUser* user = _dataAccess.getUser(userId);

	pictures taggedPictures = _dataAccess.getTaggedPicturesOfUser(userId);

	cout << "List of pictures of tagged User [" << user->getName() << "]" << ":\n";
	for (pictures_const_iter iter = taggedPictures.begin(); iter != taggedPictures.end(); ++iter) {
		cout << "   + Picture [" << iter->getId() << "] - " << iter->getName() <<
			"\tLocation: [" << iter->getLocation() << "]\tCreation Date: [" <<
			iter->getCreationDate() << "]\tTags: [" << iter->getTagsCount() << "]\n";
	}
	cout << "\n";
}

string CAlbumManager::readName()
{
	string name;

	cout << "Enter name: ";
	getline(cin, name);

	return name;
}

bool CAlbumManager::fileExistsOnDisk(const string& filename)
{
	struct stat buffer;   
	return (stat(filename.c_str(), &buffer) == 0); 
}
