#pragma once

#include "Components.h"
#include <cmath>
// will access components of entity
class Physics {
public:
	sf::Vector2f GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
		// TODO: return the overlap rectangle size of the bounding boxes of entity a and b
		// first, check if a and b has bounding boxes and transform component
		// then compute and return vec
		
		sf::Vector2f overlap;

		auto& posA = a->getComponent<CTransform>().pos;
		auto& posB = b->getComponent<CTransform>().pos;
		auto& hSizeA = a->getComponent<CBoundingBox>().halfSize;
		auto& hSizeB = b->getComponent<CBoundingBox>().halfSize;

		// distance between entity a and b in the x and y direction
		sf::Vector2f delta{ abs(posA.x - posB.x), abs(posA.y - posB.y) };

		// compute overlap in x-direction and y-direction
		float ox{ (hSizeA.x + hSizeB.x) - delta.x };
		float oy{ (hSizeA.y + hSizeB.y) - delta.y };

		overlap.x = ox;
		overlap.y = oy;

		return overlap;

	
	}

	sf::Vector2f GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
		// HINT: use definiton of GetOVerlap but use prevPosition instead
		// TODO: return the previous overlap rectangle size of the bounding boxes of entity a and b
		//       previous overlap uses the entity's previous position
		sf::Vector2f overlap;

		auto& posA = a->getComponent<CTransform>().prevPos;
		auto& posB = b->getComponent<CTransform>().pos;
		auto& hSizeA = a->getComponent<CBoundingBox>().halfSize;
		auto& hSizeB = b->getComponent<CBoundingBox>().halfSize;
		// solve dx and dy, distance between entity a and b
		sf::Vector2f delta{ abs(posA.x - posB.x), abs(posA.y - posB.y) };

		// compute overlap in x-direction and y-direction
		float ox{ (hSizeA.x + hSizeB.x) - delta.x };
		float oy{ (hSizeA.y + hSizeB.y) - delta.y };

		overlap.x = ox;
		overlap.y = oy;

		return sf::Vector2f(ox, oy);
	}

	// physics
	struct Intersect {
		bool result;
		sf::Vector2f pos;
	};

	Intersect LineIntersect(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d) {
		// get difference 
		sf::Vector2f r = (b - a);
		sf::Vector2f s = (d - c);

		// calculate cross product 
		float rxs = r.x * s.y - r.y * s.x;

		sf::Vector2f cma = c - a;

		// scalar value multiplied to line segment
		float t = (cma.x * s.y - cma.y * s.x) / rxs;
		float u = (cma.x * r.y - cma.y * r.x) / rxs;

		// if 0<=t<=1, then instersection between line segments
		if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
			return { true, sf::Vector2f(a.x + t * r.x, a.y + t * r.y) };
		}

		return { false, sf::Vector2f(0,0) };
	}

	// test whether line segment AB intersects with any lines of the bounding box of Entity e
	bool EntityIntersect(const sf::Vector2f& a, const sf::Vector2f& b, std::shared_ptr<Entity> e) {

		sf::Vector2f e1, e2, e3, e4;

		sf::Vector2f pos = e->getComponent<CTransform>().pos;
		sf::Vector2f hs = sf::Vector2f(e->getComponent<CAnimation>().animation.getSize().x/2, e->getComponent<CAnimation>().animation.getSize().y / 2);


		e1 = sf::Vector2f(pos.x - hs.x, pos.y - hs.y);
		e2 = sf::Vector2f(pos.x + hs.x, pos.y - hs.y);
		e3 = sf::Vector2f(pos.x - hs.x, pos.y + hs.y);
		e4 = sf::Vector2f(pos.x + hs.x, pos.y + hs.y);


		Intersect i1, i2, i3, i4;

		i1 = LineIntersect(a, b, e1, e2);

		i2 = LineIntersect(a, b, e2, e3);
		i3 = LineIntersect(a, b, e3, e4);
		i4 = LineIntersect(a, b, e4, e1);

		std::cout << i1.pos.x << " " << i1.pos.y << std::endl;
		std::cout << i2.pos.x << " " << i2.pos.y << std::endl;

		std::cout << i3.pos.x << " " << i3.pos.y << std::endl;

		std::cout << i4.pos.x << " " << i4.pos.y << std::endl;



		return i1.result || i2.result || i3.result || i4.result;
	}
};

