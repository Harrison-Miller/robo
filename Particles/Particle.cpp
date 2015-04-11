#include "Particle.h"

Particle::Particle(sf::Vector2f vel, float lifeTime, unsigned int num, float alpha)
{
	this->num = num;
	this->vel = vel;
	this->lifeTime = lifeTime;
	origLifeTime = lifeTime;
	isFree = false;
	this->alpha = alpha;

}

Particle::Particle(unsigned int num)
{
	this->num = num;
	isFree = true;

}

void Particle::update(sf::Vertex* vert, float dt)
{
	lifeTime -= dt;
	vert[0].position += vel * dt;
	vert[1].position += vel * dt;
	vert[2].position += vel * dt;
	vert[3].position += vel * dt;

	float ratio = lifeTime / origLifeTime;
	vert[0].color.a = static_cast<sf::Uint8>(ratio * alpha);
	vert[1].color.a = static_cast<sf::Uint8>(ratio * alpha);
	vert[2].color.a = static_cast<sf::Uint8>(ratio * alpha);
	vert[3].color.a = static_cast<sf::Uint8>(ratio * alpha);


}

bool Particle::isDead()
{
	return lifeTime <= 0;

}
