#pragma once
#include <cmath>
#include <string>
#include <memory>
#include "SFML/Graphics.hpp"
#include <iostream>

class Animation {
private:
	std::string         m_name          = "none";
	sf::Sprite          m_sprite;
	size_t              m_frameCount    = 1;       // total number of frames of animation
	size_t              m_currentFrame  = 0;       // the current frame of animation being played
	size_t              m_speed         = 0;       // the speed to play this animation
	sf::Vector2f        m_size          = { 1, 1 };// size of the animation frame
	size_t              m_totalFrames   = 0;

public:
	Animation() {}

	Animation(const std::string& name, const sf::Texture& t) : Animation(name, t, 1, 0) {}

	Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
		: m_name           (name)
		, m_sprite         (t)
		, m_frameCount     (frameCount)
		, m_currentFrame   (0)
		, m_speed          (speed)
	{
		m_size = sf::Vector2f((float)t.getSize().x / m_frameCount, (float)t.getSize().y);
		m_sprite.setOrigin(m_size.x / 2.f, m_size.y / 2.f);
		m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));
	}

	// updates the animation to show the next frame, depending on its speed
	// animation loops when it reaches the end
	void update() {
		// add the speed variable to the current frame
		// TODO: 1) calculate the correct frame of animation to play based on currentFrame and speed
		//       2) set the texture rectangle properly
		
		if (m_speed > 0 && m_frameCount > 1) {
			m_totalFrames++;
			m_currentFrame = (m_totalFrames / m_speed) % (m_frameCount);
			sf::IntRect frame((m_currentFrame * m_size.x), 0, m_size.x, m_size.y);
			m_sprite.setTextureRect(frame);
		}
		
		
	}

	const sf::Vector2f& getSize() const {
		return m_size;
	}
	void setSize(sf::Vector2f s) {
		m_size = s;
	}
	const std::string& getName() const {
		return m_name;
	}
	sf::Sprite& getSprite() {
		return m_sprite;
	}
	bool hadEnded() const {
		if ((m_currentFrame) == (m_frameCount - 1 )) return true;
		return false;
	}
};