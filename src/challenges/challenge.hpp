#ifndef CHALLENGE_H
#define CHALLENGE_H

#include <string>

#include "challenge_data.hpp"
#include "game_window.hpp"
#include "gui_manager.hpp"
#include "input_manager.hpp"
#include "interpreter.hpp"
#include "lifeline.hpp"
#include "map.hpp"
#include "map_viewer.hpp"
#include "notification_bar.hpp"
#include "object_manager.hpp"
#include "sprite.hpp"
#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"

class InputManager;
class ChallengeData;

class Challenge {
protected:
    ChallengeData* challenge_data;
    Map* map;
    Lifeline esc_callback;
    bool run_challenge;
    std::vector<int> sprite_ids;
    std::vector<int> map_object_ids;
public:
    // TODO: Make the editor part of the core
    // engine, not the challenge class
    Challenge(ChallengeData* _challenge_data);

    virtual ~Challenge();

    virtual void start() = 0;
    virtual void finish() = 0;

    ///
    /// Runs the challenge by calling all the needed subsystems
    ///
    void run();

    ///
    /// Makes a sprite and adds it to the object manager and the current map
    /// @param position the (x, y) position of the sprite on the map
    /// @param name the name of the sprite
    /// @param walkability the walkability properties of the sprite
    /// @param sheet_id the id of the image of the sprite in its sprite sheet
    /// @param sheet_name the name of the sprite sheet
    /// @return the sprite's id
    ///
    int make_sprite(glm::vec2 position, std::string name, Walkability walkability, int sheet_id, std::string sheet_name);

    ///
    /// Makes a map object and adds it to the object manager and the current map
    /// @return the sprite's id
    ///    
    int make_map_object(glm::vec2 position, std::string name, Walkability walkability, int sheet_id, std::string sheet_name);
};

#endif
