#ifndef FEEDBUFFER_H_
#define FEEDBUFFER_H_

#include <SFML/Graphics.hpp>

class FeedBuffer : public sf::Transformable
{
public:
	FeedBuffer(sf::Font& font,
			int charSize,
			float lifeTime,
			bool up = false) :
		font(font),
		charSize(charSize),
		lifeTime(lifeTime),
		up(up)
	{
	}

	void add(std::string info, sf::Color color)
	{
		sf::Text text(info, font, charSize);
		text.setColor(color);
		sf::Clock timer;
		texts.push_back(text);
		timers.push_back(timer);

	}

	void update()
	{
		for(unsigned int i = 0; i < timers.size(); i++)
		{
			if(timers[i].getElapsedTime().asSeconds() > lifeTime)
			{
				std::vector<sf::Text>::iterator textIt;
				textIt = texts.begin();
				textIt += i;
				texts.erase(textIt);

				std::vector<sf::Clock>::iterator timerIt;
				timerIt = timers.begin();
				timerIt += i;
				timers.erase(timerIt);
				break;

			}

		}

	}

	void draw(sf::RenderWindow& window, sf::RenderStates states = sf::RenderStates::Default)
	{
		states.transform *= getTransform();
		sf::Vector2f pos;
		for(unsigned int i = 0; i < texts.size(); i++)
		{
			texts[i].setPosition(pos);
			window.draw(texts[i], states);
			if(up)
			{
				pos.y -= texts[i].getLocalBounds().height + 2;

			}
			else
			{
				pos.y += texts[i].getLocalBounds().height + 2;

			}

		}

	}

	sf::Font& font;
	int charSize;
	float lifeTime;
	bool up;

	std::vector<sf::Text> texts;
	std::vector<sf::Clock> timers;

};

#endif /* FEEDBUFFER_H_ */
