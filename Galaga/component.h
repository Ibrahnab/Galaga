#include <set> 
#include "object_pool.h" 

class GameObject;
class AvancezLib;
class Sprite;
class AnimatedSprite;

class Component
{
protected:
	AvancezLib * system;	// used to access the system
	GameObject * go;		// the game object this component is part of
	std::set<GameObject*> * game_objects;	// the global container of game objects

public:
	virtual ~Component() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects);

	virtual void Init() {}
	virtual void Update(float dt) = 0;
	virtual void Receive(int message) {}
	virtual void Destroy() {}
};


class RenderComponent : public Component
{
	Sprite* sprite;
	AnimatedSprite* animatedSprite;

public:

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, const char * sprite_name);
	void makeDeathAnimation(const char * sprite_name, int spriteAmount, int animationSpeed);
	void runDeathAnimation();
	void changeSprite(Sprite* s);
	virtual void Update(float dt);
	virtual void Destroy();

	Sprite * GetSprite() { return sprite; }
};


class CollideComponent : public Component
{
	ObjectPool<GameObject> * coll_objects; // collision will be tested with these objects

public:
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects);
	virtual void Update(float dt);
};

