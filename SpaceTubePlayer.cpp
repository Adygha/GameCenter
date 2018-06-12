/**************************************************************************************/
// File:	SpaceTubePlayer.cpp
// Summary:	A class used to create objects to handle the playing of a 'SpaceTubeGame'
// object and presenting the states to the player (required by the assignment).
// -------------------------------------------
// Author:	Janty Azmat
// -------------------------------------------
// Log:		
// -------------------------------------------
// Notes:	
/**************************************************************************************/

#include "Console.h"
#include "Menu.h"
#include "SpaceTubePlayer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <string.h>

#pragma region CONSTs
const int SMALL_BUF_LEN = 128; // Small buffer size
const int BIG_BUF_LEN = 512; // Big buffer size
const int MIN_GAME_COST = 50; // The cost of minimally a single game
const char *MSG_MAIN_TITLE = "\n\n\tWelcome to 'Space Tube Ship' game.\n\n Your current states are:\n";
const char *CHOICES_MAIN[] = { // Main menu choices
	"Play the next game round with the specified states",
	"Specify the next game's difficulty",
	"Quit Game"
};
const int  CHOICES_MAIN_COUNT = sizeof(CHOICES_MAIN) / sizeof(CHOICES_MAIN[0]); // Choices' count
const char *MSG_STATS = "\n Your total credit/money ammount (including winnings/losses): %d kr."
						"\n Number of remaining times that you may play: %d"
						"\n Next round's difficulty: %s\n";
const char *MSG_MONEY_LOW = "\nOperation failed.. Your total credit/money is not enough.";
const char *MSG_ENTER_ROUND_DIFFICULTY = "\nEnter 'y' to choose 'HARD' difficulty for the next round,"
										" or just 'Enter' for 'EASY': ";
const char *MSG_WAIT_ENTER = "\n\nPress 'Enter' to continue...";
const char *MSG_ERR_ARG = "Argument cannot be NULL.";
const union NamerUnion {
	NamerUnion() : GAME_NAME{"Ship"} {};
	const int GAME_ID;
	const char GAME_NAME[5]; // Must be four letters (five with the null char)
} GAME_NAME_ID_CONV; // Used to convert the game's name "SHIP" to an integer
#pragma endregion

/**************************************************************************************/
// Conversion constructor.
// -------------------------------------------
// Parameters:
//		- theCredit:	the initial credit/money to start playing with.
/**************************************************************************************/
SpaceTubePlayer::SpaceTubePlayer(int theCredit) : _meMon(theCredit), _meMaxPlayCnt(0), _meRemPlayCnt(0),
				_meIsNextHard(false), _meGame(nullptr), _meCon(new Console()), _meMenu(new Menu(_meCon)) {
}

// Destrcutor
SpaceTubePlayer::~SpaceTubePlayer() {
	delete this->_meMenu;
	this->_meMenu = nullptr;
	delete this->_meCon;
	this->_meCon = nullptr;
}

/**************************************************************************************/
// Assign the 'IGame' object to pair with this object.
// -------------------------------------------
// Parameters:
//		- game:	the 'IGame' object to be paired with this object.
// -------------------------------------------
// Returns:	'true' if the 'game' object can be paired with this object; or else 'false'
/**************************************************************************************/
bool SpaceTubePlayer::setGame(IGame *game) {

	if (game->getID() != GAME_NAME_ID_CONV.GAME_ID) { // Check if IDs match
		return false;
	} else if (this->_meGame && this->_meGame != game) { // Check if same assigned before
		delete this->_meGame;
	}

	this->_meGame = game;
	return true;
}

/**************************************************************************************/
// Play the game for the specified number of rounds.
// -------------------------------------------
// Parameters:
//		- numberOfTimes:	the number of times to play the game.
// -------------------------------------------
// Returns:	'true' if playing the game went well; otherwise 'false'.
/**************************************************************************************/
bool SpaceTubePlayer::play(int numberOfTimes) {

	if (this->_meMon < 0) // No minus credit (if it's zero it's OK but cannot play a bit later)
		return false;

	this->_meMaxPlayCnt = this->_meRemPlayCnt = numberOfTimes;

	int tmpChoice;
	char tmpStr[SMALL_BUF_LEN]{'\0'};
	char tmpState[BIG_BUF_LEN]{'\0'};

	try { // In case something wrong happens (can I use the Windows '__try' in this lab-work??)

		do {
			this->_meCon->clearPage();
			this->fillStates(tmpState, BIG_BUF_LEN); // Fill the user's current state
			this->_meCon->displayMessage(MSG_MAIN_TITLE);								//
			this->_meMenu->assignChoices(tmpState, CHOICES_MAIN, CHOICES_MAIN_COUNT);	// Display the menu and wait for input
			tmpChoice = this->_meMenu->select(false);									//
			this->_meCon->clearPage();

			switch (tmpChoice) {
				case 1: // Play the game

					if (this->_meMon < MIN_GAME_COST) { // In case not enough money to play a single game
						this->_meCon->displayMessage(MSG_MONEY_LOW);
						this->_meCon->readEnter(MSG_WAIT_ENTER);
					} else {
#ifdef _WIN32
						sprintf_s(tmpStr, SMALL_BUF_LEN, "%d %d", this->_meIsNextHard ? 1 : 0, this->_meRemPlayCnt);	//
#else																													// Format the 'bet' argument
						sprintf(tmpStr, "%d %d", this->_meIsNextHard ? 1 : 0, this->_meRemPlayCnt);						//
#endif // _WIN32
						this->_meMon = this->_meGame->play(tmpStr, this->_meMon); // Start the game
						this->_meRemPlayCnt -= atoi(tmpStr); // Deduct the outputted number of times actually played
					}
					break;

				case 2: // Enter next round's difficulty
#ifdef _WIN32
					this->_meIsNextHard = !_stricmp(this->_meCon->readString(MSG_ENTER_ROUND_DIFFICULTY, true), "y");	//
#else																													// Set difficulty
					this->_meIsNextHard = !strcmp(this->_meCon->readString(MSG_ENTER_ROUND_DIFFICULTY, true), "y");		//
#endif // _WIN32
			}
		} while (tmpChoice < CHOICES_MAIN_COUNT); // Loop untill quit specified

	} catch (...) {
		return false;
	}

	return true;
}

/**************************************************************************************/
// A private helper method to fills the game states in the specifies 'outStats' buffer.
// -------------------------------------------
// Parameters:
//		- outStats:		the c-string buffer to fill the states with.
//		- outBufSize	the 'outStats' c-string buffer size.
/**************************************************************************************/
void SpaceTubePlayer::fillStates(char *outStats, int outBufSize) {
	
	if (!outStats)
		throw std::runtime_error(MSG_ERR_ARG);

#ifdef _WIN32
	sprintf_s(outStats, outBufSize, MSG_STATS, this->_meMon, this->_meRemPlayCnt, this->_meIsNextHard ? "HARD" : "EASY");
#else
	sprintf(outStats, MSG_STATS, this->_meMon, this->_meRemPlayCnt, this->_meIsNextHard ? "HARD" : "EASY");
#endif // _WIN32
}
