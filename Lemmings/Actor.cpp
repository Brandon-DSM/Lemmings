#include "Actor.h"
#include "StudentWorld.h"












// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor

Actor::Actor() : GraphObject(0, Coord(0, 0)) {}

Actor::Actor(int imageID, Coord startCoord, int dir = right, double size = DEFAULT_SPRITE_SCALE) : GraphObject(imageID, startCoord, dir, size) {}

void Actor::doSomething() {}


// FloorBlock

FloorBlock::FloorBlock(Coord startCoord) : Actor(IID_FLOOR, startCoord) {}

void FloorBlock::doSomething() {}


// IceMonster

IceMonster::IceMonster(Coord startCoord, int dir = right) : Actor(IID_ICE_MONSTER, startCoord, dir) {}

void IceMonster::doSomething() {
	// if(object in front of me is impassible or object below my feet and in front is passible) {
	//		turn around
	// } else {
	//		move forward
	// }

	// for now do nothing
}