#pragma once
#include <SDL.h>
#include "Renderer.h"
#include <SDL_image.h>
using namespace std;
class Enemy
{
private:
	SDL_Rect* m_render_rect;
	SDL_Texture* m_texture = NULL;
	SDL_Surface* m_surface;
	float xPosition;
	float yPosition;
	SDL_Point* center;
	int type;
	bool alive;
	void LoadAssets(Renderer &r);
	float directionAngleX;
	float directionAngleY;
	float directionAngle;
	//SDL_RendererFlip flipType = SDL_FLIP_VERTICAL;
	
public:
	Enemy();
	Enemy(int xPos,int yPos, int width, int height, Renderer &r, int t);
	~Enemy();
	unsigned int Update(SDL_Rect*, bool);
	void Movement(SDL_Rect*, bool);
	void CollisionResponse();
	bool CheckAlive();
	void Draw(Renderer &r);
};

