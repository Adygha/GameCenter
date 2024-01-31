// IGame.h (provided by the assignment)

#ifndef IGAME_H
#define IGAME_H


class IGame {
public:
	virtual int play(char* bet, int amount) = 0; // Place amount money on position bet and return winnings
	virtual int getID() const = 0; // Returns a unique ID for the actual game type
	virtual ~IGame(){} // To properly delete using IGame pointer
};


#endif