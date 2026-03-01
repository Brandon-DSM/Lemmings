#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <vector>

using namespace std;

class Actor;
class Player;






// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath);
	~StudentWorld();
	virtual int init();
  	virtual int move();
	virtual void cleanUp();
	void updateGameStatText();
	void addActor(Actor* a);

	bool AttemptToPlaceActorAt(Actor* a, Coord c);
	bool removeCharFromTools(char c);
	void decLemmingsLeft() { --m_lemmingsLeft; }

	bool isCoordPassible(Coord c) const;
	void BurnBurnablesAtCoord(Coord c);
	bool FreezeFreezablesAtCoord(Coord c);
	void BounceBounceablesAtCoord(Coord c, int height);
	void SaveSavableAtCoord(Coord c);
	bool isCoordClimbable(Coord c) const;
	void setDirectionAtCoord(Coord c, int dir);
	bool isCoordAttractor(Coord c);
	int getAttractorDirAtCoord(Coord c);

private:
	int loadLevel();
	int getCurrentStatus();
	const int POINTS_PER_SAVED = 100;
	const int NUM_LEMMINGS_TO_SAVE = 10;

	vector<Actor*> m_actors;
	Player* m_player = nullptr;
	int m_saved;
	int m_lemmingsLeft;
	string m_tools;
	int m_timeLeft;
};

#endif // STUDENTWORLD_H_
