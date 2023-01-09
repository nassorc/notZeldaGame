#pragma once

#include <string>
#include "SFML/Graphics.hpp"
// action class will store the type of action that the player wants the scene to perform
// DF = input -> gameEngine -> construct actions -> send to scene -> perform logic via doAction()

// two main variables
// - name (jump, shoot, etc)
// - type: START or END

// e.g. ("Right", "Start") - player will run right as if you held down the move right
//							 input button continuously
//                         - will only stop moving on ("right", "end")

class Action {
public:
	std::string m_name = "NONE";
	std::string m_type = "NONE";
	sf::Vector2f  m_pos = sf::Vector2f(0, 0);

	Action() {

	}
	Action(const std::string& name, const std::string& type)
		: m_name(name)
		, m_type(type)
	{
	}
	Action(const std::string& name, const std::string& type, const sf::Vector2f& pos)
		: m_name(name)
		, m_type(type)
		, m_pos(pos)
	{
	}
	const std::string& name() const {
		return m_name;
	}
	const std::string& type() const {
		return m_type;
	}
	const sf::Vector2f& pos() const {
		return m_pos;
	}
	std::string toString() const {
		return m_type + " " + m_name;

	}
};