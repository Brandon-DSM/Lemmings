#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"










// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


// Actor

class Actor : public GraphObject {
public:
	Actor(StudentWorld* sw, int imageID, Coord startCoord, int dir = right, double size = DEFAULT_SPRITE_SCALE);

	virtual void doSomething() = 0;

	Coord getCoord() const { return GraphObject::getCoord(); }

	virtual bool isActive() const { return true; }

	virtual bool isPassable() const { return true; }

	virtual bool isBurnable() const { return false; }

	virtual bool isFreezable() const { return false; }

	virtual bool isClimbable() const { return false; }

	virtual bool isBounceable() const { return false; }

	virtual bool isSavable() const { return false; }

	virtual bool isAttractor() const { return false; }

	virtual void bounce(int height) {}

	virtual void setDead() {}

	virtual void save() {}

	StudentWorld* getWorld() const;

private:
	StudentWorld* m_sw;
};


// Player

class Player : public Actor {
public:

	Player(StudentWorld* sw) : Actor(sw, IID_PLAYER, Coord(VIEW_WIDTH / 2, VIEW_HEIGHT / 2)) {};
	virtual void doSomething() override;
};

// FloorBlock

class FloorBlock : public Actor {
public:
	FloorBlock(StudentWorld* sw, Coord startCoord);

	virtual void doSomething() override;

	virtual bool isPassable() const override { return false; }
};


// Lemming Factory

class LemmingFactory : public Actor {
public:
	LemmingFactory(StudentWorld* sw, Coord pos)
		: Actor(sw, IID_LEMMING_FACTORY, pos),
		m_ticksSinceLastSpawn(-1), m_numSpawned(0) {
	}

	virtual void doSomething() override;

private:
	const int TICKS_TO_SPAWN_LEMMING = 100;
	const int MAX_LEMMINGS = 10;
	int m_ticksSinceLastSpawn;
	int m_numSpawned;
};


// Lemming

class Lemming : public Actor {
public:
	Lemming(StudentWorld* sw, Coord pos)
		: Actor(sw, IID_LEMMING, pos), m_state(WALKING), m_ticksInCurrentState(-1), m_maxTicksInCurrentState(TICKS_TO_WALK) {
	}

	virtual void doSomething() override;

	virtual bool isBurnable() const { return true; }

	virtual bool isFreezable() const override { return true; }

	virtual bool isBounceable() const { return true; }

	virtual bool isSavable() const override { return true; }

	virtual bool isActive() const override { return m_active; }
	
	virtual void bounce(int height) override;

	virtual void save() override;
	
	virtual void setDead() override;

private:
	void CheckToChangeState();

	// These constants represent the different states a lemming can be in
	const int WALKING = 0;
	const int FALLING = 1;
	const int CLIMBING = 2;
	const int BOUNCING = 3;

	const int MAX_FALL_DISTANCE = 5; // If a lemming falls more than this distance, it dies

	// These constants determine how many ticks a lemming spends in its current state before potentially changing state
	const int TICKS_TO_DEFAULT_ACT = 2;
	const int TICKS_TO_WALK = 4;

	bool m_active = true;
	int m_fallDistance = 0; // Used to determine if a lemming dies from falling
	int m_bounceHeight = 0; // Used to determine how high a lemming bounces when it bounces

	int m_state;
	int m_ticksInCurrentState;
	int m_maxTicksInCurrentState;
};


// Bonfire

class Bonfire : public Actor {
public:
	Bonfire(StudentWorld* sw, Coord startCoord) : Actor(sw, IID_BONFIRE, startCoord) {};
	virtual void doSomething() override;
};


// IceMonster

class IceMonster : public Actor {
public:
	IceMonster(StudentWorld* sw, Coord startCoord, int dir = right) : Actor(sw, IID_ICE_MONSTER, startCoord, dir) {};

	virtual void doSomething() override;
private:
	const int TICKS_TO_WALK = 10;
	int m_ticksInCurrentState = -1;
};


// Trampoline

class Trampoline : public Actor {
public:
	Trampoline(StudentWorld* sw, Coord startCoord) : Actor(sw, IID_TRAMPOLINE, startCoord) {};
	virtual void doSomething() override;
};


// Net	
class Net : public Actor {
public:
	Net(StudentWorld* sw, Coord startCoord) : Actor(sw, IID_NET, startCoord) {};
	virtual void doSomething() override {};
	virtual bool isClimbable() const override { return true; }
};


// One Way Door
class OneWayDoor : public Actor {
public:
	OneWayDoor(StudentWorld* sw, Coord startCoord, int dir) : Actor(sw, IID_ONE_WAY_DOOR, startCoord, dir) {};
	virtual void doSomething() override;
};

// Pheromone
class Pheromone : public Actor {
public:
	Pheromone(StudentWorld* sw, Coord startCoord) : Actor(sw, IID_PHEROMONE, startCoord) {};
	virtual void doSomething() override;

	virtual bool isAttractor() const override { return true; }
};

// Spring
class Spring : public Actor {
public:
	Spring(StudentWorld* sw, Coord startCoord) : Actor(sw, IID_SPRING, startCoord) {};
	virtual void doSomething() override;
};

// Exit
class Exit : public Actor {
public:
	Exit(StudentWorld* sw, Coord startCoord) : Actor(sw, IID_EXIT, startCoord) {};
	virtual void doSomething() override;
};

#endif // ACTOR_H_
