

//class FormationBehaviourComponent;

class Formation : public GameObject
{
public:

	virtual ~Formation() { SDL_Log("Formation::~Formation"); }

	int amountReachedFormation;
	int* amountEnemies;


	virtual void Init()
	{
		SDL_Log("Formation::Init");
		GameObject::Init();
		amountReachedFormation = 0;
		horizontalPosition = 0;
		verticalPosition = 50;
	}
};


class FormationBehaviourComponent : public Component
{
	float time_bomb_launched;
	bool change_direction;
	bool formationLock;
	int direction;

	Formation* aliensGrid;

	ObjectPool<Enemy>* enemy_pool;
	ObjectPool<Bomb>* bombs_pool;

	std::vector<BossGalaga*> bg_vector;
	std::vector<Butterfly*> bf_vector;

	std::vector<std::vector<Vector2D>> * Paths;

	bool* doubleShipActive;
	Player* player;
	Vector2D * playerPosition;

	int amountBossGalaga;
	int amountButterfly;

	unsigned int* stage;
	
	bool tractorBeamActivated;
	bool challengeStage;

	std::map<std::string, Sprite*>* spriteMap;

public:

	int amountEnemies;

	virtual ~FormationBehaviourComponent() {}

	int * getAmountEnemiesPtr() {
		return &amountEnemies;
	}

	virtual void Create(AvancezLib* system, GameObject* go, std::set<GameObject*>* game_objects, ObjectPool<Enemy>* enemy_pool, ObjectPool<Bomb>* bombs_pool, std::vector<std::vector<Vector2D>> * Paths, Player * player,Vector2D * playerPosition, bool * doubleShipActive, std::map<std::string, Sprite*>* spriteMap,unsigned int * stage)
	{
		Component::Create(system, go, game_objects);

		this->enemy_pool = enemy_pool;
		this->bombs_pool = bombs_pool;
		this->Paths = Paths;
		this->playerPosition = playerPosition;
		this->player = player;
		this->doubleShipActive = doubleShipActive;
		this->spriteMap = spriteMap;
		this->stage = stage;
	}

	void lockFormation() {
		formationLock = true;
	}

	virtual void Init()
	{

		tractorBeamActivated = false;
		amountBossGalaga = 0;
		amountButterfly = 0;
		int currentPath;

		if (((*stage) % 3) > 0) {
			challengeStage = false;
			currentPath = 0;
		}
		else {
			challengeStage = true;
			currentPath = 4;
		}
		/*for (int i = 0; i < 15; i++) {
			//int m = (*stage) % 3;
			int m = i % 3;
			std::string s = "STAGETHINGY: " + std::to_string(m);
			SDL_Log(s.c_str());
		}*/




		formationLock = false;

		Formation* g = (Formation*)go;
		time_bomb_launched = -10000.f;	// time from the last time a bomb was dropped by one of the aliens
		this->direction = 1;

		/*
		
		for (auto i = 0; i < 3; i++)
				for (auto j = 0; j < 10; j++)
				{
					Enemy* enemy = enemy_pool->FirstAvailable();

					if (enemy->nameId == "BossGalaga") {
						BossGalaga* bg = (BossGalaga*)enemy;
						bg_vector.push_back(bg);
						bg->setPlayer(player, spriteMap);

						amountBossGalaga++;
						enemy->Init(&change_direction, Vector2D{ j * 38 + go->horizontalPosition ,i * 32 + 32 * 2 + go->verticalPosition }, currentPath, 0, &challengeStage, &go->horizontalPosition, &go->verticalPosition, Paths, &amountBossGalaga, playerPosition);
						game_objects->insert(enemy);
					}
					if (enemy->nameId == "Butterfly") {
						Butterfly* bf = (Butterfly*)enemy;
						bf_vector.push_back(bf);

						amountButterfly++;
						enemy->Init(&change_direction, Vector2D{ j * 38 + go->horizontalPosition ,i * 32 + 32 * 2 + go->verticalPosition }, currentPath, 0, &challengeStage, &go->horizontalPosition, &go->verticalPosition, Paths, &amountButterfly, playerPosition);
						game_objects->insert(enemy);
					}
				}

			change_direction = false;
		}
		
		
		*/



		//FIXA SÅ ATT BOSS PÅ TOPP OCH BUTTERFLY BOTT
		if (!challengeStage) {
			for (auto i = 0; i < 30; i++)
			{
				Enemy* enemy = enemy_pool->FirstAvailable();

				if (enemy->nameId == "BossGalaga") {
					BossGalaga* bg = (BossGalaga*)enemy;
					bg_vector.push_back(bg);
					bg->setPlayer(player, spriteMap);

					amountBossGalaga++;
					enemy->Init(&change_direction, Vector2D{ 1 * 38 + go->horizontalPosition ,i * 32 + 32 * 2 + go->verticalPosition }, currentPath, 0, &challengeStage, &go->horizontalPosition, &go->verticalPosition, Paths, &amountBossGalaga, playerPosition);
					game_objects->insert(enemy);
				}
				if (enemy->nameId == "Butterfly") {
					Butterfly* bf = (Butterfly*)enemy;
					bf_vector.push_back(bf);

					amountButterfly++;
					enemy->Init(&change_direction, Vector2D{ 1 * 38 + go->horizontalPosition ,i * 32 + 32 * 2 + go->verticalPosition }, currentPath, 0, &challengeStage, &go->horizontalPosition, &go->verticalPosition, Paths, &amountButterfly, playerPosition);
					game_objects->insert(enemy);
				}
				
			}

			change_direction = false;
		}
	}

	

	virtual void Update(float dt)
	{
		amountEnemies = amountBossGalaga + amountButterfly;
		// check is one of tha aliens wants to change direction becase it reached the edge of the window 
		go->horizontalPosition += direction * FORMATION_SPEED * dt;

		if ((direction == 1) && (go->horizontalPosition > (300)))
			direction *= -1;

		if ((direction == -1) && (go->horizontalPosition < 0))
			direction *= -1;


		if (change_direction) {
			//this->direction *= -1;
			for (auto enemy = enemy_pool->pool.begin(); enemy != enemy_pool->pool.end(); enemy++)
				if ((*enemy)->enabled)
					(*enemy)->ChangeDirection();
		}

		// is enough time passed from the last bomb, shoot another bomb from a random active alien
		if (CanFire())
		{
			Bomb* bomb = bombs_pool->FirstAvailable();
			if (bomb != NULL)
			{
				Enemy* enemy = enemy_pool->SelectRandom();
				if (enemy != NULL)
				{
					bomb->Init(enemy->horizontalPosition, enemy->verticalPosition + 32);
					game_objects->insert(bomb);
				}
			}
		}

		if (CanDive() && formationLock) {
			Enemy* enemy = enemy_pool->SelectRandom();
			if (enemy != NULL && !(enemy->usedDive))
			{
				enemy->usedDive = true;
				EnemyBehaviourComponent* comp = enemy->GetComponent<EnemyBehaviourComponent*>();
				comp->activateDive();
			}
		}

		if (CanTractorBeam() && formationLock) {
			SDL_Log("Selecting boss for tractor beam");
			BossGalaga* bg = bg_vector[rand() % bg_vector.size()];
			if (bg != NULL && !(bg->usedDive)) {
				tractorBeamActivated = true;
				BossGalagaBehaviorComponent* comp = bg->GetComponent<BossGalagaBehaviorComponent*>();
				comp->StartTractorBeam();
			}
		}

		change_direction = false;
	}

	// return true if enough time has passed from the previous bomb
	bool CanFire()
	{
		// shoot just if enough time passed by
		if ((system->getElapsedTime() - time_bomb_launched) < (BOMB_TIME_INTERVAL / game_speed))
			return false;

		// drop the bomb with 3% chance
		if ((rand() / (float)RAND_MAX) < 0.97f)
			return false;

		time_bomb_launched = system->getElapsedTime();

		SDL_Log("bomb!");
		return true;
	}

	bool CanDive() {
		// shoot just if enough time passed by
		if ((system->getElapsedTime() - time_bomb_launched) < (BOMB_TIME_INTERVAL / game_speed))
			return false;

		
		if ((rand() / (float)RAND_MAX) < 0.97f)
			return false;

		time_bomb_launched = system->getElapsedTime();

		SDL_Log("bomb!");
		return true;
	}

	bool CanTractorBeam() {

		if (!tractorBeamActivated && !(*doubleShipActive)) {
			if ((rand() / (float)RAND_MAX) < 0.98f) {
				return false;
			}
			return true;
		}
		return false;
		
	}
};


