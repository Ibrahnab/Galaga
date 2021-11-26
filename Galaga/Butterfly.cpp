
#include "BezierPath.h"
#include "SDL.h"
#include "game_object.h"
#include "component.h"
#include "enemy.h";

#include "Butterfly.h"
#include "Constants.h"


void Butterfly::Init(bool* change_direction, double xPos, double yPos) {
	SDL_Log("Butterfly::Init");

	this->horizontalPosition = xPos;
	this->verticalPosition = yPos;
	this->change_direction = change_direction;
}

void Butterfly::Receive(Message m)
{

	if (!enabled)
		return;


	if (m == HIT)
	{
		(*sameTypeLeft)--;
	
		//callDeathAnimation();

		enabled = false;
		Send(BF_HIT);
		SDL_Log("Butterfly::Hit");
	}
}

void ButterflyBehaviorComponent::Update(float dt) {
	this->dt = dt;
	enemy = (Enemy*)go;
	HandleState();
	//SDL_Log("bRun");
}

/*void ButterflyBehaviorComponent::HandleDiveState()
{
}*/

void ButterflyBehaviorComponent::HandleFormationState()
{
	enemy->horizontalPosition += (enemy->direction) * FORMATION_SPEED * dt; // direction * speed * time

	if ((enemy->direction == 1) && (enemy->horizontalPosition > (640 - 32)))
		*(enemy->change_direction) = true;

	if ((enemy->direction == -1) && (enemy->horizontalPosition < 0))
		*(enemy->change_direction) = true;
}


