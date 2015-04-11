#ifndef PARTICLE_H_
#define PARTICLE_H_

#include <SFML/Graphics.hpp>

class Particle
{
public:
	Particle(sf::Vector2f vel, float lifeTime, unsigned int num, float alpha = 255);

	Particle(unsigned int num);

	void update(sf::Vertex* vert, float dt);

	bool isDead();

	unsigned int num;
	sf::Vector2f vel;
	float lifeTime;
	float origLifeTime;
	bool isFree;
	float alpha;

};


#endif /* PARTICLE_H_ */
