// символы кириллицы, чтобы наверняка не было приколов с кодировкой

#pragma once

namespace gcolor
{
	sf::Color dark(46, 52, 64);
	sf::Color white(236, 239, 244);
	sf::Color red(191, 97, 106);
	sf::Color green(163, 190, 140);
	sf::Color orange(208, 135, 112);
	sf::Color yellow(235, 203, 139);
	sf::Color dgrey(76, 86, 106);
	sf::Color wgrey(216, 222, 233);
}

namespace ui
{
	sf::Font terminus;
	sf::Text fps(terminus);

	bool init_text()
	{
		if (!terminus.openFromFile("../resource/fonts/terminus.ttf"))
		{
			return 0;
		}
		fps.setCharacterSize(32);
		fps.setPosition({8, 8});
		return 1;
	}

	void calc()
	{
		int fps_val = 10000 / databus::loop_time;

		char fpsbuf_[32];
		sprintf_s(fpsbuf_, "FPS: %d", fps_val);
		std::string fps_buf(fpsbuf_);
		fps.setString(fps_buf);
	}

	void draw()
	{
		databus::window->draw(fps);
	}
}

namespace sounds
{
	float maxvol = 0.f;

	sf::SoundBuffer click_buf;
	sf::SoundBuffer hit_buf;
	sf::SoundBuffer hit_wall_buf;
	sf::SoundBuffer lose_in_buf;
	sf::SoundBuffer lose_out_buf;
	sf::SoundBuffer point_up_buf;

	sf::Sound click(click_buf);
	sf::Sound hit(hit_buf);
	sf::Sound hit_wall(hit_wall_buf);
	sf::Sound lose_in(lose_in_buf);
	sf::Sound lose_out(lose_out_buf);
	sf::Sound point_up(point_up_buf);



	bool init()
	{
		if (!click_buf.loadFromFile("../resource/sounds/click.ogg"))
		{
			return false;
		}
		if (!hit_buf.loadFromFile("../resource/sounds/hit.ogg"))
		{
			return false;
		}
		if (!hit_wall_buf.loadFromFile("../resource/sounds/hit_wall.ogg"))
		{
			return false;
		}
		if (!lose_in_buf.loadFromFile("../resource/sounds/lose_in.ogg"))
		{
			return false;
		}
		if (!lose_out_buf.loadFromFile("../resource/sounds/lose_out.ogg"))
		{
			return false;
		}
		if (!point_up_buf.loadFromFile("../resource/sounds/point_up.ogg"))
		{
			return false;
		}

		click.setVolume(maxvol);
		hit.setVolume(maxvol);
		hit_wall.setVolume(maxvol);
		lose_in.setVolume(maxvol);
		lose_out.setVolume(maxvol);
		point_up.setVolume(maxvol);

		return true;
	}
}

namespace game
{
	const sf::Vector2f view_size = {1600,900};
	sf::View view;

	bool initial = true;

	enum class whois { game, player, enemy, server, client };

	whois ball_owner = whois::game;

	sf::Keyboard::Key key_player_up = sf::Keyboard::Key::W;
	sf::Keyboard::Key key_player_dn = sf::Keyboard::Key::S;
	sf::Keyboard::Key key_enemy_up = sf::Keyboard::Key::Up;
	sf::Keyboard::Key key_enemy_dn = sf::Keyboard::Key::Down;
	sf::Keyboard::Key key_start = sf::Keyboard::Key::Space;

	namespace racket
	{
		float offset = 14;
		float speed = 0.1;
		float outline = 4;
		sf::RectangleShape player;
		sf::RectangleShape enemy;
	}
	namespace ball
	{
		sf::CircleShape sprite;

		const float base_speed = 0.05;
		const float bounced_speed = base_speed * 4;
		float current_speed = bounced_speed;
		float y_deviation_offset = (0.f / 100.f);
		float y_deviation_modifier = 8;
		bool unrecovery = false;
	}

	sf::Vector2f calculated_racket_size()
	{
		return sf::Vector2f(view_size.x / 30, view_size.y / 4);
	}


	void (*calc)();
	namespace calc_type
	{
		namespace local_fns
		{
			void point_up(whois winner)
			{
				sounds::point_up.play();
			}

			void player_move()
			{
				float player_move_sign = sf::Keyboard::isKeyPressed(key_player_up) * -1 + sf::Keyboard::isKeyPressed(key_player_dn) * 1;
				float player_pos_min = racket::player.getLocalBounds().size.y / 2 + racket::offset;
				float player_pos_max = view_size.y - racket::player.getLocalBounds().size.y / 2 - racket::offset;
				float player_pos = racket::player.getPosition().y;
				player_pos += player_move_sign * databus::loop_time * racket::speed;
				player_pos = std::clamp(player_pos, player_pos_min, player_pos_max);
				racket::player.setPosition({ racket::player.getPosition().x, player_pos });
			}

			void enemy_move()
			{
				float enemy_move_sign = sf::Keyboard::isKeyPressed(key_enemy_up) * -1 + sf::Keyboard::isKeyPressed(key_enemy_dn) * 1;
				float enemy_pos_min = racket::enemy.getLocalBounds().size.y / 2 + racket::offset;
				float enemy_pos_max = view_size.y - racket::enemy.getLocalBounds().size.y / 2 - racket::offset;
				float enemy_pos = racket::enemy.getPosition().y;
				enemy_pos += enemy_move_sign * databus::loop_time * racket::speed;
				enemy_pos = std::clamp(enemy_pos, enemy_pos_min, enemy_pos_max);
				racket::enemy.setPosition({ racket::enemy.getPosition().x, enemy_pos });
			}

			void ball_move()
			{
			sf::Vector2f ball_pos(ball::sprite.getPosition());
				if (databus::game_playing)
				{
					float ball_direction_sign = 0;
				def_dir:
					switch (ball_owner)
					{
					case whois::player: 
					{
						ball_direction_sign = -1;
						break;
					}
					case whois::enemy: 
					{
						ball_direction_sign = 1;
						break;
					}
					case whois::game: 
					{
						if (std::rand() / RAND_MAX == 1)
							ball_owner = whois::enemy;
						else
							ball_owner = whois::player;
						goto def_dir;
						break;
					}
					default:
					{
						ball_owner = whois::game;
						goto def_dir;
						break;
					}
					}

					ball::current_speed = std::clamp((ball::current_speed - ball::base_speed / 4000 * databus::loop_time), ball::base_speed, ball::bounced_speed);
					ball_pos = sf::Vector2f
					(
						ball_pos.x + ball::current_speed * ball_direction_sign * databus::loop_time,
						ball_pos.y + ball::y_deviation_offset * databus::loop_time
					);

					if (
						ball::y_deviation_offset < 0
						&&
						ball_pos.y <= ball::sprite.getLocalBounds().size.y
						)
						{
							sounds::hit_wall.play();
							ball::y_deviation_offset *= -1;
						}
					if (
					ball_pos.y >= databus::default_window_view->getSize().y - (ball::sprite.getLocalBounds().size.y)
					&&
					ball::y_deviation_offset > 0
						)
						{
							sounds::hit_wall.play();
							ball::y_deviation_offset *= -1;
						}

					if (!ball::unrecovery && ball_owner == whois::player
						&&
						ball_pos.x < (racket::player.getLocalBounds().size.x + ball::sprite.getLocalBounds().size.x / 2))
					{
						if (
							ball::sprite.getPosition().y <= (racket::player.getPosition().y + racket::player.getSize().y / 2)
							&&
							ball::sprite.getPosition().y >= (racket::player.getPosition().y - racket::player.getSize().y / 2)
							)
						{
							sounds::hit.play();
							ball_owner = whois::enemy;
							ball::current_speed = ball::bounced_speed;
							ball::y_deviation_offset +=
								(racket::player.getPosition().x - (databus::default_window_view->getSize().x / ball::y_deviation_modifier))
								/
								(databus::default_window_view->getSize().x / 2) / 100;
						}
						else
						{
							sounds::lose_in.play();
							ball::unrecovery = true;
						}
					}
					if (!ball::unrecovery && ball_owner == whois::enemy
						&&
						ball_pos.x > (databus::default_window_view->getSize().x - racket::enemy.getLocalBounds().size.x - ball::sprite.getLocalBounds().size.x / 2))
					{
						auto spr = ball::sprite.getPosition();
						if (
							spr.y <= (racket::enemy.getPosition().y + racket::enemy.getSize().y / 2)
							&&
							ball::sprite.getPosition().y >= (racket::enemy.getPosition().y - racket::enemy.getSize().y / 2)
							)
						{
							sounds::hit.play();
							ball_owner = whois::player;
							ball::current_speed = ball::bounced_speed;
							ball::y_deviation_offset +=
								(racket::enemy.getPosition().x - (databus::default_window_view->getSize().x / ball::y_deviation_modifier))
								/
								(databus::default_window_view->getSize().x / 2) / 100;
						}
						else
						{
							sounds::lose_in.play();
							ball::unrecovery = true;
						}
					}

					if (ball::unrecovery && sounds::lose_in.getStatus() == sf::SoundSource::Status::Stopped)
					{
						sounds::lose_out.play();
						point_up(ball_owner);
						
						databus::game_playing = false;
						ball::unrecovery = false;
						switch(ball_owner)
						{
						case whois::enemy:
							ball_owner = whois::player; break;
						case whois::player:
							ball_owner = whois::enemy; break;
						default: break;
						}
					}
				}
				else
				{
					switch (ball_owner)
					{
					case whois::player:
					{
						ball_pos = racket::player.getPosition() + sf::Vector2f(racket::player.getLocalBounds().size.x, 0);
						break;
					}
					case whois::enemy:
					{
						ball_pos = racket::enemy.getPosition() - sf::Vector2f(racket::enemy.getLocalBounds().size.x, 0);
						break;
					}
					case whois::game:
					{
						if (!initial)
						{
							if (std::rand() / RAND_MAX == 1)
								ball_owner = whois::enemy;
							else
								ball_owner = whois::player;
						}
						break;
					}

					default: break;
					}
					if (sf::Keyboard::isKeyPressed(key_start))
					{
						if (initial) initial = true;
						databus::game_playing = true;
						sounds::click.play();
					}
				}
				ball::sprite.setPosition(ball_pos);
			}
		}

		void local()
		{
			local_fns::player_move();
			local_fns::enemy_move();
			local_fns::ball_move();
		}

		void netmaster()
		{

		}

		void netslave()
		{

		}
	}
	
	bool init()
	{
		view.setSize(view_size);
		view.setCenter(view_size / 2.f);

		racket::player.setSize(calculated_racket_size());
		racket::player.setOrigin({ -racket::offset, racket::player.getLocalBounds().size.y / 2});
		racket::player.setFillColor(gcolor::dgrey);
		racket::player.setOutlineColor(gcolor::orange);
		racket::player.setOutlineThickness(racket::outline);

		racket::enemy.setSize(calculated_racket_size());
		racket::enemy.setOrigin({ racket::enemy.getLocalBounds().size.x + racket::offset, racket::enemy.getLocalBounds().size.y / 2});
		racket::enemy.setFillColor(gcolor::wgrey);
		racket::enemy.setOutlineColor(gcolor::orange);
		racket::enemy.setOutlineThickness(racket::outline);

		ball::sprite.setRadius(calculated_racket_size().y / 10);
		ball::sprite.setOrigin({ ball::sprite.getLocalBounds().size.x / 2 , ball::sprite.getLocalBounds().size.y / 2 });
		ball::sprite.setFillColor(gcolor::red);
		ball::sprite.setOutlineColor(gcolor::orange);
		ball::sprite.setOutlineThickness(2);

		racket::player.setPosition({ 0, view_size.y / 2 });
		racket::enemy.setPosition({ view_size.x , view_size.y / 2 });
		ball::sprite.setPosition({ view_size.x / 2 , view_size.y / 2 });

		switch (databus::gamemode)
		{
		case databus::game_mode::local:
			calc = calc_type::local;
			break;
		case databus::game_mode::net_master:
			calc = calc_type::netmaster;
			break;
		case databus::game_mode::net_slave:
			calc = calc_type::local;
			break;
		default:
			calc = calc_type::local;
			break;
		}

		databus::game_loaded = true;
		return 1;
	}

	void draw()
	{
		databus::window->draw(racket::enemy);
		databus::window->draw(racket::player);
		databus::window->draw(ball::sprite);
	}
}

namespace music
{
	sf::Music fg;
	sf::Music bg;

	int32_t music_loop_point = 16902;
	float maxvol = 0.f;
	float bg_vol = 100.f;
	float fg_vol = 0.f;

	bool init()
	{
		if (!fg.openFromFile("../resource/music/cant take it.ogg")) return 0;
		if (!bg.openFromFile("../resource/music/cant take it BG.ogg")) return 0;
		bg.setLoopPoints({sf::milliseconds(music_loop_point), bg.getDuration()});
		bg.setLooping(true);
		fg.setLoopPoints({ sf::milliseconds(music_loop_point), fg.getDuration() });
		fg.setLooping(true);

		bg.setVolume(maxvol);
		bg.play();
		fg.setVolume(0.f);
		fg.play();
		return 1;
	}

	void calc()
	{
		float change_factor = databus::loop_time / 40;

		if (databus::game_playing)
		{
			fg_vol = std::clamp((fg_vol + change_factor), 0.f, 100.f);
			bg_vol = std::clamp((bg_vol - change_factor), 0.f, 100.f);
		}
		else
		{
			fg_vol = std::clamp((fg_vol - change_factor), 0.f, 100.f);
			bg_vol = std::clamp((bg_vol + change_factor), 0.f, 100.f);
		}

		bg.setVolume(bg_vol * maxvol / 100);
		fg.setVolume(fg_vol * maxvol / 100);
	}
}