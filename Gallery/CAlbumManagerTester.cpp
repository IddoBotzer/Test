#include "CAlbumManagerTester.h"

#define DEFINE_SUCCESS bool success = false
#define RETURN_RESULT if (success) return "PASSED"; else return "FAILED"

CAlbumManagerTester::CAlbumManagerTester(CAlbumManager& manager) : _manager(manager)
{
}

string CAlbumManagerTester::createFiveUsers()
{
	DEFINE_SUCCESS;

	RETURN_RESULT;
}
