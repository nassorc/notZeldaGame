#pragma once

#include "EntityManager.h"
#include "Entity.h"
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <random>
#include <ctime>
#include <iomanip>

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct WindowConfig { int W, H; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

// <Window config> =======================
//    W           Window Width
//    H           Window Height

// <Player config> =======================
//    SR          Shape Radius
//    CR          Collision Radius
//    S           Speed                    float
//    FR,FG,FB    Fill Color
//    OR,OG,OB    Outline Color
//    OT          Outline Thickness
//    V           Shape Vertices

// <Enemy config> =======================
//    SR          Shape Radius
//    CR          Collision Radius
//    SMIN, SMAX  Min/Max Speed            float, float
//    OR,OG,OB    Outline Color
//    OT          Outline Thickness
//    VMIN, VMAX  Min, Max Vertices
//    L           Small Lifespan
//    SI          Spawn Interval (FRAME)  

// <Bullet config> =======================
//    SR          Shape Radius
//    CR          Collision Radius
//    S           Speed                    float
//    FR,FG,FB    Fill Color
//    OR,OG,OB    Outline Color
//    OT          Outline Thickness
//    V           Shape Vertices
//    L           Lifespan


class Game {
	sf::RenderWindow   m_window;
	EntityManager      m_entities;
	WindowConfig       m_windowConfig;
	PlayerConfig       m_playerConfig;
	BulletConfig       m_bulletConfig;
	EnemyConfig        m_enemyConfig;
	bool               m_pause = false;
	bool               m_running = true;
	int                m_currentFrame = 0;  // used as a substite to time. spawn enemy ever 3 frames (3secs)

	std::shared_ptr<Entity> m_player;

	void init(const std::string& config) {
		// load config file and save to config variable
		std::ifstream fin(config, std::ios::in);
		if (!fin) {
			std::cout << "File did not open" << std::endl;
			exit(EXIT_FAILURE);
		}
		std::string type;
		while (fin >> type) {
			if (type == "Window") {
				fin >> m_windowConfig.W >> m_windowConfig.H;
			}
			if (type == "Player") {
				fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S 
					>> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB 
					>> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB 
					>> m_playerConfig.OT >> m_playerConfig.V;
			}
			if (type == "Bullet") {
				fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S
					>> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB
					>> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB
					>> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
			}
			if (type == "Enemy") {
				fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN
					>> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG
					>> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN
					>> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
			}
		}
		m_window.create(sf::VideoMode(m_windowConfig.W, m_windowConfig.H), "Game 1");
		m_window.setFramerateLimit(60);
		spawnPlayer();
	}

	// systems
	void sMovement() {
		// implement plyaer movement
		// set cTranform velcoity to speed/velocity

		// lastly
		// m_player->cTransform->pos.x += m_player->cTransform->velocity.x
		float s{ m_playerConfig.S };
		if (m_player->cInput->up) {
			m_player->cTransform->pos.y = m_player->cTransform->pos.y - s;
		}
		if (m_player->cInput->down) {
			m_player->cTransform->pos.y = m_player->cTransform->pos.y + s;
		}
		if (m_player->cInput->left) {
			m_player->cTransform->pos.x = m_player->cTransform->pos.x - s;
		}
		if (m_player->cInput->right) {
			m_player->cTransform->pos.x = m_player->cTransform->pos.x + s;
		}
	}
	void sUserInput() {
		// poll events
		// if key down set cInput to tru
		// if key release set cInput to false
		// use sMovement
		// handle events and input, but don't implement user movement here. just set the component value to true

		sf::Event event;

		while (m_window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				m_running = false;
			}
			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::W:
					m_player->cInput->up = true;
					break;
				case sf::Keyboard::A:
					m_player->cInput->left = true;
					break;
				case sf::Keyboard::S:
					m_player->cInput->down = true;
					break;
				case sf::Keyboard::D:
					m_player->cInput->right = true;
					break;
				}
			}
			if (event.type == sf::Event::KeyReleased) {
				switch (event.key.code) {
				case sf::Keyboard::W:
					m_player->cInput->up = false;
					break;
				case sf::Keyboard::A:
					m_player->cInput->left = false;
					break;
				case sf::Keyboard::S:
					m_player->cInput->down = false;
					break;
				case sf::Keyboard::D:
					m_player->cInput->right = false;
					break;
				}
			}
			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					spawnBullet(m_player, sf::Vector2f(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)));
				}
			}
		}
	}
	void sRender() {
		m_window.clear();
		for (auto& e : m_entities.getEntities()) {
			e->cTransform->pos.x += e->cTransform->velocity.x;
			e->cTransform->pos.y += e->cTransform->velocity.y;
			e->cShape->circle.setPosition(e->cTransform->pos);
			e->cTransform->angle += 4.0f;
			e->cShape->circle.setRotation(e->cTransform->angle);
			m_window.draw(e->cShape->circle);
		}

		m_window.display();

		++m_currentFrame;
	}
	void sCollision() {
		// for every bullet
		//		for every enemy
					// decide if collided
		for (auto& b : m_entities.getEntities("bullet")) {
			for (auto& e : m_entities.getEntities("enemy")) {
				if (b->cShape->circle.getGlobalBounds().intersects(e->cShape->circle.getGlobalBounds())) {
					b->setNotActive();
					e->setNotActive();
					spawnSmallEnemies(e);
				}
			}
			for (auto& e : m_entities.getEntities("smallenemy")) {
				if (b->cShape->circle.getGlobalBounds().intersects(e->cShape->circle.getGlobalBounds())) {
					b->setNotActive();
					e->setNotActive();
				}
			}
		}
	}
	void sWindowCollsion() {
		for (auto& e : m_entities.getEntities("enemy")) {
			float r = e->cShape->circle.getRadius();
			if ((e->cTransform->pos.x - r) <= 0 || ((e->cTransform->pos.x + r) >= m_windowConfig.W)) {
				e->cTransform->velocity.x *= -1;
			}
			if ((e->cTransform->pos.y - r) <= 0 || ((e->cTransform->pos.y + r) >= m_windowConfig.H)) {
				e->cTransform->velocity.y *= -1;
			}


		}
	}
	void sEnemySpawner() {
		// will call enemyspawer
		// use frames to determine when to spawn enemy
			// m_currentFrame - m_lastEnmeySpawnTime
			// last enemy spawn time will be set at spawnenemy member function
		if (m_currentFrame % 100 == 0) {
			spawnEnemy();
		}


		//TESTING ONE ENEMEY
		/*if (m_currentFrame == 100) {
			spawnEnemy();
		}*/
	}

	void spawnEnemy() {


		//lastly, record here the last enemy spawn time
		// m_lastEnemySpawnTime = m_currentFrame

		//srand(time(0));

		int rx{ (rand() % 600) + 100 };
		int ry{ (rand() % 400) + 100 };

		int speeds[2]{-1,1};
		int rsx{ speeds[rand() % 2] * 3 };
		int rsy{ speeds[rand() % 2] * 3 };

		int vertices{ rand() % m_enemyConfig.VMAX + m_enemyConfig.VMIN };

		int red{ rand() % 255 };
		int green{ rand() % 255 };
		int blue{ rand() % 255 };

		auto entity = m_entities.addEntity("enemy");
		entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, vertices, sf::Color(red, green, blue), sf::Color(m_enemyConfig.OG, m_enemyConfig.OR, m_enemyConfig.OB), 2.0f);
		entity->cTransform = std::make_shared<CTransform>(sf::Vector2f(rx, ry), sf::Vector2f(rsx, rsy), 0);
		std::cout << "Enemy spawned\n";
	}

	void spawnPlayer() {
		// create temp var and add entity tag player
		// add components

		auto entity = m_entities.addEntity("player");
		entity->cTransform = std::make_shared<CTransform>(sf::Vector2f(m_window.getSize().x / 2, m_window.getSize().y / 2), sf::Vector2f(0.0f, 0.0f), 0);
		entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);
		entity->cInput = std::make_shared<CInput>();
		entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);
		m_player = entity;
	}



	void spawnBullet(std::shared_ptr<Entity> entity, const sf::Vector2f& target) {
		// MATH
		// |V| = sqrt (V.x^2 + V.y^2)      c^2 = a^2 + b^2
		// normalized v = v/|V|            = x = y = (n <= 1)
		auto bullet = m_entities.addEntity("bullet");
		bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);

		// dest - source
		sf::Vector2f aimDirection{target - entity->cTransform->pos};       // line between player and mouse clicked position
		sf::Vector2f aimDirectionNormalized;

		aimDirectionNormalized.x = aimDirection.x / sqrt(pow(aimDirection.x,2) + pow(aimDirection.y, 2));
		aimDirectionNormalized.y = aimDirection.y / sqrt(pow(aimDirection.x, 2) + pow(aimDirection.y, 2));

		aimDirectionNormalized.x *= m_bulletConfig.S;
		aimDirectionNormalized.y *= m_bulletConfig.S;

		bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, aimDirectionNormalized, 0);
	}

	// if ded or dying, then spawn smaller ones
	// given distance of hypotenuse and angle, find x and y value to determine the direction.
	void spawnSmallEnemies(std::shared_ptr<Entity> entity) {
		

		// compute change in x and y
		float h = 4;
		float d = 0;
		float cd = 90;
		float resultX, resultY;


		//std::cout << resultX << " " << resultY << std::endl;

		for (int i{ 0 }; i < 3; ++i) {
			auto enemy = m_entities.addEntity("smallenemy");
			enemy->cShape = std::make_shared<CShape>(15, 3, sf::Color(0, 0, 255), sf::Color(255, 255, 255), 1.f);

			resultX = sin(d) * 2;
			resultY = cos(d) * 2;

			std::cout << resultX << " " << resultY << std::endl;


			enemy->cTransform = std::make_shared<CTransform>(sf::Vector2f(entity->cShape->circle.getPosition().x, entity->cShape->circle.getPosition().y), sf::Vector2f(resultX, resultY), 0);
			enemy->cLifespan = std::make_shared<CLifespan>(100);
			d += cd;
			resultX = resultY = 0;
		}


	}
	

	// deplete lifespan to ded
	void sLifespan() {
		for (auto& e : m_entities.getEntities()) {
			if (e->cLifespan) {
				e->cLifespan->remaining -= 1;
				int c = e->cLifespan->remaining;
				//e->cShape->circle.setFillColor(sf::Color(e->cShape->circle));
				e->cShape->circle.setFillColor(sf::Color(e->cShape->circle.getFillColor().r, e->cShape->circle.getFillColor().g, e->cShape->circle.getFillColor().b, c));
				e->cShape->circle.setOutlineColor(sf::Color(e->cShape->circle.getFillColor().r, e->cShape->circle.getFillColor().g, e->cShape->circle.getFillColor().b, c));

				if (e->cLifespan->remaining == 0) {
					e->setNotActive();
				}
			}
		}
	}

public:
	Game(const std::string& config) {
		init(config);
	}
	void run() {
		while (m_running) {
			m_entities.update();   // add wait list to main vector
			sEnemySpawner();
			sRender();
			sWindowCollsion();
			sCollision();
			sLifespan();
			sUserInput();          // system handles keyboard events and userinput
			sMovement();           // handle player movement
		}
	}
};

// current todo SPAWN BULLET AT GIVEN MOUSE LOCATION
	// display bullet
	// display at mouse x y position
	// make it move in the correct direction