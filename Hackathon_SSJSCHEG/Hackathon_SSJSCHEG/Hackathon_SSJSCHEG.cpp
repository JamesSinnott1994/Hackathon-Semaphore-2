#pragma comment(lib, "SDL2_image.lib")

#pragma region IncludeHeaders
#include "stdafx.h"
#include <SDL.h>
#include "Enemy.h"
#include "AudioManager.h"
#include "Menu.h"
#include "Level.h"
#include "Player.h"
#include "Timer.h"

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>  
#pragma endregion


#include "SDL_thread.h"
#include "SDL_timer.h"

#include <thread>
#include <chrono>
#include <mutex>

//Screen dimension constants
const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 600;

const int MENU = 0, PLAY = 1;
int gameState;

SDL_Renderer* gRenderer = NULL;

#pragma region FPS Timer
//The application timer
Timer timer;

//The frames per second timer
Timer fpsTimer;

//Start counting frames per second
int countedFrames = 0;

//In memory text stream
std::stringstream timeText;

float avgFPS = 0;
float highestAvgFPS = 0;

int time2 = 0;
int timer2 = 1000;

void calculateFPS();
#pragma endregion

Enemy en;

// THREADS \\

//Data access semaphore
SDL_sem* dataLock = NULL;

// Run thread function
static int run(void* data);

// Enables/Disables threading
bool threadEnabled = true;

// THREADS \\

int wmain()
{
	#pragma region OtherStuff

		// Hide cursor
		SDL_ShowCursor(0);

		gameState = PLAY;

		//The window we'll be rendering to
		SDL_Window* window = NULL;

		//The surface contained by the window
		SDL_Surface* screenSurface = NULL;

		//SDL
		Renderer r = Renderer(SCREEN_WIDTH, SCREEN_HEIGHT);

		#pragma region Audio
		/*AudioManager::GetInstance()->Init();
		AudioManager::GetInstance()->LoadMedia();
		AudioManager::GetInstance()->PlaySoundEffect(1);*/
		#pragma endregion

		#pragma region Enemy
		// Enemy
		SDL_Point ePos;
		ePos.x = 100;
		ePos.y = 100;
		en = Enemy(200, 200, 62 / 3, 77 / 3, r, 0);
		Level lvl = Level(r);
		#pragma endregion


		#pragma region MENU
		// MENU
		SDL_Point bPos;
		bPos.x = 0;
		bPos.y = 0;
		Menu background = Menu(bPos, SCREEN_WIDTH, SCREEN_HEIGHT, r, 0, "background");

		SDL_Point pPos;
		pPos.x = SCREEN_WIDTH / 3;
		pPos.y = SCREEN_HEIGHT / 4;
		Menu playBtn = Menu(pPos, 263, 44, r, 0, "playBtn");

		SDL_Point e2Pos;
		e2Pos.x = (SCREEN_WIDTH / 2) - 50;
		e2Pos.y = (SCREEN_HEIGHT / 2) - 20;
		Menu exitBtn = Menu(e2Pos, 111, 45, r, 0, "exitBtn");
		#pragma endregion

		#pragma region Player
		// Player
		SDL_Point playerPos;
		playerPos.x = SCREEN_WIDTH / 2 - 25;
		playerPos.y = SCREEN_HEIGHT / 2 - 10;
		Player player = Player(playerPos, 44, 32, r, 0);
		#pragma endregion

	#pragma endregion

	// Creates our SDL_Semaphore
	// The initial value controls how many times code can pass through a semaphore before it locks.
	// We only want 1 thread accessing the data buffer at once so the mutex starts with a value of one.
	dataLock = SDL_CreateSemaphore(1);

	// Launch our thread
	SDL_Thread* aiThread = NULL;
	if (threadEnabled)
	{
		aiThread = SDL_CreateThread(run, "aiThread", &player.GetRect());
	}

	bool quit = false;
	float oldTime = SDL_GetTicks();
	float delta = 0.f;
	float newTime = 0.f;
	SDL_Event e;
	fpsTimer.start();

	#pragma region MainThread
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			InputManager::GetInstance()->UpdatePolledEvents(e);
		}

		if (e.button.button == SDL_SCANCODE_ESCAPE)
			quit = true;

		r.Begin();

		switch (gameState)
		{

		#pragma region MENU
				case MENU:

					//detect button click
					if (e.type == SDL_MOUSEBUTTONDOWN)
					{

						//If the left mouse button was pressed
						if (e.button.button == SDL_BUTTON_LEFT)
						{
							//Get the mouse offsets
							int mouse_x = e.button.x;
							int mouse_y = e.button.y;

							if (playBtn.IsClicked(mouse_x, mouse_y))
							{
								gameState = PLAY;
							}
							else if (exitBtn.IsClicked(mouse_x, mouse_y))
							{
								quit = true;
							}
						}
					}// End detect button click  

					background.Draw(r);
					playBtn.Draw(r);
					exitBtn.Draw(r);
					break;
		#pragma endregion

		#pragma region PLAY
				case PLAY:
					newTime = SDL_GetTicks();
					delta = newTime - oldTime;
					oldTime = newTime;

					if (!threadEnabled)
					{
						en.Update(&player.GetRect(), threadEnabled);
					}
					lvl.Draw(r);
					en.Draw(r);
					player.Update(r, delta);
					player.Draw(r);

					//// Calculate Frames Per Second
					//calculateFPS();

					break;

				}
		#pragma endregion

		InputManager::GetInstance()->UpdateState();

		r.End();
	}
	#pragma endregion


	// Here the main thread runs while the threads to their work.
	//If the main loop end before the threads finish working, 
	// we wait on them to finish with SDL_WaitThread.
	SDL_WaitThread(aiThread, NULL);

	return EXIT_SUCCESS;
}

void calculateFPS()
{
	time2++;

	//Calculate and correct fps
	avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
	if (avgFPS > 2000000)
	{
		avgFPS = 0;
	}
	//cout << avgFPS << endl;

	if (avgFPS > highestAvgFPS)
	{
		highestAvgFPS = avgFPS;
	}

	if (time2 > timer2)
	{
		cout << highestAvgFPS << endl;
		time2 = 0;
	}

	++countedFrames;
}

// Thread enemy AI
static int run(void* data)
{
	// Get player position from data
	SDL_Rect* playerRect = (SDL_Rect*)data;

	unsigned int result = 0;

	while (true)// Update
	{
		//Lock
		// The reason we need to lock data is because we do not want two threads reading or writing our shared data at the same time.
		SDL_SemWait(dataLock);// SDL_SemWait decrements the semaphore count and since the initial value is one, it will lock.

		/* CRITICAL SECTION */

		result = en.Update(playerRect, threadEnabled);

		/* CRITICAL SECTION */

		//Unlock
		// After the critical section executes, we call SDL_SemPost to increment the semaphore and unlock it.
		SDL_SemPost(dataLock);
	}
	return result;
}

