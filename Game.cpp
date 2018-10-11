#include "pch.h"
#include "Game.h"


sf::Vector2f rectPosition(sf::Vector2f(300, 100)); // coordinates of the rectangle
sf::Vector2f rectVelocity(sf::Vector2f(0, 0)); // coordinates of the rectangle
sf::RectangleShape rectangle(rectPosition);


sf::Time ups;
sf::Event currentEvent;
Game::GameState Game::_gameState = Uninitialized;
sf::RenderWindow Game::_mainWindow;




void Game::Start(void)
{
	if (_gameState != Uninitialized)
		return;

	_mainWindow.create(sf::VideoMode(1024, 768, 32), "Hello, World!");
	_mainWindow.setVerticalSyncEnabled(true);
	_mainWindow.setFramerateLimit(60);
	_gameState = Game::Playing;
	
	rectangle.setFillColor(sf::Color(0, 0, 100));
	rectangle.setOrigin(150, 50); // change the centre of balance of the rectangle
	rectangle.setPosition(500, 500);

	sf::View view = _mainWindow.getDefaultView();
	view.setSize(1024, -768);
	_mainWindow.setView(view);

	while (!IsExiting())
	{
		GameLoop();
	}

	_mainWindow.close();
}

bool Game::IsExiting()
{
	if (_gameState == Game::Exiting)
		return true;
	else
		return false;
}

void Game::GameLoop()
{
	sf::Clock clock;
	sf::Time accumulator = sf::Time::Zero;
	ups = sf::seconds(1.f / 60.f);
	
	// the game loop, nothing here should be changed
	while(_gameState != Game::Exiting){
		_mainWindow.pollEvent(currentEvent);
		if (currentEvent.type == sf::Event::Closed)
		{
			_gameState = Game::Exiting;
			break;
		}
		processEvents();


		while (accumulator > ups)
		{
			accumulator -= ups;
			updateLogic();
			getInput();
			//AI();
			//Physics();
		}
		render();
		accumulator = clock.restart();

	}
	
}

void Game::processEvents() {
	while (_mainWindow.pollEvent(currentEvent)) {
		if (currentEvent.type == sf::Event::Closed) {
			_gameState = Exiting;
		}
	}
}

void Game::render() {
	_mainWindow.clear();

	// Draw here
	rectangle.setPosition(rectPosition);
	_mainWindow.draw(rectangle);
	_mainWindow.display();
}

void Game::updateLogic() {

	rectVelocity.y -= 1.0f;
	
	rectPosition += rectVelocity;
	if (rectPosition.y < 0) {
		rectPosition.y = 0;
		rectVelocity.y = 0;
	}
}

void Game::getInput() {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		rectangle.rotate(1.0f);
	} 
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		rectangle.rotate(359.0f);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		rectVelocity.y += 1.75f;
	}
}
