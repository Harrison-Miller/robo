#include "ParticleSystem.h"
#include <iostream>

ParticleSystem::ParticleSystem(sf::Texture* texture, sf::Vector2u frameSize, unsigned int maxParticles)
{
	this->texture = texture;
	this->frameSize = frameSize;
	this->maxParticles = maxParticles;
	verticies = sf::VertexArray(sf::Quads, maxParticles*4);
	for(unsigned int i = 0; i < maxParticles; i++)
	{
		particles.push_back(Particle(i));

	}
	useColor = false;
	useFrame = false;
	variance = 0;

}

ParticleSystem::ParticleSystem(sf::Texture* texture, sf::IntRect frame, unsigned int maxParticles)
{
	this->texture = texture;
	this->frame = frame;
	this->maxParticles = maxParticles;
	this->frameSize = sf::Vector2u(frame.width, frame.height);
	verticies = sf::VertexArray(sf::Quads, maxParticles*4);
	for(unsigned int i = 0; i < maxParticles; i++)
	{
		particles.push_back(Particle(i));

	}
	useColor = false;
	useFrame = true;
	variance = 0;
}

ParticleSystem::ParticleSystem(sf::Color color, float variance, sf::Vector2u size, unsigned int maxParticles)
{
	this->color = color;
	this->variance = variance;
	frameSize = size;
	this->maxParticles = maxParticles;
	verticies = sf::VertexArray(sf::Quads, maxParticles*4);
	for(unsigned int i = 0; i < maxParticles; i++)
	{
		particles.push_back(Particle(i));

	}
	useColor = true;
	useFrame = false;
	texture = NULL;

}

void ParticleSystem::update()
{
	float dt = deltaTimer.restart().asSeconds();
	for(unsigned int i = 0; i < particles.size(); i++)
	{
		sf::Vertex* vert = &verticies[particles[i].num*4];
		particles[i].update(vert, dt);
		if(particles[i].isDead())
		{
			resetParticle(particles[i]);

		}

	}

}

void ParticleSystem::resetParticle(Particle& p)
{
	p.isFree = true;
	sf::Vertex* vert = &verticies[p.num*4];
	vert[0] = sf::Vertex();
	vert[1] = sf::Vertex();
	vert[2] = sf::Vertex();
	vert[3] = sf::Vertex();


}

sf::IntRect ParticleSystem::getRandomFrame()
{
	unsigned int frame = 0;


	if(texture == NULL || frameSize.x == 0 || frameSize.y == 0)
		return sf::IntRect(0, 0, 0, 0);

	unsigned int x = texture->getSize().x/frameSize.x;
	unsigned int y = texture->getSize().y/frameSize.y;

	if(x == 0 || y == 0)
		return sf::IntRect(0, 0, 0, 0);

	unsigned int frames = x*y;
	frame = rand()%frames;

	if(frame > frames)
	{
		return sf::IntRect(0, 0, frameSize.x, frameSize.y);

	}

	unsigned int fx = frame%x;
	unsigned int fy = frame/x;
	fx *= frameSize.x;
	fy *= frameSize.y;

	return sf::IntRect(fx, fy, frameSize.x, frameSize.y);

}

void ParticleSystem::emit(sf::Vector2f pos, sf::Vector2f vel, float lifetime, float scale)
{
	for(unsigned int i = 0; i < particles.size(); i++)
	{
		if(particles[i].isFree)
		{
			emit(particles[i], pos, vel, lifetime, scale);
			return;

		}

	}


}

void ParticleSystem::emit(Particle& p, sf::Vector2f pos, sf::Vector2f vel, float lifetime, float scale)
{
	p = Particle(vel, lifetime, p.num);
	sf::Vertex* vert = &verticies[p.num*4];
	vert[0].position = pos;
	vert[1].position = sf::Vector2f(pos.x + (frameSize.x*scale), pos.y);
	vert[2].position = sf::Vector2f(pos.x + (frameSize.x*scale), pos.y + (frameSize.y*scale));
	vert[3] = sf::Vector2f(pos.x, pos.y + (frameSize.y*scale));

	if(useFrame)
	{
		vert[0].texCoords = sf::Vector2f(frame.left, frame.top);
		vert[1].texCoords = sf::Vector2f(frame.left + frame.width, frame.top);
		vert[2].texCoords = sf::Vector2f(frame.left + frame.width, frame.top + frame.top);
		vert[3].texCoords = sf::Vector2f(frame.left, frame.top + frame.top);


	}
	else if(!useColor)
	{
		sf::IntRect fframe = getRandomFrame();
		vert[0].texCoords = sf::Vector2f(fframe.left, fframe.top);
		vert[1].texCoords = sf::Vector2f(fframe.left + frameSize.x, fframe.top);
		vert[2].texCoords = sf::Vector2f(fframe.left + frameSize.x, fframe.top + frameSize.y);
		vert[3].texCoords = sf::Vector2f(fframe.left, fframe.top + frameSize.y);

	}
	else
	{
		p.alpha = (float)color.a;
		sf::Color ncolor = color;

		if(variance != 0)
		{
			/*float offset = game->randFrom(-variance/2, variance/2);
			color.r += static_cast<sf::Uint8>(offset);
			color.g += static_cast<sf::Uint8>(offset);
			color.b += static_cast<sf::Uint8>(offset);*/

		}

		vert[0].color = ncolor;
		vert[1].color = ncolor;
		vert[2].color = ncolor;
		vert[3].color = ncolor;

	}

}

void ParticleSystem::draw(sf::RenderTarget& window, sf::RenderStates& states)
{
	if(useColor)
	{
		window.draw(verticies);

	}
	else
	{
		states.texture = texture;
		window.draw(verticies, states);

	}

}

