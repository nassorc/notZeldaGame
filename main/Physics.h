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
};

