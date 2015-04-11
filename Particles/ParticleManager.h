#ifndef PARTICLEMANAGER_H_
#define PARTICLEMANAGER_H_

#include <list>
#include "ParticleSystem.h"
#include <string>

class ParticleManager
{
public:
	ParticleManager();

	void add(std::string name, sf::Texture* texture, sf::Vector2u frameSize, unsigned int maxParticles);
	void add(std::string name, sf::Texture* texture, sf::IntRect frame, unsigned int maxParticles);
	void add(std::string name, sf::Color color, float variance, sf::Vector2u size, unsigned int maxParticles);
	bool has(std::string name);
	void rem(std::string name);
	ParticleSystem* get(std::string name);
	void emit(std::string name, sf::Vector2f pos, sf::Vector2f vel, float lifetime, float scale = 1.0f);
	void update();
	void draw(sf::RenderTarget& window, sf::RenderStates states = sf::RenderStates::Default);

	std::list<std::pair<std::string, ParticleSystem> > systems;

};

#endif /* PARTICLEMANAGER_H_ */
