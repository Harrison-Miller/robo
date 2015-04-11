#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <math.h>
#include <iostream>
#include "Particles/ParticleManager.h"
#include <fstream>
#include <sstream>
#include <enet/enet.h>
#include <list>
#include "FeedBuffer.h"
#include "Button.h"
#include <stdlib.h>
#include <SFML/System.hpp>
#include <assert.h>
#include <stdint.h>
#include <SFML/System.hpp>

#ifdef __linux__
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
pid_t child_process = -1;
#elif _WIN32
#include <windows.h>
STARTUPINFO si;
PROCESS_INFORMATION pi;
bool spawned = false;
#endif

/*TODO: list
 * 1) ///DONE:smooth movement of proxy////
 * 			-I don't know if this truely works as I haven't been able to test with lag
 * 6) /////DONE-ISHcompress all the packets/////
 * 7) figure out if I can send less input packet stuff
 * 9) look into syncing the bullets better
 * 12) add a better menu system with a way to change your name set the server
 * 14) add server port to the server browser
 * 16) get different reload sounds
 * 17) get some hurt sounds
 * 18) //////DONE: add logging so it's easier to find skinneys bug and others/////
 * 			- maybe to conserviative on the messages ( might need to add more)
 * 19) ///DONE:smooth local players movement when replaying////
 * 			- I think I have, I haven't really gotten any major dsync from it yet
 * 20) /////DONE: make it so players appear in the tab list even if dead /////
 * 			- unfortunately they have to spawn at least once
 * 22) add more team specific info - like whoes core stolen - point to core if it's stolen
 * 24) add spec controls for when dead
 * 25) collate more information into a single packet
 * 26) in init command sync who has the flag
 */

typedef enum NetCommand
{
	None,
	Join,
	NameTaken,
	Init,
	Spawn,
	Leave,
	Shoot,
	Kill,
	SyncInput,
	Sync,
	CoreStolen,
	CoreDropped,
	CoreReturned,
	CoreCaptured,
	GameOver

} NetCommand;

std::vector<sf::RectangleShape> mapRects;
sf::CircleShape fountainShape;
void loadMap()
{
	sf::RectangleShape shape;
	shape.setFillColor(sf::Color::Transparent);
	shape.setOutlineColor(sf::Color::Red);
	shape.setOutlineThickness(2);

	//boundaries
	shape.setPosition(-100, -100);
	shape.setSize(sf::Vector2f(4200, 100));
	mapRects.push_back(shape);

	shape.setPosition(-100, -100);
	shape.setSize(sf::Vector2f(100, 1800));
	mapRects.push_back(shape);

	shape.setPosition(-100, 1536);
	shape.setSize(sf::Vector2f(4200, 100));
	mapRects.push_back(shape);

	shape.setPosition(4096, -100);
	shape.setSize(sf::Vector2f(100, 1800));
	mapRects.push_back(shape);

	//crates on the left
	shape.setPosition(327, 1083);
	shape.setSize(sf::Vector2f(144, 74));
	mapRects.push_back(shape);

	shape.setPosition(483, 692);
	shape.setSize(sf::Vector2f(75, 152));
	mapRects.push_back(shape);

	shape.setPosition(558, 669);
	shape.setSize(sf::Vector2f(76, 227));
	mapRects.push_back(shape);

	shape.setPosition(360, 364);
	shape.setSize(sf::Vector2f(75, 75));
	mapRects.push_back(shape);

	//left building horizontal pieces
	shape.setPosition(887, 1448);
	shape.setSize(sf::Vector2f(850, 90));
	mapRects.push_back(shape);

	shape.setPosition(1312, 1184);
	shape.setSize(sf::Vector2f(423, 85));
	mapRects.push_back(shape);

	shape.setPosition(897, 329);
	shape.setSize(sf::Vector2f(161, 83));
	mapRects.push_back(shape);

	shape.setPosition(1238, 329);
	shape.setSize(sf::Vector2f(157, 83));
	mapRects.push_back(shape);

	//left building vertical pieces
	shape.setPosition(897, 331);
	shape.setSize(sf::Vector2f(83, 334));
	mapRects.push_back(shape);

	shape.setPosition(1313, 331);
	shape.setSize(sf::Vector2f(83, 334));
	mapRects.push_back(shape);

	shape.setPosition(1311, 931);
	shape.setSize(sf::Vector2f(83, 334));
	mapRects.push_back(shape);

	shape.setPosition(890, 943);
	shape.setSize(sf::Vector2f(83, 253));
	mapRects.push_back(shape);

	shape.setPosition(887, 1310);
	shape.setSize(sf::Vector2f(83, 226));
	mapRects.push_back(shape);

	//crates in left building
	shape.setPosition(1239, 532);
	shape.setSize(sf::Vector2f(75, 75));
	mapRects.push_back(shape);

	shape.setPosition(1210, 611);
	shape.setSize(sf::Vector2f(75, 75));
	mapRects.push_back(shape);

	shape.setPosition(977, 1047);
	shape.setSize(sf::Vector2f(75, 75));
	mapRects.push_back(shape);

	//water tower
	shape.setPosition(2221, 161);
	shape.setSize(sf::Vector2f(254, 254));
	mapRects.push_back(shape);

	//crates in courtyard
	shape.setPosition(1956, 390);
	shape.setSize(sf::Vector2f(75, 75));
	mapRects.push_back(shape);

	shape.setPosition(2690, 1045);
	shape.setSize(sf::Vector2f(75, 75));
	mapRects.push_back(shape);

	shape.setPosition(2396, 1323);
	shape.setSize(sf::Vector2f(75, 75));
	mapRects.push_back(shape);

	shape.setPosition(2316, 1286);
	shape.setSize(sf::Vector2f(79, 151));
	mapRects.push_back(shape);

	//right side building
	shape.setPosition(2972, 411);
	shape.setSize(sf::Vector2f(83, 254));
	mapRects.push_back(shape);

	shape.setPosition(3263, 409);
	shape.setSize(sf::Vector2f(83, 254));
	mapRects.push_back(shape);

	shape.setPosition(2969, 940);
	shape.setSize(sf::Vector2f(83, 254));
	mapRects.push_back(shape);

	shape.setPosition(3265, 939);
	shape.setSize(sf::Vector2f(83, 254));
	mapRects.push_back(shape);

	//shipping container
	shape.setPosition(3574, 1167);
	shape.setSize(sf::Vector2f(145, 368));
	mapRects.push_back(shape);

	//crates at right base
	shape.setPosition(3697, 937);
	shape.setSize(sf::Vector2f(155, 77));
	mapRects.push_back(shape);

	shape.setPosition(3563, 556);
	shape.setSize(sf::Vector2f(75, 75));
	mapRects.push_back(shape);

	shape.setPosition(3602, 710);
	shape.setSize(sf::Vector2f(75, 75));
	mapRects.push_back(shape);

	shape.setPosition(3563, 631);
	shape.setSize(sf::Vector2f(155, 78));
	mapRects.push_back(shape);

	//fountain
	shape.setPosition(2298, 800);
	shape.setSize(sf::Vector2f(48, 48));
	mapRects.push_back(shape);

	fountainShape.setRadius(182.5);
	fountainShape.setOrigin(182.5, 182.5);
	fountainShape.setPosition(2299+24, 824);
	fountainShape.setFillColor(sf::Color::Transparent);
	fountainShape.setOutlineColor(sf::Color::Blue);
	fountainShape.setOutlineThickness(1);

}

unsigned int ai_testers = 4;

bool hasFocus = true;

//SETTINGS STUFF
std::string cl_username;
unsigned int sv_port;
std::string sv_ip;
bool g_isServer = false;

int redScore = 0;
int blueScore = 0;

sf::Texture charactersText;
sf::Texture bulletsText;
sf::Texture redGib;
sf::Texture brownGib;
sf::Texture greenGib;
sf::Texture sniperGib;
sf::Texture coreText;

sf::Texture mapText;
sf::Texture treeText;

sf::Shader waveShader;
sf::Shader bloomShader;

sf::SoundBuffer assault1Buffer;
sf::Sound assault1Sound;

sf::SoundBuffer assault2Buffer;
sf::Sound assault2Sound;

sf::SoundBuffer assault3Buffer;
sf::Sound assault3Sound;

sf::SoundBuffer sniperBuffer;
sf::Sound sniperSound;

sf::SoundBuffer reloadBuffer;
sf::Sound reloadSound;

sf::SoundBuffer btfrBuffer;
sf::Sound btfr;
sf::SoundBuffer bthtfBuffer;
sf::Sound bthtf;
sf::SoundBuffer btsBuffer;
sf::Sound bts;
sf::SoundBuffer gameOverBuffer;
sf::Sound gameOverSfx;
sf::SoundBuffer rtfrBuffer;
sf::Sound rtfr;
sf::SoundBuffer rthtfBuffer;
sf::Sound rthtf;
sf::SoundBuffer rtsBuffer;
sf::Sound rts;

sf::Font font;

sf::Vector2f camPos;
float zoom = 1.5;

ParticleManager particles;

sf::RectangleShape blueCore;
sf::RectangleShape redCore;

sf::Clock blueReturn;
sf::Clock redReturn;

sf::Vector2f blueCoreSpawn(675, 777);
sf::Vector2f redCoreSpawn(3560, 777);

bool goodCoreStolen = false;
bool badCoreStolen = false;

uint16_t nextNetID;

FeedBuffer gameInfo(font, 12, 10.0, false);

std::ofstream logFile;

sf::View guiView(sf::FloatRect(0, 0, 854, 480));

float clamp(float n, float lower, float upper)
{
	return std::max(lower, std::min(n, upper));

}

void playSound(sf::Sound& sound, sf::Vector2f pos, float max)
{
	sf::Vector2f dif = camPos - pos;
	float dist = sqrt(pow(dif.x, 2) + pow(dif.y, 2));
	float ratio = dist/2000;
	ratio = clamp(ratio, 0.0, 1.0);
	ratio = 1 - ratio;
	sound.setVolume(max*ratio);
	sound.play();

}


std::string toString(int i)
{
	std::stringstream ss;
	ss << i;
	std::string s;
	ss >> s;
	return s;

}

sf::Vector2f lerp(sf::Vector2f current, sf::Vector2f target, float fac = 0.16)
{
	return current + (target - current) * fac;

}

float lerp(float current, float target, float fac = 0.16)
{
	return current + (target - current) * fac;

}

class Gib : public sf::Transformable
{
public:
	Gib(sf::Texture& texture, sf::IntRect frame, float scale, sf::Vector2f pos,
			float radius, float angle, float speed, float life) :
				life(life)
	{
		shape.setRadius(radius);
		shape.setFillColor(sf::Color::Transparent);
		shape.setOutlineColor(sf::Color::Red);
		shape.setOutlineThickness(1);
		shape.setOrigin(radius, radius);

		set(pos, angle, speed);

		sprite.setTexture(&texture);
		sprite.setSize(sf::Vector2f(frame.width*scale, frame.height*scale));
		sprite.setOrigin(frame.width*scale/2, frame.height*scale/2);
		sprite.setRotation(rand()%360);
		sprite.setTextureRect(frame);

		dead = false;
		lifeTimer.restart();
		sprite.setFillColor(sf::Color::White);
		collideable = true;
		bloody = true;
		bloodCount = 0;

	}

	void set(sf::Vector2f pos, float angle, float speed)
	{
		setPosition(pos);
		setRotation(angle+90);
		vel.x = speed*cos(angle/180*3.14159);
		vel.y = speed*sin(angle/180*3.14159);
		rotVel = speed/2;
		lifeTimer.restart();
		sprite.setFillColor(sf::Color::White);

	}

	sf::FloatRect getBounds()
	{
		sf::FloatRect bounds = shape.getGlobalBounds();
		bounds.left += getPosition().x;
		bounds.top += getPosition().y;
		return bounds;

	}

	void update()
	{
		move(vel);
		vel *= 0.93f;
		sprite.rotate(rotVel);
		rotVel *= 0.93;

		float t = lifeTimer.getElapsedTime().asSeconds();
		//float ratio = t/life;
		/*sf::Color c = sprite.getFillColor();
		c.a *= ratio;
		sprite.setFillColor(c);*/

		if(t > life)
		{
			dead = true;

		}

		if(bloody)
		{
			bloodCount++;
			if(bloodCount%10 == 0)
			{
				float angle = rand()%360;
				float speed = rand()%10+20;
				sf::Vector2f v(speed*cos(angle*3.14159/180), speed*sin(angle*3.14159/180));
				particles.emit("blood", getPosition(), v, 1.0, 1.0);

			}

		}

	}

	float getRadius()
	{
		return shape.getRadius();

	}

	void draw(sf::RenderWindow& window,
			sf::RenderStates states = sf::RenderStates::Default)
	{
		states.transform *= getTransform();
		window.draw(shape, states);
		window.draw(sprite, states);

	}


	sf::RectangleShape sprite;
	sf::CircleShape shape;
	sf::Vector2f vel;
	sf::Clock lifeTimer;
	float rotVel;
	bool dead;
	float life;
	bool collideable;
	bool bloody;
	int bloodCount;

};

class Bullet : public sf::Transformable
{
public:
	typedef enum Team
	{
		Good = 0,
		Bad,
		Neither

	} Team;

	Bullet(Team team, sf::Vector2f pos, float angle, float speed, int damage,
			sf::IntRect frame, sf::Vector2f origin, float scale) :
				team(team),
				damage(damage),
				speed(speed)
	{
		dead = false;

		shape.setRadius(6);
		shape.setFillColor(sf::Color::Transparent);
		shape.setOutlineColor(sf::Color::Red);
		shape.setOutlineThickness(1);
		shape.setOrigin(6, 6);


		sprite.setSize(sf::Vector2f(frame.width*scale, frame.height*scale));
		sprite.setOrigin(origin*scale);
		sprite.setTexture(&bulletsText);
		sprite.setTextureRect(frame);

		set(pos, angle);
		life = 4*30*10; //2 seconds with 4 updates a frame

	}

	void set(sf::Vector2f pos, float angle)
	{
		setPosition(pos);
		setRotation(angle+90);
		vel.x = speed*cos(angle/180*3.14159);
		vel.y = speed*sin(angle/180*3.14159);

	}

	void update(float ratio)
	{
		move(vel*ratio);
		life--;
		if(life <= 0)
			dead = true;

	}

	float getRadius()
	{
		return shape.getRadius();

	}

	sf::FloatRect getBounds()
	{
		sf::FloatRect bounds = shape.getGlobalBounds();
		bounds.left += getPosition().x;
		bounds.top += getPosition().y;
		return bounds;

	}

	void draw(sf::RenderWindow& window,
			sf::RenderStates states = sf::RenderStates::Default)
	{
		states.transform *= getTransform();
		window.draw(shape, states);
		states.shader = &bloomShader;
		bloomShader.setParameter("ratio", 0.25);
		bloomShader.setParameter("alpha", 0.12);
		window.draw(sprite, states);

	}

	Team team;
	sf::Vector2f vel;
	int damage;
	float speed;

	sf::RectangleShape sprite;
	sf::CircleShape shape;

	int life;
	bool dead;

	std::string owner;

};

class Input
{
public:
	Input() :
		up(false),
		down(false),
		left(false),
		right(false),
		shoot(false),
		reload(false),
		aimAngle(0)
	{
	}

	void set(sf::RenderWindow& window, sf::View view, sf::Vector2f pos)
	{
		if(hasFocus)
		{
			mPos = sf::Mouse::getPosition(window);
			sf::Vector2f aimPos = window.mapPixelToCoords(mPos, view);
			aimPos -= pos;
			aimAngle = (-atan2(aimPos.x, aimPos.y)*180/3.14159)+180;
			aimAngle = (unsigned int)aimAngle/2.0;
			aimAngle *= 2.0;

			up = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
			down = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
			left = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
			right = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
			shoot = sf::Mouse::isButtonPressed(sf::Mouse::Left);
			reload = sf::Keyboard::isKeyPressed(sf::Keyboard::R);

		}

	}

	void pack(sf::Packet& packet)
	{
		//2 bytes
		uint8_t flags = 0;
		flags = up ? flags|(1<<0) : flags;
		flags = down ? flags|(1<<1) : flags;
		flags = left ? flags|(1<<2) : flags;
		flags = right ? flags|(1<<3) : flags;
		uint8_t r = (unsigned int)(aimAngle/2.0);
		packet << flags << r;

	}

	void unpack(sf::Packet& packet)
	{
		uint8_t flags = 0;
		uint8_t r = 0;
		packet >> flags >> r;
		up = (flags&(1<<0)) != 0;
		down = (flags&(1<<1)) != 0;
		left = (flags&(1<<2)) != 0;
		right = (flags&(1<<3)) != 0;
		aimAngle = (float)(((unsigned int)r)*2.0);

	}

	bool operator==(const Input& rhs)
	{
		return up == rhs.up && down == rhs.down
				&& left == rhs.left && right == rhs.right;

	}

	bool operator!=(const Input& rhs)
	{
		return up != rhs.up || down != rhs.down
				|| left != rhs.left || right != rhs.right;

	}

	//sent
	bool up;
	bool down;
	bool left;
	bool right;

	//not sent
	bool shoot;
	bool reload;

	//sent
	float aimAngle;

	//not sent
	sf::Vector2i mPos;

};

class Move
{
public:
	Move()
	{
		t = 0;

	}

	Move(Input input, sf::Vector2f pos, unsigned int t) :
		input(input),
		pos(pos),
		t(t)
	{
	}

	void pack(sf::Packet& packet)
	{
		//5 bytes - can I make t be wrapping so I can lower it's byte size
		uint8_t flags = 0;
		flags = input.up ? flags|(1<<0) : flags;
		flags = input.down ? flags|(1<<1) : flags;
		flags = input.left ? flags|(1<<2) : flags;
		flags = input.right ? flags|(1<<3) : flags;
		packet << t << flags;

	}

	void unpack(sf::Packet& packet)
	{
		uint8_t flags = 0;
		packet >> t >> flags;
		input.up = (flags&(1<<0)) != 0;
		input.down = (flags&(1<<1)) != 0;
		input.left = (flags&(1<<2)) != 0;
		input.right = (flags&(1<<3)) != 0;

	}

	Input input;
	sf::Vector2f pos;
	unsigned int t;

};

class CircularBuffer
{
public:
	int head;
	int tail;

	CircularBuffer()
	{
		head = 0;
		tail = 0;

	}

	void resize(int size)
	{
		head = 0;
		tail = 0;
		moves.reserve(size);
		moves.resize(size);

	}

	int size()
	{
		int count = head - tail;
		if (count < 0)
			count += (int)moves.size();
		return count;

	}

	void add(const Move &move)
	{
		moves[head] = move;
		next(head);

	}

	void remove()
	{
		assert(!empty());
		next(tail);

	}

	Move& oldest()
	{
		assert(!empty());
		return moves[tail];

	}

	Move& newest()
	{
		assert(!empty());
		int index = head-1;
		if (index==-1)
			index = (int) moves.size() - 1;
		return moves[index];

	}

	bool empty() const
	{
		return head==tail;

	}

	void next(int &index)
	{
		index ++;
		if (index>=(int)moves.size())
			index -= (int)moves.size();

	}

	void previous(int &index)
	{
		index --;
		if (index<0)
		index += (int)moves.size();

	}

	Move& operator[](int index)
	{
		assert(index>=0);
		assert(index<(int)moves.size());
		return moves[index];

	}

	std::vector<Move> moves;

};

sf::RectangleShape red;

class Character : public sf::Transformable
{
public:
	typedef enum PlayerType
	{
		Local,
		Net,
		AI

	} PlayerType;

	Character(PlayerType playerType,
			Bullet::Team team,
			sf::Vector2f pos,
			float radius,
			sf::IntRect frame,
			float scale,
			sf::Vector2f origin,
			sf::Vector2f muzzle,
			sf::IntRect flareFrame,
			float accel,
			float maxSpeed,
			float fric,
			float mass,
			float shootSpeed,
			Bullet bullet,
			sf::Sound fireSound,
			int magSize,
			float reloadTime) :
				playerType(playerType),
				team(team),
				accel(accel),
				maxSpeed(maxSpeed),
				fric(fric),
				mass(mass),
				muzzle(muzzle),
				shootSpeed(shootSpeed),
				bullet(bullet),
				fireSound(fireSound),
				magSize(magSize),
				reloadTime(reloadTime)
	{
		netId = 0;
		if(g_isServer)
		{
			netId = nextNetID;
			nextNetID++;

		}

		setPosition(pos);

		if(playerType == AI)
		{
			nextPos.x = rand()%4096;
			nextPos.y = rand()%1536;

		}
		else
		{
			nextPos = pos;

		}

		shooting = false;
		shape.setRadius(radius);
		shape.setFillColor(sf::Color::Transparent);
		shape.setOutlineThickness(1);
		shape.setOrigin(radius, radius);

		sprite.setSize(sf::Vector2f(frame.width*scale, frame.height*scale));
		sprite.setOrigin(origin*scale);
		sprite.setTexture(&charactersText);
		sprite.setTextureRect(frame);

		smoothed = sprite;
		smoothed.setFillColor(sf::Color(255, 255, 255, 80));

		flare.setSize(sf::Vector2f(flareFrame.width*scale*0.5,
				flareFrame.height*scale*0.5));

		flare.setOrigin((origin.x - muzzle.x + (flareFrame.width*scale*0.45))*scale,
				(origin.y - muzzle.y + (flareFrame.height*scale*0.80))*scale );

		flare.setTexture(&bulletsText);
		flare.setTextureRect(flareFrame);
		flare.setFillColor(sf::Color::Transparent);
		flareAngle = 0;

		bulletsLeft = magSize;
		reloading = false;

		nameTag.setPosition(0, -76);
		nameTag.setFont(font);
		nameTag.setCharacterSize(25);
		pingText.setPosition(0, -96);
		pingText.setFont(font);
		pingText.setCharacterSize(25);
		if(playerType == Local)
			healthBar.setPosition(70, 460);
		else
			healthBar.setPosition(0, -36);
		healthBar.setOutlineColor(sf::Color::Black);
		healthBar.setOutlineThickness(1);

		if(team == Bullet::Good)
		{
			nameTag.setColor(sf::Color::Cyan);
			pingText.setColor(sf::Color::Cyan);
			shape.setOutlineColor(sf::Color::Cyan);
			healthBar.setFillColor(sf::Color(0, 255, 255, 75));

		}
		else
		{
			nameTag.setColor(sf::Color::Red);
			pingText.setColor(sf::Color::Red);
			shape.setOutlineColor(sf::Color::Red);
			healthBar.setFillColor(sf::Color(255, 0, 0, 75));

		}

		setHealth(100);
		variance = 6;
		data = NULL;
		hasCore = false;
		tframe = 0;

		moves.resize(200);
		importantMoves.resize(200);
		replaying = false;
		classNum = 0;

	}

	void update()
	{
		shooting = false;

		if(playerType == Local)
		{
			/*add the new move to the circular buffers*/
			if(!replaying)
			{
				Move move(input, getPosition(), tframe);
				if(!moves.empty())
				{
					Move& prev = moves.newest();
					if(move.input != prev.input)
					{
						importantMoves.add(move);

					}

				}
				else
				{
					importantMoves.add(move);

				}

				moves.add(move);

			}

			tframe++;

		}

		move(vel);

		float tmaxSpeed = hasCore ? maxSpeed/2 : maxSpeed;

		vel *= fric;
		if(fabs(vel.x) > tmaxSpeed)
		{
			vel.x = vel.x/fabs(vel.x)*tmaxSpeed;

		}

		if(fabs(vel.y) > tmaxSpeed)
		{
			vel.y = vel.y/fabs(vel.y)*tmaxSpeed;

		}

		sf::Color flareColor = flare.getFillColor();
		if(flareColor.a > 0)
		{
			if(flareColor.a >= 50)
				flareColor.a -= 50;
			else
				flareColor.a = 0;

			flare.setFillColor(flareColor);

		}

		sf::Vector2f pos = getPosition();
		/*pos.x = (int)pos.x;
		pos.y = (int)pos.y;
		setPosition(pos);*/
		if(hasCore)
		{
			if(team == Bullet::Good)
			{
				redCore.setPosition(pos);
				redCore.setRotation(sprite.getRotation() + 90);

			}
			else if(team == Bullet::Bad)
			{
				blueCore.setPosition(pos);
				blueCore.setRotation(sprite.getRotation()+ 90);

			}

		}

		float taccel = hasCore ? accel/2 : accel;

		if(reloading && shootTimer.getElapsedTime().asSeconds() > reloadTime)
		{
			bulletsLeft = magSize;
			reloading = false;

		}

		if(playerType == Local)
		{
			if(!hasFocus)
				return;

			if(!replaying)
			{
				sf::Vector2i mPos = input.mPos;
				float rot = input.aimAngle;
				sprite.setRotation(rot);
				flare.setRotation(rot+flareAngle);
				mPos.x = clamp(mPos.x, 0.0, 854.0) - 427;
				mPos.y = clamp(mPos.y, 0.0, 480.0) - 240;
				camPos = pos;
				camPos.x -= 854/2 - mPos.x/1.5;
				camPos.y -= 480/2 - mPos.y/1.5;

				if(classNum == 3)
					zoom = 2;
				else
					zoom = 1.5;

			}

			if(input.up)
			{
				vel.y -= taccel;

			}

			if(input.down)
			{
				vel.y += taccel;

			}

			if(input.left)
			{
				vel.x -= taccel;

			}

			if(input.right)
			{
				vel.x += taccel;

			}

			if(!reloading && input.shoot
				&& shootTimer.getElapsedTime().asSeconds() > shootSpeed)
			{

				if(bulletsLeft <= 0)
				{
					reloadSound.play();
					reloading = true;
					shootTimer.restart();

				}
				else
				{
					shooting = true;
					bulletsLeft--;
					flareAngle = (rand()%variance)-variance/2;
					flare.setFillColor(sf::Color(255, 255, 255, 255));
					shootTimer.restart();

				}

			}

			if(input.reload && !reloading && bulletsLeft != magSize)
			{
				reloadSound.play();
				reloading = true;
				shootTimer.restart();

			}

		}
		else if(playerType == AI)
		{
			if(aiTimer.getElapsedTime().asSeconds() > 5)
			{
				nextPos.x = rand()%4096;
				nextPos.y = rand()%1536;
				aiTimer.restart();

			}

			if(nextPos.y < pos.y)
			{
				vel.y -= taccel;

			}

			if(nextPos.y > pos.y)
			{
				vel.y += taccel;

			}

			if(nextPos.x < pos.x)
			{
				vel.x -= taccel;

			}

			if(nextPos.x > pos.x)
			{
				vel.x += taccel;

			}

		}
		else
		{
			sf::Vector2f pos = getPosition();
			if(pos != nextPos)
			{
				/*std::cout << "iterpolate to: " << nextPos.x << ", " << nextPos.y << "\n";
				std::cout << "from: " << pos.x << ", " << pos.y << "\n";*/
				pos += (nextPos - pos) * smoothFac;
				setPosition(pos);
				smoothFac += (1.0 - smoothFac) * 0.15f;

			}

			//setPosition(lerp(getPosition(), nextPos, smoothFac));

			/*float d = nextRot - getRotation();
			if(d > 1.5)
			{
				float l = lerp(nextRot, getRotation());
				sprite.setRotation(l);
				flare.setRotation(l);

			}*/

			//

		}

	}

	void draw(sf::RenderWindow& window,
			sf::RenderStates states = sf::RenderStates::Default)
	{
		states.transform *= getTransform();
		window.draw(shape, states);

		/*states.shader = &bloomShader;
		float ratio = (float)flare.getFillColor().a/255.0;
		bloomShader.setParameter("ratio", 0.025);
		bloomShader.setParameter("alpha", ratio);*/
		window.draw(flare, states);

		states.shader = &waveShader;
		waveShader.setParameter("wave_phase", waveTimer.getElapsedTime().asSeconds());
		waveShader.setParameter("wave_amplitude", 0.01*40, 0.01*40);
		waveShader.setParameter("blur_radius", (0.2) * 0.008f);
		window.draw(sprite, states);

		//if(playerType == Local)
			window.draw(smoothed);

		states.shader = NULL;
		window.draw(nameTag, states);
		window.draw(pingText, states);

		if(playerType == Local)
		{
			sf::View v = window.getView();
			window.setView(guiView);
			window.draw(healthBar);

			window.setView(v);

		}
		else
		{
			window.draw(healthBar, states);

		}

	}

	float getRadius()
	{
		return shape.getRadius();

	}

	sf::FloatRect getBounds()
	{
		sf::FloatRect bounds = shape.getGlobalBounds();
		bounds.left += getPosition().x;
		bounds.top += getPosition().y;
		return bounds;

	}

	float getShootAngle()
	{
		return flare.getRotation()-90;

	}

	sf::Vector2f getShootPos()
	{
		sf::Transform trans = getTransform();
		trans *= flare.getTransform();
		sf::Vector2f pos(16, 48);
		return trans.transformPoint(pos);

	}

	void setName(std::string name)
	{
		nameTag.setString(name);
		nameTag.setOrigin((int)nameTag.getLocalBounds().width/2, (int)nameTag.getLocalBounds().height/2);
		/*if(name == "Verra" || name == "Verrazano")
			nameTag.setColor(sf::Color::Magenta);*/

	}

	std::string getName() const
	{
		return nameTag.getString();

	}

	void setHealth(int nhealth)
	{
		if(nhealth < health && playerType == Local)
			red.setFillColor(sf::Color(255, 0, 0, 150));
		health = nhealth;

		if(nhealth < 0)
		{
			healthBar.setSize(sf::Vector2f(0, 20));

		}
		else
		{
			float ratio = (float)nhealth/100.0;
			healthBar.setSize(sf::Vector2f(100*ratio, 20));
			healthBar.setOrigin(50*ratio, 2);

		}

	}

	void setGibs(std::vector<Gib> ngibs)
	{
		gibs.clear();
		for(unsigned int i = 0; i < ngibs.size(); i++)
		{
			if(rand()%100 < 75) //adds some viarety
			{
				gibs.push_back(ngibs[i]);

			}

		}

	}

	bool operator==(const Character& rhs)
	{
		return getName() == rhs.getName();

	}

	void pack(sf::Packet& packet)
	{
		//24 bytes
		sf::Vector2f pos = getPosition();
		packet << netId;
		packet << tframe; //4 bytes
		packet << pos.x << pos.y;
		int8_t xvel = (int)vel.x;
		int8_t yvel = (int)vel.y;
		packet << xvel << yvel;
		int8_t h = (int)health;
		uint8_t r = (unsigned int)(sprite.getRotation()/2.0);
		packet << h << r << ping;


	}

	void unpack(sf::Packet& packet)
	{
		int8_t velx;
		int8_t vely;
		sf::Vector2f pos;
		sf::Vector2f nvel;
		int8_t h;
		int nhealth = 100;
		uint8_t r;
		float nrot = 0;
		unsigned int nframe = 0;
		packet >> nframe;
		packet >> pos.x >> pos.y;
		packet >> velx >> vely;
		nvel.x = (float)vel.x;
		nvel.y = (float)vel.y;
		packet >> h >> r >> ping;
		nhealth = (int)h;
		nrot = (float)(((unsigned int)r)*2.0);

		setHealth(nhealth);
		sf::Vector2f oldpos = getPosition();

    	smoothed.setPosition(pos);
    	smoothed.setRotation(nrot);
    	pingText.setString(toString(ping));
    	pingText.setOrigin((int)pingText.getLocalBounds().width/2, (int)pingText.getLocalBounds().height/2);

		//TODO: add smoothing

		if(playerType == Local)
		{
			//get rid of old moves
			while (!importantMoves.empty() && importantMoves.oldest().t<nframe)
				importantMoves.remove();

	        while (!moves.empty() && moves.oldest().t<nframe)
	            moves.remove();

	        if(moves.empty())
	        	return;

	        if(pos != moves.oldest().pos)
	        {
	        	//std::cout << "replaying\n";
	        	Input prev = input;
	        	Input daone= moves.oldest().input;
	        	input = daone;
	        	//reset the player to the last received pos
	        	moves.remove();
	        	tframe = nframe;
	        	setPosition(pos);

	        	replaying = true;
	        	int i = moves.tail;
	        	while(i != moves.head)
	        	{
	        		while(tframe < moves[i].t)
	        		{
	        			update();
	        			resolveMapCollisions();

	        		}
	        		input = moves[i].input;
	        		moves[i].pos = getPosition();
	        		moves.next(i);

	        	}

				update();
				resolveMapCollisions();
				input = prev;
	        	replaying = false;

	        	/*sf::Vector2f npos = getPosition();

	        	sf::Vector2f dif = oldpos - npos;
	        	float dist = sqrt(pow(dif.x, 2) + pow(dif.y, 2));
	        	if(dist >= 8)
	        	{
	        		setPosition(npos);
	        		pos = lerp(oldpos, pos);
	        		//dif = oldpos - pos;
	        		smoothed.setPosition(pos); //TODO: is this ok

	        	}*/

	        }

		}
		else
		{

			nextPos = pos;
			nextRot = nrot;
			//setPosition(pos);
			sprite.setRotation(nrot);
			flare.setRotation(nrot);
			sf::Vector2f dif = oldpos - pos;
			float dist = sqrt(pow(dif.x, 2) + pow(dif.y, 2));
			if(dist >= 64*4)
			{
				std::cout << "teleporting\n";
				setPosition(nextPos);
				smoothFac = 0.1;

			}
			else if(dist > 0.1)
			{
				//std::cout << "oldpos: " << oldpos.x << ", " << oldpos.y << "\n";
				//std::cout << "pos: " << pos.x << ", " << pos.y << "\n";
				//std::cout << "big gap\n";
				//setPosition(nextPos);
				smoothFac = 0.1;

			}
			else
			{
				setPosition(pos);

			}
			/*else if(dist >= 8)
			{
				smoothFac = 0.1;

			}*/

			vel = nvel;
			/*float l = lerp(nRot, getRotation());
			nRot = nrot;
			sprite.setRotation(l);
			flare.setRotation(l);*/

		}

	}

	void packImportant(sf::Packet& packet)
	{
		packet << tframe;
		unsigned int size = importantMoves.size();
		packet << size;
		int i = importantMoves.tail;
		for(unsigned int j = 0; j < size; j++)
		{
			importantMoves[i].pack(packet);
			importantMoves.next(i);

		}

	}

	std::vector<Move> unpackImportant(sf::Packet& packet)
	{
		unsigned int size = 0;
		packet >> size;
		std::vector<Move> m;
		for(unsigned int i = 0; i < size; i++)
		{
			Move move;
			move.unpack(packet);
			m.push_back(move);

		}

		return m;

	}

	void resolveMapCollisions()
	{
		for(unsigned int j = 0; j < mapRects.size(); j++)
		{
			sf::FloatRect inter;
			if(mapRects[j].getGlobalBounds().intersects(getBounds(), inter))
			{
				if(inter.width > inter.height)
					if(getPosition().y > inter.top)
						move(0, inter.height);
					else
						move(0, -inter.height);
				else
					if(getPosition().x > inter.left)
						move(inter.width, 0);
					else
						move(-inter.width, 0);

			}

		}

		sf::Vector2f pos = getPosition();
		sf::Vector2f fpos = fountainShape.getPosition();
		sf::Vector2f dif = pos - fpos;
		float dist = sqrt(pow(dif.x, 2) + pow(dif.y, 2));
		float totalRadius = getRadius() + 182.5;
		float intersection = totalRadius - dist;
		if(intersection > 0)
		{
			if(dist != 0)
			{
				dif /= dist;

			}

			pos += dif*intersection;
			setPosition(pos);

		}

	}

	PlayerType playerType;
	Bullet::Team team;
	sf::RectangleShape sprite;
	sf::RectangleShape flare;
	sf::CircleShape shape;

	float accel;
	float maxSpeed;
	float fric;
	float mass;
	sf::Vector2f vel;

	sf::Vector2f muzzle;
	float shootSpeed;
	sf::Clock shootTimer;
	float flareAngle;
	bool shooting;
	Bullet bullet;

	sf::Clock waveTimer;
	sf::Sound fireSound;

	int magSize;
	float reloadTime;
	int bulletsLeft;
	bool reloading;

	int health;
	int variance;

	sf::Text nameTag;
	sf::RectangleShape healthBar;

	std::vector<Gib> gibs;

	sf::Text pingText;
	int16_t ping;

	std::string lastHitter;
	void* data;

	Input input;

	bool hasCore;

	unsigned int tframe;
	CircularBuffer moves;
	CircularBuffer importantMoves;
	bool replaying;
	sf::RectangleShape smoothed;

	unsigned int classNum;
	uint16_t netId;

	 //used for proxy
	sf::Vector2f nextPos;
	float nextRot;
	float smoothFac;

	Input previous;
	sf::Clock aiTimer;

};

ENetPacket* toEnet(sf::Packet sp, int flags)
{
	  ENetPacket* ep = enet_packet_create(sp.getData(), sp.getDataSize(), flags);
	  return ep;

}

sf::Packet toSFML(ENetPacket* ep)
{
	sf::Packet sp;
	sp.clear();
	sp.append(ep->data, ep->dataLength);
	return sp;

}

bool send(ENetPeer* peer, sf::Packet packet, unsigned int chan = 0,
		bool reliable = false, ENetHost* host = NULL)
{
	ENetPacket* epacket = NULL;
	epacket = toEnet(packet, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
	if(packet == NULL)
	{
		return false;
	}

	enet_peer_send(peer, chan, epacket);

	if(host != NULL)
		enet_host_flush(host);

	return true;

}

class PeerData
{
public:
	PeerData(std::string username, ENetPeer* peer) :
		username(username),
		peer(peer)
	{
		character = NULL;
		team = Bullet::Good;

	}

	bool operator==(const PeerData& rhs)
	{
		return username == rhs.username;

	}

	std::string username;
	ENetPeer* peer;
	Character* character;
	Bullet::Team team;
	sf::Clock respawnTimer;

};

bool broadcast(std::list<PeerData> peers, sf::Packet packet, ENetPeer* ex = NULL,
		unsigned int chan = 0, bool reliable = false, ENetHost* host = NULL, bool flush = true)
{
	ENetPacket* epacket = NULL;
	epacket = toEnet(packet, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
	if(packet == NULL)
	{
		return false;
	}

	if(ex == NULL && host != NULL)
	{
		enet_host_broadcast(host, chan, epacket);

	}
	else
	{
		std::list<PeerData>::iterator it;
		for(it = peers.begin(); it != peers.end(); it++)
		{
			PeerData& data = (*it);
			if(data.peer != ex)
			{
				enet_peer_send(data.peer, chan, epacket);

			}

		}

	}

	if(host != NULL && flush)
		enet_host_flush(host);

	return true;

}


Character& makeAssault1(std::list<Character>& characters,
		Character::PlayerType playerType, Bullet::Team team, sf::Vector2f pos,
		std::string name)
{
	Bullet bullet(team, sf::Vector2f(0, 0), 0, 50, 8,
			sf::IntRect(210, 0, 64, 402), sf::Vector2f(32, 30), 0.25);
	Character character(playerType, team, pos,
				32, sf::IntRect(0, 0, 144, 210), 0.5,
				sf::Vector2f(72, 125), sf::Vector2f(80, 145),
				sf::IntRect(176, 448, 144, 668), 2, 8, 0.83, 50, 0.2, bullet,
				assault1Sound, 15, 2);
	character.setName(name);

	std::vector<Gib> gibs;
	gibs.push_back(Gib(brownGib, sf::IntRect(0, 0, 64, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(brownGib, sf::IntRect(64, 0, 72, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(brownGib, sf::IntRect(64+72, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	Gib gunGib(brownGib, sf::IntRect(64+72+104, 0, 152, 96), 0.5, sf::Vector2f(0, 0),
				15, 0, 0, 60);
	gunGib.bloody = false;
	gibs.push_back(gunGib);
	gibs.push_back(Gib(brownGib, sf::IntRect(64+72+104+152, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(brownGib, sf::IntRect(64+72+104+152+104, 0, 71, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	character.setGibs(gibs);
	character.classNum = 0;

	characters.push_back(character);
	return (*characters.rbegin());

}

Character& makeAssault2(std::list<Character>& characters,
		Character::PlayerType playerType, Bullet::Team team, sf::Vector2f pos,
		std::string name)
{
	Bullet bullet(team, sf::Vector2f(0, 0), 0, 40, 4,
			sf::IntRect(43, 164, 70, 210), sf::Vector2f(35, 26), 0.25);
	Character character(playerType, team, pos,
				32, sf::IntRect(144, 0, 135, 210), 0.5,
				sf::Vector2f(72, 125), sf::Vector2f(80, 115),
				sf::IntRect(0, 412, 144, 668), 2, 8, 0.83, 50, 0.05, bullet,
				assault2Sound, 30, 2);
	character.setName(name);

	std::vector<Gib> gibs;
	gibs.push_back(Gib(redGib, sf::IntRect(0, 0, 64, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(redGib, sf::IntRect(64, 0, 72, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(redGib, sf::IntRect(64+72, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 6));
	Gib gunGib(redGib, sf::IntRect(64+72+104, 0, 152, 96), 0.5, sf::Vector2f(0, 0),
				15, 0, 0, 60);
	gunGib.bloody = false;
	gibs.push_back(gunGib);
	gibs.push_back(Gib(redGib, sf::IntRect(64+72+104+152, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(redGib, sf::IntRect(64+72+104+152+104, 0, 71, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	character.setGibs(gibs);
	character.classNum = 1;

	characters.push_back(character);
	return (*characters.rbegin());

}

Character& makeAssault3(std::list<Character>& characters,
		Character::PlayerType playerType, Bullet::Team team, sf::Vector2f pos,
		std::string name)
{
	Bullet bullet(team, sf::Vector2f(0, 0), 0, 40, 10,
			sf::IntRect(372, 256, 82, 100), sf::Vector2f(36, 18), 0.25);
	Character character(playerType, team, pos,
				32, sf::IntRect(279, 0, 146, 210), 0.5,
				sf::Vector2f(72, 125), sf::Vector2f(80, 115),
				sf::IntRect(320, 382, 200, 700), 2, 8, 0.83, 50, 0.01, bullet,
				assault3Sound, 3, 0.4);
	character.setName(name);

	std::vector<Gib> gibs;
	gibs.push_back(Gib(greenGib, sf::IntRect(0, 0, 64, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(greenGib, sf::IntRect(64, 0, 72, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(greenGib, sf::IntRect(64+72, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	Gib gunGib(greenGib, sf::IntRect(64+72+104, 0, 152, 96), 0.5, sf::Vector2f(0, 0),
				15, 0, 0, 60);
	gunGib.bloody = false;
	gibs.push_back(gunGib);
	gibs.push_back(Gib(greenGib, sf::IntRect(64+72+104+152, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(greenGib, sf::IntRect(64+72+104+152+104, 0, 71, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	character.setGibs(gibs);
	character.classNum = 2;

	characters.push_back(character);
	return (*characters.rbegin());

}

Character& makeSniper(std::list<Character>& characters,
		Character::PlayerType playerType, Bullet::Team team, sf::Vector2f pos,
		std::string name)
{
	Bullet bullet(team, sf::Vector2f(0, 0), 0, 80, 40,
			sf::IntRect(512, 392, 105, 176), sf::Vector2f(55, 23), 0.25);
	Character character(playerType, team, pos,
				24, sf::IntRect(456, 0, 127, 230), 0.5,
				sf::Vector2f(67, 177), sf::Vector2f(54, 0),
				sf::IntRect(511, 550, 107, 154), 2, 8, 0.83, 50, 0.8, bullet,
				sniperSound, 4, 3.0);
	character.setName(name);
	character.variance = 2;

	std::vector<Gib> gibs;
	gibs.push_back(Gib(sniperGib, sf::IntRect(0, 0, 64, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(sniperGib, sf::IntRect(64, 0, 72, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(sniperGib, sf::IntRect(64+72, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(sniperGib, sf::IntRect(64+72+104, 0, 152, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(sniperGib, sf::IntRect(64+72+104+152, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(sniperGib, sf::IntRect(64+72+104+152+104, 0, 71, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	character.setGibs(gibs);
	character.classNum = 3;

	characters.push_back(character);
	return (*characters.rbegin());

}


Character& makeAssault1(std::vector<Character>& characters,
		Character::PlayerType playerType, Bullet::Team team, sf::Vector2f pos,
		std::string name)
{
	Bullet bullet(team, sf::Vector2f(0, 0), 0, 50, 8,
			sf::IntRect(210, 0, 64, 402), sf::Vector2f(32, 30), 0.25);
	Character character(playerType, team, pos,
				32, sf::IntRect(0, 0, 144, 210), 0.5,
				sf::Vector2f(72, 125), sf::Vector2f(80, 145),
				sf::IntRect(176, 448, 144, 668), 2, 8, 0.83, 50, 0.2, bullet,
				assault1Sound, 15, 2);
	character.setName(name);

	std::vector<Gib> gibs;
	gibs.push_back(Gib(brownGib, sf::IntRect(0, 0, 64, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(brownGib, sf::IntRect(64, 0, 72, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(brownGib, sf::IntRect(64+72, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	Gib gunGib(brownGib, sf::IntRect(64+72+104, 0, 152, 96), 0.5, sf::Vector2f(0, 0),
				15, 0, 0, 60);
	gunGib.bloody = false;
	gibs.push_back(gunGib);
	gibs.push_back(Gib(brownGib, sf::IntRect(64+72+104+152, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(brownGib, sf::IntRect(64+72+104+152+104, 0, 71, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	character.setGibs(gibs);
	character.classNum = 0;

	characters.push_back(character);
	return characters[characters.size()-1];

}

Character& makeAssault2(std::vector<Character>& characters,
		Character::PlayerType playerType, Bullet::Team team, sf::Vector2f pos,
		std::string name)
{
	Bullet bullet(team, sf::Vector2f(0, 0), 0, 40, 4,
			sf::IntRect(43, 164, 70, 210), sf::Vector2f(35, 26), 0.25);
	Character character(playerType, team, pos,
				32, sf::IntRect(144, 0, 135, 210), 0.5,
				sf::Vector2f(72, 125), sf::Vector2f(80, 115),
				sf::IntRect(0, 412, 144, 668), 2, 8, 0.83, 50, 0.05, bullet,
				assault2Sound, 30, 2);
	character.setName(name);

	std::vector<Gib> gibs;
	gibs.push_back(Gib(redGib, sf::IntRect(0, 0, 64, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(redGib, sf::IntRect(64, 0, 72, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(redGib, sf::IntRect(64+72, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 6));
	Gib gunGib(redGib, sf::IntRect(64+72+104, 0, 152, 96), 0.5, sf::Vector2f(0, 0),
				15, 0, 0, 60);
	gunGib.bloody = false;
	gibs.push_back(gunGib);
	gibs.push_back(Gib(redGib, sf::IntRect(64+72+104+152, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(redGib, sf::IntRect(64+72+104+152+104, 0, 71, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	character.setGibs(gibs);
	character.classNum = 1;

	characters.push_back(character);
	return characters[characters.size()-1];

}

Character& makeAssault3(std::vector<Character>& characters,
		Character::PlayerType playerType, Bullet::Team team, sf::Vector2f pos,
		std::string name)
{
	Bullet bullet(team, sf::Vector2f(0, 0), 0, 40, 10,
			sf::IntRect(372, 256, 82, 100), sf::Vector2f(36, 18), 0.25);
	Character character(playerType, team, pos,
				32, sf::IntRect(279, 0, 146, 210), 0.5,
				sf::Vector2f(72, 125), sf::Vector2f(80, 115),
				sf::IntRect(320, 382, 200, 700), 2, 8, 0.83, 50, 0.01, bullet,
				assault3Sound, 3, 0.4);
	character.setName(name);

	std::vector<Gib> gibs;
	gibs.push_back(Gib(greenGib, sf::IntRect(0, 0, 64, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(greenGib, sf::IntRect(64, 0, 72, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(greenGib, sf::IntRect(64+72, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	Gib gunGib(greenGib, sf::IntRect(64+72+104, 0, 152, 96), 0.5, sf::Vector2f(0, 0),
				15, 0, 0, 60);
	gunGib.bloody = false;
	gibs.push_back(gunGib);
	gibs.push_back(Gib(greenGib, sf::IntRect(64+72+104+152, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(greenGib, sf::IntRect(64+72+104+152+104, 0, 71, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	character.setGibs(gibs);
	character.classNum = 2;

	characters.push_back(character);
	return characters[characters.size()-1];

}

Character& makeSniper(std::vector<Character>& characters,
		Character::PlayerType playerType, Bullet::Team team, sf::Vector2f pos,
		std::string name)
{
	Bullet bullet(team, sf::Vector2f(0, 0), 0, 80, 40,
			sf::IntRect(512, 392, 105, 176), sf::Vector2f(55, 23), 0.25);
	Character character(playerType, team, pos,
				24, sf::IntRect(456, 0, 127, 230), 0.5,
				sf::Vector2f(67, 177), sf::Vector2f(54, 0),
				sf::IntRect(511, 550, 107, 154), 2, 8, 0.83, 50, 0.8, bullet,
				sniperSound, 4, 3.0);
	character.setName(name);
	character.variance = 2;

	std::vector<Gib> gibs;
	gibs.push_back(Gib(sniperGib, sf::IntRect(0, 0, 64, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(sniperGib, sf::IntRect(64, 0, 72, 72), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(sniperGib, sf::IntRect(64+72, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(sniperGib, sf::IntRect(64+72+104, 0, 152, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(sniperGib, sf::IntRect(64+72+104+152, 0, 104, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	gibs.push_back(Gib(sniperGib, sf::IntRect(64+72+104+152+104, 0, 71, 96), 0.5, sf::Vector2f(0, 0),
			15, 0, 0, 60));
	character.setGibs(gibs);
	character.classNum = 3;

	characters.push_back(character);
	return characters[characters.size()-1];

}


void resolveCollisions(std::vector<Character>& characters, int i)
{
	Character& character = characters[i];
	for(unsigned int j = i+1; j < characters.size(); j++)
	{
		Character& other = characters[j];
		sf::Vector2f pos = character.getPosition();
		sf::Vector2f opos = other.getPosition();
		sf::Vector2f diff = pos - opos;
		float dist = sqrt(pow(diff.x, 2) + pow(diff.y, 2));
		float totalRadius = character.getRadius() + other.getRadius();
		float intersection = totalRadius - dist;
		if(intersection > 0)
		{
			if(dist != 0)
			{
				diff /= dist;

			}

			pos += diff*0.5f*intersection;
			opos -= diff*0.5f*intersection;

			character.setPosition(pos);
			other.setPosition(opos);

		}

	}

	character.resolveMapCollisions();

}

void resolveCollisions(Character& character, std::list<Character>& characters)
{
	std::list<Character>::iterator it;
	for(it = characters.begin(); it != characters.end(); it++)
	{
		Character& other = (*it);
		sf::Vector2f pos = character.getPosition();
		sf::Vector2f opos = other.getPosition();
		sf::Vector2f diff = pos - opos;
		float dist = sqrt(pow(diff.x, 2) + pow(diff.y, 2));
		float totalRadius = character.getRadius() + other.getRadius();
		float intersection = totalRadius - dist;
		if(intersection > 0)
		{
			if(dist != 0)
			{
				diff /= dist;

			}

			pos += diff*0.5f*intersection;
			opos -= diff*0.5f*intersection;

			character.setPosition(pos);
			other.setPosition(opos);

		}

	}

	character.resolveMapCollisions();

}

void resolveCollisions(std::vector<Character>& characters, Gib& gib)
{
	for(unsigned int i = 0; i < characters.size(); i++)
	{
		Character& other = characters[i];
		sf::Vector2f pos = gib.getPosition();
		sf::Vector2f opos = other.getPosition();
		sf::Vector2f diff = pos - opos;
		float dist = sqrt(pow(diff.x, 2) + pow(diff.y, 2));
		float totalRadius = gib.getRadius() + other.getRadius();
		float intersection = totalRadius - dist;
		if(intersection > 0)
		{
			if(dist != 0)
			{
				diff /= dist;

			}

			pos += diff*intersection;
			gib.setPosition(pos);

		}

	}

	for(unsigned int j = 0; j < mapRects.size(); j++)
	{
		sf::FloatRect inter;
		if(mapRects[j].getGlobalBounds().intersects(gib.getBounds(), inter))
		{
			if(inter.width > inter.height)
				if(gib.getPosition().y > inter.top)
					gib.move(0, inter.height);
				else
					gib.move(0, -inter.height);
			else
				if(gib.getPosition().x > inter.left)
					gib.move(inter.width, 0);
				else
					gib.move(-inter.width, 0);

		}

	}

}

bool readConfig();
void loadResources();
void runServer();
void runClient();

sf::RenderWindow window;

unsigned int keepPlayers = 0;
void keepAlive()
{
	logFile << "sending keep alive to master server" << std::endl;
	sf::Http::Request request("/alive.php", sf::Http::Request::Post);
	request.setBody("name="+cl_username+"%27s Server&players="+toString(keepPlayers));
	sf::Http http("http://www.libellegame.com/");
	sf::Http::Response response = http.sendRequest(request);
	if(response.getStatus() != sf::Http::Response::Ok)
		logFile << "failed to contact the master server" << std::endl;

}

void spawnServer()
{
#ifdef __linux__
				child_process = fork();
				if(child_process == 0)
				{
					execl("./robo", "robo", "server", NULL);
				}
#elif _WIN32
				spawned = true;
			    ZeroMemory(&si, sizeof(si));
			    si.cb = sizeof(si);
			    ZeroMemory(&pi, sizeof(pi));

			    CreateProcess( NULL,   // No module name (use command line)
			            "robo.exe server",        // Command line
			            NULL,           // Process handle not inheritable
			            NULL,           // Thread handle not inheritable
			            FALSE,          // Set handle inheritance to FALSE
			            0,              // No creation flags
			            NULL,           // Use parent's environment block
			            NULL,           // Use parent's starting directory
			            &si,            // Pointer to STARTUPINFO structure
			            &pi );

#endif

}

int main(int argc, char** argv)
{
	if(!readConfig())
	{
		std::cout << "Please set your username:\n";
		std::string username;
		std::cin >> username;
		cl_username = username;
		sv_port = 50301;
		sv_ip = "localhost";

		std::ofstream f;
		f.open("settings.cfg");
		if(f)
		{
			f << cl_username << "\n";
			f << sv_ip << "\n";
			f << sv_port << "\n";

		}
		else
		{
			std::cout << "can't write to config :C\n";

		}
		f.close();

	}

	if(argc > 1)
		g_isServer = true;

	blueCore.setSize(sf::Vector2f(75, 112.5));
	redCore.setSize(sf::Vector2f(75, 112.5));
	blueCore.setOrigin(37.5-32, 56.25);
	redCore.setOrigin(37.5-32, 56.25);
	blueCore.setPosition(blueCoreSpawn);
	redCore.setPosition(redCoreSpawn);
	redCore.rotate(180);
	blueCore.setOutlineColor(sf::Color::Cyan);
	blueCore.setOutlineThickness(2);
	redCore.setOutlineColor(sf::Color::Red);
	redCore.setOutlineThickness(2);

	loadResources();
	loadMap();

	if(!g_isServer) //DO MENU STUFF
	{
		window.create(sf::VideoMode(854, 480), "robo", sf::Style::Close);
		window.setFramerateLimit(30);

		bool browsing = false;

		Button join(font, "Join", sf::Vector2f(854/2, 240),
				sf::Vector2f(192, 32));

		Button browse(font, "Browse", sf::Vector2f(854/2, 240 - 64),
				sf::Vector2f(192, 32));


		Button host(font, "Host", sf::Vector2f(854/2, 240 + 64),
				sf::Vector2f(192, 32));

		Button back(font, "Back", sf::Vector2f(96+10, 16+10),
				sf::Vector2f(192, 32));

		std::vector<Button> servers;
		std::vector<std::string> serverips;
		sf::Text noservers("No Servers are up :C", font);
		noservers.setPosition(854/2 - 175, 240);

		while(window.isOpen())
		{
			sf::Event event;
			while(window.pollEvent(event))
			{
				if(event.type == sf::Event::Closed)
				{
					window.close();
					return 0;

				}

			}

			window.clear(sf::Color(90, 90, 90));

			if(browsing)
			{
				if(servers.size() == 0)
					window.draw(noservers);

				if(back.update(window, guiView))
				{
					browsing = false;

				}

				bool clicked = false;
				for(unsigned int i = 0; i < servers.size(); i++)
				{
					if(servers[i].update(window, guiView))
					{
						sv_ip = serverips[i];
						clicked = true;
						break;

					}

				}

				if(clicked)
				{
					break;

				}

			}
			else
			{
				if(browse.update(window, guiView))
				{
					browsing = true;
					servers.clear();
					serverips.clear();

					sf::Http::Request request("/list.php", sf::Http::Request::Get);
				    sf::Http http("http://www.libellegame.com/");
					sf::Http::Response response = http.sendRequest(request);

					if(response.getStatus() == sf::Http::Response::Ok)
					{
						std::string resp = response.getBody();
						std::istringstream f(resp);
						resp = "";
						std::string line;
						while(std::getline(f, line))
						{
							if(!(line == "<html>" || line == "<body>"
									|| line == "</html>" || line == "</body>"
											|| line == ""))
							{
								resp += line + "\n";

							}

						}

						if(resp != "no servers")
						{
							std::cout << "\"" <<  resp << "\"\n";


							std::istringstream is(resp);
							std::string name;
							std::string players;
							std::string ip;

							while(std::getline(is, name))
							{
								std::getline(is, players);
								std::getline(is, ip);

								Button button(font, name + "  -  " + players + "/8", sf::Vector2f(854/2, 26 + 64*servers.size()),
										sf::Vector2f(256, 32));
								servers.push_back(button);
								serverips.push_back(ip);

							}

						}

					}

				}

				if(join.update(window, guiView))
				{
					break;

				}

				if(host.update(window, guiView))
				{
					spawnServer();
					break;

				}

			}

			window.display();

		}

	}


	srand(time(NULL));

	enet_initialize();

	if(g_isServer)
	{
		runServer();

	}
	else
	{
		runClient();

	}

	enet_deinitialize();

#ifdef __linux__
	if(child_process != -1)
	{
		kill(child_process, SIGKILL);

	}
#elif _WIN32
	if(spawned)
	{
		TerminateProcess(pi.hProcess, 0);
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );

	}
#endif

	logFile.flush();
	logFile.close();

	return 0;

}

bool readConfig()
{
	std::fstream f;
	f.open("settings.cfg");
	if(!f)
	{
		return false;

	}

	f >> cl_username;
	f >> sv_ip;
	f >> sv_port;
	return true;

}

void loadResources()
{
	if(!g_isServer)
	{
		charactersText.loadFromFile("res/characters.png");
		bulletsText.loadFromFile("res/bullets.png");
		redGib.loadFromFile("res/redgib.png");
		brownGib.loadFromFile("res/browngib.png");
		greenGib.loadFromFile("res/greengib.png");
		sniperGib.loadFromFile("res/snipergib.png");
		coreText.loadFromFile("res/core.png");
		redCore.setTexture(&coreText);
		blueCore.setTexture(&coreText);

		mapText.loadFromFile("res/robomap.png");
		treeText.loadFromFile("res/robomaptrees.png");

		waveShader.loadFromFile("res/wave.vert", "res/blur.frag");
		bloomShader.loadFromFile("res/bloom.frag", sf::Shader::Fragment);

		assault1Buffer.loadFromFile("res/assault1.wav");
		assault1Sound.setBuffer(assault1Buffer);
		assault1Sound.setVolume(15);

		assault2Buffer.loadFromFile("res/assault2.wav");
		assault2Sound.setBuffer(assault2Buffer);
		assault2Sound.setVolume(15);

		assault3Buffer.loadFromFile("res/assault3.wav");
		assault3Sound.setBuffer(assault3Buffer);
		assault3Sound.setVolume(15);

		sniperBuffer.loadFromFile("res/sniper.wav");
		sniperSound.setBuffer(sniperBuffer);

		reloadBuffer.loadFromFile("res/reload.wav");
		reloadSound.setBuffer(reloadBuffer);
		reloadSound.setVolume(15);

		btfrBuffer.loadFromFile("res/halosfx/blueteamflagreturned.wav");
		btfr.setBuffer(btfrBuffer);
		btfr.setVolume(50);
		bthtfBuffer.loadFromFile("res/halosfx/blueteamhastheflag.wav");
		bthtf.setBuffer(bthtfBuffer);
		bthtf.setVolume(50);
		btsBuffer.loadFromFile("res/halosfx/blueteamscore.wav");
		bts.setBuffer(btsBuffer);
		bts.setVolume(50);


		rtfrBuffer.loadFromFile("res/halosfx/redteamflagreturned.wav");
		rtfr.setBuffer(rtfrBuffer);
		rtfr.setVolume(50);
		rthtfBuffer.loadFromFile("res/halosfx/redteamhastheflag.wav");
		rthtf.setBuffer(rthtfBuffer);
		rthtf.setVolume(50);
		rtsBuffer.loadFromFile("res/halosfx/redteamscore.wav");
		rts.setBuffer(rtsBuffer);
		rts.setVolume(50);

		gameOverBuffer.loadFromFile("res/halosfx/gameover.wav");
		gameOverSfx.setBuffer(gameOverBuffer);

		font.loadFromFile("res/neuropol.ttf");

		particles.add("blood", sf::Color(200, 20, 20), 0, sf::Vector2u(5, 5), 10000);
		red.setSize(sf::Vector2f(1000, 1000));
		red.setFillColor(sf::Color(255, 0, 0, 0));

	}

}

void runServer()
{
	std::stringstream logName;
	logName << "logs/log" << time(NULL) << ".txt";
	logFile.open(logName.str().c_str());
	if(!logFile)
		std::cout << "FAILED TO OPEN LOG FILE\n";
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = sv_port;

	ENetHost* server = enet_host_create(&address, 8, 2, 0, 0);
	if(server == NULL)
	{
		logFile << "Can't bind server to port: " << sv_port << std::endl;
		logFile << "Closing server.\n";
		std::cout << "Can't bind server to port: " << sv_port << std::endl;
		return;

	}

	sf::Thread keepThread(&keepAlive);
	keepThread.launch();

	int goodTeam = 0;
	int badTeam = 0;

	int frame = 0;
	std::list<PeerData> peers;
	std::list<Character> characters;
	std::vector<Bullet> bullets;

	bool gameOver = false;
	sf::Clock restartTimer;

	sf::Clock keepAliveTimer;

	for(unsigned int i = 0; i < ai_testers; i++)
	{
		makeAssault1(characters, Character::AI, Bullet::Bad, sf::Vector2f(20, 20), "AI " + toString(i));

	}

	while(server != NULL)
	{
		if(keepAliveTimer.getElapsedTime().asSeconds() > 30)
		{
			keepPlayers = peers.size();
			keepThread.launch();
			keepAliveTimer.restart();

		}

		sf::Clock clock;
		ENetEvent event;
		bool shotweb = false;
		sf::Packet shootPacket;
		sf::Clock pollTimer;
		while(enet_host_service(server, &event, 0) > 0
				&& pollTimer.getElapsedTime().asMilliseconds() < 10)
		{
			if(event.type == ENET_EVENT_TYPE_CONNECT)
			{
				logFile << "client connected to the server " << event.peer->address.host << std::endl;
				//We don't care about client connecting here because
				//we don't consider them connected until they've sent info

			}
			else if(event.type == ENET_EVENT_TYPE_DISCONNECT)
			{
				PeerData* data = (PeerData*)(event.peer->data);
				logFile << "client is disconnecting " << event.peer->address.host << std::endl;
				if(data != NULL)
				{
					std::string username = data->username;
					Character* character = data->character;

					if(character != NULL)
					{
						logFile << "they had a character removed it" << std::endl;
						characters.remove((*character));

					}

					if(data->team == Bullet::Good)
					{
						goodTeam--;

					}
					else if(data->team == Bullet::Bad)
					{
						badTeam--;

					}

					logFile << "removing their peer data from the list, broadcasting leave to other players" << std::endl;
					peers.remove((*data));
					sf::Packet d;
					d << (int)Leave;
					d << username;
					broadcast(peers, d, event.peer, 0, true, server);
					enet_host_flush(server);

					std::cout << username << " disconnected from the server\n";

				}

			}
			else if(event.type == ENET_EVENT_TYPE_RECEIVE)
			{
				sf::Packet packet = toSFML(event.packet);
				int cmd = None;
				packet >> cmd;
				if(cmd == Join)
				{
					std::string username;
					packet >> username;
					logFile << "player " << username << " wants to join" << std::endl;

					//check if the username is already taken
					bool taken = false;
					std::list<PeerData>::iterator Pit;
					for(Pit = peers.begin(); Pit != peers.end(); Pit++)
					{
						if((*Pit).username == username)
						{
							taken = true;
							break;

						}

					}

					//send a taken reply if it is
					if(taken)
					{
						logFile << "their username is already taken nono" << std::endl;
						sf::Packet t;
						t << NameTaken;
						send(event.peer, t, 0, true, server);
						enet_peer_reset(event.peer);
						enet_packet_destroy(event.packet); //reset client so they disconnect
						continue;

					}

					peers.push_back(PeerData(username, event.peer));
					PeerData* data = &(*peers.rbegin());
					event.peer->data = data;

					logFile << "adding client to peer list (connected officially)" << std::endl;
					std::cout << username << " connected to the server\n";

					if(goodTeam <= badTeam)
					{
						data->team = Bullet::Good;
						goodTeam++;

					}
					else
					{
						data->team = Bullet::Bad;
						badTeam++;

					}

					logFile << "sending init packet" << std::endl;
					sf::Packet p;
					p << (int)Init;
					p << blueScore << redScore;
					p << (unsigned int)characters.size();
					std::list<Character>::iterator Cit;
					for(Cit = characters.begin(); Cit != characters.end(); Cit++)
					{
						Character& character = (*Cit);
						sf::Vector2f cPos = character.getPosition();
						std::string cName = character.getName();
						Bullet::Team cTeam = character.team;
						//TODO: add class num in there
						p << cName << character.netId << (unsigned int)cTeam << character.classNum << cPos.x << cPos.y;

					}

					send(event.peer, p, 0, true, server);

					logFile << "sending join packet  to everyone" << std::endl;
					//send player join to everyone
					sf::Packet s;
					s << (int)Join;
					s << username << (unsigned int)data->team;
					broadcast(peers, s, NULL, 0, true, server);
					enet_host_flush(server);


				}
				else if(cmd == Shoot)
				{

					float angle;
					packet >> angle;

					PeerData* data = (PeerData*)(event.peer->data);
					if(data->character != NULL)
					{
						if(!shotweb)
						{
							shootPacket << (int)Shoot;
							shotweb = true;

						}

						Character& character = (*data->character);
						character.flare.setRotation(angle+90); //+90 counte acts the -90 from getShotAngle

						character.bullet.set(character.getShootPos(), angle);//don't need to counteract here because this is the thing that cares about -90
						Bullet b(character.bullet);
						b.owner = character.getName();

						//TODO: step the bullet forward according to latency

						bullets.push_back(b);

						sf::Vector2f bPos = b.getPosition();

						shootPacket << character.netId << bPos.x << bPos.y << angle;

					}

				}
				else if(cmd == SyncInput)
				{
					//unpack player inputs
					Input input;
					input.unpack(packet);

					PeerData* data = (PeerData*)(event.peer->data); //retrieve player character
					Character* character = (data->character);

					if(character != NULL) //Simulate player input
					{
						unsigned int t = 0;
						packet >> t;
						std::vector<Move> important = character->unpackImportant(packet);
						for(unsigned int i = 0; i < important.size(); i++)
						{
							Move& move = important[i];
							while(character->tframe < move.t)
							{
								character->update();
								resolveCollisions(*character, characters);

							}
							character->input = move.input;

						}

						while(character->tframe < t)
						{
							character->update();
							resolveCollisions(*character, characters);

						}

						character->input = input;
						character->sprite.setRotation(input.aimAngle);
						character->flare.setRotation(input.aimAngle);

					}

				}

				enet_packet_destroy(event.packet);
			}

		}

		if(shotweb)
		{

			shootPacket << ((int8_t)-1);
			broadcast(peers, shootPacket, NULL, 0, true, server, true);

		}

		/////SERVER GAME UPDATE STUFF/////
		if(!gameOver)
		{
			if(peers.size() > 0)
			{
				std::list<PeerData>::iterator pit;
				for(pit = peers.begin(); pit != peers.end(); pit++)
				{
					PeerData& data = (*pit);
					if(data.character == NULL
							&& data.respawnTimer.getElapsedTime().asSeconds() > 5.0)
					{
						logFile << "respawning player" << std::endl;
						sf::Vector2f spawnLocal;
						if(data.team == Bullet::Good)
						{
							spawnLocal.x = 320.0;
							spawnLocal.y = 776.0;

						}
						else
						{
							spawnLocal.x = 3812.0;
							spawnLocal.y = 776.0;

						}

						unsigned int classNum = rand()%4;

						Character* charac = NULL;
						if(classNum == 0)
						{
							charac = &makeAssault1(characters, Character::Local,
									data.team, spawnLocal, data.username);

						}
						else if(classNum == 1)
						{
							charac = &makeAssault2(characters, Character::Local,
									data.team, spawnLocal, data.username);

						}
						else if(classNum == 2)
						{
							charac = &makeAssault3(characters, Character::Local,
									data.team, spawnLocal, data.username);

						}
						else if(classNum == 3)
						{
							charac = &makeSniper(characters, Character::Local,
									data.team, spawnLocal, data.username);

						}

						logFile << "spawning as class: " << classNum << std::endl;

						data.character = charac;
						data.character->data = &data;

						sf::Packet p;
						p << (int)Spawn; //int
						unsigned int size = 1;
						p << size; //unsigned int
						std::cout << "\nserver\n";
						std::cout << "spawning: " << size << " players\n";
						sf::Vector2f cPos = data.character->getPosition();
						std::string cName = data.character->getName();
						int team = (int)data.character->team;
						uint16_t id = data.character->netId;
						std::cout << "spawning: \"" << cName << "\"\n";
						std::cout << "with Id: " << id << "\n";
						std::cout << "on Team: " << team << "\n";
						std::cout << "as Class: " << classNum << "\n";
						std::cout << "at Pos: " << cPos.x << ", " << cPos.y << "\n";
						//string, uint16_t, unsigned int, unsigned int, float, float
						p << cName << id << team << classNum << cPos.x << cPos.y;// << data.character->netId << (unsigned int)cTeam << charac->classNum << cPos.y << cPos.y;
						broadcast(peers, p, NULL, 0, true, server, true);

					}

				}

				//Get rid of old bullets
				std::vector<Bullet>::iterator bulletIt;
				for(bulletIt = bullets.begin(); bulletIt != bullets.end();)
				{
					if(bulletIt->dead)
					{
						bulletIt = bullets.erase(bulletIt);
						continue;

					}

					bulletIt++;

				}

				//update bullet position and check for hits
				for(unsigned int i = 0; i < bullets.size(); i++)
				{
					Bullet& bullet = bullets[i];

					bool hit = false;
					for(int j = 0; j < 4; j++)
					{
						bullet.update(0.25);
						sf::Vector2f pos = bullet.getPosition();
						float radius = bullet.getRadius();

						for(unsigned int j = 0; j < mapRects.size(); j++)
						{
							sf::FloatRect inter;
							if(mapRects[j].getGlobalBounds().intersects(bullet.getBounds(), inter))
							{
								hit = true;
								bullet.dead = true;
								break;

							}

						}

						if(hit)
							break;

						std::list<Character>::iterator it;
						for(it = characters.begin(); it != characters.end(); it++)
						{
							Character& character = (*it);
							if(bullet.team != character.team)
							{
								sf::Vector2f cpos = character.getPosition();
								sf::Vector2f diff = pos - cpos;
								float dist = sqrt(pow(diff.x, 2) + pow(diff.y, 2));
								float totalRadius = radius + character.getRadius();
								float intersection = totalRadius - dist;
								if(intersection > 0)
								{
									character.setHealth(character.health-bullet.damage);
									character.lastHitter = bullet.owner;
									hit = true;
									bullet.dead = true;
									break;

								}

							}

						}

						if(hit)
						{
							break;

						}

					}

				}

				if(!goodCoreStolen && blueCore.getPosition() != blueCoreSpawn
						&& blueReturn.getElapsedTime().asSeconds() > 6.0)
				{
					blueCore.setPosition(blueCoreSpawn);
					blueCore.setRotation(0);
					sf::Packet r;
					r << (int)CoreReturned << (unsigned int)Bullet::Good;
					broadcast(peers, r, NULL, 0, true, server, false);

				}

				if(!badCoreStolen && redCore.getPosition() != redCoreSpawn
						&& redReturn.getElapsedTime().asSeconds() > 6.0)
				{
					redCore.setPosition(redCoreSpawn);
					redCore.setRotation(180);
					sf::Packet r;
					r << (int)CoreReturned << (unsigned int)Bullet::Bad;
					broadcast(peers, r, NULL, 0, true, server, false);

				}

				std::list<Character>::iterator charIt;
				int i = 0;
				for(charIt = characters.begin(); charIt != characters.end();)
				{
					Character& character = (*charIt);
					if(character.playerType == Character::AI)
					{
						character.update();

					}

					if(character.health <= 0)
					{
						logFile << "player " << character.getName() << " died sending kill packets" << std::endl;
						sf::Packet k;
						k << (int)Kill;
						k << character.lastHitter << character.getName();
						if(character.playerType != Character::AI)
						{
							((PeerData*)character.data)->respawnTimer.restart();
							((PeerData*)character.data)->character = NULL;

						}

						broadcast(peers, k, NULL, 0, true, server, false);

						if(character.hasCore)
						{
							logFile << "player had the core dropping it" << std::endl;
							sf::Packet d;
							d << (int)CoreDropped;
							d << character.getName();
							if(character.team == Bullet::Good)
							{
								d << (unsigned int)Bullet::Bad << redCore.getPosition().x << redCore.getPosition().y;
								badCoreStolen = false;
								redReturn.restart();

							}
							else
							{
								d << (unsigned int)Bullet::Good << blueCore.getPosition().x << blueCore.getPosition().y;
								goodCoreStolen = false;
								blueReturn.restart();

							}
							broadcast(peers, d, NULL, 0, true, server, false);
							character.hasCore = false;

						}

						charIt = characters.erase(charIt);
						continue;

					}

					resolveCollisions(character, characters);

					//DEAL WITH PLAYER TOUCHING CORES
					if(character.hasCore)
					{
						if(character.team == Bullet::Good && !goodCoreStolen)
						{
							//GOOOAAALLL
							if(blueCore.getPosition() == blueCoreSpawn && character.getBounds().intersects(blueCore.getGlobalBounds()))
							{
								sf::Packet g;
								g << (int)CoreCaptured << character.getName() << Bullet::Bad;
								broadcast(peers, g, NULL, 0, true, server, false);
								redCore.setPosition(redCoreSpawn);
								redCore.setRotation(180);
								badCoreStolen = false;
								character.hasCore = false;
								blueScore++;

							}

						}
						else if(character.team == Bullet::Bad && !badCoreStolen)
						{
							//GOOOAAALLL
							if(redCore.getPosition() == redCoreSpawn && character.getBounds().intersects(redCore.getGlobalBounds()))
							{
								sf::Packet g;
								g << (int)CoreCaptured << character.getName() << Bullet::Good;
								broadcast(peers, g, NULL, 0, true, server, false);
								blueCore.setPosition(blueCoreSpawn);
								blueCore.setRotation(0);
								goodCoreStolen = false;
								character.hasCore = false;
								redScore++;


							}

						}

						if(redScore >= 3 || blueScore >= 3) //GAME is over
						{
							logFile << "sending game over packet" << std::endl;
							sf::Packet r;
							r << (int)GameOver;
							if(blueScore >= 3)
								r << (unsigned int)Bullet::Good;
							else
								r << (unsigned int)Bullet::Bad;

							broadcast(peers, r, NULL, 0, true, server); //send imediately

							gameOver = true;
							characters.clear();
							bullets.clear();

							//TODO: shuffle teams
							std::list<PeerData>::iterator ppit;
							for(ppit = peers.begin(); ppit != peers.end(); ppit++)
							{
								ppit->character = NULL;

							}

							blueCore.setPosition(blueCoreSpawn);
							blueCore.setRotation(0);
							redCore.setPosition(redCoreSpawn);
							redCore.setRotation(180);
							goodCoreStolen = false;
							badCoreStolen = false;
							blueScore = 0;
							redScore = 0;

							restartTimer.restart();
							break;

						}

					}
					else
					{

						if(character.team == Bullet::Good && !badCoreStolen)
						{
							if(character.getBounds().intersects(redCore.getGlobalBounds()))
							{
								character.hasCore = true;
								sf::Packet c;
								c << (int)CoreStolen << character.getName() << "\n";
								broadcast(peers, c, NULL, 0, true, server, false);
								badCoreStolen = true;

							}

						}
						else if(character.team == Bullet::Bad && !goodCoreStolen)
						{
							if(character.getBounds().intersects(blueCore.getGlobalBounds()))
							{
								character.hasCore = true;
								sf::Packet c;
								c << (int)CoreStolen << character.getName() << "\n";
								broadcast(peers, c, NULL, 0, true, server, false);
								goodCoreStolen = true;

							}

						}

					}

					charIt++;
					i++;

				}

				if(frame%3 == 0) //sync player movement
				{
					sf::Packet s;
					s << (int)Sync;
					s << (unsigned int)characters.size();
					std::list<Character>::iterator it;
					for(it = characters.begin(); it != characters.end(); it++)
					{
						if(it->playerType == Character::AI)
							it->ping = 0;
						else
							it->ping = ((PeerData*)it->data)->peer->lastRoundTripTime;

						it->pack(s);

					}

					broadcast(peers, s, NULL, 1, false, server); //broadcast to all clients

				}

				frame++;

			}

		}
		else
		{
			if(restartTimer.getElapsedTime().asSeconds() > 20.0)
			{
				gameOver = false;

			}

		}

		int milli = clock.getElapsedTime().asMilliseconds();
		int waitTime = 22 - milli; /*17 is the number of millis per frame ... hopefully*/
		if(waitTime > 0)
		{
			sf::sleep(sf::milliseconds(waitTime));

		}

	}

}

std::vector<std::string> goodNames;
std::vector<std::string> badNames;

bool handleSpawn(std::vector<Character>& characters, sf::Packet& packet)
{
	unsigned int size = 0; //unsigned int
	packet >> size;
	std::cout << "\nclient\n";
	std::cout << "spawning: " << size << " players\n";
	bool me = false;
	for(unsigned int i = 0; i < size; i++)
	{
		std::string username;
		uint16_t id;
		int team;
		unsigned int classNum;
		sf::Vector2f pos;

		packet >> username >> id >> team >> classNum >> pos.x >> pos.y;

		std::cout << "spawning: \"" << username << "\"\n";
		std::cout << "with Id: " << id << "\n";
		std::cout << "on Team: " << team << "\n";
		std::cout << "as Class: " << classNum << "\n";
		std::cout << "at Pos: " << pos.x << ", " << pos.y << "\n";

		Character::PlayerType playerType = Character::Net;
		if(username == cl_username)
		{
			camPos.x = pos.x - 854/2;
			camPos.y = pos.y - 240;
			playerType = Character::Local;
			me = true;
		}

		bool found = false;
		for(unsigned int j = 0; j < goodNames.size(); j++)
		{
			if(goodNames[j] == username)
			{
				found = true;
				break;

			}

		}

		if(!found)
		{
			for(unsigned int j = 0; j < badNames.size(); j++)
			{
				if(badNames[j] == username)
				{
					found = true;
					break;

				}

			}

		}

		if(!found)
		{
			if(team == Bullet::Good)
				goodNames.push_back(username);
			else
				badNames.push_back(username);

		}

		if(classNum == 0)
		{
			Character& c = makeAssault1(characters, playerType,
				(Bullet::Team)team, pos, username);
			c.netId = id;

		}
		else if(classNum == 1)
		{
			Character& c = makeAssault2(characters, playerType,
				(Bullet::Team)team, pos, username);
			c.netId = id;

		}
		else if(classNum == 2)
		{
			Character& c = makeAssault3(characters, playerType,
				(Bullet::Team)team, pos, username);
			c.netId = id;

		}
		else if(classNum == 3)
		{
			Character& c = makeSniper(characters, playerType,
				(Bullet::Team)team, pos, username);
			c.netId = id;

		}

	}

	return me;

}

void handleLeave(std::vector<Character>& characters, std::string username)
{
	std::vector<std::string>::iterator strit;
	for(strit = goodNames.begin(); strit != goodNames.end();)
	{
		if((*strit) == username)
		{
			strit = goodNames.erase(strit);
			continue;

		}

		strit++;

	}

	for(strit = badNames.begin(); strit != badNames.end();)
	{
		if((*strit) == username)
		{
			strit = badNames.erase(strit);
			continue;

		}

		strit++;

	}

	std::vector<Character>::iterator it;
	for(it = characters.begin(); it != characters.end(); it++)
	{
		Character& character = (*it);
		if(character.getName() == username)
		{
			characters.erase(it);
			return;

		}

	}

}

void handleShooting(std::vector<Character>& characters,
		std::vector<Bullet>& bullets, sf::Packet& packet, int latency)
{
	while(true) //TODO: this is prolly crazy dangerous
	{
		sf::Vector2f pos;
		float angle;
		uint16_t netId;
		packet >> netId >> pos.x >> pos.y >> angle;

		for(unsigned int i = 0; i < characters.size(); i++)
		{
			Character& character = characters[i];
			if(character.netId == netId)
			{
				character.bullet.set(pos, angle);
				character.bullet.owner = character.getName();

				float move = (float)latency/17.0;
				character.bullet.update(move);
				//TODO: handle sound and animation

				//TODO: step the bullet forward small increments so we can hit detect here

				bullets.push_back(character.bullet);
				character.flare.setFillColor(sf::Color(255, 255, 255, 255));
				if(character.playerType == Character::Net)
					playSound(character.fireSound, pos, 40.0);

				return;

			}

		}

		{
			sf::Packet p = packet;
			int8_t end = 0;
			p >> end;
			if(end == -1)
			{
				return;

			}

		}

	}

}

bool handleKill(std::vector<Character>& characters, std::vector<Gib>& gibs,
		std::string killer, std::string victim)
{
	std::vector<Character>::iterator it;
	for(it = characters.begin(); it != characters.end(); it++)
	{
		Character& character = (*it);
		if(character.getName() == victim)
		{
			for(unsigned int j = 0; j < character.gibs.size(); j++)
			{
				character.gibs[j].set(character.getPosition(), rand()%360, rand()%10+5);
				gibs.push_back(Gib(character.gibs[j]));

			}

			characters.erase(it);
			return victim == cl_username;

		}

	}

	return false;

}

void handleSync(std::vector<Character>& characters, sf::Packet& packet)
{
	unsigned int size = 0;
	packet >> size;
	for(unsigned int i = 0; i < size; i++)
	{
		uint16_t id;
		packet >> id;

		bool found = false;

		for(unsigned int j = 0; j < characters.size(); j++)
		{
			Character& character = characters[j];
			if(character.netId == id)
			{
				character.unpack(packet);
				found = true;

			}

		}

		if(!found)
		{
			std::cout << "something horribly wrong has happened and we can't find the player to sync.\n";
			std::cout << "clearing packet info so we can continue\n";
			unsigned int n,x;
			packet >> n;
			packet >> x >> x;
			packet >> x >> x;
			packet >> n >> n;
		}

	}

}

void handleCoreStolen(std::vector<Character>& characters, std::string username)
{
	//TODO: add sounds for when the core gets stolen/dropped/returned/captured
	for(unsigned int i = 0; i < characters.size(); i++)
	{
		Character& character = characters[i];
		if(character.getName() == username)
		{
			character.hasCore = true;
			if(character.team == Bullet::Good)
			{
				bthtf.play();
				badCoreStolen = true;
				gameInfo.add(username + " stole the core", sf::Color::Cyan);

			}
			else
			{
				rthtf.play();
				goodCoreStolen = true;
				gameInfo.add(username + " stole the core", sf::Color::Red);

			}

			return;

		}

	}

}

void handleCoreDropped(sf::Packet& packet, std::string username)
{
	unsigned int team = Bullet::Good;
	sf::Vector2f corePos;
	packet >> team >> corePos.x >> corePos.y;
	if(team == Bullet::Good)
	{
		blueCore.setPosition(corePos);
		goodCoreStolen = false;
		blueReturn.restart();
		gameInfo.add(username + " dropped the core", sf::Color::Red);

	}
	else
	{
		redCore.setPosition(corePos);
		badCoreStolen = false;
		redReturn.restart();
		gameInfo.add(username + " dropped the core", sf::Color::Cyan);

	}

}

void handleCoreReturned(sf::Packet& packet)
{
	unsigned int team = Bullet::Good;
	packet >> team;
	if(team == Bullet::Good)
	{
		blueCore.setPosition(blueCoreSpawn);
		blueCore.setRotation(0);
		goodCoreStolen = false;
		btfr.play();
		gameInfo.add("core returned", sf::Color::Cyan);

	}
	else
	{
		redCore.setPosition(redCoreSpawn);
		redCore.setRotation(180);
		badCoreStolen = false;
		rtfr.play();
		gameInfo.add("core returned", sf::Color::Red);

	}

}

void handleCoreCaputred(std::vector<Character>& characters, std::string username, sf::Packet& packet)
{
	unsigned int team = Bullet::Good;
	packet >> team;
	if(team == Bullet::Good)
	{
		blueCore.setPosition(blueCoreSpawn);
		blueCore.setRotation(0);
		goodCoreStolen = false;
		redScore++;
		rts.play();
		gameInfo.add(username + " captued the core", sf::Color::Red);

	}
	else
	{
		redCore.setPosition(redCoreSpawn);
		redCore.setRotation(180);
		badCoreStolen = false;
		blueScore++;
		bts.play();
		gameInfo.add(username + " captued the core", sf::Color::Cyan);

	}

	for(unsigned int i = 0; i < characters.size(); i++)
	{
		Character& character = characters[i];
		if(character.getName() == username)
		{
			character.hasCore = false;
			return;

		}

	}

}

void runClient()
{
	window.setView(guiView);

	sf::RectangleShape ground(sf::Vector2f(4096, 1536));
	ground.setTexture(&mapText);

	sf::RectangleShape trees(sf::Vector2f(4096, 1536));
	trees.setTexture(&treeText);
	trees.setFillColor(sf::Color(255, 255, 255, 220));

	bool respawning = true;
	sf::Text respawnText("respawning in", font);
	respawnText.setOrigin(respawnText.getLocalBounds().width/2,
			respawnText.getLocalBounds().height/2);
	respawnText.setPosition(sf::Vector2f(854/2, 480/2));

	std::vector<Character> characters;
	std::vector<Bullet> bullets;
	std::vector<Gib> gibs;

	gameInfo.setPosition(10, 10);

	sf::Text conStatus("Connecting to:  " + sv_ip + ":" + toString(sv_port) + " . . .", font, 15);
	conStatus.move(10, 10);
	conStatus.setColor(sf::Color::Black);

	ENetHost* client = enet_host_create(NULL, 1, 2, 0, 0);
	ENetAddress address;
	enet_address_set_host(&address, sv_ip.c_str());
	address.port = sv_port;

	ENetPeer* server = NULL;
	bool connected = false;
	int attempts = 1;
	int constate = 0;

	window.clear(sf::Color::White);
	window.draw(conStatus);
	window.display();

	bool gameOver = false;
	bool blueWon = true;
	sf::Clock restartTimer;

	while(attempts < 11 && constate != 2)
	{
		if(constate == 0)
			server = enet_host_connect(client, &address, 2, 0);

		window.clear(sf::Color::White);
		window.draw(conStatus);
		window.display();

		ENetEvent event;
		if(enet_host_service(client, &event, 5000) > 0
				&& event.type == ENET_EVENT_TYPE_CONNECT)
		{
			sf::Packet packet;
			packet << (int)Join << cl_username;
			send(server, packet, 0, true, client);

			connected = true;
			constate = 1;
			conStatus.setString(conStatus.getString() + "\nGetting info from server . . .");

		}
		else if(event.type == ENET_EVENT_TYPE_RECEIVE)
		{
			sf::Packet packet = toSFML(event.packet);
			int cmd = None;
			packet >> cmd;
			if(cmd == NameTaken)
			{
				conStatus.setString(conStatus.getString() + "\n" + cl_username + " is already in use.");
				window.clear(sf::Color::White);
				window.draw(conStatus);
				window.display();
				sf::sleep(sf::seconds(2));
				constate = 2;
				connected = false;
				return;

			}
			else if(cmd == Init)
			{
				std::cout << "Joined the server\n";
				conStatus.setString(conStatus.getString() + "\nJoined the server");
				constate = 2;
				connected = true;
				packet >> blueScore >> redScore;
				handleSpawn(characters, packet);

			}

			enet_packet_destroy(event.packet);

		}
		else
		{
			constate = 0;
			attempts++;
			conStatus.setString(conStatus.getString() + "\nFailed to connect, attempt  #"+toString(attempts));
			enet_peer_reset(server);

		}

	}

	sf::Clock respawnTimer;
	sf::Vector2f lastPos;
	unsigned int inputWait = 0;
	while(window.isOpen() && connected)
	{
		{
		ENetEvent event;
		if(enet_host_service(client, &event, 0) > 0)
		{
			if(event.type == ENET_EVENT_TYPE_CONNECT)
			{
			}
			else if(event.type == ENET_EVENT_TYPE_DISCONNECT)
			{
				connected = false;

			}
			else if(event.type == ENET_EVENT_TYPE_RECEIVE)
			{
				sf::Packet packet = toSFML(event.packet);
				int cmd = None;
				packet >> cmd;
				if(cmd == Spawn)
				{
					if(handleSpawn(characters, packet))
					{
						if(gameOver)
							gameOver = false;
						respawning = false;

					}

				}
				else if(cmd == Join)
				{
					std::string username;
					unsigned int team;
					packet >> username >> team;
					sf::Color c = sf::Color::Cyan;
					if(team == Bullet::Good)
						goodNames.push_back(username);
					else
					{
						badNames.push_back(username);
						c = sf::Color::Red;

					}

					gameInfo.add(username + " joined the game", c);

				}
				else if(cmd == Leave)
				{
					std::string username;
					packet >> username;
					std::cout << username << " left the game\n";
					gameInfo.add(username + "  left the game", sf::Color::White);
					handleLeave(characters, username);

				}
				else if(cmd == Shoot)
				{
					handleShooting(characters, bullets, packet, event.peer->lastRoundTripTime);

				}
				else if(cmd == Kill)
				{
					std::string killer;
					std::string victim;
					packet >> killer >> victim;
					gameInfo.add(killer + " killed " + victim, sf::Color::White);
					if(handleKill(characters, gibs, killer, victim))
					{
						respawning = true;
						respawnTimer.restart();

					}


				}
				else if(cmd == Sync)
				{
					handleSync(characters, packet);

				}
				else if(cmd == CoreStolen)
				{
					std::string username;
					packet >> username;
					handleCoreStolen(characters, username);

				}
				else if(cmd == CoreDropped)
				{
					std::string username;
					packet >> username;
					handleCoreDropped(packet, username);

				}
				else if(cmd == CoreReturned)
				{
					handleCoreReturned(packet);

				}
				else if(cmd == CoreCaptured)
				{
					std::string username;
					packet >> username;
					handleCoreCaputred(characters, username, packet);

				}
				else if(cmd == GameOver)
				{
					unsigned int team = Bullet::Good;
					packet >> team;
					if(team == Bullet::Bad)
					{
						blueWon = false;

					}

					gameOver = true;
					restartTimer.restart();

					characters.clear();
					gibs.clear();
					bullets.clear();

					camPos.x = 4096/2;
					camPos.y = 1536/2 - 240;
					respawning = false;
					blueCore.setPosition(blueCoreSpawn);
					blueCore.setRotation(0);
					redCore.setPosition(redCoreSpawn);
					redCore.setRotation(180);
					goodCoreStolen = false;
					badCoreStolen = false;
					blueScore = 0;
					redScore = 0;
					rts.stop();
					bts.stop();
					bthtf.stop();
					rthtf.stop();
					gameOverSfx.play();

				}

				enet_packet_destroy(event.packet);

			}

		}

		}

		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				window.close();

			}
			else if(event.type == sf::Event::GainedFocus)
			{
				hasFocus = true;

			}
			else if(event.type == sf::Event::LostFocus)
			{
				hasFocus = false;

			}

		}

		std::vector<Bullet>::iterator bulletIt;
		for(bulletIt = bullets.begin(); bulletIt != bullets.end();)
		{
			if(bulletIt->dead)
			{
				bulletIt = bullets.erase(bulletIt);
				continue;

			}

			bulletIt++;

		}

		sf::FloatRect gameViewRect(camPos.x, camPos.y, 854, 480);
		sf::View gameView(gameViewRect);
		gameView.zoom(zoom);
		window.setView(gameView);

		Input input;
		input.set(window, gameView, lastPos);

		window.clear(sf::Color::Black);
		window.draw(ground);

		std::vector<Gib>::iterator gibIt;
		for(gibIt = gibs.begin(); gibIt != gibs.end();)
		{
			Gib& gib = (*gibIt);
			if(gib.dead)
			{
				gibIt = gibs.erase(gibIt);
				continue;

			}

			gib.update();
			if(gib.collideable)
			{
				resolveCollisions(characters, gib);

			}

			gib.draw(window);

			gibIt++;

		}

		//DO CORE STUFF
		window.draw(blueCore);
		if(!goodCoreStolen && blueCore.getPosition() != blueCoreSpawn)
		{
			float ratio = blueReturn.getElapsedTime().asSeconds()/6.0;
			ratio = 1 - ratio;
			sf::RectangleShape bar(sf::Vector2f(100*ratio, 20));
			bar.setFillColor(sf::Color::Cyan);
			bar.setOrigin(50*ratio, 10);
			bar.setPosition(blueCore.getPosition());
			bar.move(0, -24);

			window.draw(bar);

			sf::Text retText("returning core", font);
			retText.setColor(sf::Color::Cyan);
			retText.setPosition(blueCore.getPosition());
			retText.move(-32, -64);
			window.draw(retText);

		}

		window.draw(redCore);
		if(!badCoreStolen && redCore.getPosition() != redCoreSpawn)
		{
			float ratio = redReturn.getElapsedTime().asSeconds()/6.0;
			ratio = 1 - ratio;
			sf::RectangleShape bar(sf::Vector2f(100*ratio, 20));
			bar.setFillColor(sf::Color::Red);
			bar.setOrigin(50*ratio, 10);
			bar.setPosition(redCore.getPosition());
			bar.move(0, -24);

			window.draw(bar);

			sf::Text retText("returning core", font);
			retText.setColor(sf::Color::Red);
			retText.setPosition(redCore.getPosition());
			retText.move(-32, -64);
			window.draw(retText);

		}

		particles.update();
		particles.draw(window);

		for(unsigned int i = 0; i < bullets.size(); i++)
		{
			Bullet& bullet = bullets[i];

			bool hit = false;
			for(int j = 0; j < 4; j++)
			{
				bullet.update(0.25);
				sf::Vector2f pos = bullet.getPosition();
				float radius = bullet.getRadius();

				for(unsigned int j = 0; j < mapRects.size(); j++)
				{
					sf::FloatRect inter;
					if(mapRects[j].getGlobalBounds().intersects(bullet.getBounds(), inter))
					{
						hit = true;
						bullet.dead = true;
						break;

					}

				}

				if(hit)
				{
					break;

				}

				for(unsigned int k = 0; k < characters.size(); k++)
				{
					Character& character = characters[k];
					if(bullet.team != character.team)
					{
						sf::Vector2f cpos = character.getPosition();
						sf::Vector2f diff = pos - cpos;
						float dist = sqrt(pow(diff.x, 2) + pow(diff.y, 2));
						float totalRadius = radius + character.getRadius();
						float intersection = totalRadius - dist;
						if(intersection > 0)
						{
							//character.setHealth(character.health-bullet.damage);
							//client no longer in charge of changing players health
							for(unsigned int l = 0; l < 20; l++)
							{
								float angle = rand()%360;
								float speed = rand()%40+40;
								sf::Vector2f v(speed*cos(angle*3.14159/180), speed*sin(angle*3.14159/180));
								particles.emit("blood", pos, v, 0.5, 1.0);

							}

							hit = true;
							bullet.dead = true;
							break;

						}

					}

				}

				if(hit)
				{
					break;

				}

			}

			bullet.draw(window);

		}

		for(unsigned int i = 0; i < characters.size(); i++)
		{
			Character& character = characters[i];
			if(character.getName() == cl_username)
			{
				character.previous = character.input;
				character.input = input;
				character.update();
				lastPos = character.getPosition();

				if(hasFocus)
				{
					if(character.previous != input || inputWait > 2)
					{
						//input.aimAngle = character.sprite.getRotation();
						sf::Packet p;
						p << (int)SyncInput;
						input.pack(p);
						character.packImportant(p);
						send(server, p, 1, false, client);
						inputWait = 0;

					}

				}

			}
			else
			{
				character.update();

			}

			resolveCollisions(characters, i);
			character.draw(window);

			if(character.shooting)
			{
				sf::Packet s;
				s << (int)Shoot;
				s << character.getShootAngle();
				send(server, s, 0, true, client);
				character.bullet.set(character.getShootPos(), character.getShootAngle());
				Bullet b(character.bullet);
				b.life = server->lastRoundTripTime/17.0*2;
				character.fireSound.play();
				bullets.push_back(Bullet(b));

			}

		}

		window.draw(trees);

		for(unsigned int i = 0; i < mapRects.size(); i++)
		{
			window.draw(mapRects[i]);

		}

		window.draw(fountainShape);

		window.setView(guiView);
		gameInfo.update();
		gameInfo.draw(window);

		if(respawning)
		{
			int t = 5 - (int)respawnTimer.getElapsedTime().asSeconds();
			respawnText.setString("respawning in\n          " + toString(t));
			window.draw(respawnText);
			if(t <= 0)
				respawning = false; //Just wait with no negative text please

		}

		if(hasFocus && sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
		{
			sf::RectangleShape back(sf::Vector2f(200, 400));
			back.setPosition((854.0/2.0) - 50, 40);
			back.setFillColor(sf::Color(90, 90, 90, 200));
			window.draw(back);

			std::string goodnamestr;
			std::string badnamestr;
			for(unsigned int i = 0; i < goodNames.size(); i++)
			{
				goodnamestr += goodNames[i] + "\n";

			}

			for(unsigned int i = 0; i < badNames.size(); i++)
			{
				badnamestr += badNames[i] + "\n";

			}

			sf::Text goodNames(goodnamestr, font);
			goodNames.setColor(sf::Color::Cyan);

			goodNames.setPosition((854.0/2.0) - 30, 45);

			sf::Text badNames(badnamestr, font);
			badNames.setColor(sf::Color::Red);

			badNames.setPosition((854.0/2.0) - 30, 245);

			window.draw(goodNames);
			window.draw(badNames);


		}
		else
		{
			sf::RectangleShape back(sf::Vector2f(200, 50));
			back.setOrigin(100, 25);
			back.setFillColor(sf::Color(90, 90, 90, 200));
			back.setPosition(sf::Vector2f(854/2, 25));
			window.draw(back);

			sf::Text score(toString(blueScore), font);
			score.setColor(sf::Color::Cyan);

			score.setPosition(854/2-75, 10);
			window.draw(score);

			score.setString(toString(redScore));
			score.setColor(sf::Color::Red);
			score.setPosition(854/2+50, 10);
			window.draw(score);

		}

		sf::Color rcolor = red.getFillColor();
		if(rcolor.a > 0)
		{
			if(rcolor.a >= 10)
				rcolor.a -= 10;
			else
				rcolor.a = 0;
			red.setFillColor(rcolor);

			window.draw(red);

		}


		if(gameOver)
		{
			sf::Text text("", font);
			text.setPosition(854/2 - 150, 210);
			int t = 20 - (int)restartTimer.getElapsedTime().asSeconds();
			if(blueWon)
			{
				text.setString("      Blue team won!\nNext game starts in " + toString(t));
				text.setColor(sf::Color::Cyan);

			}
			else
			{
				text.setString("      Red team won!\nNext game starts in " + toString(t));
				text.setColor(sf::Color::Red);

			}

			window.draw(text);

		}

		window.display();
		inputWait++;

	}

	characters.clear();
	bullets.clear();

	enet_peer_disconnect(server, 0);
	enet_host_flush(client);
	enet_peer_reset(server);
	sf::sleep(sf::seconds(1));

}
