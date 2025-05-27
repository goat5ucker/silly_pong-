// символы кириллицы, чтобы наверняка не было приколов с кодировкой

#pragma once

namespace databus
{
	sf::RenderWindow *window;
	const sf::View* default_window_view;

	sf::Clock logic_clock;
	float loop_time = 1;

	enum class game_mode { local, net_master, net_slave};
	game_mode gamemode;


	bool game_loaded = false;
	bool game_playing = false;
}