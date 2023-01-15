#pragma once

#include "Scene.h"
#include "GameEngine.h"
#include "EntityManager.h"
#include "Physics.h"
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <set>


// <Texture specification> ===========================
// Player GX GY CW CH SX SY SM GY B
//		GX, GY     x, y	               float float (starting position)
//		CW, CH     BoundingBox W/H     float float
//		SX         Horizontal speed    float
//		SY         Jump speed          float
//		SM         Max speed           float
//		GY         Gravity             float
//		B          Bullet animation    string

// bugs
// mouse cursor scales from player position.
// possible reason: view centered on player

// unnamed namespace - only accessible inside this file
namespace {
	// constant variables
	const int NCOLS{ 20 };     // total number of 64x64 pixel column cells in a room
	const int NROWS{ 12 };     // total number of 64x64 pixel row cells in a room
	const int CELLSIZE{ 64 };  // size of cell
	const int PADDING{ 32 };   // padding added to the entity to center align it in a cell
}


class Scene_Play : public Scene {
	struct PlayerConfig {
		float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};
protected:
	std::string              m_levelPath;
	std::shared_ptr<Entity>  m_player;
	PlayerConfig             m_playerConfig;
	bool                     m_drawTextures   = true;
	bool                     m_drawCollision  = false;
	bool                     m_drawGrid       = false;
	bool                     m_ghostPlayer    = false;
	bool                     m_editorMode     = false;
	bool                     m_captureLevel   = false;
	bool                     m_drag           = false;
	Physics                  m_physics;

	sf::Vector2f             m_mPos;
	sf::RectangleShape       m_cursor;

	// tile to draw one screen 
	std::string              m_tileToDraw;


	void init(const std::string& levelPath) {
		// register action
		registerAction(sf::Keyboard::P, "PAUSE");
		registerAction(sf::Keyboard::Escape, "QUIT");
		registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
		registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
		registerAction(sf::Keyboard::G, "TOGGLE_GRID");
		registerAction(sf::Keyboard::D, "RIGHT");
		registerAction(sf::Keyboard::A, "LEFT");
		registerAction(sf::Keyboard::W, "UP");
		registerAction(sf::Keyboard::S, "DOWN");
		registerAction(sf::Keyboard::Space, "SHOOT");
		registerAction(sf::Keyboard::Z, "CAPTURE");
		registerAction(sf::Keyboard::I, "EDITOR");
		registerAction(sf::Keyboard::Q, "DRAG");




		// load grid text asset
		//m_gridText.setCharacterSize(12);
		//m_gridText.setFont(m_game->assets().getFont("Tech"));

		// set font and font size


		// load level config
		loadLevel(levelPath);
		// load assets for level editor
		loadUniqueAssets();

	}
	sf::Vector2f gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity) {
		// TODO: takes in a grid (x, y) position and an Entity
		//		 Return a vector indicating where the center position of the entity should be
		//		 You must use the entity's animation size to position it correctly
		//       The size of the grid width and height is stored in m_gridSize.x and m_gridSize.y
		//       The bottom-left corner of the animation should align with the bottom left of the grid cell
		auto& animation = entity->getComponent<CAnimation>();
		gridX = gridX * 64 + (animation.animation.getSize().x / 2);
		gridY = m_game->window().getSize().y - (gridY * 64) - (animation.animation.getSize().y / 2);
		return sf::Vector2f(gridX, gridY);
	}

	sf::Vector2f gridToMidPixel(float roomX, float roomY, float gridX, float gridY, std::shared_ptr<Entity> entity) {
		auto& animation = entity->getComponent<CAnimation>();

		// compute room to world coordinates
		// R(0,0) = (0,0); R(1,0) = (window.size.x * roomx, window.size.y * roomy)
		roomX *= (int)m_game->window().getSize().x;
		roomY *= (int)m_game->window().getSize().y;

		// Find the world coordinates of entity given room and grid coordinates
		float wx, wy;
		// add computed room coords with half size of entity and (x*64, y*64 ) grid coordate to get entity position
		wx = roomX + (animation.animation.getSize().x / 2) + (64 * gridX);
		wy = roomY + (animation.animation.getSize().y / 2) + (64 * gridY);

		// return entity's position in world coordinates
		return sf::Vector2f(wx, wy);
	}

	void loadLevel(const std::string& path) {
		// level specification
		// @ 18:42 tile and dec
		// reset the entity manager every time we load a level
		m_entityManager = EntityManager();

		std::ifstream inFile(path, std::ios::in);
		std::cout << "Loading level\n";
		if (!inFile) {
			std::cout << "Could not load level" << std::endl;
			exit(EXIT_FAILURE);
		}


		std::string type, animationName;
		sf::Vector2f room(0,0), pos(0,0);
		int blocksMovement{ 0 }, blocksVision{ 0 };   // 0=false, 1=true

		// loop through file, and create entity based on type
		while (inFile >> type) {
			float x, y;
			if (type == "Tile") {
				inFile >> animationName >> room.x >> room.y >> pos.x >> pos.y >> blocksMovement >> blocksVision;

				// create entity
				auto tile = m_entityManager.addEntity("Tile");
				auto& animation = tile->addComponent<CAnimation>(m_game->getAssets().getAnimation(animationName, true));
				
				// compute world coordinates
				pos = sf::Vector2f((gridToMidPixel(room.x, room.y, pos.x, pos.y, tile)));
				
				tile->addComponent<CTransform>(pos, sf::Vector2f(0.f, 0.f), 0);
				tile->addComponent<CBoundingBox>(sf::Vector2f(animation.animation.getSize().x, animation.animation.getSize().y));
				tile->addComponent<CDraggable>();
			}
			else if (type == "Player") {
				inFile >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX 
					   >> m_playerConfig.CY >> m_playerConfig.SPEED >> m_playerConfig.JUMP 
					   >> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY >> m_playerConfig.WEAPON;
			}
			else if (type == "NPC") {
				std::string behavior;
				float health, damage, speed;
				inFile >> animationName >> room.x >> room.y >> x >> y >> blocksMovement >> blocksVision >> health >> damage >> behavior >> speed;

				auto npc = m_entityManager.addEntity("NPC");
				auto& animation = npc->addComponent<CAnimation>(m_game->getAssets().getAnimation(animationName, true));

				sf::Vector2f pos(gridToMidPixel(room.x, room.y, x, y, npc));
				npc->addComponent<CTransform>(pos, sf::Vector2f(0.f, 0.f), 0);
				npc->addComponent<CBoundingBox>(sf::Vector2f(animation.animation.getSize().x, animation.animation.getSize().y));
				npc->addComponent<CDraggable>();
			}
			else if (type == "Cursor") {
				int showCursor;
				inFile >> showCursor;
				if (showCursor) {
					// add tile cursor entity
					auto cursor = m_entityManager.addEntity("Cursor");
					cursor->addComponent<CRectangle>(sf::Vector2f(64.f, 64.f), sf::Color(0, 0, 0, 0), sf::Color(0, 0, 0), 2.f);
					cursor->addComponent<CTransform>();
				}
			}
		}

		

		spawnPlayer();
	}
	// utility class for level editor to create entities for all tiles
	void loadUniqueAssets() {
		int row{ 1 };
		int col{ 1 };
		int c{ 0 };

		for (auto item : m_game->getAssets().getAnimations()) {
			if (c % 3 == 0) {
				++row;
				col = 1;
			}
			auto tile = m_entityManager.addEntity("Asset");
			auto& ani = tile->addComponent<CAnimation>(m_game->getAssets().getAnimation(item.first, true));

			sf::Vector2f pos(sf::Vector2f(47.f * col, 47.f * row));
			tile->addComponent<CTransform>(pos, sf::Vector2f(0.f, 0.f), 0);
			tile->addComponent<CBoundingBox>(sf::Vector2f(ani.animation.getSize().x, ani.animation.getSize().y));
			tile->addComponent<CDraggable>();

			++col;
			++c;
		}
	}

	void spawnPlayer() {
		m_player = m_entityManager.addEntity("Player");
		auto& animation = m_player->getComponent<CAnimation>();

		m_player->addComponent<CAnimation>(m_game->getAssets().getAnimation("Stand", true));

		float x, y;

		// calculate player position
		x = m_playerConfig.X * 64 + (animation.animation.getSize().x / 2);
		y = m_game->window().getSize().y - (m_playerConfig.Y * 64) - (animation.animation.getSize().y / 2);

		// add player components
		m_player->addComponent<CInput>();
		m_player->addComponent<CTransform>(sf::Vector2f(x, y), sf::Vector2f(0.f, 0.f), 0);
		m_player->addComponent<CBoundingBox>(sf::Vector2f(m_playerConfig.CX, m_playerConfig.CY));
		m_player->addComponent<CState>();
	}

	void spawnBullet(std::shared_ptr<Entity> entity) {
		float direction = entity->getComponent<CAnimation>().animation.getSprite().getScale().x;
		auto bullet = m_entityManager.addEntity("Bullet");
		auto& animation = bullet->addComponent<CAnimation>(m_game->getAssets().getAnimation("Bullet", true));
		bullet->addComponent<CTransform>(sf::Vector2f(entity->getComponent<CTransform>().pos.x, entity->getComponent<CTransform>().pos.y), sf::Vector2f(15 * direction, 0), 0);
		bullet->addComponent<CBoundingBox>(sf::Vector2f(20, 20));
		bullet->addComponent<CLifespan>(40);
	}

	bool IsInside(sf::Vector2f pos, std::shared_ptr<Entity> e) {
		auto ePos = e->getComponent<CTransform>().pos;
		auto size = e->getComponent<CAnimation>().animation.getSize();

		float dx = fabs(pos.x - ePos.x);
		float dy = fabs(pos.y - ePos.y);

		return (dx <= size.x / 2) && (dy <= size.y / 2);
	}

	// function adjusts window coordinates to the view
	sf::Vector2f windowToWorld(const sf::Vector2f& window) const {
		// x coord of variable view is the "world" x coord of the center of the window
		sf::View view = m_game->window().getView();
		float wx = view.getCenter().x - m_game->window().getSize().x / 2;
		float wy = view.getCenter().y - m_game->window().getSize().y / 2;

		//std::cout << "COORD: " << window.x + wx << " " << window.y + wy << std::endl;

		return sf::Vector2f(window.x + wx, window.y + wy);
	}

	void sDoAction(const Action& action) {
		if (action.type() == "START") {
			if (action.name() == "RIGHT") m_player->getComponent<CInput>().right = true;
			else if (action.name() == "LEFT") m_player->getComponent<CInput>().left = true;
			else if (action.name() == "UP") m_player->getComponent<CInput>().up = true;
			else if (action.name() == "DOWN") m_player->getComponent<CInput>().down = true;
			else if (action.name() == "TOGGLE_COLLISION") m_drawCollision = !m_drawCollision;
			else if (action.name() == "TOGGLE_TEXTURE") m_drawTextures = !m_drawTextures;
			else if (action.name() == "SHOOT") m_player->getComponent<CInput>().shoot = true;
			else if (action.name() == "CAPTURE") m_captureLevel = !m_captureLevel;
			else if (action.name() == "EDITOR") m_editorMode = !m_editorMode;
			else if (action.name() == "DRAG") m_drag = !m_drag;
			else if (action.name() == "LEFT_CLICK") {
				// convert mouse coords relevant to window to world coordinates
				sf::Vector2f worldPos = windowToWorld(action.pos());

				// divide word coords by grid size (64) to get values(0,1,2,3...)
				worldPos.x = ((int)worldPos.x / 64);
				worldPos.y = ((int)worldPos.y / 64);

				for (auto e : m_entityManager.getEntities()) {
					// if mouse pointer is in entity and has draggable component
					if (e->hasComponent<CDraggable>() && IsInside(windowToWorld(action.pos()), e)) {
						// flip value
						e->getComponent<CDraggable>().dragging = !e->getComponent<CDraggable>().dragging;
					}
				}
			}
			else if (action.name() == "MOUSE_MOVE") {
				// lock cursor into a grid cell
				sf::Vector2f wp = windowToWorld(action.pos());

				// create a cell like coordinate system
				m_mPos.x = (wp.x < 0) ? ((int)wp.x / 64) * 64 -64 : ((int)wp.x / 64) * 64;
				m_mPos.y = (wp.y < 0) ? ((int)wp.y / 64) * 64 - 64 : ((int)wp.y / 64) * 64;
			}
		}
		else if (action.type() == "END") {
			if (action.name() == "RIGHT") m_player->getComponent<CInput>().right = false;
			if (action.name() == "LEFT") m_player->getComponent<CInput>().left = false;
			if (action.name() == "UP") m_player->getComponent<CInput>().up = false;
			if (action.name() == "DOWN") m_player->getComponent<CInput>().down = false;
			if (action.name() == "SHOOT") m_player->getComponent<CInput>().shoot = false; m_player->getComponent<CInput>().canShoot = true;
		}
	}

	void sMovement() {
		// movement, gravity, max speed, scaling, set animation state
		sf::Vector2f playerVelocity(0, m_player->getComponent<CTransform>().velocity.y);
		if (m_player->getComponent<CInput>().up) {
			playerVelocity.y = -10;
		}
		if (m_player->getComponent<CInput>().down) {
			playerVelocity.y = 10;
		}
		if (m_player->getComponent<CInput>().right) {
			playerVelocity.x = 10;
			m_player->getComponent<CAnimation>().animation.getSprite().setScale(sf::Vector2f(1, 1));
			if (m_player->getComponent<CInput>().canJump) m_player->getComponent<CState>().state = "Running";
		}
		else if (m_player->getComponent<CInput>().left) {
			playerVelocity.x = -10;
			m_player->getComponent<CAnimation>().animation.getSprite().setScale(sf::Vector2f(-1, 1));
			if (m_player->getComponent<CInput>().canJump) m_player->getComponent<CState>().state = "Running";

		}
		if (m_player->getComponent<CInput>().shoot && m_player->getComponent<CInput>().canShoot) {
			spawnBullet(m_player);
			m_player->getComponent<CInput>().canShoot = false;
		}
		m_player->getComponent<CTransform>().prevPos = m_player->getComponent<CTransform>().pos;
		m_player->getComponent<CTransform>().velocity = playerVelocity;
		m_player->getComponent<CTransform>().pos += m_player->getComponent<CTransform>().velocity;
		m_player->getComponent<CTransform>().velocity = sf::Vector2f(0, 0);
	}
	
	void sCollision() {
		// player/entity collision
		for (auto& e : m_entityManager.getEntities()) {
			if (e->tag() == "Player") continue;
			if (e->tag() == "Bullet") continue;
			if (!e->hasComponent<CBoundingBox>()) continue;

			sf::Vector2f overlap = m_physics.GetOverlap(m_player, e);
			if (overlap.x > 0 && overlap.y > 0) {
				auto& transform = m_player->getComponent<CTransform>();
				sf::Vector2f prevOverlap = m_physics.GetPreviousOverlap(m_player, e);
				if (prevOverlap.x > 0 && e->getComponent<CTransform>().pos.y > transform.pos.y) {
					// top box collision
					transform.pos.y -= overlap.y;
					transform.velocity.y = 0;
					if (transform.velocity.x == 0 && transform.velocity.y == 0) {
						m_player->getComponent<CState>().state = "Standing";
					}
					m_player->getComponent<CInput>().canJump = true;

				}
				else if (prevOverlap.x > 0 && e->getComponent<CTransform>().pos.y < transform.pos.y) {
					// bottom box collision
					transform.velocity.y = 0; // gravity
					transform.pos.y += overlap.y;

					if (e->getComponent<CAnimation>().animation.getName() == "Question") {
						// add coin
						auto coin = m_entityManager.addEntity("Tile");
						auto& ani = coin->addComponent<CAnimation>(m_game->getAssets().getAnimation("Coin", true));
						sf::Vector2f pos(0,0);
						pos.x = e->getComponent<CTransform>().pos.x;
						pos.y = e->getComponent<CTransform>().pos.y - 64;
						coin->addComponent<CTransform>(pos, sf::Vector2f(0.f, 0.f), 0);

						// replace question block animation with question2
						e->addComponent<CAnimation>(m_game->getAssets().getAnimation("Question2", true));
				
					}
				}
				else if (prevOverlap.y > 0 && e->getComponent<CTransform>().pos.x > transform.pos.x) {
					// right box collision
					transform.pos.x -= overlap.x;
				}
				else if (prevOverlap.y > 0 && e->getComponent<CTransform>().pos.x < transform.pos.x) {
					// left box collision
					transform.pos.x += overlap.x;
				}
			}
		}

		if (m_player->getComponent<CTransform>().pos.x <= m_player->getComponent<CBoundingBox>().halfSize.x) {
			m_player->getComponent<CTransform>().pos.x = m_player->getComponent<CBoundingBox>().halfSize.x;
		}
		if (m_player->getComponent<CTransform>().pos.y >= m_game->window().getSize().y) {
			m_player->setNotActive();
			spawnPlayer();
		}

		// bullet collision
		for (auto& b : m_entityManager.getEntities("Bullet")) {
			for (auto& e : m_entityManager.getEntities("Tile")) {
				if (e->getComponent<CAnimation>().animation.getName() == "Brick") {

					sf::Vector2f overlap = m_physics.GetOverlap(b,e);
					if (overlap.x > 0 && overlap.y > 0) {
						sf::Vector2f pos = e->getComponent<CTransform>().pos;
						b->setNotActive();
						e->setNotActive();

						auto explosion = m_entityManager.addEntity("Dec");
						auto& animation = explosion->addComponent<CAnimation>(m_game->getAssets().getAnimation("Explosion", true));
						explosion->addComponent<CTransform>(pos, sf::Vector2f(0.f, 0.f), 0);
					}
				}
			}
		}
	}
	void sAnimation() {
		// set player animation based on player state
		if (m_player->getComponent<CState>().state == "Standing" && m_player->getComponent<CAnimation>().animation.getName() != "Stand") {
			float xs{ m_player->getComponent<CAnimation>().animation.getSprite().getScale().x };
			auto& animation = m_player->addComponent<CAnimation>(m_game->getAssets().getAnimation("Stand", true));
			animation.animation.getSprite().setScale(xs, 1);
		}
		if (m_player->getComponent<CState>().state == "Jumping" && m_player->getComponent<CAnimation>().animation.getName() != "Air") {
			m_player->addComponent<CAnimation>(m_game->getAssets().getAnimation("Air", true));
		}
		if (m_player->getComponent<CState>().state == "Running" && m_player->getComponent<CAnimation>().animation.getName() != "Run") {
			m_player->addComponent<CAnimation>(m_game->getAssets().getAnimation("Run", true));
		}

		// logic for non repeating animation
		for (auto& e : m_entityManager.getEntities()) {
			if (e->hasComponent<CAnimation>()) {
				e->getComponent<CAnimation>().animation.update();
			}
			if (e->getComponent<CAnimation>().animation.getName() == "Explosion" || e->getComponent<CAnimation>().animation.getName() == "Coin") {
				if (e->getComponent<CAnimation>().animation.hadEnded()) {
					e->setNotActive();
				}
			}
		}
	}

	void sRender() {
		// render all entities
		sf::View view(sf::FloatRect(0.f, 0.f, m_game->window().getSize().x, m_game->window().getSize().y));

		// update current list of entities
		m_entityManager.update();
		m_game->window().clear(sf::Color(111, 113, 232));

		 //camera
		float cx{ m_player->getComponent<CTransform>().pos.x - (m_game->window().getSize().x / 2) };
		float cy{ m_player->getComponent<CTransform>().pos.y - (m_game->window().getSize().y / 2) };

		// set camera to follow player
		view.setCenter(sf::Vector2f((float)m_game->window().getSize().x / 2, (float)m_game->window().getSize().y / 2));
		view.move(cx, cy);

		m_game->window().setView(view);

		// loop and display entitiy
		for (auto& e : m_entityManager.getEntities()) {
			if (e->tag() == "Cursor") {                 // draw cursor shapes
				auto& transform = e->getComponent<CTransform>();
				auto& rect = e->getComponent<CRectangle>();
				rect.rect.setPosition(m_mPos.x + 32, m_mPos.y + 32);
				m_game->window().draw(rect.rect);
			}
			if (e->tag() == "Asset")             continue;    // skip type Asset 
			if (!e->hasComponent<CAnimation>())  continue;    // skip entity with no animation component
			if (!m_drawTextures)                 continue;    // if textures are off, then dont draw anything

			// display entity
			auto& transform = e->getComponent<CTransform>();
			e->getComponent<CAnimation>().animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
			m_game->window().draw(e->getComponent<CAnimation>().animation.getSprite());
		}

		// draw bounding box if true
		if (m_drawCollision) {
			for (auto& e : m_entityManager.getEntities()) {
				if (e->tag() == "Asset") continue;
				if (e->hasComponent<CBoundingBox>()) {
					auto& box = e->getComponent<CBoundingBox>();
					auto& transform = e->getComponent<CTransform>();
					sf::RectangleShape rect;
					rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
					rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
					rect.setPosition(transform.pos.x, transform.pos.y);
					rect.setFillColor(sf::Color(0, 0, 0, 0));
					rect.setOutlineColor(sf::Color(255, 255, 255, 255));
					rect.setOutlineThickness(1);
					m_game->window().draw(rect);
				}
			}
		}

		// display editor gui if stru
		if (m_editorMode) {
			gInventory();
		}

		// display changes
		m_game->window().display();

	}

	void sLifespan() {
		// subtract n to the lifespan of all bullets
		for (auto e : m_entityManager.getEntities("Bullet")) {
			e->getComponent<CLifespan>().remaining -= 1;
			if (e->getComponent<CLifespan>().remaining == 0) {
				e->setNotActive();
			}
		}
	}

	void onEnd() {
		// end of scene
		// when the scene ends, change back to menu
		// m_game change scene
		// called when you hit escape

	}

	void sDragAndDrop() {
		for (auto e : m_entityManager.getEntities()) {
			if (e->hasComponent<CDraggable>() && e->getComponent<CDraggable>().dragging) {	
				auto& transform = e->getComponent<CTransform>();
				sf::Vector2f animSize = e->getComponent<CAnimation>().animation.getSize();	
				transform.pos.x = m_mPos.x + 32;
				transform.pos.y = m_mPos.y + 32;
			}
		}
	}
	// calculates the inverse of entities position to a zero based coordinate system relative to the room
	sf::Vector2f inverseWorldXY(sf::Vector2f pos, sf::Vector2f size) {
		float x, y;
		x = y = 0;
		// calculate calculate inverse of entities position to zero based xy coordinates
		x = (pos.x - (size.x / 2)) / CELLSIZE;
		x = ((int)x + NCOLS) % NCOLS;

		y = (pos.y - (size.y / 2)) / CELLSIZE;
		y = ((int)y + NROWS) % NROWS;
		return sf::Vector2f(x,y);
	}
	// calculates the inverse of world coordinates to produce a coordinate system that starts with zero and is incremented by 1
	sf::Vector2f worldToZerobasedRoomCoordinates(sf::Vector2f pos, sf::Vector2f iPos) {
		float rx, ry;

		// calculate room y coord
		rx = (pos.x - PADDING - (CELLSIZE * iPos.x)) / m_game->window().getSize().x;
		// calculate room x coord
		ry = (pos.y - PADDING - (CELLSIZE * iPos.y)) / m_game->window().getSize().y;


		return sf::Vector2f(rx,ry);
	}

	void sCaptureLevel() {
		// display all entities x and y position

		std::ofstream file("level1.txt", std::ios::out);

		if (!file) {
			std::cout << "Failed to open file" << std::endl;
		}
		// save entities to file
		for (auto e : m_entityManager.getEntities()) {
			// skip entities that are not part of the map design
			if (e->tag() == "Asset") continue;
			if (e->tag() == "Cursor") continue;

			std::string type, animName;
			float x, y, rx, ry;

			// get tile information
			type = e->tag();
			animName = e->getComponent<CAnimation>().animation.getName();

			// get data for calculations
			sf::Vector2f ePos = e->getComponent<CTransform>().pos;
			sf::Vector2f aSize = e->getComponent<CAnimation>().animation.getSize();


			// inverse entities position to get the zero based room and xy coordintes to save in file
			sf::Vector2f inverseXY = inverseWorldXY(ePos, aSize);
			sf::Vector2f inverseRoomXY = worldToZerobasedRoomCoordinates(ePos, inverseXY);

			std::cout << type << " " << animName << " " << inverseRoomXY.x << " " << inverseRoomXY.y << " " << inverseXY.x << " " << inverseXY.y << std::endl;
			file 
			  << type              << " " 
			  << animName		   << " " 
			  << inverseRoomXY.x   << " " 
			  << inverseRoomXY.y   << " " 
			  << inverseXY.x       << " " 
			  << inverseXY.y       << "\n";
		
		}
		// write player last to file
		std::shared_ptr<Entity> player = m_entityManager.getEntities("Player")[0];
		file 
		  << player->tag()                                 << " "
	      << m_playerConfig.X                              << " "
	      << m_playerConfig.Y                              << " "
	      << player->getComponent<CBoundingBox>().size.x   << " "
	      << player->getComponent<CBoundingBox>().size.y   << " "
	      << m_playerConfig.SPEED                          << " "
	      << m_playerConfig.JUMP                           << " "
	      << m_playerConfig.MAXSPEED                       << " "
		  << player->getComponent<CGravity>().gravity      << " "
		  << m_playerConfig.WEAPON                         << " \n";


		// reverse m_captureLevel to limit function to run once
		m_captureLevel = false;
		file.close();
	}

	void gInventory() {
		// view before change
		sf::View originalView = m_game->window().getView();

		sf::View view;
		view.reset(sf::FloatRect(0,0,200, 800));
		view.setViewport(sf::FloatRect(0.75f, 0, 0.2f, 0.8f));

		m_game->window().setView(view);

		sf::RectangleShape background(sf::Vector2f(200, 800));
		background.setFillColor(sf::Color(220, 60, 110, 130));

		sf::Font font;
		sf::Text text;

		font.loadFromFile("./jb.ttf");
		text.setFont(font);
		text.setFillColor(sf::Color(0, 0, 0));
		text.setCharacterSize(18);
		text.setString("BRICK");
		text.setPosition(sf::Vector2f(40.f, 60.f));

		m_game->window().draw(background);

		m_game->window().draw(text);

		//// call grid to mid function. Need animation component
		
		for (auto& e : m_entityManager.getEntities("Asset")) {
			if (e->hasComponent<CAnimation>()) {
				auto& transform = e->getComponent<CTransform>();

				//transform.pos += transform.velocity;
				e->getComponent<CAnimation>().animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				if (m_drawTextures) {
					e->getComponent<CAnimation>().animation.getSprite().setScale(sf::Vector2f(0.6,0.6));
					if (e->getComponent<CAnimation>().animation.getSize().x > 64) {
						e->getComponent<CAnimation>().animation.getSprite().setScale(sf::Vector2f(0.1, 0.1));

					}
					m_game->window().draw(e->getComponent<CAnimation>().animation.getSprite());
				}

			}
		}

		// check if mouse is between view
		// viewport x and size
		/*if (sf::Mouse::getPosition(m_game->window()).x > m_game->window().getSize().x * 0.75 && sf::Mouse::getPosition(m_game->window()).x < m_game->window().getSize().x * (0.75 + 0.2)) {*/
		if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			for (auto& e : m_entityManager.getEntities("Asset")) {
				sf::Vector2f mPos = m_game->window().mapPixelToCoords(sf::Mouse::getPosition(m_game->window()));
				//std::cout << "Entity.x: " << e->getComponent<CTransform>().pos.x << " " << m.x << std::endl;
				if (mPos.x > e->getComponent<CTransform>().pos.x - 32
					&& mPos.x < e->getComponent<CTransform>().pos.x + e->getComponent<CAnimation>().animation.getSize().x - 32
					&& mPos.y > e->getComponent<CTransform>().pos.y - 32
					&& mPos.y < e->getComponent<CTransform>().pos.y + e->getComponent<CAnimation>().animation.getSize().y - 32
				) {
					m_tileToDraw = e->getComponent<CAnimation>().animation.getName();
					std::cout << "draw " << m_tileToDraw << "\n";
				}

			}
		}

		m_game->window().setView(originalView);
	}

	void sDrawTile() {
		if (m_tileToDraw == "") return;
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
			// set tile details
			auto tile = m_entityManager.addEntity("Tile");
			auto& animation = tile->addComponent<CAnimation>(m_game->getAssets().getAnimation(m_tileToDraw, true));

			// set position to mouse pointer
			sf::Vector2f pos(m_mPos);              // m_mPos uses a cell system
			// adjust position
			pos.x += 32;
			pos.y += 32;

			tile->addComponent<CTransform>(pos, sf::Vector2f(0.f, 0.f), 0);

			tile->addComponent<CDraggable>();
		}
	}

public:
	Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
		: Scene(gameEngine)
		, m_levelPath(levelPath)
	{
		init(m_levelPath);
	}

	void update() {
		sMovement();
		sLifespan();
		sAnimation();
		if (m_drag) {
			sDragAndDrop();
		}
		if (m_captureLevel) {
			sCaptureLevel();
		}
		if (m_editorMode) {
			sDrawTile();
		}
		sRender();
	}
};

// 1 frame befoer collision, moving dowwards will set y velocity to 10.
// SCollision will not detect any collision
// render will add the velocity to the player, and will go beyond a bounding box.