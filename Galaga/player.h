

class PlayerBehaviourComponent : public Component
{
	float time_fire_pressed;	// time from the last time the fire button was pressed
	ObjectPool<Rocket> * rockets_pool;
	bool controlsLocked;
	Sprite* extraShipSprite;
	bool twoShips;
	int extraSpace;

public:
	virtual ~PlayerBehaviourComponent() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<Rocket> * rockets_pool)
	{
		Component::Create(system, go, game_objects);
		this->rockets_pool = rockets_pool;
	}

	virtual void Init()
	{
		go->horizontalPosition = SCREEN_MIDPOINT;
		go->verticalPosition = SCREEN_HEIGHT - 32;

		time_fire_pressed = -10000.f;
		controlsLocked = false;
		twoShips = false;
		extraSpace = 0;

		RenderComponent* rc = go->GetComponent<RenderComponent*>();
		extraShipSprite = rc->GetSprite();

	}

	virtual void Update(float dt)
	{
		if (!controlsLocked) {
			AvancezLib::KeyStatus keys;
			system->getKeyStatus(keys);
			if (keys.right)
				Move(dt * PLAYER_SPEED);
			else if (keys.left)
				Move(-dt * PLAYER_SPEED);
			else if (keys.fire)
			{
				if (CanFire())
				{
					// fetches a rocket from the pool and use it in game_objects
					Rocket* rocket = rockets_pool->FirstAvailable();
					if (rocket != NULL)	// rocket is NULL is the object pool can not provide an object
					{
						rocket->Init(go->horizontalPosition);
						game_objects->insert(rocket);
					}
					if (twoShips) {
						rocket = rockets_pool->FirstAvailable();
						if (rocket != NULL)	// rocket is NULL is the object pool can not provide an object
						{
							rocket->Init(go->horizontalPosition + extraSpace);
							game_objects->insert(rocket);
						}
					}
				}
			}
		}

		if (twoShips) {
			extraShipSprite->draw(go->horizontalPosition + extraSpace, go->verticalPosition);
		}

		
	}


	// move the player left or right
	// param move depends on the time, so the player moves always at the same speed on any computer
	void Move(float move)
	{
		go->horizontalPosition += move;

		if (go->horizontalPosition > (640 - 32 - extraSpace))
			go->horizontalPosition = 640 - 32 - extraSpace;
			
		if (go->horizontalPosition < 0)
			go->horizontalPosition = 0;
	}

	// return true if enough time has passed from the previous rocket
	bool CanFire()
	{
		// shoot just if enough time passed by
		if ((system->getElapsedTime() - time_fire_pressed) < (FIRE_TIME_INTERVAL / game_speed))
			return false;

		time_fire_pressed = system->getElapsedTime();

		SDL_Log("fire!");
		return true;
	}

	void lockControls() {
		controlsLocked = true;
	}
	void unlockControls() {
		controlsLocked = false;
	}
	void activateSecondShip() {
		twoShips = true;
		extraSpace = 32;
	}
	void deactivateSecondShip() {
		twoShips = false;
		extraSpace = 0;
	}
};


// the main player
class Player : public GameObject
{
public:

	int lives;	// it's game over when goes below zero 
	bool captured; //check if the player is captured
	
	

	virtual ~Player()	{		SDL_Log("Player::~Player");	}

	virtual void Init()
	{
		SDL_Log("Player::Init");
		GameObject::Init();
		lives = NUM_LIVES;
		captured = false;
	}

	virtual void Receive(Message m) 
	{
		if (m == HIT && !captured)
		{ 
			SDL_Log("Player::Hit!");
			RemoveLife();

			if (lives < 0)
				Send(GAME_OVER);
		}
		if (m == PLAYER_CAPTURED) {
			SDL_Log("Player::Captured!");
			RemoveLife();

			PlayerBehaviourComponent* pbc = GetComponent<PlayerBehaviourComponent*>();
			pbc->lockControls();
			captured = true;

			if (lives < 0)
				Send(GAME_OVER);
		}
		if (m == RETURNED_TO_FORMATION) {

			PlayerBehaviourComponent* pbc = GetComponent<PlayerBehaviourComponent*>();
			pbc->unlockControls();
			captured = false;
		}
		if (m == UNLOCK_SECOND_SHIP) {
			//RenderComponent* rc = GetComponent<RenderComponent*>();
			PlayerBehaviourComponent* pbc = GetComponent<PlayerBehaviourComponent*>();
			pbc->activateSecondShip();
			//pbcextraShipSprite = rc->GetSprite();
		}
	}

	void RemoveLife()
	{
		lives--;
		SDL_Log("remaining lives %d", lives);
	}
};
