//
// Created by mohammad on 27/01/25.
// Updated by jeffrey on 30/09/25
// Updated by chris on 13/02/25
//

#pragma once
#include "GameObject.h"

class Game {
public:
    // Configures the game and data
    Game();

    // Delete copy constructor and copy assignment operator (Rule of 3)
    Game(const Game&) = delete;            // Copy constructor
    Game& operator=(const Game&) = delete; // Copy assignment
    
    void RenderImGui();

	std::vector<GameObject> enemies;

    // Unless explicitly stated in the doc, you may change these. You many (and encouraged!) to add more

    // Ship Constants
    static constexpr float ship_movement_speed_ndc_per_sec = 0.4f;
    static constexpr float ship_rotation_speed_rad_per_sec = 6.0f;
    static constexpr float croc_rotation_speed_rad_per_sec = 1.5f;

    // Player Ship Constants
    static constexpr int player_start_health = 4;

    // Enemy Ship Constants
    static constexpr int number_of_pirateships              = 8;

    // Cannonball Constants
    static constexpr float cannon_reload_time_sec                  = 0.5f;
    static constexpr float cannonball_movement_speed_ndc_per_sec   = 3.0f;
    static constexpr float cannonball_player_col_cooldown_time_sec = 0.1f;
    

    //Game variables
    int score = 0;
    int health = 4;
};
