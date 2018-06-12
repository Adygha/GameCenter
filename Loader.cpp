/**************************************************************************************/
// File:	Loader.cpp
// Summary:	The 'Game Center' loader that contains the 'main()' and	the main 'App'
//			class that represents the application. An 'App' object represents a
//			framework and has a polymorphic function that accepts objects that
//			implement/inherit the 'IGame' and 'IPlayer' pure abstract classes.
// -------------------------------------------
// Author:	Janty Azmat
// -------------------------------------------
// Log:		
// -------------------------------------------
// Notes:	
/**************************************************************************************/

#include "SpaceTubePlayer.h"
#include "SpaceTubeGame.h"
#include <iostream>
#include <string.h>
#include "Console.h"
#include "Menu.h"

#pragma region CONSTs
const int DEF_MONEY = 500; // Default money a player can have
const int DEF_PLAYTIMES = 10; // Default number of times to play in a round
const char *MSG_ERR_GEN = "\nAn unexpected error occured with message: \"";
const char *MSG_ERR_PROG = "\nAn error occured with message: \"";
const char *MSG_TEMINATE = "\nTerminating application...";
const char *MSG_ERR_NO_GAME = "No game added yet. Please add some games first.";
const char *MSG_ERR_FULL = "Cannot add more game to this 'App' object.";
const char *MSG_ERR_NOT_MATCH = "A provided game and player objects don't match.";
static const char *MSG_WELCOME = "\n\tWelcome to the Game-Center\n\n";
const char *MSG_GAME_MONEY = "\n Your credit/money for the game '";
const char *MSG_GAME_TIMES = "\n The number of times you'll play the next chosen game is: ";
const char *MSG_APP_MENU_TITLE = "\n Please enter your choice from the menu:";
const char *MSG_GAME_MENU_CHOICE = "Play the game: ";
const char *MSG_TIMES_MENU_CHOICE = "Specify the number of times you want to play the next chosen game";
const char *MSG_QUIT_MENU_CHOICE = "Quit Game-Center";
const char *MSG_ENTER_TIMES = "\n Pleae specify the number of times you want to play the next chosen game: ";
const char *MSG_CANNOT_PLAY = "\n Cannot play the game right now. Please check if your credit/money is"
							" enough, or try again a bit later.\n\n Press 'Enter' to continue...";

union NameId { // Used to convert the game's ID to a 4 char string
	int gameID;
	char gameName[5]{}; // Must be four letters (five with the null char)
};
#pragma endregion

#pragma region APP_DEC
class App {
public:
	App(unsigned maxGameCount);
	~App();

	void addGame(IGame &theGame, IPlayer &thePlayer); // Adds a ''-'' pair to this framework
	int run(); // The framework's running method

private:
	unsigned _mePlayTimes; // The number of times to play in a round
	unsigned _meMaxGmCnt; // Maximum games that can fit in the framework
	unsigned _meGmCnt; // Actual games plugged in to the framework
	char **_meChoices; // Array of c-string choices
	IPlayer **_mePlayers;
	IGame **_meGames;
	Console *_meCon;	//
	Menu *_meMenu;		// From the static library to display choices menu and stuff

	const char *getGameName(int gameID); // Extracts the name of the game from its ID
};
#pragma endregion

#pragma region APP_DEF

/**************************************************************************************/
// Conversion constructor.
// -------------------------------------------
// Parameters:
//		- maxGameCount:	the maximum number of games that can fit in this framework.
/**************************************************************************************/
App::App(unsigned maxGameCount) : _mePlayTimes(DEF_PLAYTIMES),
								_meMaxGmCnt(maxGameCount),
								_meGmCnt(0),
								_meChoices(new char *[_meMaxGmCnt + 2]{}), // Has 2 extra choice (in addition to games)
								_mePlayers(new IPlayer *[_meMaxGmCnt]{}),
								_meGames(new IGame *[_meMaxGmCnt]{}),
								_meCon(new Console()),
								_meMenu(new Menu(_meCon)) {

	// int tmpLen = strlen(MSG_TIMES_MENU_CHOICE) + 1;
	size_t tmpLen = strlen(MSG_TIMES_MENU_CHOICE) + 1;
	this->_meChoices[0] = new char[tmpLen]{'\0'};					//
#ifdef _WIN32														//
	strcpy_s(this->_meChoices[0], tmpLen, MSG_TIMES_MENU_CHOICE);	// Add the add-times choice
#else																//
	strcpy(this->_meChoices[0], MSG_TIMES_MENU_CHOICE);				//
#endif // _WIN32

	tmpLen = strlen(MSG_QUIT_MENU_CHOICE) + 1;
	this->_meChoices[1] = new char[tmpLen]{'\0'};					//
#ifdef _WIN32														//
	strcpy_s(this->_meChoices[1], tmpLen, MSG_QUIT_MENU_CHOICE);	// Add the quit choice
#else																//
	strcpy(this->_meChoices[1], MSG_QUIT_MENU_CHOICE);				//
#endif // _WIN32
}

// Destructor
App::~App() {
	unsigned idx;

	for (idx = 0; idx < this->_meGmCnt; idx++) { // Delete all the second dimentions
		//delete this->_meGames[idx]; // Only when this object does not loan them
		this->_meGames[idx] = nullptr;
		//delete this->_mePlayers[idx]; // Only when this object does not loan them
		this->_mePlayers[idx] = nullptr;
		delete[] this->_meChoices[idx];
		this->_meChoices[idx] = nullptr;
	}
	delete[] this->_meChoices[idx];		// Delete the additional c-string that
	this->_meChoices[idx++] = nullptr;	// holds the play-times choice
	delete[] this->_meChoices[idx];			// Delete the additional c-string that
	this->_meChoices[idx] = nullptr;		// holds the quit-app choice
	delete[] this->_meGames;	//
	this->_meGames = nullptr;	//
	delete[] this->_mePlayers;	//
	this->_mePlayers = nullptr;	//
	delete[] this->_meChoices;	// De-allocate all fields
	this->_meChoices = nullptr;	//
	delete this->_meMenu;		//
	this->_meMenu = nullptr;	//
	delete this->_meCon;		//
	this->_meCon = nullptr;		//
}

/**************************************************************************************/
// Adds a player-game object pair that represents a game to this framework.
// -------------------------------------------
// Parameters:
//		- theGame:		the 'IGame' part of the game.
//		- thePlayer:	the 'IPlayer' part of the game.
/**************************************************************************************/
void App::addGame(IGame &theGame, IPlayer &thePlayer) {

	if (this->_meGmCnt == this->_meMaxGmCnt) { // When maximum games reached
		throw std::runtime_error(MSG_ERR_FULL);
	} else if (!thePlayer.setGame(&theGame)) { // When 'setGame()' detects mismatched game/player
		throw std::runtime_error(MSG_ERR_NOT_MATCH);
	}

	const char *tmpName = this->getGameName(theGame.getID());
	// unsigned tmpLen = strlen(MSG_GAME_MENU_CHOICE) + strlen(tmpName) + 1;
	size_t tmpLen = strlen(MSG_GAME_MENU_CHOICE) + strlen(tmpName) + 1;

	this->_meChoices[this->_meGmCnt + 2] = this->_meChoices[this->_meGmCnt + 1]; // Push the add-times choice one step down
	this->_meChoices[this->_meGmCnt + 1] = this->_meChoices[this->_meGmCnt]; // Push the quit-app choice one step down
	this->_mePlayers[this->_meGmCnt] = &thePlayer;			//
	this->_meGames[this->_meGmCnt] = &theGame;				// Stash them with their choices
	this->_meChoices[this->_meGmCnt] = new char[tmpLen];	//
#ifdef _WIN32
	strcpy_s(this->_meChoices[this->_meGmCnt], tmpLen, MSG_GAME_MENU_CHOICE);	//
	strncat_s(this->_meChoices[this->_meGmCnt++], tmpLen, tmpName, _TRUNCATE);	//
#else																			// Build choice line
	strcpy(this->_meChoices[this->_meGmCnt], MSG_GAME_MENU_CHOICE);				//
	strncat(this->_meChoices[this->_meGmCnt++], tmpName, tmpLen - 1);			//
#endif // _WIN32
}

/**************************************************************************************/
// A helper private method that extracts the name of the game from its ID.
// -------------------------------------------
// Note:	I assumed that the standard will be that the name of the game will be held
//			in the game's ID, as a 4 letter c-string, since neither the 'IGame' nor the
//			'IPlayer' interfaces contain a way to get the game's name to show to the
//			user (it's better than trying to cast every 'IGame' or 'IPlayer' to check
//			if they have a name or not).
// -------------------------------------------
// Parameters:
//		- gameID:	the game's ID.
// -------------------------------------------
// Returns:	the game's 4-letter name that is represented by the specified ID, or the
//			string "CRAP" if the ID does not hold readable characters.
/**************************************************************************************/
const char *App::getGameName(int gameID) {
	static NameId nameId; // Used to convert the game's ID to a 4 char string
	nameId.gameID = gameID;
	return (nameId.gameName[0] > 31 && nameId.gameName[0] < 127) ? nameId.gameName : "CRAP"; // Not readable text (not mine) then CRAP
}

/**************************************************************************************/
// The framework's running method.
/**************************************************************************************/
int App::run() {
	this->_meCon->clearPage();

	if (!this->_meGmCnt)
		throw std::runtime_error(MSG_ERR_NO_GAME);

	unsigned tmpChoiceIdx;

	do {
		this->_meCon->displayMessage(MSG_WELCOME, true); // The welcome message

		for (unsigned idx = 0; idx < this->_meGmCnt; idx++) { // Loop for every game to display its available money
			this->_meCon->displayMessage(MSG_GAME_MONEY); // Display the game-money line
			this->_meCon->displayMessage(this->getGameName(this->_meGames[idx]->getID())); // Display game name
			this->_meCon->displayMessage("' is: ");
			this->_meCon->displayNumber(this->_mePlayers[idx]->getMoney(), true);// Display the game-money number
		}

		this->_meCon->displayMessage(MSG_GAME_TIMES); // Display the play-times line
		this->_meCon->displayNumber((int)this->_mePlayTimes, true); // Display the play-times number

		this->_meMenu->assignChoices(MSG_APP_MENU_TITLE, this->_meChoices, this->_meGmCnt + 2); // With the extra 2 choices
		tmpChoiceIdx = this->_meMenu->select(false) - 1;

		if (tmpChoiceIdx < this->_meGmCnt) { // Playing a game is chosen

			if (!this->_mePlayers[tmpChoiceIdx]->play(this->_mePlayTimes))
				this->_meCon->readEnter(MSG_CANNOT_PLAY); // In case something wrong happens

			this->_meCon->clearPage();
		} else if (tmpChoiceIdx == this->_meGmCnt) { // The play-times choice
			this->_meCon->clearPage();
			this->_mePlayTimes = this->_meCon->readInteger(MSG_ENTER_TIMES, 1);
			this->_meCon->clearPage();
		}
	} while (tmpChoiceIdx <= this->_meGmCnt); // Loop until exit choice
	return 0;
}
#pragma endregion

// Main entry point
int main(int argc, char **argv) {
	int outInt; // Mutipurpose int

	if (argc < 2) { // Take the starting money from command line argument if exists or set to default
		outInt = DEF_MONEY;
	} else {
		outInt = atoi(argv[1]);
		if (outInt < 1)
			outInt = DEF_MONEY;
	}

	try {
		App *tmpApp = new App(2); // The constructor takes the max number of games the object takes
		SpaceTubePlayer *tmpSpacePlay = new SpaceTubePlayer(outInt);	// Can't declare these two using 'IPlayer' and 'IGame'
		SpaceTubeGame *tmpSpaceGame = new SpaceTubeGame();				// since they don't have virtual destructors (but are
																		// passed as 'IPlayer' and 'IGame' to 'addGame()')

		tmpApp->addGame(*tmpSpaceGame, *tmpSpacePlay); // Add the space-tube-game (loan objects)

		outInt = tmpApp->run();

		delete tmpSpaceGame;	//
		delete tmpSpacePlay;	// No need to put them in the catch again
		delete tmpApp;			// since the app is quitting anyway.
		return outInt;
	} catch (const std::runtime_error &ex) { // My own thrown exception (intended for the programmer using the classes)
		std::cerr << MSG_ERR_PROG;
		std::cerr << ex.what() << "\"" << std::endl;
		std::cerr << MSG_TEMINATE << std::endl;
		return 1;
	} catch (const std::exception &ex) { // When unexpected exception
		std::cerr << MSG_ERR_GEN;
		std::cerr << ex.what() << "\"" << std::endl;
		std::cerr << MSG_TEMINATE << std::endl;
		return 1;
	}
}
