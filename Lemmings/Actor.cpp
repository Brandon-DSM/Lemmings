#include "Actor.h"
#include "StudentWorld.h"
#include "Level.h"
#include <cctype>

using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// Helper functions
int flipDirection(int d)
{
    while (d < 0)
        d += 360;

    return (d + 180) % 360;
}

// Actor
Actor::Actor(StudentWorld* sw, int imageID, Coord startCoord, int dir, double size)
    : GraphObject(imageID, startCoord, dir, size), m_sw(sw)
{
}

StudentWorld* Actor::getWorld() const
{
    return m_sw;
}


// Player
void Player::doSomething()
{
    int key;
    int moveDir = -1;

    if (!getWorld()->getKey(key))
        return;

    // Movement keys
    switch (key) {
    case KEY_PRESS_UP:
        moveDir = up;
        break;
    case KEY_PRESS_DOWN:
        moveDir = down;
        break;
    case KEY_PRESS_LEFT:
        moveDir = left;
        break;
    case KEY_PRESS_RIGHT:
        moveDir = right;
        break;
    }

    const Coord newPos = getTargetCoord(moveDir);
    if (newPos.x >= 1 && newPos.x < VIEW_WIDTH - 1 && newPos.y >= 1 && newPos.y < VIEW_HEIGHT - 1)
        moveTo(moveDir);

    // Tool placement: only proceed if the character exists in the tools string
    if (!getWorld()->removeCharFromTools(static_cast<char>(toupper(key))))
        return;

    Actor* a = nullptr;
    switch (key) {
    case 'T':
    case 't':
        a = new Trampoline(getWorld(), getCoord());
        break;
    case 'N':
    case 'n':
        a = new Net(getWorld(), getCoord());
        break;
    case 'P':
    case 'p':
        a = new Pheromone(getWorld(), getCoord());
        break;
    case 'S':
    case 's':
        a = new Spring(getWorld(), getCoord());
        break;
    case '<':
        a = new OneWayDoor(getWorld(), getCoord(), left);
        break;
    case '>':
        a = new OneWayDoor(getWorld(), getCoord(), right);
        break;
    }

    if (a != nullptr)
        getWorld()->AttemptToPlaceActorAt(a, getCoord());
}


// FloorBlock
FloorBlock::FloorBlock(StudentWorld* sw, Coord startCoord)
    : Actor(sw, IID_FLOOR, startCoord)
{
}


void FloorBlock::doSomething()
{
}


// IceMonster
void IceMonster::doSomething()
{
    // Kill any freezable actors on the same tile first
    getWorld()->FreezeFreezablesAtCoord(getCoord());

    // Wait until it's time to move
    if (m_ticksInCurrentState < TICKS_TO_WALK - 1) {
        ++m_ticksInCurrentState;
        return;
    }

    m_ticksInCurrentState = 0;
    const Coord front = getTargetCoord(getDirection());
    const Coord frontDown = getTargetCoord(front, down);

    if (!getWorld()->isCoordPassible(front) || getWorld()->isCoordPassible(frontDown))
        setDirection(flipDirection(getDirection()));
    else
        moveForward();
}


// Lemming
void Lemming::doSomething()
{
    // 1) wait to move
    if (m_ticksInCurrentState < m_maxTicksInCurrentState - 1) {
        ++m_ticksInCurrentState;
        return;
    }

    m_ticksInCurrentState = 0;

    // 2) process pheromone attractions
    const int attractorDir = getWorld()->getAttractorDirAtCoord(getCoord());
    if (attractorDir != -1)
        setDirection(attractorDir);

    // 3) possibly change state (falling, climbing, bouncing)
    CheckToChangeState();

    // Act based on current state
    if (m_state == BOUNCING) {
        --m_bounceHeight;
        moveTo(up);
        return;
    }

    if (m_state == FALLING) {
        ++m_fallDistance;
        moveTo(down);
        return;
    }

    // if not falling, reset fall distance and check for fatal fall
    if (m_fallDistance > MAX_FALL_DISTANCE && m_state == WALKING) {
        setDead();
        return;
    }
    m_fallDistance = 0;

    if (m_state == CLIMBING) {
        moveTo(up);
        return;
    }

    if (m_state == WALKING) {
        const Coord front = getTargetCoord(getDirection());
        if (!getWorld()->isCoordPassible(front))
            setDirection(flipDirection(getDirection()));
        else
            moveForward();
    }
}


// internal function
void Lemming::CheckToChangeState()
{
    if (m_state == BOUNCING) {
        if (m_bounceHeight <= 0)
            m_state = WALKING;
        return;
    }

    if (getWorld()->isCoordClimbable(getCoord())) {
        m_state = CLIMBING;
        m_maxTicksInCurrentState = TICKS_TO_DEFAULT_ACT;
        return;
    }

    if (m_state == CLIMBING) {
        m_state = WALKING;
        m_maxTicksInCurrentState = TICKS_TO_WALK;
        return;
    }

    const Coord below(getCoord().x, getCoord().y - 1);
    if (getWorld()->isCoordPassible(below)) {
        m_state = FALLING;
        m_maxTicksInCurrentState = TICKS_TO_DEFAULT_ACT;
        return;
    }

    // default: start/continue walking
    m_state = WALKING;
    m_maxTicksInCurrentState = TICKS_TO_WALK;
}


void Lemming::bounce(int height)
{
    if (m_state == BOUNCING)
        return;

    getWorld()->playSound(SOUND_BOUNCE);
    m_state = BOUNCING;
    m_bounceHeight = (height == -1) ? (m_fallDistance - 1) : height;
    m_fallDistance = 0;
}


void Lemming::save()
{
    getWorld()->playSound(SOUND_LEMMING_SAVED);
    m_active = false;
}


void Lemming::setDead()
{
    getWorld()->decLemmingsLeft();
    getWorld()->playSound(SOUND_LEMMING_DIE);
    m_active = false;
}


// Lemming Factory
void LemmingFactory::doSomething()
{
    ++m_ticksSinceLastSpawn;
    if (m_ticksSinceLastSpawn >= TICKS_TO_SPAWN_LEMMING && m_numSpawned < MAX_LEMMINGS) {
        m_ticksSinceLastSpawn = 0;
        getWorld()->addActor(new Lemming(getWorld(), getCoord()));
        ++m_numSpawned;
    }
}


// Bonfire
void Bonfire::doSomething()
{
    getWorld()->BurnBurnablesAtCoord(getCoord());
}


// Trampoline
void Trampoline::doSomething()
{
    // -1 indicates bounce height uses how far the lemming fell
    getWorld()->BounceBounceablesAtCoord(getCoord(), -1);
}


// One Way Door
void OneWayDoor::doSomething()
{
    getWorld()->setDirectionAtCoord(getCoord(), getDirection());
}


// Pheromone
void Pheromone::doSomething()
{
    // no behavior yet
}


// Spring
void Spring::doSomething()
{
    getWorld()->BounceBounceablesAtCoord(getCoord(), 15);
}


// Exit
void Exit::doSomething()
{
    getWorld()->SaveSavableAtCoord(getCoord());
}