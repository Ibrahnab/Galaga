
bool change_direction = false;

class Game : public GameObject
{
	std::set<GameObject*> game_objects;	// http://www.cplusplus.com/reference/set/set/
	
	AvancezLib* system;

	ObjectPool<Rocket> rockets_pool;	// used to instantiate rockets
	ObjectPool<Alien> aliens_pool;
	ObjectPool<Bomb> bombs_pool;

	Player * player;
	AliensGrid * aliens_grid;

	Sprite * life_sprite;
	bool game_over;

	unsigned int score = 0;

public:

	virtual void Create(AvancezLib* system)
	{
		SDL_Log("Game::Create");

		this->system = system;

		player = new Player();
		PlayerBehaviourComponent * player_behaviour = new PlayerBehaviourComponent();
		player_behaviour->Create(system, player, &game_objects, &rockets_pool);
		RenderComponent * player_render = new RenderComponent();
		player_render->Create(system, player, &game_objects, "data/player.bmp");
		CollideComponent * player_bomb_collision = new CollideComponent();
		player_bomb_collision->Create(system, player, &game_objects, reinterpret_cast<ObjectPool<GameObject>*>(&bombs_pool));
		CollideComponent * player_alien_collision = new CollideComponent();
		player_alien_collision->Create(system, player, &game_objects, reinterpret_cast<ObjectPool<GameObject>*>(&aliens_pool));

		player->Create();
		player->AddComponent(player_behaviour);
		player->AddComponent(player_render);
		player->AddComponent(player_bomb_collision);
		player->AddComponent(player_alien_collision);
		player->AddReceiver(this);
		game_objects.insert(player);

		rockets_pool.Create(30);
		for (auto rocket = rockets_pool.pool.begin(); rocket != rockets_pool.pool.end(); rocket++)
		{
			RocketBehaviourComponent * behaviour = new RocketBehaviourComponent();
			behaviour->Create(system, *rocket, &game_objects);
			RenderComponent * render = new RenderComponent();
			render->Create(system, *rocket, &game_objects, "data/rocket.bmp");
			(*rocket)->Create();
			(*rocket)->AddComponent(behaviour);
			(*rocket)->AddComponent(render);
		}
		//-------------ALIENS----------------
		/*aliens_grid = new AliensGrid();
		AliensGridBehaviourComponent  * aliensgrid_behaviour = new AliensGridBehaviourComponent();
		aliensgrid_behaviour->Create(system, aliens_grid, &game_objects, &aliens_pool, &bombs_pool);
		aliens_grid->Create();
		aliens_grid->AddComponent(aliensgrid_behaviour);
		game_objects.insert(aliens_grid);


		aliens_pool.Create(55);
		for (auto alien = aliens_pool.pool.begin(); alien != aliens_pool.pool.end(); alien++)
		{
			AlienBehaviourComponent * alien_behaviour = new AlienBehaviourComponent();
			alien_behaviour->Create(system, *alien, &game_objects);
			RenderComponent * alien_render = new RenderComponent();
			alien_render->Create(system, *alien, &game_objects, "data/enemy_0.bmp");
			CollideComponent * alien_coll = new CollideComponent();
			alien_coll->Create(system, *alien, &game_objects, (ObjectPool<GameObject>*)&rockets_pool);
			(*alien)->Create();
			(*alien)->AddComponent(alien_behaviour);
			(*alien)->AddComponent(alien_render);
			(*alien)->AddComponent(alien_coll);
			(*alien)->AddReceiver(this);
		}*/
		//-------------ENEMY----------------
		aliens_grid = new AliensGrid();
		AliensGridBehaviourComponent* aliensgrid_behaviour = new AliensGridBehaviourComponent();
		aliensgrid_behaviour->Create(system, aliens_grid, &game_objects, &aliens_pool, &bombs_pool);
		aliens_grid->Create();
		aliens_grid->AddComponent(aliensgrid_behaviour);
		game_objects.insert(aliens_grid);


		aliens_pool.Create(55);
		for (auto alien = aliens_pool.pool.begin(); alien != aliens_pool.pool.end(); alien++)
		{
			AlienBehaviourComponent* alien_behaviour = new AlienBehaviourComponent();
			alien_behaviour->Create(system, *alien, &game_objects);
			RenderComponent* alien_render = new RenderComponent();
			alien_render->Create(system, *alien, &game_objects, "data/enemy_0.bmp");
			CollideComponent* alien_coll = new CollideComponent();
			alien_coll->Create(system, *alien, &game_objects, (ObjectPool<GameObject>*) & rockets_pool);
			(*alien)->Create();
			(*alien)->AddComponent(alien_behaviour);
			(*alien)->AddComponent(alien_render);
			(*alien)->AddComponent(alien_coll);
			(*alien)->AddReceiver(this);
		}
		//----------------------------------

		bombs_pool.Create(55);
		for (auto bomb = bombs_pool.pool.begin(); bomb != bombs_pool.pool.end(); bomb++)
		{
			BombBehaviourComponent * bomb_behaviour = new BombBehaviourComponent();
			bomb_behaviour->Create(system, *bomb, &game_objects);
			RenderComponent * bomb_render = new RenderComponent();
			bomb_render->Create(system, *bomb, &game_objects, "data/bomb.bmp");

			(*bomb)->Create();
			(*bomb)->AddComponent(bomb_behaviour);
			(*bomb)->AddComponent(bomb_render);
		}

		life_sprite = system->createSprite("data/player.bmp");
		score = 0;
	}

	virtual void Init()
	{
		player->Init();
		aliens_grid->Init();

		enabled = true;
		game_over = false;
	}

	virtual void Update(float dt)
	{
		if (IsGameOver())
			dt = 0.f;

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Update(dt);

		// check if there are still active aliens
		// if not, send a message to re-init the level
		bool are_aliens_still_there = false;
		for (auto alien = aliens_pool.pool.begin(); alien != aliens_pool.pool.end(); alien++)
			are_aliens_still_there |= (*alien)->enabled;
		if (!are_aliens_still_there)
		{
			// level win!
			game_speed += 0.4f;
			aliens_grid->Init();
		}
	}

	virtual void Draw()
	{
		char msg[1024];
		sprintf(msg, "%07d", Score());
		system->drawText(300, 32, msg);
		sprintf(msg, "bonus: %.1fX", game_speed);
		system->drawText(510, 32, msg);

		for (int i = 0; i < player->lives; i++)
			life_sprite->draw(i*36+20, 16);

		if (IsGameOver())
		{
			sprintf(msg, "*** G A M E  O V E R ***");
			system->drawText(250, 8, msg);
		}
	}

	virtual void Receive(Message m)
	{
		if (m == GAME_OVER)
			game_over = true;

		if (m == ALIEN_HIT)
			score += POINTS_PER_ALIEN * game_speed;
	}


	bool IsGameOver()
	{
		return game_over;
	}

	unsigned int Score()
	{
		return score;
	}

	virtual void Destroy()
	{
		SDL_Log("Game::Destroy");

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Destroy();

		life_sprite->destroy();
	
		rockets_pool.Destroy();
		aliens_pool.Destroy();
		bombs_pool.Destroy();

		delete aliens_grid;
		delete player;
	}
};