// символы кириллицы, чтобы наверняка не было приколов с кодировкой

#pragma once
#include "game.hpp"

namespace network
{
	unsigned short port = 42069;
	
	sf::IpAddress server_addr(127,0,0,1);
	void ip_parser_for_init()
	{

	}

	namespace data
	{
		bool game_active = false;
		sf::Vector2f left_pos;
		sf::Vector2f right_pos;
		sf::Vector2f ball_pos;
	}

	void (*calc)();
	namespace calc_type
	{
		void local()
		{

		}

		void master()
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
				databus::game_playing = true;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
				databus::game_playing = false;
		}

		void slave()
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Backslash))
				databus::game_playing = true;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Backspace))
				databus::game_playing = false;
		}
	}

	bool (*init)();
	namespace init_type
	{
		bool local()
		{
			return 1;
		}
		bool master()
		{
			return 1;
		}
		bool slave()
		{
			return 1;
		}
	}

	bool pre_init()
	{
		switch (databus::gamemode)
		{
		case databus::game_mode::local:
			init = init_type::local;
			calc = calc_type::local;
			break;
		case databus::game_mode::net_master:
			init = init_type::master;
			calc = calc_type::master;
			break;
		case databus::game_mode::net_slave:
			init = init_type::slave;
			calc = calc_type::slave;
			break;
		default:
			init = init_type::local;
			calc = calc_type::local;
			break;
		}

		return 1;
	}
}