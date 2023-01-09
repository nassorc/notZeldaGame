#pragma once

#include "Action.h"
#include "EntityManager.h"

#include <memory>
#include <iostream>

class GameEngine;


typedef std::map<int, std::string> ActionMap;


// The scene layer is on top of entity manager, each scene has its own entities to manage
// store common scene data: entities, frame count, actions

class Scene {

protected:
	GameEngine *     m_game = nullptr;
	EntityManager    m_entityManager;
	ActionMap        m_actionMap;
	bool             m_paused = false;
	bool             m_hasEnded = false;
	size_t           m_currentFrame = 0;

	virtual void onEnd() = 0;
	void setPaused(bool paused) {
		m_paused = paused;
	}

public:
	Scene(GameEngine* gameEngine) {
		m_game = gameEngine;
		std::cout << "Scene class created\n";
	}

	virtual void update() = 0;
	virtual void sDoAction(const Action& action) = 0;
	virtual void sRender() = 0;

	virtual void doAction(const Action& action) {

		sDoAction(action);
	}

	// function simulate calls the derived scene's update() a given number of times
	void simulate(const size_t frames) {

	}
	void registerAction(int inputKey, const std::string& actionName) {
		m_actionMap[inputKey] = actionName;
	}

	size_t width() const {}
	size_t height() const{}
	size_t currentFrmae() const{}

	bool hasEnded() const {
	}
	const ActionMap& getActionMap() const {
		return m_actionMap;
	}
	void drawLine(const sf::Vector2f p1, const sf::Vector2f& p2) {
	}

	void test() {
		std::cout << "SCENE WORKING" << std::endl;
	}
};