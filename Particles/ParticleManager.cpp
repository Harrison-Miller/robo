#include "ParticleManager.h"

ParticleManager::ParticleManager()
{
}

void ParticleManager::add(std::string name, sf::Texture* texture, sf::Vector2u frameSize, unsigned int maxParticles)
{
	if(has(name))
		return;

	systems.push_back(std::pair<std::string, ParticleSystem>(name, ParticleSystem(texture, frameSize, maxParticles)));

}

void ParticleManager::add(std::string name, sf::Texture* text, sf::IntRect frame, unsigned int maxParticles)
{
	if(has(name))
		return;

	systems.push_back(std::pair<std::string, ParticleSystem>(name, ParticleSystem(text, frame, maxParticles)));

}


void ParticleManager::add(std::string name, sf::Color color, float variance, sf::Vector2u size, unsigned int maxParticles)
{
	if(has(name))
		return;

	systems.push_back(std::pair<std::string, ParticleSystem>(name, ParticleSystem(color, variance, size, maxParticles)));

}

bool ParticleManager::has(std::string name)
{
	return get(name) != NULL;

}

void ParticleManager::rem(std::string name)
{
	std::list<std::pair<std::string, ParticleSystem> >::iterator it;
	for(it = systems.begin(); it != systems.end(); it++)
	{
		if(it->first == name)
		{
			systems.erase(it);
			return;

		}

	}

}

ParticleSystem* ParticleManager::get(std::string name)
{
	std::list<std::pair<std::string, ParticleSystem> >::iterator it;
	for(it = systems.begin(); it != systems.end(); it++)
	{
		if(it->first == name)
		{
			return &it->second;

		}

	}

	return NULL;

}
void ParticleManager::emit(std::string name, sf::Vector2f pos, sf::Vector2f vel, float lifetime, float scale)
{
	ParticleSystem* p = get(name);
	if(p == NULL)
	{
		return;

	}

	p->emit(pos, vel, lifetime, scale);

}

void ParticleManager::update()
{
	std::list<std::pair<std::string, ParticleSystem> >::iterator it;
	for(it = systems.begin(); it != systems.end(); it++)
	{
		it->second.update();

	}

}

void ParticleManager::draw(sf::RenderTarget& window, sf::RenderStates states)
{
	std::list<std::pair<std::string, ParticleSystem> >::iterator it;
	for(it = systems.begin(); it != systems.end(); it++)
	{
		it->second.draw(window, states);

	}

}
