#include "stdafx.h"
#include "CUser.h"

CUser::CUser() : _id(0)
{
	
}

string CUser::getName() const
{
	return _name;
}

void CUser::setName(string name)
{
	_name = name;
}

int CUser::getId() const
{
	return _id;
}

void CUser::setId(int id)
{
	_id = id;
}

