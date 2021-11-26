#include "component.h"
#include "game_object.h"
#include "avancezlib.h"

void Component::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects)
{
	this->go = go;
	this->system = system;
	this->game_objects = game_objects;
}

void RenderComponent::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, const char * sprite_name)
{
	Component::Create(system, go, game_objects);

	sprite = system->createSprite(sprite_name);
}

void RenderComponent::makeDeathAnimation(const char* sprite_name, int spriteAmount, int animationSpeed)
{
	animatedSprite = system->createAnimatedSprite(sprite_name, spriteAmount, animationSpeed);
}

void RenderComponent::runDeathAnimation()
{
	animatedSprite->activateAnimation = true;
	//animatedSprite->AnimateOnce(go->horizontalPosition, go->verticalPosition);
}

void RenderComponent::changeSprite(Sprite* s)
{
	this->sprite = s;
}

void RenderComponent::Update(float dt)
{
	/*if(animatedSprite != nullptr)
	if (animatedSprite->activateAnimation) {
		animatedSprite->AnimateOnce(go->horizontalPosition, go->verticalPosition);
		SDL_Log("arrived");
	}
	*/

	if (!go->enabled)
		return;

	if (sprite)
		sprite->draw(int(go->horizontalPosition), int(go->verticalPosition));
}

void RenderComponent::Destroy()
{
	if (sprite != NULL)
		sprite->destroy();
	sprite = NULL;
}


void CollideComponent::Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects)
{
	Component::Create(system, go, game_objects);
	this->coll_objects = coll_objects;
}


void CollideComponent::Update(float dt)
{
	for (auto i = 0; i < coll_objects->pool.size(); i++)
	{
		GameObject * go0 = coll_objects->pool[i];
		if (go0->enabled)
		{
			if ((go0->horizontalPosition > go->horizontalPosition - 10) &&
				(go0->horizontalPosition < go->horizontalPosition + 10) &&
				(go0->verticalPosition   > go->verticalPosition - 10) &&
				(go0->verticalPosition   < go->verticalPosition + 10))
			{
				go->Receive(HIT);
				go0->Receive(HIT);
			}
		}
	}
}
