#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include <SFML/Graphics.hpp>
#include "Particle.h"

class ParticleSystem
{
public:
	ParticleSystem(sf::Texture* texture, sf::Vector2u frameSize, unsigned int maxParticles);
	ParticleSystem(sf::Texture* texture, sf::IntRect frame, unsigned int maxParticles);
	ParticleSystem(sf::Color color, float variance, sf::Vector2u size, unsigned int maxParticles);

	void update();

	void emit(sf::Vector2f pos, sf::Vector2f vel, float lifetime, float scale);

	void draw(sf::RenderTarget& window, sf::RenderStates& states = (sf::RenderStates&)sf::RenderStates::Default);

private:
	void resetParticle(Particle& p);
	sf::IntRect getRandomFrame();
	void emit(Particle& p, sf::Vector2f pos, sf::Vector2f vel, float lifetime, float scale);

	sf::VertexArray verticies;
	std::vector<Particle> particles;
	sf::Texture* texture;
	sf::Vector2u frameSize;
	unsigned int maxParticles;
	sf::Clock deltaTimer;
	bool useColor;
	sf::Color color;
	float variance;
	sf::IntRect frame;
	bool useFrame;

};

#endif /* PARTICLESYSTEM_H_ */
