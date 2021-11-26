#pragma once


class Butterfly : public Enemy
{
public:

	//int* sameTypeLeft; //Keep track of how many of this type of enemy is left

	virtual void Init(bool* change_direction, double xPos, double yPos);
	virtual void Receive(Message m);
};

class ButterflyBehaviorComponent : public EnemyBehaviourComponent
{
	
public:
	void Update(float dt);
	//void HandleDiveState();
	void HandleFormationState();
	
};
