#include "Pacman.h"
#include <time.h>
#include <iostream>
#include <sstream>


Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanFrameTime(250), _cMunchieFrameTime(500)
{
	srand(time(NULL));

	// munchie initialisation
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i] = new Enemy();
		_munchies[i]->frame = 0;
		_munchies[i]->currentFrameTime = rand() % 500 + 50;
		_munchies[i]->direction = 0;
		_munchies[i]->collected = false;
	}

	for (int i = 0; i < CHERRYCOUNT; i++)
	{
		_cherries[i] = new Enemy();
		_cherries[i]->frame = 0;
		_cherries[i]->currentFrameTime = rand() % 500 + 50;
		_cherries[i]->direction = 0;
		_cherries[i]->collected = false;
	}

	// pacman initialisation
	_pacman = new Player();
	_pacman->dead = false;

	// ghost initialisation
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_blueGhosts[i] = new MovingEnemy();
		_blueGhosts[i]->direction = 0;
		_blueGhosts[i]->speed = 0.2f;

		_purpleGhosts[i] = new MovingEnemy();
		_purpleGhosts[i]->direction = 0;
		_purpleGhosts[i]->speed = 0.35f;
	}

	_menu = new Menu();	
	_gameOver = new Menu();
	_endGame = new Menu();
	
	_paused = false;
	_pKeyDown = false;

	_pacman->direction = 0;
	_pacman->currentFrameTime = 0;
	_pacman->frame = 0;	
	_pacman->speedMultiplier = 1.0f;

	_pop = new SoundEffect();
	_background = new SoundEffect();
	_death = new SoundEffect();
	_cherry = new SoundEffect();
	
	
	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();	

	do
	{
		Audio::Play(_background);
	} while (Audio::Play(_background) == false);

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();	
}

Pacman::~Pacman()
{
	delete _pacman->texture;
	delete _pacman->sourceRect;
	delete _pacman->position;
	delete _pacman;

	delete _munchies[0] -> texture;

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		delete _munchies[i]->texture;
		delete _munchies[i]->invertedTexture;
		delete _munchies[i]->rect;
		delete _munchies[i]->sourceRect;
	}	

	for (int i = 0; i < CHERRYCOUNT; i++)
	{
		delete _cherries[i]->texture;
		delete _cherries[i]->invertedTexture;
		delete _cherries[i]->rect;
		delete _cherries[i]->sourceRect;
	}

	delete _cherries;
	delete _munchies;
	delete _blueGhosts;
	delete _purpleGhosts;
	delete _pop;
	delete _background;
	delete _death;
	delete _cherry;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/PacMan3.png", false);
	_pacman->position = new Vector2(350.0f, 350.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// load ghost
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_blueGhosts[i]->texture = new Texture2D();
		_blueGhosts[i]->texture->Load("Textures/BlueGhost.png", false);
		_blueGhosts[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_blueGhosts[i]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

		_purpleGhosts[i]->texture = new Texture2D();
		_purpleGhosts[i]->texture->Load("Textures/PurpleGhost.png", false);
		_purpleGhosts[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_purpleGhosts[i]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	}
		
	// Load Munchie
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i]->texture = new Texture2D();
		_munchies[i]->texture->Load("Textures/Munchie2.png", false);
		_munchies[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_munchies[i]->sourceRect = new Rect(0.0f, 0.0f, 16, 16);
	}	

	// load cherries
	for (int i = 0; i < CHERRYCOUNT; i++)
	{
		_cherries[i]->texture = new Texture2D();
		_cherries[i]->texture->Load("Textures/CherrySheet.png", false);
		_cherries[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_cherries[i]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	}

	// load sound
	_pop->Load("Sounds/pop.wav");
	_background->Load("Sounds/background.wav");
	_death->Load("Sounds/death.wav");
	_cherry->Load("Sounds/cherry.wav");
	
	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	// Set menu parameters
	_menu->background = new Texture2D();
	_menu->background->Load("Textures/Transparancy.png", false);
	_menu->rectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menu->stringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	_gameOver->background = new Texture2D();
	_gameOver->background->Load("Textures/Transparancy.png", false);
	_gameOver->rectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_gameOver->stringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	_endGame->background = new Texture2D();
	_endGame->background->Load("Textures/Transparancy.png", false);
	_endGame->rectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_endGame->stringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

	// gets the current state of the mouse
	Input::MouseState* mouseState = Input::Mouse::GetState();

	if (keyboardState->IsKeyDown(Input::Keys::P) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
	}

	if (keyboardState->IsKeyUp(Input::Keys::P))
	{
		_pKeyDown = false;
	}

	if (!_paused)
	{
		Input(elapsedTime, keyboardState, mouseState);
		UpdatePacman(elapsedTime);
		UpdateBlueGhost(_blueGhosts[0], elapsedTime);
		UpdatePurpleGhost(_purpleGhosts[0], elapsedTime);
		CheckGhostCollisions();
		CheckMunchieCollisions();
		CheckCherryCollisions();
		CheckViewportCollision(_pacman);

		for (int i = 0; i < MUNCHIECOUNT; i++)
		{
			UpdateMunchies(_munchies[i], elapsedTime);
		}

		for (int i = 0; i < CHERRYCOUNT; i++)
		{
			UpdateCherries(_cherries[i], elapsedTime);
		}
	}
	
	mouseState->LeftButton;

	if (mouseState->LeftButton == Input::ButtonState::PRESSED)
	{
		_cherries[1]->position->X = mouseState->X;
		_cherries[1]->position->Y = mouseState->Y;
	}	
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState)
{
	float _pacmanSpeed = _cPacmanSpeed * elapsedTime * _pacman->speedMultiplier;

	// Checks if D key is pressed
	if (state->IsKeyDown(Input::Keys::D))
	{
		_pacman->position->X += _pacmanSpeed; //Moves Pacman across X axis
		_pacman->direction = 0;
	}
	if (state->IsKeyDown(Input::Keys::A))
	{
		_pacman->position->X -= _pacmanSpeed;
		_pacman->direction = 2;
	}
	if (state->IsKeyDown(Input::Keys::S))
	{
		_pacman->position->Y += _pacmanSpeed;
		_pacman->direction = 1;
	}
	if (state->IsKeyDown(Input::Keys::W))
	{
		_pacman->position->Y -= _pacmanSpeed;
		_pacman->direction = 3;
	}

	// speed multiplier
	if(state->IsKeyDown(Input::Keys::LEFTSHIFT))
	{
		// apply multiplier
		_pacman->speedMultiplier = 2.0f;
	}
	else
	{
		// reset multipler
		_pacman->speedMultiplier = 1.0f;
	}
}

void Pacman::CheckViewportCollision(Player* _pacman)
{
	// Checks if Pacman is trying to disappear
	if (_pacman->position->X + _pacman->sourceRect->Width >= Graphics::GetViewportWidth()) // 1024 is game width // Pacman hit right wall - reset his position		
		_pacman->position->X = 1024 - _pacman->sourceRect->Width;
	else if (_pacman->position->X <= 0) // left wall 
		_pacman->position->X = -31 + _pacman->sourceRect->Width;

	if (_pacman->position->Y + _pacman->sourceRect->Height >= Graphics::GetViewportHeight()) // 	
		_pacman->position->Y = 767 - _pacman->sourceRect->Height;
	else if (_pacman->position->Y <= 0) // 
		_pacman->position->Y = -32 + _pacman->sourceRect->Height;
}

void Pacman::UpdatePacman(int elapsedTime)
{
	_pacman->currentFrameTime += elapsedTime;

	_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;
	_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frame;

	if (_pacman->currentFrameTime > _cPacmanFrameTime)
	{
		_pacman->frame++;

		if (_pacman->frame >= 2)
			_pacman->frame = 0;

		_pacman->currentFrameTime = 0; 
	}
	if (_pacman->dead)
	{
		_pacman->position->X = -100;
		_pacman->position->Y = -100;
	}
}

void Pacman::UpdateBlueGhost(MovingEnemy* ghost, int elapsedTime)
{
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_blueGhosts[i]->sourceRect->Y = _blueGhosts[i]->sourceRect->Height * _blueGhosts[i]->direction;
		_blueGhosts[i]->sourceRect->X = _blueGhosts[i]->sourceRect->Width * _blueGhosts[i]->frame;

		if (_blueGhosts[i]->direction == 0) // moves right
		{
			_blueGhosts[i]->position->X += _blueGhosts[i]->speed * elapsedTime;
		}
		else if (_blueGhosts[i]->direction == 1) // moves left
		{
			_blueGhosts[i]->position->X -= _blueGhosts[i]->speed * elapsedTime;
		}

		if (_blueGhosts[i]->position->X + _blueGhosts[i]->sourceRect->Width >= Graphics::GetViewportWidth()) // hits right edge
		{
			_blueGhosts[i]->direction = 1; // change direction
		}
		else if (_blueGhosts[i]->position->X <= 0) // hits left edge
		{
			_blueGhosts[i]->direction = 0; // change direction
		}
	}
}

void Pacman::UpdatePurpleGhost(MovingEnemy* ghost, int elapsedTime)
{
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_purpleGhosts[i]->sourceRect->Y = _purpleGhosts[i]->sourceRect->Height * _purpleGhosts[i]->direction;
		_purpleGhosts[i]->sourceRect->X = _purpleGhosts[i]->sourceRect->Width * _purpleGhosts[i]->frame;

		if (_purpleGhosts[i]->direction == 0) // moves right
		{
			_purpleGhosts[i]->position->X += _purpleGhosts[i]->speed * elapsedTime;
		}
		else if (_purpleGhosts[i]->direction == 1) // moves left
		{
			_purpleGhosts[i]->position->X -= _purpleGhosts[i]->speed * elapsedTime;
		}

		if (_purpleGhosts[i]->position->X + _purpleGhosts[i]->sourceRect->Width >= Graphics::GetViewportWidth()) // hits right edge
		{
			_purpleGhosts[i]->direction = 1; // change direction
		}
		else if (_purpleGhosts[i]->position->X <= 0) // hits left edge
		{
			_purpleGhosts[i]->direction = 0; // change direction
		}
	}
}

void Pacman::UpdateMunchies(Enemy* _munchie, int elapsedTime)
{
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i]->sourceRect->Y = _munchies[i]->sourceRect->Height * _munchies[i]->direction;
		_munchies[i]->sourceRect->X = _munchies[i]->sourceRect->Width * _munchies[i]->frame;

		_munchies[i]->currentFrameTime += elapsedTime;

		if (_munchies[i]->currentFrameTime > _cMunchieFrameTime)
		{
			_munchies[i]->frame++;

			if (_munchies[i]->frame >= 2)
				_munchies[i]->frame = 0;

			_munchies[i]->currentFrameTime = 0;
		}
	}
}

void Pacman::UpdateCherries(Enemy* _cherries, int elapsedTime)
{
	_cherries->sourceRect->Y = _cherries->sourceRect->Height * _cherries->direction;
	_cherries->sourceRect->X = _cherries->sourceRect->Width * _cherries->frame;

	for (int i = 0; i < CHERRYCOUNT; i++)
	{
		_cherries->currentFrameTime += elapsedTime;

		if (_cherries->currentFrameTime > _cMunchieFrameTime)
		{
			_cherries->frame++;

			if (_cherries->frame >= 2)
				_cherries->frame = 0;

			_cherries->currentFrameTime = 0;
		}
	}
}

void Pacman::CheckGhostCollisions()
{
	// local variable
	int i = 0;
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	for (i = 0; i < GHOSTCOUNT; i++)
	{
		// populate variable with ghost data
		bottom2 = _blueGhosts[i]->position->Y + _blueGhosts[i]->sourceRect->Height;
		left2 = _blueGhosts[i]->position->X;
		right2 = _blueGhosts[i]->position->X + _blueGhosts[i]->sourceRect->Width;
		top2 = _blueGhosts[i]->position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_pacman->dead = true;
			Audio::Play(_death);
			i = GHOSTCOUNT;
		}
	}
	for (i = 0; i < GHOSTCOUNT; i++)
	{
		// populate variable with ghost data
		bottom2 = _purpleGhosts[i]->position->Y + _purpleGhosts[i]->sourceRect->Height;
		left2 = _purpleGhosts[i]->position->X;
		right2 = _purpleGhosts[i]->position->X + _purpleGhosts[i]->sourceRect->Width;
		top2 = _purpleGhosts[i]->position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_pacman->dead = true;
			Audio::Play(_death);
			i = GHOSTCOUNT;
		}
	}
}

void Pacman::CheckMunchieCollisions()
{
	// local variable
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		bottom2 = _munchies[i]->position->Y + _munchies[i]->sourceRect->Height;
		left2 = _munchies[i]->position->X;
		right2 = _munchies[i]->position->X + _munchies[i]->sourceRect->Width;
		top2 = _munchies[i]->position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{			
			_munchies[i]->collected = true;	
			i = MUNCHIECOUNT;		
			Audio::Play(_pop);

			for (int i = 0; i < MUNCHIECOUNT; i++)
			{
				if (_munchies[i]->collected)
				{
					_munchies[i]->position->X = -200;
					_munchies[i]->position->Y = -200;
				}
			}
		}
	}	
}

void Pacman::CheckCherryCollisions()
{
	// local variable
	int i = 0;
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	for (i = 0; i < CHERRYCOUNT; i++)
	{
		bottom2 = _cherries[i]->position->Y + _cherries[i]->sourceRect->Height;
		left2 = _cherries[i]->position->X;
		right2 = _cherries[i]->position->X + _cherries[i]->sourceRect->Width;
		top2 = _cherries[i]->position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_cherries[i]->collected = true;
			i = CHERRYCOUNT;
			Audio::Play(_cherry);

			for (int i = 0; i < CHERRYCOUNT; i++)
			{
				if (_cherries[i]->collected)
				{
					_cherries[i]->position->X = -200;
					_cherries[i]->position->Y = -200;
				}
			}
		}
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacman->position->X << " Y: " << _pacman->position->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	
	// draws pacman
	if (!_pacman->dead) // if pacman is not dead
	{		
		SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect); 			
	}

	// draw ghosts
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		SpriteBatch::Draw(_blueGhosts[i]->texture, _blueGhosts[i]->position, _blueGhosts[i]->sourceRect);
		SpriteBatch::Draw(_purpleGhosts[i]->texture, _purpleGhosts[i]->position, _purpleGhosts[i]->sourceRect);
	}	

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		if (!_munchies[i]->collected)
		{
			if (_munchies[i]->frame == 0)
			{
				SpriteBatch::Draw(_munchies[i]->texture, _munchies[i]->position, _munchies[i]->sourceRect);
				_munchies[i]->direction = 0;
			}
			else
			{
				SpriteBatch::Draw(_munchies[i]->texture, _munchies[i]->position, _munchies[i]->sourceRect);
				_munchies[i]->direction = 1;
			}
		}
	}

	for (int i = 0; i < CHERRYCOUNT; i++)
	{		
		if (!_cherries[i]->collected)
		{
			if (0 <= _cherries[i]->frame <= 14)
			{
				SpriteBatch::Draw(_cherries[i]->texture, _cherries[i]->position, _cherries[i]->sourceRect);
				_cherries[i]->direction = 0;
			}
			else
			{
				SpriteBatch::Draw(_cherries[i]->texture, _cherries[i]->position, _cherries[i]->sourceRect);
				_cherries[i]->direction = 1;
			}
		}		
	}	

	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	if (_paused)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menu->background, _menu->rectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menu->stringPosition, Color::Red);
	}

	if (_pacman->dead)
	{
		std::stringstream gameOverStream;
		gameOverStream << "GAME OVER!";

		SpriteBatch::Draw(_gameOver->background, _gameOver->rectangle, nullptr);
		SpriteBatch::DrawString(gameOverStream.str().c_str(), _gameOver->stringPosition, Color::Red);

	}

	SpriteBatch::EndDraw(); // Ends Drawing
}