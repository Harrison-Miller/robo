#ifndef TEXTBOX_H_
#define TEXTBOX_H_

#include <SFML/Graphics.hpp>
#include <string>

class TextBox : public sf::Transformable
{
public:
	TextBox(sf::Vector2f pos, sf::Font& font, sf::Vector2f size)
	{
		setPosition(pos);
		sf::Vector2f outerSize = size;
		outerSize.x += 10;
		outerSize.y += 10;
		outer.setSize(outerSize);
		outer.setOrigin(outerSize/2.0f);
		outer.setFillColor(sf::Color(160, 82, 45));
		outer.setOutlineColor(sf::Color::Black);
		outer.setOutlineThickness(2);
		inner.setSize(size);
		inner.setOrigin(size/2.0f);
		inner.setFillColor(sf::Color::White);

		cursor.setSize(sf::Vector2f(3, size.y));
		cursor.setOrigin(1.5, size.y/2);
		cursor.setFillColor(sf::Color::Black);
		cursor.setPosition(-size.x/2, 0);

		text.setFont(font);
		text.setColor(sf::Color::Black);
		text.setCharacterSize(20);
		text.setOrigin(0, size.y/2);
		text.setPosition(-size.x/2, 0);
		cursorHidden = false;
		retpressed = false;

	}

	bool update(sf::Event event)
	{
		retpressed = false;
		if(event.key.code == sf::Keyboard::Return)
		{
			retpressed = true;
			return true;

		}

		if(event.type == sf::Event::TextEntered)
		{
			if(event.text.unicode == 8) //backspace
			{
				realText = realText.substr(0, realText.size()-1);
				return false;

			}

			realText += (char)event.text.unicode;

		}

		return false;

	}

	bool draw(sf::RenderWindow& window,
			sf::RenderStates states = sf::RenderStates::Default) //returns true on enter
	{
		text.setString(realText);
		cursor.setPosition(-inner.getSize().x/2 + text.getGlobalBounds().width, 0);

		if(cursorTime.getElapsedTime().asSeconds() >= 0.5)
		{
			if(cursorHidden)
				cursor.setFillColor(sf::Color::Black);
			else
				cursor.setFillColor(sf::Color::Transparent);
			cursorHidden = !cursorHidden;
			cursorTime.restart();

		}

		states.transform *= getTransform();
		window.draw(outer, states);
		window.draw(inner, states);
		window.draw(text, states);
		window.draw(cursor, states);
		return retpressed;

	}

	std::string getString()
	{
		return text.getString();

	}

	void clear()
	{
		realText = "";
		cursor.setPosition(-inner.getSize().x/2, 0);
		text.setString("");

	}

	sf::RectangleShape outer;
	sf::RectangleShape inner;
	sf::RectangleShape cursor;
	bool cursorHidden;
	sf::Clock cursorTime;
	sf::Text text;
	std::string realText;
	bool retpressed;

};

#endif /* TEXTBOX_H_ */
