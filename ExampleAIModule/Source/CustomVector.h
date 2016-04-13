#pragma once
#include <BWAPI.h>
#include <math.h>

class CustomVector {
public:
	// Constructors & Destructor
	CustomVector();
	CustomVector(int newX, int newY);
	CustomVector(const BWAPI::Unit& u1, BWAPI::Unit& u2);
	~CustomVector();

	int getX() const;
	int getY() const;

	int length() const;
	int lengthSq() const;
	void normalize();

	// Custom method for BWAPI
	void setStartPosition(BWAPI::Position pos);
	BWAPI::Position getStartPosition();
	

private:
	int x, y, threat;
	BWAPI::Position startPos;
};