#ifndef BUTTON_H_
#define BUTTON_H_

#include <SFML/Graphics.hpp>

class Button
{
public:
	Button(sf::Font& font, std::string str, sf::Vector2f pos,
			sf::Vector2f size)
	{
		sprite.setSize(size);
		sprite.setOrigin(size.x/2, size.y/2);
		sprite.setPosition(pos);
		sprite.setFillColor(sf::Color(160, 82, 45));
		sprite.setOutlineColor(sf::Color::Black);
		sprite.setOutlineThickness(2);

		highlight = sprite;
		highlight.setFillColor(sf::Color(255, 255, 0, 100));

		text.setFont(font);
		text.setString(str);
		text.setOrigin(text.getLocalBounds().width/2, text.getLocalBounds().height);
		text.setPosition(pos);

	}

	bool update(sf::RenderWindow& window, sf::View view)
	{
		sf::Vector2f mpos = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
		bool hovered = sprite.getGlobalBounds().contains(mpos);
		bool clicked  = sf::Mouse::isButtonPressed(sf::Mouse::Left);
		window.draw(sprite);
		window.draw(text);
		if(hovered)
			window.draw(highlight);

		return hovered && clicked;

	}

	sf::RectangleShape highlight;
	sf::RectangleShape sprite;
	sf::Text text;

};

#endif /* BUTTON_H_ */
