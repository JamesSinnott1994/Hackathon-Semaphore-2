#include "stdafx.h"
#include "Player.h"
#include <cmath>
#pragma comment(lib, "SDL2_image.lib")

Player::Player(SDL_Point pos, int width, int height, Renderer& r, int t) :type(t)
{
	m_render_rect = new SDL_Rect{ pos.x, pos.y, width, height };

	rect.x = pos.x;
	rect.y = pos.y;
	rect.w = width;
	rect.h = height;

	LoadAssets(r);

	directionAngle = 0;

	differenceX = 0;
	differenceY = 0;
}

Player::~Player()
{
}

void Player::LoadAssets(Renderer& r)
{
	std::string path = "cowboy.png";
	m_surface = IMG_Load(path.c_str());

	if (m_surface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		m_texture = SDL_CreateTextureFromSurface(r.getRender(), m_surface);
		if (m_texture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		SDL_FreeSurface(m_surface);
	}
}

void Player::Update(Renderer &r, float delta)
{
	// Mouse position
	float mouseX = InputManager::GetInstance()->GetMousePos().x;
	float mouseY = InputManager::GetInstance()->GetMousePos().y;

	differenceX = mouseX - rect.x;
	differenceY = mouseY - rect.y;

	// Face sprite in direction of cursor
	directionAngle = atan2(differenceY, differenceX) * 180 / 3.141;


	// Update bullets
	if (bullets.size() > 0)
	{
		// Iterate through list of bullets
		for (bulletIterator = bullets.begin(); bulletIterator != bullets.end(); ++bulletIterator)
		{
			(*bulletIterator)->Update(delta, directionAngle);

			// Remove bullet if out of bounds
			if ((*bulletIterator)->offScreen())
			{
				bullets.erase(bulletIterator);
				break;
			}
		}
	}

	if (InputManager::GetInstance()->IsKeyDown(SDLK_SPACE) || InputManager::GetInstance()->IsMouseButtonDown(SDL_BUTTON_LEFT))
	{
		Shoot(r);
	}
	if (InputManager::GetInstance()->IsKeyDown(SDLK_w))
	{

	}
	if (InputManager::GetInstance()->IsKeyDown(SDLK_s))
	{

	}
	if (InputManager::GetInstance()->IsKeyDown(SDLK_a))
	{

	}
	if (InputManager::GetInstance()->IsKeyDown(SDLK_d))
	{

	}
}// End update

void Player::Shoot(Renderer &r)
{
	// Bullet initial position
	SDL_Point bulletPoint;
	bulletPoint.x = rect.x;
	bulletPoint.y = rect.y;

	int x = (int)bulletPoint.x;
	int y = (int)bulletPoint.y;

	Bullet* bullet = new Bullet(x, y, 12, 12, r, directionAngle);

	bullets.push_back(bullet);
}

void Player::Draw(Renderer &r)
{
	SDL_RenderCopyEx(r.getRender(), m_texture, NULL, m_render_rect, directionAngle, NULL, SDL_FLIP_NONE);

	// Draw bullets
	if (bullets.size() > 0)
	{
		for each(Bullet* bullet in bullets)
		{
			bullet->Draw(r);
		}
	}
}

SDL_Rect Player::GetRect()
{
	return rect;
}