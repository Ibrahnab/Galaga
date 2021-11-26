#pragma once

//#include "BezierPath.h" 
#include <string>


class Enemy : public GameObject
{
private:



public:

	bool usedDive;
	std::string nameId;
	int direction;
	bool* change_direction;
	int* sameTypeLeft; //Keep track of how many of this type of enemy is left

	virtual void Init(bool* change_direction, Vector2D targetPosition, int path, int index, bool * challengeStage, double* gridPosX, double* gridPosY, std::vector<std::vector<Vector2D>>* Paths, int* sameTypeLeft, Vector2D * playerPosition);
	//virtual void Init(bool* change_direction, double xPos, double yPos);
	virtual void Receive(Message m) {};
	void Enemy::CreatePaths();
	void Pos(Vector2D v);
	Vector2D Pos();
	void ChangeDirection();
	void setNameId(std::string);
	void callDeathAnimation();
};

class EnemyBehaviourComponent : public Component
{

public:
	Enemy * enemy;
	
	float dt;

	//Sprite s;
	
	std::vector<std::vector<Vector2D>>* Paths;
	enum STATES { flyIn, flyToPosition, formation,returnToFormation, dive, dead,tractorBeam,flyToPlayer,tractorBeamActive };
	STATES currentState = flyIn;
	int currentPath;
	int currentWaypoint;
	const float EPSILON = 5.0f;
	Vector2D targetPosition;
	Vector2D diveTargetPosition;
	int index;
	bool * challengeStage;

	Vector2D * playerPosition; 

	double* gridPosX;
	double* gridPosY;

	virtual ~EnemyBehaviourComponent() {}

	//virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects,int path, int index, bool challengeStage, double* gridPosX, double* gridPosY, Vector2D targetPosition);
	virtual void Init(int path, int index, bool * challengeStage, double* gridPosX, double* gridPosY, Vector2D targetPosition, std::vector<std::vector<Vector2D>>* Paths, Vector2D * playerPosition);
	virtual void Update(float dt) = 0;
	void HandleState();

	void HandleDeadState();
	virtual void HandleDiveState();
	virtual void HandleFormationState() =0;
	virtual void HandleBeamState() {};
	virtual void HandleActiveBeamState() {};
	virtual void HandleReturnToFormation() {};
	void HandleFlyInState();
	void HandleFlyToPlayer();
	void HandleFlyToPosition();
	void setPath(int pathNum);
	void activateDive();

};