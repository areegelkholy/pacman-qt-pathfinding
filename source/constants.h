#ifndef CONSTANTS_H
#define CONSTANTS_H
#pragma once

#include <QString>

// Score values
#define BALL_SCORE        10   // Points for eating a regular dot
#define POWERBALL_SCORE   30   // Points for eating a powerball
#define GHOST_SCORE       50   // Points for eating a ghost in panic mode

#define GHOST_RESPAWN_TIME   200  // Time until a ghost respawns after being eaten
#define HIT_COOLDOWN_FRAMES 30    // Cooldown after Pacman is hit before he can be hit again

constexpr int INTERVAL         = 10;  //   interval for Pacman
constexpr int NORMAL_INTERVAL  = 10;  // Default ghost movement interval
constexpr int RUNNING_INTERVAL = 8;   // Faster interval when ghost is running back to cage

// Global variable to hold chosen Pac-Man skin
extern QString chosenPacmanSpritePath;

#define FLASH_INTERVAL   200   // Powerball flashing rate in ms

#define PANIC_DURATION   500   // Duration of ghost panic mode
#define PANIC_INTERVAL    50   // Ghost movement interval during panic mode

#endif  // CONSTANTS_H
