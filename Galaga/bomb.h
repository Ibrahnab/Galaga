class BombBehaviourComponent : public Component
{
public:
	virtual void Update(float dt)
	{
		go->verticalPosition += BOMB_SPEED * dt;

		if (go->verticalPosition > SCREEN_HEIGHT) // When a bomb reaches the bottom of the screen, it disappears.
			go->enabled = false;
	}
};





class Bomb : public GameObject
{
public:
	virtual void Init(double xPos, double yPos)
	{
		SDL_Log("Bomb::Init");
		GameObject::Init();

		horizontalPosition = xPos;
		verticalPosition = yPos;
	}


	virtual void Receive(Message m) 
	{
		if (m == HIT)
		{ 
			enabled = false;
			SDL_Log("Bomb::Hit");
		}
	}
};

