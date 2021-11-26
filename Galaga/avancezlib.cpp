#include "avancezlib.h"

//#include "SDL_ttf.h"


// Creates the main window. Returns true on success.
bool AvancezLib::init(int width, int height)
{
	SDL_Log("Initializing the system...\n");

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL failed the initialization: %s\n", SDL_GetError());
		return false;
	}

	//Create window
	window = SDL_CreateWindow("aVANCEZ", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	
	//Create renderer for window
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	TTF_Init();
	font = TTF_OpenFont("data/space_invaders.ttf", 12); //this opens a font style and sets a size
	if (font == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "font cannot be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}


	// initialize the keys
	key.fire = false;	key.left = false;	key.right = false;

	//Initialize renderer color
	//SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);

	//Clear screen
	SDL_RenderClear(renderer);

	SDL_Log("System up and running...\n");
	return true;
}


// Destroys the avancez library instance
void AvancezLib::destroy()
{
	SDL_Log("Shutting down the system\n");

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_CloseFont(font);

	TTF_Quit();
	SDL_Quit();
}


bool AvancezLib::update()
{
	bool go_on = true;
	SDL_Event event;


	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			go_on = false;

		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			case SDLK_q:
				go_on = false;
				break;
			case SDLK_SPACE:
				key.fire = true;
				break;
			case SDLK_LEFT:
				key.left = true;
				break;
			case SDLK_RIGHT:
				key.right = true;
				break;
			}
		}

		if (event.type == SDL_KEYUP)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_SPACE:
				key.fire = false;
				break;
			case SDLK_LEFT:
				key.left = false;
				break;
			case SDLK_RIGHT:
				key.right = false;
				break;
			}
		}

	}

	//Update screen
	SDL_RenderPresent(renderer);

	//Clear screen
	SDL_RenderClear(renderer);

	return go_on;
}


Sprite * AvancezLib::createSprite(const char * path)
{
	SDL_Surface* surf = SDL_LoadBMP(path);
	if (surf == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load image %s! SDL_image Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 255, 255, 255));
	SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0, 0, 0));

	//Create texture from surface pixels
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//Get rid of old loaded surface
	SDL_FreeSurface(surf);

	Sprite * sprite = new Sprite(renderer, texture);

	return sprite;
}

AnimatedSprite* AvancezLib::createAnimatedSprite(const char* path, int spriteAmount, int animationSpeed)
{
	SDL_Surface* surf = SDL_LoadBMP(path);
	if (surf == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load image %s! SDL_image Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 255, 255, 255));
	SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0, 0, 0));

	//Create texture from surface pixels
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//Get rid of old loaded surface
	SDL_FreeSurface(surf);

	AnimatedSprite* sprite = new AnimatedSprite(renderer, texture,spriteAmount,animationSpeed);

	return sprite;
}

void AvancezLib::drawLine(float startX, float startY, float endX, float endY) {

	SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(this->renderer, startX, startY, endX, endY);
	SDL_SetRenderDrawColor(this->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
}

void AvancezLib::drawText(int x, int y, const char * msg)
{
	SDL_Color black = { 255, 255, 255 };  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

	SDL_Surface* surf = TTF_RenderText_Solid(font, msg, black); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

	SDL_Texture* msg_texture = SDL_CreateTextureFromSurface(renderer, surf); //now you can convert it into a texture
	//SDL_SetTextureColorMod(msg_texture, 255, 0, 0);

	int w = 0;
	int h = 0;
	SDL_QueryTexture(msg_texture, NULL, NULL, &w, &h);
	SDL_Rect dst_rect = { x, y, w, h };

	SDL_RenderCopy(renderer, msg_texture, NULL, &dst_rect);

	SDL_DestroyTexture(msg_texture);
	SDL_FreeSurface(surf);
}

float AvancezLib::getElapsedTime()
{
	return SDL_GetTicks() / 1000.f;
}

void AvancezLib::getKeyStatus(KeyStatus & keys)
{
	keys.fire = key.fire;
	keys.left = key.left;
	keys.right = key.right;
}


Sprite::Sprite(SDL_Renderer * renderer, SDL_Texture * texture)
{
	this->renderer = renderer;
	this->texture = texture;
}


void Sprite::draw(int x, int y)
{
	SDL_Rect rect;

	rect.x = x;
	rect.y = y;

	SDL_QueryTexture(texture, NULL, NULL, &(rect.w), &(rect.h));

	//Render texture to screen
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}


void Sprite::destroy()
{
	SDL_DestroyTexture(texture);
}


AnimatedSprite::AnimatedSprite(SDL_Renderer* renderer, SDL_Texture* texture, int spriteAmount, int animationSpeed) {
	this->renderer = renderer;
	this->texture = texture;
	this->spriteAmount = spriteAmount;
	this->animationSpeed = animationSpeed;
	this->spriteNum = 0;
	activateAnimation = false;
	animationDone = false;
}

void AnimatedSprite::destroy()
{
	SDL_DestroyTexture(texture);
}

void AnimatedSprite::AnimateOnce(int x, int y)
{

	int ticks = SDL_GetTicks();
	int seconds = (ticks/100) / 1000;
	spriteNum = seconds % spriteAmount; //Which sprite to pick from sheet

	SDL_Rect srcrect = { spriteNum * 32, 0, 32, 32 };
	SDL_Rect dstrect = { 10, 10, 32, 32 };
	SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);
	if (spriteNum == spriteAmount) {
		activateAnimation = false;
		animationDone = true;
	}
	
}

void AnimatedSprite::resetAnimation()
{
	animationDone = false;
}
