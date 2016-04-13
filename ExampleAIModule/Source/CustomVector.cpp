#include "CustomVector.h"

CustomVector::CustomVector() {
	x = 0;
	y = 0;
}

CustomVector::CustomVector(int newX, int newY) {
	x = newX;
	y = newX;
}

CustomVector::CustomVector(const BWAPI::Unit& u1, BWAPI::Unit& u2) {
	// u1 is the start unit to make the vector from
	// and u2 is the goal (i.e. enemy unit would be u1, and our scout would be u2)
	int normalX = u2->getPosition().x - u1->getPosition().x;
	int normalY = u2->getPosition().y - u1->getPosition().y;
	int len = sqrt(normalX * normalX + normalY * normalY);
	int radius = u1->getType().groundWeapon().maxRange();
	
	int newX = u1->getPosition().x + radius * ((double)normalX / len);
	int newY = u1->getPosition().y + radius * ((double)normalY / len);
	BWAPI::Position periferiPosition(newX, newY);
	setStartPosition(periferiPosition);

	x = u2->getPosition().x - periferiPosition.x;
	y = u2->getPosition().y - periferiPosition.y;
}

CustomVector::~CustomVector() { }

int CustomVector::getX() const {
	return x;
}

int CustomVector::getY() const {
	return y;
}

int CustomVector::length() const {
	return sqrt(x * x + y * y);
}

int CustomVector::lengthSq() const {
	return x * x + y * y;
}

void CustomVector::normalize() {
	int l = length();
	x = x / l;
	y = y / l;
}

// Custom BWAPI Functions
void CustomVector::setStartPosition(BWAPI::Position pos) {
	startPos = pos;
}

BWAPI::Position CustomVector::getStartPosition() {
	return startPos;
}