#ifndef MISC_H_
#define MISC_H_

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <math.h>
#include <enet/enet.h>

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

float clamp(float n, float lower, float upper)
{
	return std::max(lower, std::min(n, upper));

}

void playSound(sf::Sound& sound, sf::Vector2f lpos, sf::Vector2f pos, float max)
{
	sf::Vector2f dif = lpos - pos;
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

class Input
{
public:
	Input() :
		up(false),
		down(false),
		left(false),
		right(false),
		shooting(false),
		secondary(false),
		shoot(false),
		reload(false),
		aimAngle(0),
		flareAngle(0)
	{
		throwing = false;
	}

	void set(sf::RenderWindow& window, sf::View view, sf::Vector2f pos, bool hasFocus)
	{
		if(hasFocus)
		{
			mPos = sf::Mouse::getPosition(window);
			raimPos = window.mapPixelToCoords(mPos, view);
			sf::Vector2f aimPos = raimPos;
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

			static bool rightState = false;
			if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				if(!rightState)
				{
					rightState = true;
					//right click

				}

			}
			else if(rightState)
			{
				rightState = false;
				secondary = true;
				//right click released

			}
			else
			{
				secondary = false;

			}

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
		flags = shooting ? flags|(1<<4) : flags;
		flags = throwing ? flags|(1<<5) : flags;
		uint8_t r = (unsigned int)(aimAngle/2.0);
		packet << flags << r;
		if(shooting)
		{
			packet << flareAngle;

		}

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
		shooting = (flags&(1<<4)) != 0;
		throwing = (flags&(1<<5)) != 0;
		aimAngle = (float)(((unsigned int)r)*2.0);
		if(shooting)
		{
			packet >> flareAngle;

		}

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
	bool shooting;
	bool throwing;

	//not sent
	bool shoot;
	bool reload;
	bool secondary;

	//sent
	float aimAngle;
	float flareAngle;

	//not sent
	sf::Vector2i mPos;
	sf::Vector2f raimPos;

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

class NewBulletData
{
public:
	NewBulletData() :
		angle(0),
		id(0)
	{
	}

	NewBulletData(sf::Vector2f pos,
			float angle,
			uint16_t id) :
				pos(pos),
				angle(angle),
				id(id)
	{
	}

	void pack(sf::Packet& packet)
	{
		packet << pos.x << pos.y << angle << id;

	}

	void unpack(sf::Packet& packet)
	{
		packet >> pos.x >> pos.y >> angle >> id;

	}

	sf::Vector2f pos;
	float angle;
	uint16_t id;

};

#endif /* MISC_H_ */
