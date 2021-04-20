#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#define MUNCHIECOUNT 150
#define GHOSTCOUNT 6
#define CHERRYCOUNT 2
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Structure Definition
struct Player
{
	float speedMultiplier;
	int currentFrameTime;
	int direction;
	int frame;
	bool dead;
	Rect* sourceRect;
	Texture2D* texture;
	Vector2* position;
};

struct Enemy
{
	int frame;
	int direction;
	int currentFrameTime;
	int collected;
	bool _allCollected;
	Rect* sourceRect;
	Rect* rect;
	Texture2D* texture;
	Texture2D* invertedTexture;
	Vector2* position;
};

struct Menu
{
	Texture2D* background;
	Rect* rectangle;
	Vector2* stringPosition;
};

struct MovingEnemy
{
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
	int direction;
	float speed;
	int frame;
};

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:

	Player* _pacman;

	Enemy* _munchies[MUNCHIECOUNT];

	Enemy* _cherries[CHERRYCOUNT];

	MovingEnemy* _blueGhosts[GHOSTCOUNT];

	MovingEnemy* _purpleGhosts[GHOSTCOUNT];

	Menu* _menu;

	Menu* _gameOver;

	Menu* _endGame;

	SoundEffect* _pop;

	SoundEffect* _background;

	SoundEffect* _death;

	SoundEffect* _cherry;

	// Position for String
	Vector2* _stringPosition;

	// Constant data for Game Variables
	const float _cPacmanSpeed;

	bool _paused;

	bool _pKeyDown;

	const int _cPacmanFrameTime;

	const int _cMunchieFrameTime;

	// Input methods
	void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);

	// Check Methods
	void CheckViewportCollision(Player*);
	void CheckGhostCollisions();
	void CheckMunchieCollisions();
	void CheckCherryCollisions();

	// Update Methods
	void UpdatePacman(int elapsedTime);
	void UpdateMunchies(Enemy*, int elapsedTime);
	void UpdateBlueGhost(MovingEnemy*, int elapsedTime);
	void UpdatePurpleGhost(MovingEnemy*, int elapsedTime);
	void UpdateCherries(Enemy*, int elapsedTime);
	

public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};