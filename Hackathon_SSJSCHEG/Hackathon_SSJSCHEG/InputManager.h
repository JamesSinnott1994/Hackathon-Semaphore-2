#ifndef INPUTMANAGER 
#define INPUTMANAGER

class InputManager
{
public:
	//Functions
	static InputManager* GetInstance();
	~InputManager()
	{
		instanceFlag = false;
	}

	void UpdateState();
	void UpdatePolledEvents(SDL_Event e);
	bool IsKeyDown(SDL_Keycode key);
	bool IsKeyHeld(SDL_Keycode key);
	bool IsKeyReleased(SDL_Keycode key);

	bool IsMouseButtonDown(int mouseButtonIndex);
	bool IsMouseButtonHeld(int mouseButtonIndex);
	bool IsMouseButtonReleased(int mouseButtonIndex);
	SDL_Point GetMousePos();

	//Variables

private:
	//Functions
	InputManager()
	{
		//private constructor
		downKeys = std::list<SDL_Keycode>();
		heldKeys = std::list<SDL_Keycode>();
		releasedKeys = std::list<SDL_Keycode>();

		downClicks = std::list<int>();
		heldClicks = std::list<int>();
		releasedClicks = std::list<int>();
	}

	//Variables
	std::list<SDL_Keycode> downKeys;
	std::list<SDL_Keycode> heldKeys;
	std::list<SDL_Keycode> releasedKeys;

	std::list<int> downClicks;
	std::list<int> heldClicks;
	std::list<int> releasedClicks;
	SDL_Point mousePos;

	static bool instanceFlag;
	static InputManager *instance;
};

#endif
