

#include "BezierPath.h" 
//#include <string>


class Alien : public GameObject
{
private:

	

public:

	std::vector<std::vector<Vector2D>> Paths;
	enum STATES { flyIn, formation, dive, dead };
	STATES currentState;
	int currentPath;
	int currentWaypoint;
	const float EPSILON = 5.0f;
	Vector2D targetPosition;
	int index;
	bool challengeStage;

	double * gridPosX;
	double * gridPosY;

	int direction;
	bool * change_direction;
	

	virtual void Init(bool * change_direction, double xPos, double yPos, int path, int index,bool challengeStage, double * gridPosX, double * gridPosY)
	{
		SDL_Log("Alien::Init");
		this->horizontalPosition = xPos;
		this->verticalPosition = yPos;
		this->change_direction = change_direction;
		this->currentPath = path;
		this->currentWaypoint = 0;
		this->currentState = flyIn;
		this->targetPosition = Vector2D{ xPos,yPos };
		this->index = index;
		this->challengeStage = challengeStage;
		this->gridPosX = gridPosX;
		this->gridPosY = gridPosY;
		CreatePaths();
		//Pos(Paths[currentPath][currentWaypoint]);

		direction = 1;

		enabled = true;
	}

	void Alien::CreatePaths() {
		BezierPath* path = new BezierPath(); //This makes one collection of bezierpaths, and are put together to 0
		int currentPath = 0; //Defines the path that is going to be followed

		//path->AddCurve({ Vector2D(500.f,10.f),Vector2D(500.f,0.f) ,Vector2D(500.f,310.f) ,Vector2D(500.f,300.f) },1);
		path->AddCurve({ Vector2D(SCREEN_MIDPOINT + 50.f,-10.f),Vector2D(SCREEN_MIDPOINT + 50.f,-20.f) ,Vector2D(SCREEN_MIDPOINT + 50.f,30.f) ,Vector2D(SCREEN_MIDPOINT + 50.f,20.f) }, 1);
		path->AddCurve({ Vector2D(SCREEN_MIDPOINT + 50.f,20.f),Vector2D(SCREEN_MIDPOINT + 50.f,100.f) ,Vector2D(75.f,325.f) ,Vector2D(75,425.f) }, 25);
		path->AddCurve({ Vector2D(75.f,425.f),Vector2D(75.f,650.f) ,Vector2D(325.f,650.f) ,Vector2D(325,425.f) }, 25);



		Paths.push_back(std::vector<Vector2D>());
		path->Sample(&Paths[currentPath]);
		delete path;
	}

	virtual void Receive(Message m)
	{
		if (!enabled)
			return;

		if (m == HIT)
		{
			enabled = false;
			Send(ALIEN_HIT); // re-broadcast the message to signal that the aliens has been hit (used to increase the score)
			SDL_Log("Alien::Hit");
		}
	}

	void Pos(Vector2D v) {

		this->horizontalPosition = v.x;
		this->verticalPosition = v.y;
	}
	Vector2D Pos() {
		Vector2D pos;
		pos.x = this->horizontalPosition;
		pos.y = this->verticalPosition;
		return pos;
	}
	void ChangeDirection()
	{
		direction *= -1;
		//verticalPosition += 32;

		//if (verticalPosition > (480 - 32))
		//	Send(GAME_OVER);
	}

};



class AlienBehaviourComponent : public Component
{
	
public:
	Alien* alien;
	enum STATES { flyIn,flyToPosition, formation, dive, dead };
	STATES currentState = flyIn;
	float dt;

	virtual ~AlienBehaviourComponent() {}

	virtual void Update(float dt)
	{
		this->dt = dt;
		alien = (Alien *)go;
		HandleState();
	}


	void HandleState() {
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

		}
	}

	void HandleDeadState() {

	}
	void HandleDiveState() {

	}
	void HandleFormationState() {

		alien->horizontalPosition += (alien->direction) * FORMATION_SPEED * dt; // direction * speed * time

		if ((alien->direction == 1) && (alien->horizontalPosition > (640 - 32)))
			*(alien->change_direction) = true;

		if ((alien->direction == -1) && (alien->horizontalPosition < 0))
			*(alien->change_direction) = true;

	}
	void HandleFlyInState() {
		if ((alien->Paths[alien->currentPath][alien->currentWaypoint] - alien->Pos()).magnitudeSqr() < alien->EPSILON) {
			alien->currentWaypoint++;

			//SDL_Log("WayPoint changed");

		}
		if (alien->currentWaypoint < alien->Paths[alien->currentPath].size()) {
			Vector2D dist = alien->Paths[alien->currentPath][alien->currentWaypoint] - alien->Pos();
			Vector2D normDist = dist / (dist.magnitude());
			alien->horizontalPosition += normDist.x *ALIEN_SPEED* dt;
			alien->verticalPosition += normDist.y * ALIEN_SPEED * dt;
		}
		else {
			//alien->currentState = alien->stat;
			
			currentState = flyToPosition;
			//SDL_Log("STATE CHANGED");
		}
	}
	void HandleFlyToPosition() {
		
		Vector2D position = Vector2D{ *alien->gridPosX + alien->targetPosition.x,*alien->gridPosY + alien->targetPosition.y };

		Vector2D dist = position - alien->Pos();
		Vector2D normDist = dist / (dist.magnitude());
		alien->horizontalPosition += normDist.x * ALIEN_SPEED * dt;
		alien->verticalPosition += normDist.y * ALIEN_SPEED * dt;
		if (dist.magnitudeSqr() < alien->EPSILON) {
			currentState = formation;
			SDL_Log("FORMATION REACHED");
		}


	}


};