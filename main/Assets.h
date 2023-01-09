
#include "Animation.h"
#include <map>
#include <string>
#include <fstream>
#include <cstdlib>

typedef std::map<std::string, sf::Texture> TextureMap;
typedef std::map<std::string, Animation> AnimationMap;
typedef std::map<std::string, sf::Font> FontMap;

class Assets {
protected:
	TextureMap     m_textureMap;
	AnimationMap   m_animationMap;
	FontMap        m_fontMap;
public:
	Assets(){}

	void loadFromFile(const std::string& path) {
		std::ifstream fin(path, std::ios::in);
		std::cout << "Loading assets from " << path << std::endl;
		if (!fin) {
			std::cout << "File did not open" << std::endl;
			exit(EXIT_FAILURE);
		}

		std::string type;

		while (fin >> type) {
			if (type == "Texture") {
				std::string name, path;
				fin >> name >> path;
				std::cout << name << " @ " << path << std::endl;
				addTexture(name, path);
			}
			else if (type == "Animation") {
				// create animation object
				std::string aniName, texName;
				size_t frameCount, speed;
				fin >> aniName >> texName >> frameCount >> speed;
				m_animationMap[aniName] = Animation(aniName, m_textureMap[texName], frameCount, speed);
			}
		}
	}

	void addTexture(const std::string& name, const std::string& path){
		sf::Texture tex;
		if (!tex.loadFromFile(path)) {
			std::cout << "couldn't load texture " << name << std::endl;
		}

		m_textureMap[name] = tex;
	}
	void addAnimation(const std::string& name, Animation animation) {}
	void addFont(const std::string& name, const std::string& path) {}

	sf::Texture& getTexture(const std::string& name) {
		return m_textureMap[name];
	}
	Animation & getAnimation(const std::string& name, bool b) {
		return m_animationMap[name];
	}
	AnimationMap getAnimations() {
		return m_animationMap;
	}
	sf::Font& getFont(const std::string& name) {}

	size_t getTextureMapSize() const {
		return m_textureMap.size();
	}

};