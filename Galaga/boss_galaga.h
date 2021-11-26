#pragma once

#include <map>


class BossGalagaBehaviorComponent : public EnemyBehaviourComponent
{
	float timeBeamActivated;
	

	
public: 
	
	GameObject* player;
	bool capturedPlayer;
	Sprite* playerSprite;
	std::map<std::string, Sprite*>* spriteMap;

	void Update(float dt);
	//void HandleDiveState();
	void HandleBeamState();
	void HandleActiveBeamState();
	void HandleFormationState();
	void HandleReturnToFormation();
	void StartTractorBeam();
	
};
class BossGalaga : public Enemy
{
	
	

public:
	BossGalagaBehaviorComponent* bgbc;

	Sprite* redShip;
	int health;
	Sprite* oneHealthSprite;
	Sprite* twoHealthSprite;
	std::map<std::string, Sprite*>* spriteMap;

	//int* sameTypeLeft; //Keep track of how many of this type of enemy is left

	virtual void Init(bool* change_direction, double xPos, double yPos);
	virtual void Receive(Message m);
	void setPlayer(GameObject* player, std::map<std::string,Sprite*> * spriteMap);


};