#pragma once
#include <unordered_map>
#include <array>
#include "QuickMaths.h"

struct AABB;
struct Bounds;

class EntityGrid
{
public:
	inline static const unsigned GridWidth = 75;
	inline static const unsigned GridHeight = 75;
	inline static const unsigned MaxEntitiesPerSquare = 50u;
	inline static const unsigned SquareSize = 25;

public:
	static EntityGrid& getInstance();
	bool add(AABB* aabb);
	bool contains(Bounds& bounds);
	void clear();
	void draw();
	Vec2i getLocalCoord(Vec2f worldPos);
	Vec2f getWorldPos(Vec2i localCoord);
	std::array<AABB*, MaxEntitiesPerSquare>& get(unsigned x, unsigned y);
	Vec2f getPos();
	Bounds& getBounds();

private:
	EntityGrid();
	Vec2f pos_ = Vec2f(0, 0);
	Bounds* bounds_;
	std::array<std::array<std::array<AABB*, MaxEntitiesPerSquare>, GridWidth>, GridHeight> grid_;
};