#pragma once
class Animator : public GameObject
{
	AvancezLib* system;

public:

	AnimatedSprite* explosionSprite;
	virtual void Init(const char* explosion, const char* playerExplosion);
	void animateMyDeath(int x, int y);

};
