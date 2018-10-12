#include "pch.h"
#include "Game.h"
#include <algorithm>
#include <cmath>

#define MAX_LINES 100

// rectangle
const sf::Vector2f rectSize(sf::Vector2f(200, 50));
sf::Vector2f rectPosition(sf::Vector2f(0, 0)); // coordinates of the rectangle
sf::Vector2f rectVelocity(sf::Vector2f(0, 0)); // velocity of the rectangle
sf::RectangleShape rectangle(rectSize);

// lighting
sf::CircleShape lightSource(2.0f, 25);
int numExtraLights = 16;
int extraLightDistance = 10; // distance from the main light source
int offset;

// lines
sf::Vertex walls[MAX_LINES][2];

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

	// window
	_mainWindow.create(sf::VideoMode(1024, 768, 32), "Hello, World!");
	_mainWindow.setVerticalSyncEnabled(true);
	_mainWindow.setFramerateLimit(60);
	_gameState = Game::Playing;
	sf::View view = _mainWindow.getDefaultView();
	view.setSize(1024, -768);
	_mainWindow.setView(view);
	
	// rectangle
	rectangle.setFillColor(sf::Color(0, 0, 100));
	rectangle.setOrigin(rectSize.x / 2, rectSize.y / 2); // set the centre of balance of the rectangle
	rectangle.setPosition(_mainWindow.getSize().x / 2, _mainWindow.getSize().x / 2);

	// edges
	walls[0][0].position = sf::Vector2f(100, 100);
	walls[0][1].position = sf::Vector2f(200, 100);
	walls[1][0].position = sf::Vector2f(200, 100);
	walls[1][1].position = sf::Vector2f(200, 200);
	walls[2][0].position = sf::Vector2f(200, 200);
	walls[2][1].position = sf::Vector2f(300, 300);
	walls[3][0].position = sf::Vector2f(300, 300);
	walls[3][1].position = sf::Vector2f(300, 50);
	walls[4][0].position = sf::Vector2f(300, 50);
	walls[4][1].position = sf::Vector2f(300, 0);
	walls[5][0].position = sf::Vector2f(300, 0);
	walls[5][1].position = sf::Vector2f(100, 100);

	

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
	// rectangle
	rectangle.setPosition(rectPosition);
	_mainWindow.draw(rectangle);

	// walls
	for (int i = 0; i < MAX_LINES; i++) {
		sf::Vertex line[] =
		{
			walls[i][0],
			walls[i][1]
		};
		_mainWindow.draw(line, 2, sf::Lines);
	}


	// lighting
	lightSource.setPosition(rectPosition);
	_mainWindow.draw(lightSource);

	sf::Vector2f last;
	float angle = (rectangle.getRotation() + -5.0f) * degreesToRadians;
	sf::Vector2f direction(cosf(angle), sinf(angle));
	last = rectPosition + direction * rayCast(rectPosition, direction);

	sf::Color c(255, 255, 255, 50);

	for (float j = -5.0f; j <= 5.0f; j += 0.1f) {
		angle = (rectangle.getRotation() + j) * degreesToRadians;
		direction = sf::Vector2f(cosf(angle), sinf(angle));
		sf::Vertex line[] =
		{
			sf::Vertex(rectPosition, c),
			sf::Vertex(rectPosition + direction * rayCast(rectPosition, direction), c),
			sf::Vertex(last, c),
			sf::Vertex(rectPosition, c),

		};
		last = rectPosition + direction * rayCast(rectPosition, direction);
		_mainWindow.draw(line, 4, sf::PrimitiveType::Quads);
	}

	for (int theta = 0.0f; theta < 360.0f; theta += 360.0f/numExtraLights) {
		sf::Vector2f offset = ((float)extraLightDistance) * sf::Vector2f(sinf(theta * degreesToRadians), cosf(theta * degreesToRadians));

		sf::Vector2f last;
		float angle = (rectangle.getRotation() + -5.0f) * degreesToRadians;
		sf::Vector2f direction(cosf(angle), sinf(angle));
		last = rectPosition + offset + direction * rayCast(rectPosition + offset, direction);

		sf::Color c(255, 255, 255, 10);

		for (float j = -5.0f; j <= 5.0f; j += 0.1f) {
			angle = (rectangle.getRotation() + j) * degreesToRadians;
			direction = sf::Vector2f(cosf(angle), sinf(angle));
			sf::Vertex line[] =
			{
				sf::Vertex(rectPosition + offset, c),
				sf::Vertex(rectPosition + offset + direction * rayCast(rectPosition + offset, direction), c),
				sf::Vertex(last, c),
				sf::Vertex(rectPosition + offset, c),

			};
			last = rectPosition + offset + direction * rayCast(rectPosition + offset, direction);
			_mainWindow.draw(line, 4, sf::PrimitiveType::Quads);
		}
	}

	_mainWindow.display();
}

/* Finds the endpoint of the ray cast  */
float Game::rayCast(sf::Vector2f start, sf::Vector2f direction) {

	float length = 2000.0f;
	sf::Vector2f end = start + length * direction;

	for (int i = 0; i < MAX_LINES; i++) {
		sf::Vector2f point = Game::intersection(start, end, walls[i][0].position, walls[i][1].position);
		sf::Vector2f displacement = start - point;

		if (Game::len(displacement) < length) {
			length = Game::len(displacement);
		}
	}

	return length;
}

double Game::len(sf::Vector2f v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

#define pdd sf::Vector2f
sf::Vector2f Game::intersection(sf::Vector2f A, sf::Vector2f B, sf::Vector2f C, sf::Vector2f D)
{
	// Line AB represented as a1x + b1y = c1 
	double a1 = B.y - A.y;
	double b1 = A.x - B.x;
	double c1 = a1 * (A.x) + b1 * (A.y);

	// Line CD represented as a2x + b2y = c2 
	double a2 = D.y - C.y;
	double b2 = C.x - D.x;
	double c2 = a2 * (C.x) + b2 * (C.y);

	double determinant = a1 * b2 - a2 * b1;

	if (determinant == 0)
	{
		// The lines are parallel. This is simplified 
		// by returning a pair of FLT_MAX 
		return pdd(FLT_MAX, FLT_MAX);
	}
	else
	{
		double x = (b2*c1 - b1 * c2) / determinant;
		double y = (a1*c2 - a2 * c1) / determinant;
		pdd result = pdd(x, y);
		if (len(result - C) <= len(C - D) && len(result - D) < len(C - D) && len(result - A) <= len(A - B) && len(result - B) < len(A - B)) {
			return result;
		}
		else {
			return pdd(FLT_MAX, FLT_MAX);
		}
	}
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