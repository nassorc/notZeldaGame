#pragma once

// component defintion 1:29:39
#include "Animation.h"

#include <SFML/Graphics.hpp>

class Component {
public:
	bool has = false;
	Component() {}
};

// contains data on where an entities is, how fast it's going, and the rotation
class CTransform : public Component{
public:
	sf::Vector2f pos      = { 0.0, 0.0 };
	sf::Vector2f prevPos  = { 0.0, 0.0 };  // position of the entity on the previous frame
	sf::Vector2f scale    = { 1.0, 1.0 };
	sf::Vector2f velocity = { 0.0, 0.0 };
	float angle           = 0;

	CTransform() {}
	CTransform(const sf::Vector2f& p, const sf::Vector2f& v, float a) 
		: pos(p), velocity(v), angle(a) {
		this->has = true;
	}
};

// contains data on shape data
//class CShape : public Component {
//public:
//	sf::CircleShape circle;
//
//	CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness)
//		: circle(radius, points)
//	{
//		circle.setFillColor(fill);
//		circle.setOutlineColor(outline);
//		circle.setOutlineThickness(thickness);
//		circle.setOrigin(radius, radius);
//	}
//};

class CRectangle : public Component {
public:
	sf::RectangleShape rect;
	std::string        id;
	CRectangle(){}
	CRectangle(sf::Vector2f size, sf::Color fill, sf::Color outline, float thickness) {
		rect.setSize(size);
		rect.setFillColor(fill);
		rect.setOutlineColor(outline);
		rect.setOutlineThickness(thickness);
		rect.setOrigin(size.x / 2, size.y / 2);
	}
};

class CCollision : public Component {
public:
	float radius = 0;
	CCollision(float r) : radius(r) {}
};

class CInput : public Component {
public:
	bool up    = false;
	bool left  = false;
	bool right = false;
	bool down  = false;
	bool shoot = false;
	bool canShoot = true; // when space is released
	bool canJump = false;

	CInput() {}
};

class CLifespan : public Component {
	// every frame we remove from remaining
public:
	int remaining = 0;  // amount of lifespan remaining on the entity
	int total     = 0;  // the total initial amount of lifespan
	CLifespan() {}
	CLifespan(int total) : remaining(total), total(total) {}
};

class CScore : public Component {
public:
	CScore() {}
};

class CBoundingBox : public Component {
public:
	sf::Vector2f size      = {0.0f, 0.0f};
	sf::Vector2f halfSize  = { 0.0f, 0.0f };
	CBoundingBox() {}
	CBoundingBox(const sf::Vector2f& s)
		: size(s), halfSize(s.x/2,s.y/2) 
	{

	}

};

class CAnimation : public Component {
public:
	Animation animation;
	bool repeat = false;
	CAnimation() {}
	CAnimation(Animation& a) : animation(a)
	{
		has = true;
	}

};

class CGravity : public Component {
	// gravity is added to the velocity
	// ctransform.velocity.y += gravity
public:
	float gravity = 0;
	CGravity() {}
	CGravity(float g) 
		: gravity(g)
	{
		has = true;
	}
};

// CState will determine the state and animation of the entity
class CState : public Component {
public:
	std::string state = "Jumping";
	CState() {}
	CState(const std::string & s) : state(s) {}
};

class CDraggable : public Component {
public:
	bool dragging = false;
	CDraggable() {}
	CDraggable(bool d)
		: dragging(d)
	{
		has = true;
	}
};