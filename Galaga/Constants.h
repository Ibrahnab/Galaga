#pragma once

const unsigned int SCREEN_WIDTH = 640;
const unsigned int SCREEN_HEIGHT = 780;
const unsigned int SCREEN_MIDPOINT = SCREEN_WIDTH / 2;

const unsigned int	MAX_NUM_GAME_OBJECTS = 10000;
const unsigned int	NUM_LIVES = 2;

const unsigned int NUM_BOSS_GALAGA = 10;
const unsigned int NUM_BUTTERFLY = 20;


const unsigned int	MAX_NUM_ROCKETS = 32;
const unsigned int	MAX_NUM_BOMBS = 32;
const unsigned int	POINTS_PER_ALIEN = 100;
const unsigned int	POINTS_PER_BOSS = 200;
const unsigned int	POINTS_PER_BUTTERFLY = 100;


const float			FIRE_TIME_INTERVAL = .5f;
const float			BOMB_TIME_INTERVAL = 1.25f;
const float			BEAM_TIME_ACTIVE = 2.25f;
const float			PLAYER_SPEED = 160.0f;
const float			ROCKET_SPEED = 2*160.0f;
const float			ALIEN_SPEED = 6 * 40.0f;
const float			FLY_TO_POSITION_SPEED = 2 * ALIEN_SPEED;
const float			FORMATION_SPEED = 40.f;
const float			BOMB_SPEED = 3*120.0f;