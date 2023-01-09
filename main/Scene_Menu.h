#pragma once

#include "Scene.h"
#include "Scene_Play.h"
#include <map>
#include <memory>
#include <deque>
#include <array>


class Scene_Menu : public Scene {
protected:
	std::string                m_title;
	//std::vector<std::string>   m_menuStrings;
	std::vector<std::string>   m_levelPaths;
	sf::Text                   m_menuText;
	size_t                     m_selectedMenuIndex = 0;
	std::array<std::string, 3> m_menuStrings{"level1", "level2", "level3"};
	std::array<std::string, 3> m_menuLevelPaths{ "level1.txt", "testLevel.txt", "level3.txt" };


	std::vector<sf::Text>    m_fontEntities;

	// FONT temp
	sf::Font font;


	void init() {
		if (!font.loadFromFile("./assets/fonts//bit.ttf")) {
			std::cout << "Could not load font" << std::endl;
		}


		// possible better soluction @1:07:02
		sf::Text text;
		text.setFont(font);
		text.setString(m_menuStrings[0]);
		text.setCharacterSize(28);
		text.setOrigin(sf::Vector2f(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2));
		text.setPosition(sf::Vector2f(m_game->window().getSize().x / 2, 200.f));
		m_fontEntities.push_back(text);

		text.setPosition(sf::Vector2f(m_game->window().getSize().x / 2, 200.f+65));
		text.setString(m_menuStrings[1]);
		m_fontEntities.push_back(text);

		text.setPosition(sf::Vector2f(m_game->window().getSize().x / 2, 200.f + 130));
		text.setString(m_menuStrings[2]);
		m_fontEntities.push_back(text);




		// register gameplay actions
		registerAction(sf::Keyboard::P, "PAUSE");
		registerAction(sf::Keyboard::S, "DOWN");
		registerAction(sf::Keyboard::W, "UP");
		registerAction(sf::Keyboard::Space, "Enter");
		// setups : loading font sizes and data to display
	}
	void update() {
		//m_entityManager.update();
		sRender();
	}
	void onEnd() {
		// end of scene

	}
	void sDoAction(const Action& action) {

		if (action.type() == "START") {
			if (action.name() == "PAUSE") { setPaused(!m_paused); }
			if (action.name() == "UP") {
				if (m_selectedMenuIndex > 0) m_selectedMenuIndex--;
				else m_selectedMenuIndex = m_menuStrings.size() - 1;
			}
			else if (action.name() == "PLAY") {
				//m_game->changeScene("PLAY", std::make_shared<Scene_Play>(m_game, m_levelPaths[m_selectedMenuIndex]));
			}
			else if (action.name() == "DOWN") {
				if (m_selectedMenuIndex < 2) m_selectedMenuIndex++;
				else m_selectedMenuIndex = 0;
			}
			else if (action.name() == "Enter") {
				m_game->changeScene("PLAY", std::make_shared<Scene_Play>(m_game, m_menuLevelPaths[m_selectedMenuIndex]));
			}
		}
	}

public:
	Scene_Menu(GameEngine* gameEngine) : Scene(gameEngine) {
		// call init class
		init();
		// pass gameengine to base class
	}
	void sRender() {
		m_game->window().clear(sf::Color(111, 113, 232));

		
		for (auto& e : m_fontEntities) {
			e.setFillColor(sf::Color(230, 230, 230));
		}

		m_fontEntities.at(m_selectedMenuIndex).setFillColor(sf::Color(0, 0, 0));

		m_game->window().draw(m_fontEntities.at(0));
		m_game->window().draw(m_fontEntities.at(1));
		m_game->window().draw(m_fontEntities.at(2));

		m_game->window().display();
	}
};