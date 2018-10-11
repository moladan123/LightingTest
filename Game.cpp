#include "pch.h"
#include "Game.h"

// rectangle
const sf::Vector2f rectSize(sf::Vector2f(200, 50));
sf::Vector2f rectPosition(sf::Vector2f(0, 0)); // coordinates of the rectangle
sf::Vector2f rectVelocity(sf::Vector2f(0, 0)); // velocity of the rectangle
sf::RectangleShape rectangle(rectSize);

// lighting
sf::CircleShape lightSource(2.0f, 4);
int numExtraLights;
int extraLightDistance; // distance from the main light source
int offset;

// lines
sf::Shape* walls[100];

// game states
sf::Time ups;
sf::Event currentEvent;
Game::GameState Game::_gameState = Uninitialized;
sf::RenderWindow Game::_mainWindow;

const float rotationSpeed = 2.5f;
const float force = 0.9f;
const float degreesToRadians = 3.14159265358979323846f / 180.0f;
const float damping = 0.94f;

void Game::Start(void)
{
	if (_gameState != Uninitialized)
		return;

	_mainWindow.create(sf::VideoMode(1024, 768, 32), "Hello, World!");
	_mainWindow.setVerticalSyncEnabled(true);
	_mainWindow.setFramerateLimit(60);
	_gameState = Game::Playing;
	
	rectangle.setFillColor(sf::Color(0, 0, 100));
	rectangle.setOrigin(rectSize.x / 2, rectSize.y / 2); // change the centre of balance of the rectangle
	rectangle.setPosition(_mainWindow.getSize().x / 2, _mainWindow.getSize().x / 2);

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
	lightSource.setPosition(rectPosition);
	_mainWindow.draw(lightSource);
	for (float j = -5.0f; j <= 5.0f; j += 0.05f) {
		sf::Vertex line[] =
		{
			sf::Vertex(rectPosition),
			sf::Vertex(sf::Vector2f(rectPosition.x + 2000.0f * cosf((rectangle.getRotation() + j) * degreesToRadians),
									rectPosition.y + 2000.0f * sinf((rectangle.getRotation() + j) * degreesToRadians)))
		};

		_mainWindow.draw(line, 2, sf::Lines);
	}

	_mainWindow.display();
}

void Game::updateLogic() {

	// gravity
	rectVelocity.y -= 0.0f;
	
	// damping movement
	rectVelocity *= damping;

	rectPosition += rectVelocity;
	if (rectPosition.y < 0) {
		rectPosition.y = 0;
		rectVelocity.y = 0;
	}
}

void Game::getInput() {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		rectangle.rotate(rotationSpeed);
	} 
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		rectangle.rotate(360.0f - rotationSpeed);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		rectVelocity.x += force * cosf(rectangle.getRotation() * degreesToRadians);
		rectVelocity.y += force * sinf(rectangle.getRotation() * degreesToRadians);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		rectVelocity.x -= force * cosf(rectangle.getRotation() * degreesToRadians);
		rectVelocity.y -= force * sinf(rectangle.getRotation() * degreesToRadians);
	}
}
