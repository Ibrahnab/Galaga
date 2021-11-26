#include "BezierPath.h"

#include "SDL.h"
#include "game_object.h"
#include "component.h"
#include "enemy.h";

#include "boss_galaga.h"
#include "Constants.h"
//#include "avancezlib.h"
//#include "player.h"

void Enemy::Init(bool* change_direction, Vector2D targetPosition, int path, int index, bool * challengeStage, double* gridPosX, double* gridPosY, std::vector<std::vector<Vector2D>>* Paths, int *sameTypeLeft, Vector2D* playerPosition)
{
	SDL_Log("Enemy::Init");
	//GameObject::Init();
	enabled = true;
	EnemyBehaviourComponent* ebc = GetComponent<EnemyBehaviourComponent*>();
	this->horizontalPosition = targetPosition.x;
	this->verticalPosition = targetPosition.y;
	this->change_direction = change_direction;
	this->direction = 1;
	this->sameTypeLeft = sameTypeLeft;
	this->usedDive = false;
	

	ebc->Init(path, index, challengeStage, gridPosX, gridPosY, targetPosition,Paths, playerPosition);
		
}

void Enemy::CreatePaths()
{
}

void Enemy::Pos(Vector2D v)
{
	horizontalPosition = v.x;
	verticalPosition = v.y;
}

Vector2D Enemy::Pos()
{

	return { Vector2D(horizontalPosition,verticalPosition) };
}

void Enemy::ChangeDirection()
{
	this->direction *= -1;
}

void Enemy::setNameId(std::string nameId)
{
	this->nameId = nameId;
}

void Enemy::callDeathAnimation()
{
	RenderComponent* rc = GetComponent<RenderComponent*>();
	rc->runDeathAnimation();
}

void EnemyBehaviourComponent::HandleState() {
	switch (currentState) {
	case(flyIn):
		HandleFlyInState();
		break;
	case(flyToPosition):
		HandleFlyToPosition();
		break;
	case(formation):
		HandleFormationState();
		break;
	case(dive):
		HandleDiveState();
		break;
	case(dead):
		HandleDeadState();
		break;
	case(flyToPlayer):
		HandleFlyToPlayer();
		break;
	case(tractorBeam):
		HandleBeamState();
		break;
	case(tractorBeamActive):
		HandleActiveBeamState();
		break;
	case(returnToFormation):
		HandleReturnToFormation();
		break;

	}
}

void EnemyBehaviourComponent::HandleDeadState() {


	//in case the enemy is a boss, make sure they switch to full health mode
	if (enemy->nameId == "BossGalaga") {
		BossGalaga* bg = (BossGalaga*)enemy;
		bg->health = 2;
		bg->bgbc->capturedPlayer = false;
		RenderComponent* rc = bg->GetComponent<RenderComponent*>();
		rc->changeSprite((*bg->spriteMap)["twoHealthBoss"]);
	}
	enemy->enabled = false;
}

void EnemyBehaviourComponent::HandleDiveState()
{

	if (((*Paths)[currentPath][currentWaypoint] - enemy->Pos()).magnitudeSqr() < EPSILON) {
		currentWaypoint++;

		//SDL_Log("WayPoint changed");

	}
	if (currentWaypoint < (*Paths)[currentPath].size()) {
		Vector2D dist = (*Paths)[currentPath][currentWaypoint] - enemy->Pos();
		Vector2D normDist = dist / (dist.magnitude());
		enemy->horizontalPosition += normDist.x * ALIEN_SPEED * dt;
		enemy->verticalPosition += normDist.y * ALIEN_SPEED * dt;
	}
	else {
		//alien->currentState = alien->stat;
		currentWaypoint = 0; 
		diveTargetPosition = (*playerPosition);

		std::string s = "targetPosX: " + std::to_string(diveTargetPosition.x) + " targetPosY: " + std::to_string(diveTargetPosition.y);
		SDL_Log(s.c_str());
		diveTargetPosition.y += 40;
		currentState = flyToPlayer;
		//SDL_Log("STATE CHANGED");
	}


}

void EnemyBehaviourComponent::HandleFlyInState() {

	if (((*Paths)[currentPath][currentWaypoint] - enemy->Pos()).magnitudeSqr() < EPSILON) {
		currentWaypoint++;

		//SDL_Log("WayPoint changed");

	}
	if (currentWaypoint < (*Paths)[currentPath].size()) {
		Vector2D dist = (*Paths)[currentPath][currentWaypoint] - enemy->Pos();
		Vector2D normDist = dist / (dist.magnitude());
		enemy->horizontalPosition += normDist.x * ALIEN_SPEED * dt;
		enemy->verticalPosition += normDist.y * ALIEN_SPEED * dt;
	}
	else {
		//alien->currentState = alien->stat;
		if (!(*challengeStage)) {
			currentState = flyToPosition;
		}
		else {
			currentState = dead;
		}
		
		
		//SDL_Log("STATE CHANGED");
	}

}
void EnemyBehaviourComponent::HandleFlyToPlayer()
{
	Vector2D position = Vector2D{diveTargetPosition.x, diveTargetPosition.y };

	Vector2D dist = position - enemy->Pos();
	Vector2D normDist = dist / (dist.magnitude());
	enemy->horizontalPosition += normDist.x * ALIEN_SPEED * dt;
	enemy->verticalPosition += normDist.y * ALIEN_SPEED * dt;
	if (dist.magnitudeSqr() < EPSILON) {
		currentState = dead;
		currentWaypoint = 0;


		std::string s = "PosX: " + std::to_string(enemy->horizontalPosition) + " PosY: " + std::to_string(enemy->verticalPosition);
		SDL_Log(s.c_str());


		//SDL_Log("FORMATION REACHED");
		//enemy->Send(FORMATION_REACHED);
	}
}

void EnemyBehaviourComponent::HandleFlyToPosition() {
	Vector2D position = Vector2D{ *gridPosX + targetPosition.x,*gridPosY + targetPosition.y };

	Vector2D dist = position - enemy->Pos();
	Vector2D normDist = dist / (dist.magnitude());
	enemy->horizontalPosition += normDist.x * FLY_TO_POSITION_SPEED * dt;
	enemy->verticalPosition += normDist.y * FLY_TO_POSITION_SPEED * dt;
	if (dist.magnitudeSqr() < EPSILON) {
		currentState = formation;
		currentWaypoint = 0;
		//SDL_Log("FORMATION REACHED");
		enemy->Send(FORMATION_REACHED);
	}
}

void EnemyBehaviourComponent::setPath(int pathNum)
{
	this->currentPath = pathNum;
}

void EnemyBehaviourComponent::activateDive()
{
	currentState = dive;
	currentWaypoint = 0;
	currentPath = rand() % 3 + 2;
	if (currentPath == 4) {
		currentPath = 3;
	}
	//std::string s= "currentPath: " + std::to_string(currentPath);
	//SDL_Log(s.c_str());
}

void EnemyBehaviourComponent::Init(int path, int index, bool * challengeStage, double* gridPosX, double* gridPosY, Vector2D targetPosition, std::vector<std::vector<Vector2D>>* Paths, Vector2D* playerPosition)
{
	//SDL_Log("Behavior::Init");


	this->currentPath = path;
	this->currentWaypoint = 0;
	this->currentState = flyIn;
	this->targetPosition = targetPosition;
	this->index = index;
	this->challengeStage = challengeStage;
	this->gridPosX = gridPosX;
	this->gridPosY = gridPosY;
	this->Paths = Paths;
	this->playerPosition = playerPosition;
}

