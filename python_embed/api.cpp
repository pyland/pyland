#include <string>
#include <iostream>
#include <sstream>
#include "api.h"

Vec2D::Vec2D(int x, int y): x(x), y(y) {};

Vec2D Vec2D::operator+(Vec2D other) {
	return Vec2D(x + other.x, y + other.y);
}

void Vec2D::operator+=(Vec2D& other) {
	x += other.x;
	y += other.y;
}

std::ostream& operator<<(std::ostream& out, Vec2D& in) {
	return out << in.to_string();
}

std::string Vec2D::to_string() {
	std::ostringstream stringStream;
	stringStream << "(" << x << " " << y << ")";
	return stringStream.str();
}



Player::Player(Vec2D start, Direction direction, std::string name):
	position(start), direction(direction), name(name), script("") {}

void Player::move(Vec2D by) {
	position += by;
}

void Player::monologue() {
	std::cout << "I am " << name << "and I am standing at " << position << "!" << std::endl;
}

// TODO:
void Player::run_script() {}

void Player::give_script(std::string in) {}

