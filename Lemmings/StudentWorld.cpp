#include "StudentWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "Actor.h"
#include "Level.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;








GameWorld* createStudentWorld(string assetPath) 
{
    return new StudentWorld(assetPath);
}





// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp
// Do not change or remove the createStudentWorld implementation above.

StudentWorld::StudentWorld(string assetPath)
	: GameWorld(assetPath), m_saved(0), m_timeLeft(2000)
{

}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    m_player = new Player(this);

    m_saved = 0;
    m_timeLeft = 2000;
	m_lemmingsLeft = NUM_LEMMINGS_TO_SAVE;


    int returnVal = loadLevel();

    // debug

    return returnVal;
}

int StudentWorld::move()
{
	m_timeLeft--;

    updateGameStatText();

	m_player->doSomething();

    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors.at(i)->isActive()) {
            m_actors.at(i)->doSomething();
        }
    }

    auto it = m_actors.begin();
    while (it != m_actors.end()) {
        if (!(*it)->isActive()) {
            delete* it;
            it = m_actors.erase(it);
        }
        else {
            it++;
        }
    }


    
    return getCurrentStatus();
}

int StudentWorld::getCurrentStatus()
{
    if (m_saved >= NUM_LEMMINGS_TO_SAVE)
    {
		playSound(SOUND_FINISHED_LEVEL);
        increaseScore(m_timeLeft);
        return GWSTATUS_FINISHED_LEVEL;
    }

	if (m_timeLeft <= 0 || m_lemmingsLeft <= NUM_LEMMINGS_TO_SAVE/2)
    {
        playSound(SOUND_PLAYER_DIE);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (int i = 0; i < m_actors.size(); i++)
    {
        delete m_actors.at(i);
    }

    m_actors.clear();

    if(m_player != nullptr)
        delete m_player;

    m_player = nullptr;
}

int StudentWorld::loadLevel() 
{
    // get the level's file name
    ostringstream oss;
    oss << "level";
    oss.fill('0');
    oss << setw(2) << getLevel();
    oss << ".txt";

    Level* lev = new Level(assetPath()); // note: GameWorld provides assetPath()
    Level::LoadResult result = lev->loadLevel(oss.str());
    if (result == Level::load_fail_file_not_found)
		return GWSTATUS_PLAYER_WON; // signal framework to end game with player victory
    else if (result == Level::load_fail_bad_format)
        cerr << lev->getErrorMessage() << endl;

    for (int i = 0; i < VIEW_WIDTH; i++)
    {
        for (int j = 0; j < VIEW_HEIGHT; j++)
        {
            Coord c(i, j);
            Level::MazeEntry me = lev->getContentsOf(c);
            switch (me)
            {
            case Level::empty:
                break;
            case Level::floor:
                m_actors.push_back(new FloorBlock(this, c));
                break;
            case Level::lemming_factory:
                m_actors.push_back(new LemmingFactory(this, c));
                break;
            case Level::ice_monster:
                m_actors.push_back(new IceMonster(this, c));
                break;
            case Level::bonfire:
                m_actors.push_back(new Bonfire(this, c));
                break;
            case Level::trampoline:
                m_actors.push_back(new Trampoline(this, c));
                break;
            case Level::net:
                m_actors.push_back(new Net(this, c));
                break;
            case Level::left_one_way_door:
                m_actors.push_back(new OneWayDoor(this, c, 180));
                break;
            case Level::right_one_way_door:
                m_actors.push_back(new OneWayDoor(this, c, 0));
                break;
            case Level::pheromone:
                m_actors.push_back(new Pheromone(this, c));
                break;
            case Level::spring:
                m_actors.push_back(new Spring(this, c));
                break;
            case Level::lemming_exit:
                m_actors.push_back(new Exit(this, c));
                break;
            }
        }
    }

    m_tools = lev->getTools();

	delete lev;

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::updateGameStatText()
{
    ostringstream oss;
    oss << "Score: ";
    oss.fill('0');
    oss << setw(5) << getScore();
    oss << "  Level: ";
    oss.fill('0');
    oss << setw(2) << getLevel();
    oss << "  Lives: ";
    oss.fill('0');
    oss << setw(2) << getLives();
    oss << "  Saved: ";
    oss.fill('0');
    oss << setw(2) << m_saved;
    oss << "  Tools: ";
    oss << m_tools;
    oss << "  Time left: ";
    oss.fill('0');
    oss << setw(4) << m_timeLeft;

    setGameStatText(oss.str());
}

void StudentWorld::addActor(Actor* a)
{
    m_actors.push_back(a);
}

bool StudentWorld::AttemptToPlaceActorAt(Actor* a, Coord c)
{
    for (Actor* a : m_actors)
    {
        if (a->getCoord() == c)
        {
            delete a;
            return false;
        }
    }

    addActor(a);
    
	return true;
}

bool StudentWorld::isCoordPassible(Coord c) const
{
	for (Actor* a : m_actors)
    {
        if (!a->isPassable() && a->getCoord() == c)
        {
            return false;
        }
    }

    return true;
}

void StudentWorld::BurnBurnablesAtCoord(Coord c)
{
    for (Actor* a : m_actors)
    {
        if (a->isBurnable() && a->getCoord() == c)
        {
            a->setDead();
        }
    }
}

bool StudentWorld::FreezeFreezablesAtCoord(Coord c)
{
    bool returnVal = false;
    for (Actor* a : m_actors)
    {
        if (a->isFreezable() && a->getCoord() == c)
        {
            a->setDead();
            returnVal = true;
        }
    }

    return returnVal;
}

void StudentWorld::BounceBounceablesAtCoord(Coord c, int height)
{
    for (Actor* a : m_actors)
    {
        if (a->isBounceable() && a->getCoord() == c)
        {
            a->bounce(height);
        }
    }
}

void StudentWorld::SaveSavableAtCoord(Coord c)
{
    for (Actor* a : m_actors)
    {
        if (a->isSavable() && a->getCoord() == c)
        {
            a->save();
            m_saved++;
            increaseScore(100);
            return;
        }
    }
}

bool StudentWorld::isCoordClimbable(Coord c) const
{
    for (Actor* a : m_actors)
    {
        if (a->isClimbable() && a->getCoord() == c)
        {
            return true;
        }
    }

    return false;
}

void StudentWorld::setDirectionAtCoord(Coord c, int dir)
{
    for (Actor* a : m_actors)
    {
        if (a->getCoord() == c)
        {
            a->setDirection(dir);
        }
    }
}

bool StudentWorld::removeCharFromTools(char c) {
    size_t pos = m_tools.find(c);
    if (pos != string::npos) {
        m_tools.erase(pos, 1);
        return true;
    }
	return false;
}

bool StudentWorld::isCoordAttractor(Coord c) {
    for (Actor* a : m_actors)
    {
        if (a->getCoord() == c && a->isAttractor())
        {
            return true;
        }
    }

    return false;
}


int StudentWorld::getAttractorDirAtCoord(Coord c) {
    int closestR = 6;
    int closestL = 6;

    // check right (distance 1..5)
    for (int i = 1; i <= 5; i++) {
        if (isCoordAttractor(Coord(c.x + i, c.y))) {
            closestR = i;
            break; // nearest to the right found
        }
    }

    // check left (distance 1..5)
    for (int i = 1; i <= 5; i++) {  
        if (isCoordAttractor(Coord(c.x - i, c.y))) {
            closestL = i;
            break; // nearest to the left found
        }
    }

    if (closestR == 6 && closestL == 6)
        return -1; // no attractor found

    if (closestR <= closestL)
        return 0; // right
    else
        return 180; // left
}