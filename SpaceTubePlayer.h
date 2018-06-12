/**************************************************************************************/
// File:	SpaceTubePlayer.h
// Summary:	A class used to create objects to handle the playing of a 'SpaceTubeGame'
// object and presenting the states to the player (required by the assignment).
// -------------------------------------------
// Author:	Janty Azmat
// -------------------------------------------
// Log:		
// -------------------------------------------
// Notes:	Most of the comments are in the accompanying '.cpp' file.
/**************************************************************************************/

#ifndef SPACETUBEPLAYER_TOKEN
#define SPACETUBEPLAYER_TOKEN

#include "IPlayer.h"

class Console;	//
class Menu;		// Forward decl.

class SpaceTubePlayer : public IPlayer {
public:
	SpaceTubePlayer(int theCredit);
	virtual ~SpaceTubePlayer();

	virtual bool setGame(IGame* game) override; // Gives the player a game object to use
	virtual bool play(int numberOfTimes) override; // Tells player to play the game numberOfTimes times
	virtual inline int getMoney() const override; // Returns the amount of player's money
	virtual inline int getBetCount() const override; // Returns the total number of bets placed during object life time

private:
	int _meMon; // The total money player has
	int _meMaxPlayCnt; // Max number of times player can play
	int _meRemPlayCnt; // Remaining number of times player can play
	bool _meIsNextHard; // If next game's level is hard (earns more money)
	IGame *_meGame;
	Console *_meCon;
	Menu *_meMenu;

	void fillStates(char *outStats, int outBufSize); // Fills the game states in the specifies 'outStats' buffer
};

inline int SpaceTubePlayer::getMoney() const {
	return this->_meMon;
}

inline int SpaceTubePlayer::getBetCount() const {
	return this->_meMaxPlayCnt - this->_meRemPlayCnt;
}
#endif // !SPACETUBEPLAYER_TOKEN