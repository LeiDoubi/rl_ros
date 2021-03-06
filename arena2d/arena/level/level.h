#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <arena/physicsWorld.h>
#include <arena/burger.h>
#include <arena/rectSpawn.h>
#include <engine/f_math.h>
#include <engine/globalSettings.h>
#include <engine/zVector2d.h>
#define LEVEL_SPAWN_AREA_CATEGORY 0x0040// category bits of spawn area (for debug drawing)

#define LEVEL_GOAL_SPAWN_COLOR 0x11FF22A0
class Level
{
public:
	enum Type{EMPTY_NO_BORDER, EMPTY_BORDER, STATIC, DYNAMIC, STATIC_DYNAMIC, SVG, INDUSTRY};
	Level(b2World * w, Level::Type);
	virtual ~Level();

	virtual void update(const b2Transform & robot_transform){}
	virtual void render(){}

	virtual void spawnGoal(const b2Vec2 & pos);
	virtual void reset(b2Vec2 & robot_position, bool hard_reset){}

	// provide the agent with additional data generated by level
	virtual void getAgentData(const b2Transform & robot_transform, std::vector<float> & data){}

	// get additional reward generated by level
	virtual float getReward(const b2Transform & robot_transform){return 0;}

	virtual void renderGoalSpawn();

	b2Fixture* getGoal(){return (_goal == NULL) ? NULL : _goal->GetFixtureList();}
	b2Vec2 getGoalPosition(){return (_goal ==  NULL) ? b2Vec2(0,0) : _goal->GetTransform().p;}
protected:
	// check whether spawn position is far enough that robot doesn't touch it initially
	static bool checkValidGoalSpawn(const b2Vec2 & robot_pos, const b2Vec2 & spawn_pos){ 
		return (robot_pos-spawn_pos).Length() > (_SETTINGS->stage.goal_size/2.f+BURGER_SAFE_RADIUS);
	}
	void randomGoalSpawnUntilValid(const b2Vec2 & robot_position){
		// spawn goal at random position
		b2Vec2 spawn_position;
		int count = 0;
		do{
			spawn_position = getRandomPointInSpawnArea();
			count++;
		}while(!checkValidGoalSpawn(robot_position, spawn_position) && count < 10);
		spawnGoal(spawn_position);
	}
	static b2Vec2 getRandomPoint(float x_min, float x_max, float y_min, float y_max);
	static b2Vec2 getRandomPointCircle(const b2Vec2 & center, float radius);
	b2Vec2 getRandomPointInSpawnArea(){zVector2D v; _goalSpawnArea.getRandomPoint(v); return b2Vec2(v.x, v.y);}
	// creating rectangular border around level with given dimensions
	void createBorder(const b2Vec2 & dim);
	void addCircle(const b2Vec2 & pos, float radius);
	void addCheeseRectToSpawnArea(const zRect & main_rect, const std::vector<zRect> & holes){
		_goalSpawnArea.addCheeseRect(main_rect, holes);
	}
	void addRectToSpawnArea(const zRect & r){_goalSpawnArea.addRect(r);}
	void calculateSpawnArea(){_goalSpawnArea.calculateArea();}
	void clearSpawnArea(){_goalSpawnArea.clear();}
	void clearBodyList();
	std::list<b2Body*> _bodyList;
	b2Body * _goal;
	RectSpawn _goalSpawnArea;
	Type _type;
	b2World * _world;
};

#endif
