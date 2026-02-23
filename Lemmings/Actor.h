#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"










// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


// Actor

class Actor : public GraphObject {
public:
	Actor();
	
	Actor(int imageID, Coord startCoord, int dir = right, double size = DEFAULT_SPRITE_SCALE);

	virtual void doSomething();
};


// FloorBlock

class FloorBlock : public Actor {
public:
	FloorBlock(Coord startCoord);

	virtual void doSomething();
};


// IceMonster

class IceMonster : public Actor {
public:
	IceMonster(Coord startCoord, int dir = right);

	virtual void doSomething();
};

#endif // ACTOR_H_
