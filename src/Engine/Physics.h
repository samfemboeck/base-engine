#pragma once
#include <vector>
#include <list>
#include <functional>
#include "QuickMaths.h"
#include "EntityGrid.h"

struct RigidBody;
class Cell;

struct ManifoldResolve
{
	RigidBody* rb1;
	RigidBody* rb2;
	Vec2f ContactPoint;
	Vec2f Normal;
	float Penetration;
};

struct CircleCollider
{
	~CircleCollider();
	Vec2f centerLocal;
	Vec2f centerWorld;
	glm::mat4 transform;
	float radius;
	RigidBody* rigidBody = nullptr;
	bool isSensor = false;
	Cell* cell = nullptr;
	std::function<void(Cell*)> collisionCallback;
};

struct Bounds
{
	Vec2f min;
	Vec2f max;

	Vec2f center()
	{
		return min + ((max - min) * 0.5f);
	}

	float width()
	{
		return max.x - min.x;
	}

	float height()
	{
		return max.y - min.y;
	}

	bool contains(Vec2f pos)
	{
		return pos.x >= min.x && pos.x <= max.x && pos.y >= min.y && pos.y <= max.y;
	}
};

struct AABB
{
	Bounds bounds;
	RigidBody* rigidBody;
	std::vector<CircleCollider*> colliders;
	bool wantsToBeDeleted = false;
	bool isCorpse = false;
	bool isMover = false;
};

class PhysicsManager
{
public:
	static bool circleVsCircle(Vec2f centerA, float radiusA, Vec2f centerB, float radiusB);
	static PhysicsManager& getInstance();
	void add(AABB* aabb);
	void update();
	void fixedUpdate();
	void testCollision(CircleCollider* collA, CircleCollider* collB);
	void resolveCollisions();
	void squareCast(Vec2f start, Vec2f end, std::vector<AABB*>& out, RigidBody* ignore = nullptr);
	bool findSpawnPosition(AABB* aabb, unsigned maxNearbyEntities, Vec2f& outpos);
	void update(AABB* aabb);
	void map(AABB* aabb);
	bool hasValidPos(AABB* aabb);
	size_t getAABBCount();

private:
	PhysicsManager();
private:
	std::vector<AABB*> aabbs_;
	std::vector<ManifoldResolve> manifolds_;
	float step_ = 1/100.0f;
};

class RigidBody
{
public:
	float getMass();
	float getInvMass();
	void setMass(float mass);
	void addImpulse(const Vec2f& impulse);
	void addTorque(float torque);
	void addForce(const Vec2f& force);
	void addPosCorrection(Vec2f correction);
	Vec2f getPosition();
	void setPosition(const Vec2f& position);
	float getRotation();
	void tick(float dt);
	Vec2f getVelocity();
	void setVelocity(Vec2f);
	void setRotation(float rad);
	void setLinearFriction(float friction);
	Vec2f getCenterOfMass();

private:
	float linearFriction_ = 0.999f;
	Vec2f position_ = Vec2f(0, 0);
	Vec2f centerOfMassLocal_ = Vec2f(0, 0);
	Vec2f centerOfMassWorld_ = Vec2f(0, 0);
	Vec2f correction_ = Vec2f(0, 0);
	float invInertia_ = 0.01f;
	float invMass_ = 1;
	float mass_ = 1;
	float velocityAngular_ = 0;
	float accelerationAngular_ = 0;
	float dampingAngular_ = 0.99f;
	float rotation_ = 0;
	Vec2f acceleration_;
	std::vector<Vec2f> forces_;
	std::vector<Vec2f> impulses_;
	std::vector<float> torques;
	Vec2f velocity_;
	float maxVelocityMagnitude_ = std::numeric_limits<float>::max();
};