#include <string>
#include <iostream>
#include "api.h"

Player::Player(int startx, int starty): x(startx), y(starty) {}

void Player::move(int dx, int dy) {
	x += dx;
	y += dy;
}

void Player::speak_position() {
	std::cout << "I am standing at " << x << ", " << y << "!" << std::endl;
}
