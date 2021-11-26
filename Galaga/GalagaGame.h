#pragma once

//bool change_direction = false;

class GalagaGame : public GameObject
{
	std::set<GameObject*> game_objects;	// http://www.cplusplus.com/reference/set/set/

	AvancezLib* system;

	ObjectPool<Rocket> rockets_pool;	// used to instantiate rockets
	ObjectPool<Enemy> enemy_pool;

	std::vector<BossGalaga*> bossVector;
	std::vector<Butterfly*> butterflyVector;

	std::vector<Enemy*> enemyVector;

	ObjectPool<Bomb> bombs_pool;

	Player* player;
	GameObject* secondShip;
	Formation* enemy_grid;
	FormationBehaviourComponent* formationBehaviourComponent;

	std::vector<std::vector<Vector2D>> Paths;

	Sprite* life_sprite;
	Sprite * levelSprite_1;
	Sprite* levelSprite_5;
	Sprite* levelSprite_10;
	Sprite* levelSprite_20;
	Sprite* levelSprite_30;
	Sprite* levelSprite_50;

	Sprite* redShip;
	Sprite* oneHealthBoss;
	Sprite* twoHealthBoss;
	Sprite* tractorBeam;

	std::vector<Sprite*> levelSprites;
	std::map<int, Sprite*> levelSpriteMap;
	std::map<std::string, Sprite*> spriteMap;

	bool game_over;

	bool doubleShipActive;

	Vector2D playerPosition;

	int amountReachedFormation;
	int * amountEnemies;


	unsigned int stage;
	unsigned int score;

public:

	virtual void Create(AvancezLib* system)
	{
		SDL_Log("Game::Create");

		CreatePaths();
		doubleShipActive = false;
		this->system = system;

		oneHealthBoss = system->createSprite("data/bossGalagaOneHP.bmp");
		twoHealthBoss = system->createSprite("data/bossGalaga.bmp");
		redShip = system->createSprite("data/redGalaga.bmp");
		tractorBeam = system->createSprite("data/tractorBeam.bmp");

		spriteMap["oneHealthBoss"] = oneHealthBoss;
		spriteMap["twoHealthBoss"] = twoHealthBoss;
		spriteMap["redShip"] = redShip;
		spriteMap["tractorBeam"] = tractorBeam;

		//----------EXTRA SHIP
		//secondShip = new GameObject(); //Update it with
		//----------
		

		player = new Player();
		PlayerBehaviourComponent* player_behaviour = new PlayerBehaviourComponent();
		player_behaviour->Create(system, player, &game_objects, &rockets_pool);
		RenderComponent* player_render = new RenderComponent();
		player_render->Create(system, player, &game_objects, "data/Galaga.bmp");
		CollideComponent* player_bomb_collision = new CollideComponent();
		player_bomb_collision->Create(system, player, &game_objects, reinterpret_cast<ObjectPool<GameObject>*>(&bombs_pool));
		CollideComponent* player_alien_collision = new CollideComponent();
		player_alien_collision->Create(system, player, &game_objects, reinterpret_cast<ObjectPool<GameObject>*>(&enemy_pool));

		player->Create();
		player->AddComponent(player_behaviour);
		player->AddComponent(player_render);
		player->AddComponent(player_bomb_collision);
		player->AddComponent(player_alien_collision);
		player->AddReceiver(this);
		game_objects.insert(player);

		this->playerPosition = { Vector2D(player->horizontalPosition,player->verticalPosition) };

		rockets_pool.Create(60);
		for (auto rocket = rockets_pool.pool.begin(); rocket != rockets_pool.pool.end(); rocket++)
		{
			RocketBehaviourComponent* behaviour = new RocketBehaviourComponent();
			behaviour->Create(system, *rocket, &game_objects);
			RenderComponent* render = new RenderComponent();
			render->Create(system, *rocket, &game_objects, "data/galagaBullet.bmp");
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
		enemy_grid = new Formation();
		FormationBehaviourComponent* formation_behavior = new FormationBehaviourComponent();
		formation_behavior->Create(system, enemy_grid, &game_objects, &enemy_pool, &bombs_pool, &Paths, player, &playerPosition, &doubleShipActive, &spriteMap,&stage);
		enemy_grid->Create();
		enemy_grid->AddComponent(formation_behavior);
		game_objects.insert(enemy_grid);


	

		for (int i = 0; i < NUM_BOSS_GALAGA; i++) {
			BossGalaga* bg = new BossGalaga();


			BossGalagaBehaviorComponent* bg_behavior = new BossGalagaBehaviorComponent();
			bg_behavior->Create(system, bg, &game_objects);
			RenderComponent* bg_render = new RenderComponent();
			bg_render->Create(system, bg, &game_objects, "data/bossGalaga.bmp");
			bg_render->makeDeathAnimation("data/explosion.bmp", 5, 2);
			CollideComponent* bg_coll = new CollideComponent();
			bg_coll->Create(system, bg, &game_objects, (ObjectPool<GameObject>*) & rockets_pool);
			bg->setNameId("BossGalaga");
			bg->Create();
			bg->AddComponent(bg_behavior);
			bg->AddComponent(bg_render);
			bg->AddComponent(bg_coll);
			bg->AddReceiver(enemy_grid);
			bg->AddReceiver(this);
			player->AddReceiver(bg);
			bg->AddReceiver(player);

			//bossVector.push_back(bg);'
			enemyVector.push_back(reinterpret_cast<Enemy*>(bg));

		}
		for (int i = 0; i < NUM_BUTTERFLY; i++) {
			Butterfly* butterfly = new Butterfly();

			ButterflyBehaviorComponent* butterfly_behavior = new ButterflyBehaviorComponent();
			butterfly_behavior->Create(system, butterfly, &game_objects);
			RenderComponent* butterfly_render = new RenderComponent();
			butterfly_render->Create(system, butterfly, &game_objects, "data/butterflyB.bmp");
			butterfly_render->makeDeathAnimation("data/explosion.bmp", 5, 2);
			CollideComponent* butterfly_coll = new CollideComponent();
			butterfly->setNameId("Butterfly");
			butterfly_coll->Create(system, butterfly, &game_objects, (ObjectPool<GameObject>*) & rockets_pool);
			butterfly->Create();
			butterfly->AddComponent(butterfly_behavior);
			butterfly->AddComponent(butterfly_render);
			butterfly->AddComponent(butterfly_coll);
			butterfly->AddReceiver(enemy_grid);
			butterfly->AddReceiver(this);

			//butterflyVector.push_back(butterfly);
			enemyVector.push_back(reinterpret_cast<Enemy*>(butterfly));
			
		}

		//enemyVector.size();


		enemy_pool.Create(NUM_BOSS_GALAGA + NUM_BUTTERFLY);
		int i=0;
		for (auto enemy = enemy_pool.pool.begin(); enemy != enemy_pool.pool.end(); enemy++)
		{
			
			(*enemy) = (enemyVector[i]);
			//(*enemy)->enabled = true;
			i++;
		}



		//----------------------------------

		bombs_pool.Create(55);
		for (auto bomb = bombs_pool.pool.begin(); bomb != bombs_pool.pool.end(); bomb++)
		{
			BombBehaviourComponent* bomb_behaviour = new BombBehaviourComponent();
			bomb_behaviour->Create(system, *bomb, &game_objects);
			RenderComponent* bomb_render = new RenderComponent();
			bomb_render->Create(system, *bomb, &game_objects, "data/enemyBullet.bmp");

			(*bomb)->Create();
			(*bomb)->AddComponent(bomb_behaviour);
			(*bomb)->AddComponent(bomb_render);
		}

		//----Load sprites

		score = 0;
		stage = 3;

		life_sprite = system->createSprite("data/Galaga.bmp");
	

		levelSprite_1 = system->createSprite("data/1LevelIcon.bmp");
		levelSprite_5 = system->createSprite("data/5LevelIcon.bmp");
		levelSprite_10 = system->createSprite("data/10LevelIcon.bmp");
		levelSprite_20 = system->createSprite("data/20LevelIcon.bmp");
		levelSprite_30 = system->createSprite("data/30LevelIcon.bmp");
		levelSprite_50 = system->createSprite("data/50LevelIcon.bmp");
		
		levelSpriteMap[1] = levelSprite_1;
		levelSpriteMap[5] = levelSprite_5;
		levelSpriteMap[10] = levelSprite_10;
		levelSpriteMap[20] = levelSprite_20;
		levelSpriteMap[30] = levelSprite_30;
		levelSpriteMap[50] = levelSprite_50;
		sortStageSprites();
		
	}

	virtual void Init()
	{
		player->Init();
		enemy_grid->Init();
		amountReachedFormation = 0;

		formationBehaviourComponent = enemy_grid->GetComponent<FormationBehaviourComponent*>();
		amountEnemies = formationBehaviourComponent->getAmountEnemiesPtr();



		enabled = true;
		game_over = false;
	}

	virtual void Update(float dt)
	{
		UpdatePlayerPositionToEnemies();
		if (IsGameOver())
			dt = 0.f;

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Update(dt);

		// check if there are still active aliens
		// if not, send a message to re-init the level
		bool are_aliens_still_there = false;
		for (auto alien = enemy_pool.pool.begin(); alien != enemy_pool.pool.end(); alien++)
			are_aliens_still_there |= (*alien)->enabled;
		if (!are_aliens_still_there)
		{
			// level win!
			game_speed += 0.f;
			stage++;
			enemy_grid->Init();
			sortStageSprites();
		}
		drawStageSprites();

		//----------Delete this later
		AvancezLib::KeyStatus keys;
		system->getKeyStatus(keys);
		if (keys.fire) {
			for (auto alien = enemy_pool.pool.begin(); alien != enemy_pool.pool.end(); alien++)
				(*alien)->enabled = false;
		}

	}

	void drawStageSprites() {

		for (int i = 0; i < levelSprites.size(); i++) {
			if (levelSprites[i] != nullptr) {
				levelSprites[i]->draw(SCREEN_WIDTH - i * 27 - 50, 32);
			}
		}
	}

	void sortStageSprites() {

		levelSprites.clear();
		int stageRef = stage;
		std::vector<int> cmpValueList = { 50,30,20,10,5,1 };
		bool found = false;

		for (int i = 0; i < 10; i++) { //maximum of 10 level sprites can be displayed

			for(int j = 0; j < cmpValueList.size(); j++){

				int currentCmpValue = cmpValueList[j];

				if (stageRef - currentCmpValue >= 0) {
					levelSprites.push_back(levelSpriteMap[currentCmpValue]);
					stageRef -= currentCmpValue;
					found = true;
					break;
				}
			}
			if (!found) {
				levelSprites.push_back(nullptr); //pushing a nullptr means draw nothing
			}
			found = true;
		}

		//What the above code does in an ugly way

		/*for (int i = 0; i < 10; i++) {
			if (stageRef - 50 > 0) {
				levelSprites.push_back(levelSprite_50);
				stageRef -= 50;
			}
			else if(stageRef - 30 >= 0) {
				levelSprites.push_back(levelSprite_30);
				stageRef -= 30;
			}
			else if(stageRef - 20 >= 0) {
				levelSprites.push_back(levelSprite_20);
				stageRef -= 20;
			}
			else if(stageRef - 10 >= 0) {
				levelSprites.push_back(levelSprite_10);
				stageRef -= 10;
			}
			else if(stageRef - 5 >= 0) {
				levelSprites.push_back(levelSprite_5);
				stageRef -= 5;
			}
			else if(stageRef - 1 >= 0){
				levelSprites.push_back(levelSprite_1);
				stageRef -= 1;
			}
			else {
				levelSprites.push_back(nullptr);
			}
		}*/

	}

	virtual void Draw()
	{
		char msg[1024];
		sprintf(msg, "%07d", Score());
		system->drawText(300, 32, msg);

		sprintf(msg, "HIGH SCORE");
		system->drawText(290, 10, msg);

		sprintf(msg, "Stage: %d", stage);
		system->drawText(SCREEN_WIDTH - 150, 8, msg);


		for (int i = 0; i < player->lives; i++)
			life_sprite->draw(i * 36 + 20, 16);

		if (IsGameOver())
		{
			sprintf(msg, "*** G A M E  O V E R ***");
			system->drawText(250, 400, msg);
		}
	}

	virtual void Receive(Message m)
	{
		if (m == GAME_OVER)
			game_over = true;

		if (m == ALIEN_HIT)
			score += POINTS_PER_ALIEN * game_speed;

		if (m == FORMATION_REACHED)
		{
	
			SDL_Log("enemy reached formation");
			amountReachedFormation++;
			if (amountReachedFormation == (*amountEnemies)) {
				SDL_Log("EVERYONE IS HERE!");
				amountReachedFormation = 0; //Reset it for the next game
				formationBehaviourComponent->lockFormation();
			}
		}

		if (m == PLAYER_CAPTURED) {
			player->verticalPosition = SCREEN_HEIGHT + 200; //Hide the player until enemy returns to the formation
		}
		if (m == RETURNED_TO_FORMATION) {
			player->verticalPosition = SCREEN_HEIGHT - 32;
		}
		if (m == UNLOCK_SECOND_SHIP) {
			doubleShipActive = true;
		}
		if (m == BG_HIT) {
			score += POINTS_PER_BOSS * game_speed;
		}
		if (m == BF_HIT) {
			score += POINTS_PER_BUTTERFLY * game_speed;
		}

	}

	void UpdatePlayerPositionToEnemies() {
		playerPosition.x = player->horizontalPosition;
		playerPosition.y = player->verticalPosition;
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
		enemy_pool.Destroy();
		bombs_pool.Destroy();

		delete enemy_grid;
		delete player;
	}


	void CreatePaths() {

		int currentPath = 0; //Defines the path that is going to be followed

		//Start,C1,C2,End

		BezierPath* flyInPath = new BezierPath(); //This makes one collection of bezierpaths, and are put together to 0
		//path->AddCurve({ Vector2D(500.f,10.f),Vector2D(500.f,0.f) ,Vector2D(500.f,310.f) ,Vector2D(500.f,300.f) },1);
		flyInPath->AddCurve({ Vector2D(SCREEN_MIDPOINT + 50.f,-10.f),Vector2D(SCREEN_MIDPOINT + 50.f,-20.f) ,Vector2D(SCREEN_MIDPOINT + 50.f,30.f) ,Vector2D(SCREEN_MIDPOINT + 50.f,20.f) }, 1);
		flyInPath->AddCurve({ Vector2D(SCREEN_MIDPOINT + 50.f,20.f),Vector2D(SCREEN_MIDPOINT + 50.f,100.f) ,Vector2D(75.f,325.f) ,Vector2D(75,425.f) }, 25);
		flyInPath->AddCurve({ Vector2D(75.f,425.f),Vector2D(75.f,650.f) ,Vector2D(325.f,650.f) ,Vector2D(325,425.f) }, 25);

		Paths.push_back(std::vector<Vector2D>());
		flyInPath->Sample(&Paths[currentPath]);
		delete flyInPath;


		currentPath = 1; //Tractor beam path
		BezierPath* divePath = new BezierPath();
		divePath->AddCurve({ Vector2D(435.f,286.f),Vector2D(435.f,390.f) ,Vector2D(386.f,294.f),Vector2D(386.f,419.f) }, 25);
		divePath->AddCurve({ Vector2D(386.f,419.f),Vector2D(386.f,515.f) ,Vector2D(435,433.f) ,Vector2D(435.f,544.f) }, 25);
		Paths.push_back(std::vector<Vector2D>());
		divePath->Sample(&Paths[currentPath]);
		delete divePath;

		currentPath = 2; //Dive path 1
		BezierPath* divePath1 = new BezierPath();
		divePath1->AddCurve({ Vector2D(490.f,348.f),Vector2D(540.f,257.f) ,Vector2D(174.f,284.f),Vector2D(363.f,500.f) }, 25);
		divePath1->AddCurve({ Vector2D(374.f,515.f), Vector2D(505.f,757.f) ,Vector2D(637,444.f) ,Vector2D(489.f,549.f) }, 25);

		Paths.push_back(std::vector<Vector2D>());
		divePath1->Sample(&Paths[currentPath]);
		delete divePath1;

		currentPath = 3; //Dive path 2
		BezierPath* divePath2 = new BezierPath();
		divePath2->AddCurve({ Vector2D(113.f,261.f),Vector2D(72.f,445.f) ,Vector2D(376.f,590.f),Vector2D(259.f,400.f) }, 25);
		divePath2->AddCurve({ Vector2D(260.f,400.f),Vector2D(175.f,165.f) ,Vector2D(25.f,705.f),Vector2D(270.f,560.f) }, 25);
		Paths.push_back(std::vector<Vector2D>());
		divePath2->Sample(&Paths[currentPath]);
		delete divePath2;

		//---------Challenge stage paths

		currentPath = 4; //Dive path 2
		BezierPath* csPath1 = new BezierPath();
		csPath1->AddCurve({ Vector2D(0.f,500.f),Vector2D(365.f,500.f) ,Vector2D(44.f,317.f),Vector2D(346.f,316.f) }, 25);
		csPath1->AddCurve({ Vector2D(347.f,313.f),Vector2D(640.f,336.f) ,Vector2D(62.f,577.f),Vector2D(317.f,240.f) }, 25);
		csPath1->AddCurve({ Vector2D(318.f,236.f),Vector2D(480.f,36.f) ,Vector2D(450.f,542.f),Vector2D(-100.f,160.f) }, 25);
		Paths.push_back(std::vector<Vector2D>());
		csPath1->Sample(&Paths[currentPath]);
		delete csPath1;

	}
};