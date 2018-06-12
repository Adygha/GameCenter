/**************************************************************************************/
// File:	SpaceTubeGame.cpp
// Summary:	A class that create the 'Space Tube Ship' game object. The game is set in
//			a space-tube that has three tracks. The player controls a ship that moves
//			on these track while avoiding the meteors on the way. This game is based
//			on the Java version of same game I made for the 'Software Testing' course.
//			I thought making it in 'C++' language is going to be kind of easy, but I
//			was wrong (very very wrong) and it took a while, and a lot of fixings, to
//			work properly. I hope You'll enjoy it.. So.. Enjoy.. :)
// -------------------------------------------
// Author:	Janty Azmat
// -------------------------------------------
// Log:		
// -------------------------------------------
// Notes:	- The game class comprises four additional classes that are nested inside
//			the 'SpaceTubeGame' class. I thought it's better to do that to give it more
//			encapsulation (plus avoiding the additional cluttering of extra 8 files
//			that might confuse while grading; so, I kept them hidden in the private
//			section). I could've just as easily make them in seperate files anyway if
//			that is better.
/**************************************************************************************/

#include "SpaceTubeGame.h"
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#endif // _WIN32
#include <iostream>
#include <string.h>

#pragma region CONSTs
const char *MSG_WELCOME = "\n Welcome to Spece-Tube game.\n\n Press '%c' to start a game,"
						" '%c' to stop running game, left/right arrows to move"
						" sideways, or '%c' to quit.";
const char *MSG_ROUND_STATS = "\n This game-round states are:\n"
							"\n Every game costs: %d kr."
							"\n Remaining game-round credit/money: %d kr."
							"\n Maximum games in this round: %d"
							"\n Actual number of games played in this round: %d"
							"\n Game-round difficulty is '%s' where you earn %d kr. for every step";
const char *MSG_RESULT = "\n\tGame Over. Your winnings are: ";
const char *MSG_MAX_PLAY = "You cannot play anymore. Maximum play count reached for this round.";
const char *MSG_NEED_MONEY = "You cannot play anymore. Not enough money for this round.";
const char *MSG_CURRENCY = " kr.";
const char *MSG_ERR_PARAM = "Error specifying the bet parameter.";
const char CHR_QUIT = 'q'; // A char to quit when pressed
const char CHR_NEW = 's'; // A char for new game when pressed
const char CHR_STOP = 'w'; // A char for stop game when pressed
//const unsigned TUBE_WIDTH = 3; // The width of the space tube. Better not change this
const unsigned TUBE_HEIGHT = 10; // The height of the space tube.
const unsigned REFRESH_SPD_EASY = 400; // Refresh speed for easy game
const unsigned REFRESH_SPD_HARD = 200; // Refresh speed for hard game
const int SING_GAME_COST = 50; // The cost of a single game
const int GEN_BUF_LEN = 128;		//
const int GEN_BIG_BUF_LEN = 512;	// Lengths of general purpose buffers
#pragma endregion

/**************************************************************************************/
// The arrow keys' raw number with the modifier that is triggered before them
/**************************************************************************************/
enum ArrowKeys { NUM_PAD = 0, ARR_PAD = 224, KEY_LEFT = 75, KEY_RIGHT = 77 };

#ifndef _WIN32

/**************************************************************************************/
// Used to compensate for '_getch()' in windows to get the input char.
// -------------------------------------------
// Returns: the input char.
/**************************************************************************************/
int _getch() {
	int outInt;
	struct termios oldattr, newattr;
	tcgetattr( 0, &oldattr );
	newattr = oldattr;
	newattr.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( 0, TCSANOW, &newattr );
	outInt = getchar();
	tcsetattr( 0, TCSANOW, &oldattr );
	return outInt;
}
#endif // _WIN32

#pragma region SPACE_TUBE_GAME

/**************************************************************************************/
// Default constructor.
/**************************************************************************************/
SpaceTubeGame::SpaceTubeGame() : _meIsHard(false), _meMaxPlayCnt(1), _mePlayCnt(0),
									_meRoundMon(0), _meLogic(nullptr), _meCon(nullptr) {
}

/**************************************************************************************/
// Destructor.
/**************************************************************************************/
SpaceTubeGame::~SpaceTubeGame() {
	this->deAllocate();
}

/**************************************************************************************/
// Plays a single game of 'Space Tube Ship'.
// -------------------------------------------
// Parameters:
//		- bet:		for th 'Space Tube Ship' game, the first 'char' of this parameter will
//					if the comming game round will be '1' for 'HARD' or '0' for 'EASY'. the
//					rest of the parameter is an integer that is the maximum times to play
//					in this round.
//		- amount	the amount of money that can be played with in this round.
// -------------------------------------------
// Returns: the winnings/remaining-money.
/**************************************************************************************/
int SpaceTubeGame::play(char *bet, int amount) {

	if (amount < 0 || bet == nullptr || strlen(bet) < 3) // If error in parameters
		throw std::runtime_error(MSG_ERR_PARAM);

	int outWin;

#ifdef _WIN32
	char *next_token, *tmpStr = strtok_s(bet, " ", &next_token);
#else
	char *tmpStr = strtok(bet, " ");
#endif // _WIN32
	this->_meIsHard = atoi(tmpStr); // Get if game is hard difficulty
#ifdef _WIN32
	tmpStr = strtok_s(nullptr, " ", &next_token);
#else
	tmpStr = strtok(nullptr, " ");
#endif // _WIN32
	this->_meMaxPlayCnt = atoi(tmpStr); // Get the maximum times to play in this round
	this->_meRoundMon = amount; // Save the money

	this->_meLogic = new SpaceTubeGameLogic(*this, this->_meIsHard);	//
	this->_meCon = new ConsoleTubeGame(*this);							// Start fresh

	this->startGameInterface();

	if (this->_meCon->getThread() && this->_meCon->getThread()->joinable())		//
		this->_meCon->getThread()->join();										// Wait for console thread

	if (this->_meLogic->getThread() && this->_meLogic->getThread()->joinable())	//
		this->_meLogic->getThread()->join();									// Wait for last logic thread

	this->deAllocate();

#ifdef _WIN32
	sprintf_s(bet, GEN_BUF_LEN, "%d", this->_mePlayCnt);	//
#else														// Output back the actual play count
	sprintf(bet, "%d", this->_mePlayCnt);					//
#endif // _WIN32
	outWin = this->_meRoundMon;
	this->_meRoundMon = 0;		//
	this->_mePlayCnt = 0;		// Reset counters
	this->_meMaxPlayCnt = 1;	//
	return outWin; // Return the winnings/remaining-money
}

/**************************************************************************************/
// Informs that the player requested a new game.
/**************************************************************************************/
void SpaceTubeGame::newGameRequested() {

	if (this->_mePlayCnt == this->_meMaxPlayCnt) { // Max allowed play reached
		this->_meCon->displayMainPage(this->fillStates(), MSG_MAX_PLAY);
	} else if (this->_meRoundMon < SING_GAME_COST) { // Not enough money
		this->_meCon->displayMainPage(this->fillStates(), MSG_NEED_MONEY);
	} else if (this->_meLogic->startGame()) { // All OK, then start playing if possible
		this->_meCon->startMusic();
	}
}

/**************************************************************************************/
// Informs that a game over condition has happened, and we need to end the game.
// -------------------------------------------
// Parameters:
//		- theScore:	the score of the to-be-ended gamed.
/**************************************************************************************/
void SpaceTubeGame::gameOver(unsigned theScore) {
	this->_meRoundMon -= SING_GAME_COST; // Remove the cost of the played game
	this->_meRoundMon += theScore; // Add the winnings
	this->_mePlayCnt++; // If there is a game in progress, increment play count (and end game a bit later)
	this->_meCon->stopMusic();
	this->_meCon->displayResults(theScore); // Display results for single game
	this->_meCon->displayMainPage(this->fillStates()); // Go to main page
}

/**************************************************************************************/
// A helper private method to deallocate logic and console objects (this method was
// made to avoid code duplication).
/**************************************************************************************/
void SpaceTubeGame::deAllocate() {
	delete this->_meLogic;
	this->_meLogic = nullptr;
	delete this->_meCon;
	this->_meCon = nullptr;
}

/**************************************************************************************/
// A helper private method to get a string formatted with game-round states.
// -------------------------------------------
// Returns: a c-string containing the states.
/**************************************************************************************/
const char *SpaceTubeGame::fillStates() {
	static char tmpStr[GEN_BIG_BUF_LEN]{'\0'};

#ifdef _WIN32
	sprintf_s(tmpStr, GEN_BIG_BUF_LEN, MSG_ROUND_STATS, SING_GAME_COST, this->_meRoundMon, // Format the round states
			  this->_meMaxPlayCnt, this->_mePlayCnt, this->_meIsHard ? "HARD" : "EASY", this->_meIsHard ? 2 : 1);
#else
	sprintf(tmpStr, MSG_ROUND_STATS, SING_GAME_COST, this->_meRoundMon, // Format the round states
			  this->_meMaxPlayCnt, this->_mePlayCnt, this->_meIsHard ? "HARD" : "EASY", this->_meIsHard ? 2 : 1);
#endif // _WIN32

	return tmpStr;
}
#pragma endregion

#pragma region SPACE_TUBE_ROW

/**************************************************************************************/
// Conversion constructor.
// -------------------------------------------
// Parameters:
//		- obstaclePosition:	meteor/obsticle position for newly created 'SpaceTubeRow'.
/**************************************************************************************/
SpaceTubeGame::SpaceTubeRow::SpaceTubeRow(Position obstaclePosition) : _meShipPos(Position::NONE), _mePlaces{} {

	switch (obstaclePosition) {
	case Position::LEFT:
		this->_mePlaces[0] = true;
		break;
	case Position::CENTER:
		this->_mePlaces[1] = true;
		break;
	case Position::RIGHT:
		this->_mePlaces[2] = true;
	}
}

/**************************************************************************************/
// Put the ship in specified position.
// -------------------------------------------
// Parameters:
//		- theShip:	the position of the ship.
// -------------------------------------------
// Returns:	'true' if the placement of the ship went OK, or 'false' if there was an
//			impact/crash and the game is over.
/**************************************************************************************/
bool SpaceTubeGame::SpaceTubeRow::putSpaceShip(Position theShip) {

	switch (theShip) { // Switch to check if the position is already occupied by a meteor (game over)
		case Position::LEFT:
			if (this->_mePlaces[0])
				return false;
			break;
		case Position::CENTER:
			if (this->_mePlaces[1])
				return false;
			break;
		case Position::RIGHT:
			if (this->_mePlaces[2])
				return false;
	}
	this->_meShipPos = theShip;
	return true;
}

/**************************************************************************************/
// Removes the ship from this tube-row.
// -------------------------------------------
// Returns:	the last position of the ship before removing.
/**************************************************************************************/
SpaceTubeGame::Position SpaceTubeGame::SpaceTubeRow::removeSpaceShip() {
	Position outPos = this->_meShipPos;
	this->_meShipPos = Position::NONE;
	return outPos;
}
#pragma endregion

#pragma region CONSOLE_TUBE_GAME

/**************************************************************************************/
// Conversion constructor.
// -------------------------------------------
// Parameters:
//		- theGameObs:	the 'SpaceTubeGame' object that acts as an observer.
/**************************************************************************************/
SpaceTubeGame::ConsoleTubeGame::ConsoleTubeGame(SpaceTubeGame &theGameObs) :
	_meIsPlayMusic(false), _meMutex(), _meGame(theGameObs), _meThrd(nullptr), _meMusicThrd(nullptr) {
}

/**************************************************************************************/
// Destructor.
/**************************************************************************************/
SpaceTubeGame::ConsoleTubeGame::~ConsoleTubeGame() {
	delete this->_meThrd;
	this->_meThrd = nullptr;
}

/**************************************************************************************/
// A private method that starts taking input from player.
/**************************************************************************************/
void SpaceTubeGame::ConsoleTubeGame::getInput() {
	int tmpIn = 0;

	while (tmpIn != CHR_QUIT) { // Loop until quit key is pressed
		tmpIn = _getch();

		switch (tmpIn) {
			case ArrowKeys::NUM_PAD: // Num-Pad Home-..-End-Arrows key modifier
			case ArrowKeys::ARR_PAD: // Normal Home-..-End-Arrows key modifier
				tmpIn = _getch(); // Since a modifier was triggered then get the actual key now

				if (tmpIn == ArrowKeys::KEY_LEFT) { // Left arrow key
					this->_meGame.moveLeftRequested();
				} else if (tmpIn == ArrowKeys::KEY_RIGHT) { // Right arrow key
					this->_meGame.moveRightRequested();
				}
				break;
			case 67: // TODO: For Linux; Do better
				this->_meGame.moveRightRequested();
				break;
			case 68: // TODO: For Linux; Do better
				this->_meGame.moveLeftRequested();
				break;
			case CHR_NEW: // New game key
				this->_meGame.newGameRequested();
				break;
			case CHR_STOP: // Stop game key
				this->_meGame.endGameRequested();
		}
	}
	//this->_meGame.quitGameRequested();
	this->_meGame.endGameRequested();
}

/**************************************************************************************/
// Displays a game step using the tube rows.
// -------------------------------------------
// Parameters:
//		- tubeRows:	the tube rows to display.
/**************************************************************************************/
void SpaceTubeGame::ConsoleTubeGame::displayGame(SpaceTubeRow **tubeRows) {
	std::unique_lock<std::mutex> lock(this->_meMutex); // Unlocked on 'lock' destruction on function end
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif // _WIN32

	for (int rowIndex = 0; rowIndex < TUBE_HEIGHT; rowIndex++) { // Loop and display each row
		std::cout << "\n|";

		for (int colIndex = 0; colIndex < TUBE_WIDTH; colIndex++) { // Loop for every row place (3 places/tracks)

			if (tubeRows[rowIndex]->getRowPlaces()[colIndex]) {
				std::cout << "[xxx]"; // Display a meteor
			} else if (tubeRows[rowIndex]->getSpaceShip() == colIndex) {
				std::cout << "|-^-|"; // Display the ship
			} else {
				std::cout << "     "; // Display empty space
			}
			std::cout << '|';
		}
	}
	std::cout << std::endl;

	//static char tmpBuf[DISP_BUF_LEN];
	//tmpBuf[0] = '\0';
	//
	//for (int rowIndex = 0; rowIndex < TUBE_HEIGHT; rowIndex++) {
	//	strncat_s(tmpBuf, DISP_BUF_LEN, "\n|", _TRUNCATE);
	//	for (int colIndex = 0; colIndex < TUBE_WIDTH; colIndex++) {
	//		if (tubeRows[rowIndex]->getRowPlaces()[colIndex]) {
	//			strncat_s(tmpBuf, DISP_BUF_LEN, "[xxx]", _TRUNCATE);
	//		} else if (tubeRows[rowIndex]->getSpaceShip() == colIndex) {
	//			strncat_s(tmpBuf, DISP_BUF_LEN, "|-^-|", _TRUNCATE);
	//		} else {
	//			strncat_s(tmpBuf, DISP_BUF_LEN, "     ", _TRUNCATE);
	//		}
	//		strncat_s(tmpBuf, DISP_BUF_LEN, "|", _TRUNCATE);
	//	}
	//}
	//strncat_s(tmpBuf, DISP_BUF_LEN, "\n", _TRUNCATE);
	//std::cout << tmpBuf;
}

/**************************************************************************************/
// Displays the main page of the game with the states and all that.
// -------------------------------------------
// Parameters:
//		- roundStates:		a c-string that represents the states of the game.
//		- additionalText	any additional text to display (like player's mistakes).
/**************************************************************************************/
void SpaceTubeGame::ConsoleTubeGame::displayMainPage(const char *roundStates, const char *additionalText) {
	// unsigned tmpLen = strlen(MSG_WELCOME) + GEN_BUF_LEN; // With extra safety length
	size_t tmpLen = strlen(MSG_WELCOME) + GEN_BUF_LEN; // With extra safety length
	char *tmpStr = new char[tmpLen]{'\0'};

#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif // _WIN32

#ifdef _WIN32
	sprintf_s(tmpStr, tmpLen, MSG_WELCOME, CHR_NEW, CHR_STOP, CHR_QUIT);	//
#else																		// Format the welcome message
	sprintf(tmpStr, MSG_WELCOME, CHR_NEW, CHR_STOP, CHR_QUIT);				//
#endif // _WIN32

	std::cout << tmpStr << std::endl; // Display the welcome message

	if (roundStates) // Display game-round states if requested
		std::cout << "\n\n" << roundStates << std::endl;

	if (additionalText) // Display additional text if requested
		std::cout << "\n\n" << additionalText << std::endl;

	delete[] tmpStr;

	if (!this->_meThrd) // Trigger a thread if there wasn't one (then it's an initial page)
		this->_meThrd = new std::thread(&ConsoleTubeGame::getInput, this);
}

/**************************************************************************************/
// Displays the results of last ended game round (for 3 seconds).
// -------------------------------------------
// Parameters:
//		- theScore:	the results to display.
/**************************************************************************************/
void SpaceTubeGame::ConsoleTubeGame::displayResults(unsigned theScore) {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif // _WIN32
	std::cout << MSG_RESULT << theScore << MSG_CURRENCY << std::endl; // Display results with currency
#ifdef _WIN32
#pragma warning(disable:4996) // Temporarily suppress warning about '_beep' (I don't want to use 'Beep' and include 'Windows.h')
	_beep(500, 1000);
	_beep(550, 300);
	_beep(600, 300);
	_beep(0, 300);
	_beep(500, 500);
	_beep(700, 2000);
#pragma warning(default:4996)
#else
	std::this_thread::sleep_for(std::chrono::seconds(3)); // Sleep to let player see
#endif // _WIN32
}

/**************************************************************************************/
// Starts playing music (when game starts).
/**************************************************************************************/
void SpaceTubeGame::ConsoleTubeGame::startMusic() {
#ifdef _WIN32
	if (!this->_meIsPlayMusic && !this->_meMusicThrd) // Trigger a thread if there wasn't one
		this->_meMusicThrd = new std::thread(&ConsoleTubeGame::threadedMusic, this);
#endif // _WIN32
}

/**************************************************************************************/
// Stops playing music (when game stops).
/**************************************************************************************/
void SpaceTubeGame::ConsoleTubeGame::stopMusic() {

	if (this->_meIsPlayMusic) {
		this->_meIsPlayMusic = false;

		if (this->_meMusicThrd->joinable()) {	// Join or detach to be able to destroy it
			this->_meMusicThrd->join();			// but it joins all the time
		} else {
			this->_meMusicThrd->detach();
		}
		delete this->_meMusicThrd;
		this->_meMusicThrd = nullptr;
	}
}

/**************************************************************************************/
// A private method used and run by the music-thread.
/**************************************************************************************/
void SpaceTubeGame::ConsoleTubeGame::threadedMusic() {
	// Actual tune numbers from 'https://www.c-plusplus.net/forum/308112-full' (changed a bit)
	static const int takt = 1700;
	static const int tmpNoteTime[]{658, takt / 4, 493, takt / 8, 522, takt / 8, 586, takt / 4,
		522, takt / 8, 493, takt / 8, 440, takt / 4, 440, takt / 8, 522, takt / 8, 658, takt / 4,
		586, takt / 8, 522, takt / 8, 493, takt / 4, 493, takt / 8, 522, takt / 8, 586, takt / 4,
		658, takt / 4, 522, takt / 4, 440, takt / 4, 440, takt / 4, 0, takt / 2, 586, takt / 4,
		698, takt / 8, 880, takt / 4, 782, takt / 8, 698, takt / 8, 658, takt / 3, 522, takt / 8,
		658, takt / 4, 586, takt / 8, 522, takt / 8, 493, takt / 4, 493, takt / 8, 522, takt / 8,
		586, takt / 4, 658, takt / 4, 522, takt / 4, 440, takt / 4, 440, takt / 4, 0, takt / 2};
	static const int tmpDubIdx = sizeof(tmpNoteTime) / sizeof(tmpNoteTime[0]) - 2; // Less with two

	this->_meIsPlayMusic = true;

#pragma warning(disable:4996) // Temporarily suppress warning about '_beep' (I don't want to use 'Beep' and include 'Windows.h')
	for (int idx = 0; this->_meIsPlayMusic; idx += 2) {
		_beep(tmpNoteTime[idx], tmpNoteTime[idx + 1]);

		if (idx == tmpDubIdx)
			idx = -2;
	}
#pragma warning(default:4996)
}
#pragma endregion

#pragma region SPACE_TUBE

/**************************************************************************************/
// Constructor that takes position and difficulty.
// -------------------------------------------
// Parameters:
//		- theShip:	the ship represented by its initial position.
//		- isHard:	specifies if the difficulty of the game is 'HARD'.
/**************************************************************************************/
SpaceTubeGame::SpaceTube::SpaceTube(Position theShip, bool isHard) :
				_meScore(1), _meIsHard(isHard), _meRows(new SpaceTubeRow *[TUBE_HEIGHT]) {

	for (int index = 0; index < TUBE_HEIGHT; index++) // Loop and build the space-tube using tube-rows
		this->_meRows[index] = new SpaceTubeRow(Position::NONE);

	this->_meRows[TUBE_HEIGHT - 1]->putSpaceShip(theShip); // Put the ship on the last row
}

/**************************************************************************************/
// Destructor.
/**************************************************************************************/
SpaceTubeGame::SpaceTube::~SpaceTube() {
	for (int index = 0; index < TUBE_HEIGHT; index++) {
		delete this->_meRows[index];
		this->_meRows[index] = nullptr;
	}
	delete[] this->_meRows;
}

/**************************************************************************************/
// Represents dropping a meteor at the beginning of the tube (or just an empty row)
// -------------------------------------------
// Returns:	'true' if the placement of the ship went OK, or 'false' if there was an
//			impact/crash and the game is over.
/**************************************************************************************/
bool SpaceTubeGame::SpaceTube::dropRow() {
	std::this_thread::sleep_for( // Sleep between every drop for a bit while (depending on difficulty)
		std::chrono::milliseconds(this->_meIsHard ? REFRESH_SPD_HARD : REFRESH_SPD_EASY));
	Position tmpShip = this->_meRows[TUBE_HEIGHT - 1]->removeSpaceShip(); // Remove the space ship from to-be-removed bottom row

	if (tmpShip == Position::NONE) // If removed ship position is 'NONE' then the ship is destroyed
		return false;

	delete this->_meRows[TUBE_HEIGHT - 1];		//
	this->_meRows[TUBE_HEIGHT - 1] = nullptr;	// Remove buttom row

	for (int index = TUBE_HEIGHT - 2; index > -1; index--) // Shift all rows one step down
		this->_meRows[index + 1] = this->_meRows[index];

	if (this->_meScore % TUBE_WIDTH == 0) { // This condition is to avoid player stuck in game without a place to escape
		this->_meRows[0] = new SpaceTubeRow(Position::NONE); // Give a place to escape
	} else {
		this->_meRows[0] = new SpaceTubeRow((Position)(rand() % (TUBE_WIDTH)));	// Drop a new row at the top with a random placed meteor
	}
	this->_meScore += this->_meIsHard ? 2 : 1; // Add score based on difficulty

	return this->_meRows[TUBE_HEIGHT - 1]->putSpaceShip(tmpShip);
}

/**************************************************************************************/
// Moves the ship that is in the tube to the left.
// -------------------------------------------
// Returns:	'true' if the new posotion of the ship is OK, or 'false' if there was an
//			impact/crash and the game is over.
/**************************************************************************************/
bool SpaceTubeGame::SpaceTube::moveShipLeft() {
	Position tmpShip = this->_meRows[TUBE_HEIGHT - 1]->removeSpaceShip();

	switch (tmpShip) {
		case Position::RIGHT:
			tmpShip = Position::CENTER;
			break;
		case Position::CENTER:
			tmpShip = Position::LEFT;
	}

	return this->_meRows[TUBE_HEIGHT - 1]->putSpaceShip(tmpShip);
}

/**************************************************************************************/
// Moves the ship that is in the tube to the right.
// -------------------------------------------
// Returns:	'true' if the new posotion of the ship is OK, or 'false' if there was an
//			impact/crash and the game is over.
/**************************************************************************************/
bool SpaceTubeGame::SpaceTube::moveShipRight() {
	Position tmpShip = this->_meRows[TUBE_HEIGHT - 1]->removeSpaceShip();

	switch (tmpShip) {
		case Position::LEFT:
			tmpShip = Position::CENTER;
			break;
		case Position::CENTER:
			tmpShip = Position::RIGHT;
	}

	return this->_meRows[TUBE_HEIGHT - 1]->putSpaceShip(tmpShip);
}
#pragma endregion

#pragma region SPACE_TUBE_GAME_LOGIC

/**************************************************************************************/
// Constructor that takes a 'SpaceTubeGame' object and difficulty.
// -------------------------------------------
// Parameters:
//		- theGameObs:	the 'SpaceTubeGame' object that acts as an observer.
//		- isHard:		specifies if the difficulty of the game is 'HARD'.
/**************************************************************************************/
SpaceTubeGame::SpaceTubeGameLogic::SpaceTubeGameLogic(SpaceTubeGame &theGameObs, bool isHard) :
								_meKeepDrop(false), _meThrd(nullptr), _meShip(Position::CENTER),
								_meTube(new SpaceTube(_meShip, isHard)), _meGame(theGameObs) {
}

/**************************************************************************************/
// Default constructor.
/**************************************************************************************/
SpaceTubeGame::SpaceTubeGameLogic::~SpaceTubeGameLogic() {
	delete this->_meTube;
	this->_meTube = nullptr;
	delete this->_meThrd;
	this->_meThrd = nullptr;
}

/**************************************************************************************/
// Starts a 'Space Tube Ship' game.
// -------------------------------------------
// Returns: 'true' if the game properly started.
/**************************************************************************************/
bool SpaceTubeGame::SpaceTubeGameLogic::startGame() {
	if (this->_meTube->getScore() < 2) { // Only start if game is not running
		if (this->_meThrd) { // If previous thread exists, then detach and delete it
			if (this->_meThrd->joinable()) {// Join or detach to be able to destroy it
				this->_meThrd->join();		// but it joins all the time
			} else {
				this->_meThrd->detach();
			}
			delete this->_meThrd;
		}
		this->_meThrd = new std::thread(&SpaceTubeGameLogic::runWithThread, this); // New thread
		return true;
	} else {
		return false;
	}
}

/**************************************************************************************/
// Moves the ship one step left.
/**************************************************************************************/
void SpaceTubeGame::SpaceTubeGameLogic::moveShipLeft() {
	if (this->_meKeepDrop && this->_meTube->getScore() > 1) { // Only move if game in progress
		if (this->_meTube->moveShipLeft()) {
			this->_meGame.spaceTubeUpdated();
		} else {
			this->_meKeepDrop = false;
		}
	}
}

/**************************************************************************************/
// Moves the ship one step right
/**************************************************************************************/
void SpaceTubeGame::SpaceTubeGameLogic::moveShipRight() {
	if (this->_meKeepDrop && this->_meTube->getScore() > 1) { // Only move if game in progress
		if (this->_meTube->moveShipRight()) {
			this->_meGame.spaceTubeUpdated();
		} else {
			this->_meKeepDrop = false;
		}
	}
}

/**************************************************************************************/
// Ends a running game
/**************************************************************************************/
void SpaceTubeGame::SpaceTubeGameLogic::endGame() {
	this->_meKeepDrop = false;
}

/**************************************************************************************/
// Return all the space-tube-rows (most likely, to be displayed).
// -------------------------------------------
// Returns:	an array of 'SpaceTubeRow' pointers that represent the rows of space-tube.
/**************************************************************************************/
SpaceTubeGame::SpaceTubeRow **SpaceTubeGame::SpaceTubeGameLogic::getSpaceTubeRows() {
	return this->_meTube->getTubeRows();
}

/**************************************************************************************/
// A private method used and run by the game-thread.
/**************************************************************************************/
void SpaceTubeGame::SpaceTubeGameLogic::runWithThread() {
	this->_meKeepDrop = true; // Set the flag
	this->_meGame.spaceTubeUpdated(); // Trigger an initial redraw
	srand((unsigned)time(NULL));

	while (this->_meKeepDrop) { // Loop while flag is true

		if (this->_meTube->dropRow()) { // If dropping a meteor when without an impact/crash
			this->_meGame.spaceTubeUpdated(); // trigger a redraw
		} else { // Else if there was an impact/crash
			this->_meKeepDrop = false;  // Set flag to false to trigger game over a bit later
		}
	}
	this->_meGame.gameOver(this->_meTube->getScore()); // Trigger game over and pass score
	bool tmpIsHard = this->_meTube->getIsHard();
	delete this->_meTube; // Delete old space-tube
	this->_meShip = Position::CENTER;
	this->_meTube = new SpaceTube(this->_meShip, tmpIsHard); // New space-tube
	this->_meTube->setScore(1); // Reset score
}
#pragma endregion
