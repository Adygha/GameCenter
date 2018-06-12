/**************************************************************************************/
// File:	SpaceTubeGame.h
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
// Notes:	- More comments are in the accompanying '.cpp' file.
//			- The game class comprises four additional classes that are nested inside
//			the 'SpaceTubeGame' class. I thought it's better to do that to give it more
//			encapsulation (plus avoiding the additional cluttering of extra 8 files
//			that might confuse while grading; so, I kept them hidden in the private
//			section). I could've just as easily make them in seperate files anyway if
//			that is better.
/**************************************************************************************/

#ifndef SPACETUBEGAME_TOKEN
#define SPACETUBEGAME_TOKEN

#include "IGame.h"	//
#include <thread>
#include <mutex>	// Couldn't avoid including these here

#pragma region CONSTs
// Couldn't move 'TUBE_WIDTH' to '.cpp' even though I used extern
const unsigned TUBE_WIDTH = 3; // The width of the space tube. Better not change this

const union NamerUnion {
	NamerUnion() : GAME_NAME{"Ship"} {};
	const int GAME_ID;
	const char GAME_NAME[5]; // Must be four letters (five with the null char)
} GAME_NAME_ID; // Used to convert the game's name "SHIP" to an integer
#pragma endregion

class SpaceTubeGame : public IGame {
public:
	SpaceTubeGame();
	~SpaceTubeGame();

	virtual int play(char* bet, int amount) override; // Play a single game
	virtual int getID() const override { return GAME_NAME_ID.GAME_ID; } // Returns the game's ID

	void startGameInterface() { this->_meCon->displayMainPage(this->fillStates()); } // Display the initial user interface
	void spaceTubeUpdated() { this->_meCon->displayGame(this->_meLogic->getSpaceTubeRows()); } // Informs that the tube has changed and needs to be displayed again
	void moveLeftRequested() { this->_meLogic->moveShipLeft(); } // Informs that the player requested to move left
	void moveRightRequested() { this->_meLogic->moveShipRight(); } // Informs that the player requested to move right
	void endGameRequested() { this->_meLogic->endGame(); }// Informs that the player requested end a game
	void newGameRequested(); // Informs that the player requested a new game
	//void quitGameRequested();
	void gameOver(unsigned theScore); // Informs that a game over condition has happened

private:
	class SpaceTubeGameLogic;	//
	class ConsoleTubeGame;		// Forward decl.

	bool _meIsHard; // To check if the game difficulty is 'HARD'
	int _meMaxPlayCnt; // Maximum round's play count
	int _mePlayCnt; // Actual round's play count
	int _meRoundMon; // Round's money
	SpaceTubeGameLogic *_meLogic;
	ConsoleTubeGame *_meCon;

	void deAllocate(); // A private method to deallocate logic and console (made to avoid code duplication)
	const char *fillStates(); // To get a string formatted with game-round states

	/**************************************************************************************/
	// An enum to specify the ship and obstacle position
	/**************************************************************************************/
	enum Position { LEFT, CENTER, RIGHT, NONE }; // 

#pragma region SPACE_TUBE_ROW

	/**************************************************************************************/
	// A class that represents a row in the space-tube-game. Every row has three horizontal
	// places that are either empty or filled with a meteor or a ship. If it contains both
	// a ship and a meteor, then the game is over (impact/crash).
	/**************************************************************************************/
	class SpaceTubeRow {
	public:
		SpaceTubeRow(Position obstaclePosition);
		//~SpaceTubeRow();

		bool putSpaceShip(Position shipPos); // Put the ship in specified position
		Position removeSpaceShip(); // Remove the ship and get its last position
		Position getSpaceShip() { return this->_meShipPos; } // Get the ship's position ('NONE' no ship)
		bool *getRowPlaces() { return this->_mePlaces; } // Get the condistion of the three places

	private:
		bool _mePlaces[TUBE_WIDTH]; // The condistion of the three places ('true' occupied by meteor)
		Position _meShipPos; // The ship's position ('NONE' no ship)
	};
#pragma endregion

#pragma region CONSOLE_TUBE_GAME

	/**************************************************************************************/
	// A class that represents the game's user-interface
	/**************************************************************************************/
	class ConsoleTubeGame {
	public:
		ConsoleTubeGame(SpaceTubeGame &theGameObs);
		~ConsoleTubeGame();

		void displayGame(SpaceTubeRow **tubeRows); // Displays a game step using the tube rows
		void displayMainPage(const char *roundStates = nullptr, const char *additionalText = nullptr); // Displays main page
		void displayResults(unsigned theScore); // Displays the results of last ended game round
		std::thread *getThread() { return this->_meThrd; } // Returns the thread that controls user input

		void startMusic(); // Starts playing music
		void stopMusic(); // Stops playing music

	private:
		bool _meIsPlayMusic; // A flag used to stop music when game ends
		std::mutex _meMutex; // Used to prevent more than one thread to enter a method
		std::thread *_meThrd; // The thread that controls user input
		std::thread *_meMusicThrd; // The thread that plays music
		SpaceTubeGame &_meGame; // The 'SpaceTubeGame' object that acts as an observer

		void getInput(); // Starts taking input from player
		void threadedMusic(); // A private method used by '_meMusicThrd' thread to play music
	};
#pragma endregion

#pragma region SPACE_TUBE

	/**************************************************************************************/
	// A class that represents a space-tube that has three tracks. A ship moves on and jumps
	// between those tracks while avoiding meteors.
	/**************************************************************************************/
	class SpaceTube {
	public:
		SpaceTube(Position theShip, bool isHard);
		~SpaceTube();

		bool dropRow(); // Represents dropping a meteor at the beginning of the tube
		bool moveShipLeft(); // Moves the ship that is in the tube to the left
		bool moveShipRight(); // Moves the ship that is in the tube to the right
		SpaceTubeRow **getTubeRows() { return this->_meRows; } // Returns all the tube-rows in the tube
		unsigned getScore() { return this->_meScore; } // Returns the current score of the running game
		void setScore(unsigned theScore) { this->_meScore = theScore; } // Sets the current score of the running game
		bool getIsHard() { return this->_meIsHard; } // Checks if the game difficulty is 'HARD'

	private:
		unsigned _meScore; // Runnng game score
		bool _meIsHard; // For game difficulty
		SpaceTubeRow **_meRows; // All the tube-rows
	};
#pragma endregion

#pragma region SPACE_TUBE_GAME_LOGIC

	/**************************************************************************************/
	// A class that represents a facade for the game logic.
	/**************************************************************************************/
	class SpaceTubeGameLogic {
	public:
		SpaceTubeGameLogic(SpaceTubeGame &theGameObs, bool isHard);
		~SpaceTubeGameLogic();

		bool startGame(); // Starts a game
		void moveShipLeft(); // Moves the ship one step left
		void moveShipRight(); // Moves the ship one step right
		void endGame(); // Ends a running game
		SpaceTubeRow **getSpaceTubeRows(); // Return all the space-tube-rows (to be displayed)
		std::thread *getThread() { return this->_meThrd; } // Returns the thread that controls the game

	private:
		bool _meKeepDrop; // To check whether to keep dropping meteors or end game -used by threads- (volatile?? some sites didn't recommend it)
		std::thread *_meThrd; // The thread that controls the game
		Position _meShip; // The position of the ship on the tracks (LEFT, CENTER, RIGHT)
		SpaceTube *_meTube; // The tube object
		SpaceTubeGame &_meGame; // The 'SpaceTubeGame' object that acts as an observer

		void runWithThread(); // The game-thread's method to run
	};
#pragma endregion
};
#endif // !SPACETUBEGAME_TOKEN