#pragma once
#include "CAlbumManager.h"

class CAlbumManagerTester
{
private:
	CAlbumManager& _manager;

public:
	CAlbumManagerTester(CAlbumManager& manager);
	string createFiveUsers();
};
