#pragma once
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"


class Game
{

public:
	static void Start();

private:
	static bool IsExiting();

	static void GameLoop();
	static void processEvents();

	static void render();
	static float rayCast(sf::Vector2f start, sf::Vector2f direction);
	static double len(sf::Vector2f v);
	static sf::Vector2f intersection(sf::Vector2f A, sf::Vector2f B, sf::Vector2f C, sf::Vector2f D);

	static void updateLogic();
	static void getInput();

	enum GameState {
		Uninitialized, ShowingSplash, Paused,
		ShowingMenu, Playing, Exiting
	};

	static GameState _gameState;
	static sf::RenderWindow _mainWindow;
};