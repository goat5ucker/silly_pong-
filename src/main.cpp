// символы кириллицы, чтобы наверняка не было приколов с кодировкой

#include <iostream>
#include <fstream>
#include <sstream>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include "databus.hpp"
#include "network.hpp"
#include "game.hpp"

    // namespace for managing settings file
namespace settings
{
        // path to settings file
    std::string settings_path = "../resource/settings";

        // flag of presence of settings file
    bool presence = false;
        // flag of settings file corruption suspicion 
    bool corrupt_question = false;
        // names of setting entry and flags of its successful reading
    std::string music_maxvol = "music_vol";
    bool music_volume_read = false;
    std::string sounds_maxvol = "sounds_vol";
    bool sounds_volume_read = false;
        // header string
    std::string header = "# this is a settings file. you should NOT be here!";

        // sub-function for writing settings to a file
    void writelist(std::string filename)
    {
            // opening the file for writing
        std::ofstream file(filename);

            // write operations
        file << header << std::endl;
        file << music_maxvol << " " << music::maxvol << std::endl;
        file << sounds_maxvol << " " << sounds::maxvol << std::endl;

            // closing the file
        file.close();
    }

        // function that checks the settings file and writes settings to it
    void write()
    {
            // openting the settings file for reading
        std::ifstream settings(settings_path);

            // handling if settings file couldn't be opened
        if (!settings)
        {
            std::cout << "opening settings file failed. god help us all.\n\n";
            presence = false;
        }
        else
        {   // handling write opetaions
                // closing the settings file and 
            settings.close();
                // passing the path of settings file to the writer function
            writelist(settings_path);
                // flagging the presence of settings file
            if(presence = false)
                presence = true;
                // cherry on top
            std::cout << "Write to settings file successful" << std::endl;
        }
    }

    // a silly function that asks you to turn up the music volume
    void volume_query()
    {
            // checking the volume
        if (music::maxvol <= 10)
        {
            std::cout << "Music volume is set to low amount, would you like to turn it up a little bit?\n";
            std::cout << "  Just enter a number from 11 to 100 to choose your music volume,\n";
            std::cout << "  or enter something that is less than 10 to turn it off,\n";
            std::cout << "       but it'd cause this prompt again on next start of the game." << std::endl << ":";

                // introducing the volume buffer
            int volbuf = 0;
        
        // identificator of starting the reading of volume input
        vol_input:
            std::cin >> volbuf;
            
                // handling the erroneous input
            if (std::cin.fail())
            {
                std::cin.clear();
                std::cin.ignore(1024, '\n');
                std::cout << "  I SAID enter a number from 1 to 100 to choose your music volume," << std::endl;
                std::cout << "  or enter 0 to turn it off" << std::endl << ":";
                goto vol_input;
            }
            else
            {
                    // clearing the cin
                std::cin.clear();
                std::cin.ignore(1024, '\n');
            }

                // writing the volume to the game memory
            music::maxvol = std::clamp((float)volbuf, 0.f, 100.f);

                // cherry ontop
            std::cout << "Max Volume set: " << music::maxvol << "\n\n" << std::endl;
        }
        else std::cout << std::endl;
    }

        // function that checks the settings file, 
    void read()
    {
            // opening the settings file for reading 
        std::ifstream settings(settings_path);

            // handling the presence of file
        if (!settings.good())
        {
            std::cout << "opening settings file failed, creating new file." << std::endl;
                // attempting to create new settings file
            std::ofstream settings(settings_path);
                // handling if even creating new settings file failed
            if (!settings.good())
            {
                presence = false;
                std::cout << "creating new settings file failed. god help us all.\n\n";
            }
            else
            {   // writing defaults to newborn settings file
                settings.close();
                    // remark: writing function below alse should modify the presence flag
                write();
                std::cout << "creating new settings file with defaults successful.\n" << std::endl;
            }
        }
        else
        {   // reading the opened settings file
                // line buffer
            std::string line;

                // getting and reading lines from file
            while (std::getline(settings, line))
            {
                    // skipping if line is commented out
                if (line.empty() || line[0] == '#') continue;

                    // creating "input string stream" buffer and writing line buffer to it
                std::istringstream iss (line);
                    // creating string buffer for setting entry name
                std::string stringbuf;
                    // creating float buffer for setting entry value
                float floatbuf = 0;

                    // proceeding if line is formatted as "string float"
                if ((iss >> stringbuf >> floatbuf)) {
                        // handling music volume
                    if (stringbuf == music_maxvol)
                    {
                        music::maxvol = std::clamp(floatbuf, 0.f, 100.f);
                        music_volume_read = true;
                        continue;
                    }
                        // handling sounds volume
                    if (stringbuf == sounds_maxvol)
                    {
                        sounds::maxvol = std::clamp(floatbuf, 0.f, 100.f);
                        sounds_volume_read = true;
                        continue;
                    }
                }
            }

                // cherry ontop reading settings. strings in quotes for std::cout speak for themselves.
            if (music_volume_read)
                std::cout << "Max Music Volume set: " << music::maxvol << std::endl;
            else
            {
                std::cout << "failed to read music volume settings." << std::endl;
                corrupt_question = true;
            }
            if (sounds_volume_read)
                std::cout << "Max Sounds Volume set: " << sounds::maxvol << std::endl;
            else
            {
                std::cout << "failed to read sounds volume settings." << std::endl;
                corrupt_question = true;
            }

                // outputting the message if there is suspicion that settings file is corrupt
            if (corrupt_question)
            {
                std::cout << "settings file may be corrupt." << std::endl;
            }
            else    // cherry ontop
                std::cout << "Settings file read successful.\n" << std::endl;
            
                // closing the settings file 
            presence = true;
            settings.close();
            volume_query();
        }
    }
}

    // this chooser is for further game expansion. in current state of all of the code (19 march 2025), there is no online mode.
    // however, this function modifies the gamemode in databus, despite contents of this function.
    // TODO: DO COMMENTS IN THIS FUNCTION AS EXPANSION OF CODE CONTINUES
void choose_mode()
{
    std::cout << "Choose game mode:" << std::endl;
chooser:
    {
        int chooser_buffer;
        std::cout << "[1] Local\n[2] [NOT IMPLEMENTED YET] Network (you are server)\n";
        std::cout << "[3] [NOT IMPLEMENTED YET] Network(you are connecting to other server)\n";
        std::cout << "  (type in a number and press enter)" << std::endl;
        std::cin >> chooser_buffer;
        std::cin.clear(); std::cin.ignore(1024, '\n');
        switch (chooser_buffer)
        {
        case 1:
        {
            databus::gamemode = databus::game_mode::local;
            std::cout << "Local gameplay is chosen!" << std::endl;
            sf::sleep(sf::seconds(0.5f));
            break;
        }
        case 2:
        {
            databus::gamemode = databus::game_mode::net_master;
            std::cout << "Network gameplay is chosen! You are becoming a server!\n";
            std::cout << "at least, if i implemented this in code..." << std::endl;
            sf::sleep(sf::seconds(2.f));

            std::cout << "loading local gameplay" << std::endl;
            sf::sleep(sf::seconds(0.5f));
            break;
        }
        case 3:
        {
            databus::gamemode = databus::game_mode::net_slave;
            std::cout << "Type in server's address (example - 192.168.0.96)\n";
            std::cout << "... but you can't do that 'cause i have not implemented it yet lmao" << std::endl;
            sf::sleep(sf::seconds(2.f));

            std::cout << "loading local gameplay" << std::endl;
            sf::sleep(sf::seconds(0.5f));
            break;
        }
        default:
        {
            std::cout << "\n\n\n\nthis is not a valid number, try again or close this window.\n\n"; 
            goto chooser;
        }
        }
    }
}

    // convenient place to do initialization code.
bool init()
{
        // flag that marks if some init code has failed
    bool init_success = true;
        // initialization of network subystem
    init_success = network::pre_init();
    init_success = network::init();
        // initialization of game: setting the sprites and calculation methods
    init_success = game::init();
        // initialization of ui: loading fonts, fine-tuning interface
    init_success = ui::init_text();
        // initialization of sounds: loading files, tuning the volume
    init_success = sounds::init();

        // return the status of success; true if successful, false if failed.
    return init_success;
}

    // convenient place to place the calculation code. names of functions speak for themselves.
void calc()
{
    network::calc();
    game::calc();
    music::calc();
    ui::calc();
}

    //convenient place to place the code that do the drawing.
void draw()
{
    databus::window->setView(game::view);
    game::draw();
    databus::window->setView(*databus::default_window_view);
    ui::draw();
}

int main()
{


        // reading the settings.
    settings::read();
        // initialization and starting the music.
    music::init();
        // mode chooser: local or network.
    // choose_mode();
        // in current state of all of the code (19 march 2025), there is no online mode. that line above is for further convenience.
        // hence, choosing the local mode of game.
    databus::gamemode = databus::game_mode::local;

        // window declaration. it also opens the declared window.
    sf::RenderWindow window(sf::VideoMode({ 1600, 900 }, 8), "sfml\\network\\pong");
        // requesting the focus for newborn window. ideally, it steals the focus from console window.
    window.requestFocus();
        //setting a window pointer in databus
    databus::window = &window;
        // this fuck-aroundery with views below is existing for unfathomable crackheaded reasons.
        // however, that is for MY convenience and it somehow works.
        // declaring a new view that copies default window view
    sf::View default_view(window.getDefaultView());
        // setting the newborn view to a window
    window.setView(default_view);
        // assigning that view to a pointer in databus
    databus::default_window_view = &default_view;
    
        // running the initialization code
    if (!init()) goto stop;
        // starting the timer that is used for calculating some time-attached events (e.g. moving the sprites of game)
    databus::logic_clock.start();

        // main loop
    while (window.isOpen())
    {
            // handling events with window
        while (const std::optional event = window.pollEvent())
        {
                // proceed to quit code if window is prompted to closing
            if (event->is<sf::Event::Closed>())
            {
                goto quit;
            }
        }

            // executing calculation by macro
        calc();
            // clearing the window of previous frame
        window.clear();
            // executing drawing code by macro
        draw();
            // displaying what drawing code virtually drew
        window.display();
            // writing the loop time elapsed and restarting the timer 
        databus::loop_time = (float)databus::logic_clock.restart().asMicroseconds() / 100;
    }

// standard code that is executing on game quit
quit:
    window.close();
    std::cout << "Quitting..." << std::endl;
        // writing the settings to a file
    settings::write();
        // sleeping so I can read the console output
    sf::sleep(sf::seconds(1));
    return 0;

// prompt that should appear if initialization of game failed
stop:
    std::cout << "loading some files failed, waiting 5 seconds and exiting.";
    sf::sleep(sf::seconds(5));
    return 1;
}