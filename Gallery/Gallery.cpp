// fry.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CMemoryAccess.h"
#include <iostream>
#include <ctime>
#include <string>
#include "CAlbumManager.h"
#include "CDataAccessTester.h"

using namespace std;

void printInfo(); 

int main(int argc, char **argv)
{
	// initialization data access
	CMemoryAccess dataAccess;
	if (dataAccess.open() == false)
	{
		cout << "Error: Failed to open database.\n";
		return 1;
	}

	// initialize album manager
	CAlbumManager albumMannager(dataAccess);

	// logic goes here
	bool keepRunning = true;
	string command;
	printInfo();
	do
	{
		cout << "\nPlease enter 'exit' to quit, 'help' for help screen or any other command: ";
		getline(cin, command);
		if (command.compare("exit") == 0) {
			keepRunning = false;
		} else if (command.compare("help") == 0) {
			albumMannager.printHelp();
		} else {
			albumMannager.executeCommand(command);
		}
	} while (keepRunning == true);

	// cleanup
	dataAccess.close();

	// done
	return 0;
}

//Prints my name and the current time + date
void printInfo()
{
	//Time manager
	const time_t ctt = time(0);
	cout << "Welcome to Iddo Botzer's Gallery!\n" << asctime(localtime(&ctt)) << endl;

}



