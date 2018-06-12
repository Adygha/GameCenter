// IPlayer.h (provided by the assignment)

#ifndef IPLAYER_H
#define IPLAYER_H
#include "IGame.h"

class IPlayer {
public:
	virtual bool setGame(IGame* game) = 0;		// Gives the player a game object to use
	virtual bool play(int numberOfTimes) = 0;	// Tells player to play the game numberOfTimes times
	virtual int	 getMoney() const = 0;			// Returns the amount of player's money
	virtual int  getBetCount() const = 0;		// Returns the total number of bets placed during object life time
};

#endif