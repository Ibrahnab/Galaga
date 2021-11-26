#include "BezierPath.h"
#include "SDL.h"
#include "game_object.h"
#include "component.h"
#include "enemy.h";
#include "avancezlib.h"

#include "boss_galaga.h"

#include <string>
#include <map>


#include "Constants.h"


void BossGalaga::Init(bool* change_direction, double xPos, double yPos) {
	SDL_Log("Butterfly::Init");

	this->horizontalPosition = xPos;
	this->verticalPosition = yPos;
	this->change_direction = change_direction;
}

void BossGalaga::setPlayer(GameObject* player, std::map<std::string, Sprite*>* spriteMap)
{
	bgbc = GetComponent<BossGalagaBehaviorComponent*>();
	bgbc->player = player;
	bgbc->spriteMap = spriteMap;
	
	this->health = 2;
	this->spriteMap = spriteMap;
}

void BossGalagaBehaviorComponent::Update(float dt) {
	this->dt = dt;
	enemy = (Enemy*)go;
	HandleState();
	if (capturedPlayer) {
		(*spriteMap)["redShip"]->draw(enemy->horizontalPosition, enemy->verticalPosition - 40);
	}
	
}
 

void BossGalaga::Receive(Message m)
{

	if (!enabled)
		return;

	if (m == HIT)
	{

		RenderComponent* rc = GetComponent<RenderComponent*>();
		
		if (health == 2) {
			
			//rc->changeSprite(oneHealthSprite);
			rc->changeSprite((*spriteMap)["oneHealthBoss"]);
			health--;
			return;

		}

		rc->changeSprite((*spriteMap)["twoHealthBoss"]); //

		(*sameTypeLeft)--;
		//callDeathAnimation();
		if (bgbc->capturedPlayer /*&& bgbc->currentState == bgbc->dive*/) {
			Send(UNLOCK_SECOND_SHIP);
			bgbc->capturedPlayer = false;
		}
		enabled = false;
		Send(BG_HIT);
		SDL_Log("Boss::Hit");
	}
}


void BossGalagaBehaviorComponent::HandleFormationState()
{
	
	//std::string text = "dir: " + std::to_string(enemy->direction);
	//SDL_Log(text.c_str());
	
	enemy->horizontalPosition += (enemy->direction) * FORMATION_SPEED * dt; // direction * speed * time

	if ((enemy->direction == 1) && (enemy->horizontalPosition > (640 - 32)))
		*(enemy->change_direction) = true;

	if ((enemy->direction == -1) && (enemy->horizontalPosition < 0))
		*(enemy->change_direction) = true;
}

void BossGalagaBehaviorComponent::HandleReturnToFormation()
{

	//COME BACK HERE
	Vector2D position = Vector2D{ *gridPosX + targetPosition.x,*gridPosY + targetPosition.y };

	Vector2D dist = position - enemy->Pos();
	Vector2D normDist = dist / (dist.magnitude());
	enemy->horizontalPosition += normDist.x * FLY_TO_POSITION_SPEED * dt;
	enemy->verticalPosition += normDist.y * FLY_TO_POSITION_SPEED * dt;
	if (dist.magnitudeSqr() < EPSILON) {
		currentState = formation;
		currentWaypoint = 0;
		enemy->Send(RETURNED_TO_FORMATION);
	}
}

void BossGalagaBehaviorComponent::StartTractorBeam()
{
	
	currentState = tractorBeam;
	currentPath = 1;
}

void BossGalagaBehaviorComponent::HandleBeamState() {


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
		timeBeamActivated = system->getElapsedTime();
		currentState = tractorBeamActive ;
		//SDL_Log("STATE CHANGED");
	}


}

void BossGalagaBehaviorComponent::HandleActiveBeamState() {
	if ((system->getElapsedTime() - timeBeamActivated) < (BEAM_TIME_ACTIVE)) {
		if ((*playerPosition).x > enemy->horizontalPosition - 50 && playerPosition->x < enemy->horizontalPosition + 50) {
	
			enemy->Send(PLAYER_CAPTURED);
			capturedPlayer = true;

			RenderComponent* player_rbc = player->GetComponent<RenderComponent*>();
			playerSprite = player_rbc->GetSprite();

			currentState = returnToFormation;
		}
		else {
			(*spriteMap)["tractorBeam"]->draw(go->horizontalPosition-52, go->verticalPosition + 32);
		}


	}
	else {
		currentState = flyToPosition;
	}
	
}

