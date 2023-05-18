#include "../pch.h"
#include "Physics.h"
#include "Time.h"
#include "Timer.h"

PhysicsManager& PhysicsManager::getInstance()
{
	static PhysicsManager instance;
	return instance;
}

void PhysicsManager::add(AABB* aabb)
{
	update(aabb);
	map(aabb);
	aabbs_.push_back(aabb);
}

void PhysicsManager::update()
{
	static float accumulator = 0;
	accumulator += Time::DeltaSeconds;

	while (accumulator > step_)
	{
		fixedUpdate();
		accumulator -= step_;
	}
}

void PhysicsManager::fixedUpdate()
{
	EntityGrid::getInstance().clear();

	for (auto it = aabbs_.begin(); it != aabbs_.end();)
	{
		if ((*it)->wantsToBeDeleted)
		{
			delete *it;
			it = aabbs_.erase(it);
		}
		else
			it++;
	}

	for (auto* aabb : aabbs_)
	{
		update(aabb);
		map(aabb);
	}

	auto& entityGrid = EntityGrid::getInstance();

	for (unsigned x = 0; x < EntityGrid::GridWidth; x++)
	{
		for (unsigned y = 0; y < EntityGrid::GridHeight; y++)
		{
			auto& entitiesInSquare = entityGrid.get(x, y);

			for (unsigned i = 0; entitiesInSquare[i] && i < EntityGrid::MaxEntitiesPerSquare - 1; i++)
			{
				for (unsigned a = i + 1; a < EntityGrid::MaxEntitiesPerSquare && entitiesInSquare[a]; a++)
				{
					auto& collidersI = entitiesInSquare[i]->colliders;
					auto& collidersA = entitiesInSquare[a]->colliders;

					for (auto* colliderI : collidersI)
					{
						for (auto* colliderA : collidersA)
						{
							testCollision(colliderI, colliderA);
						}
					}
				}
			}
		}
	}

	resolveCollisions();
}

void PhysicsManager::testCollision(CircleCollider* col1, CircleCollider* col2)
{
	auto rb1 = col1->rigidBody;
	auto rb2 = col2->rigidBody;

	Vec2f dir = col2->centerWorld - col1->centerWorld;

	if (dir.x == 0.0f && dir.y == 0.0f)
		dir = col2->centerWorld - (col2->centerWorld - 1.0f);

	float sumRadii = col1->radius + col2->radius;

	if (dir.magnitude_squared() < sumRadii * sumRadii)
	{
		if (col1->collisionCallback)
			col1->collisionCallback(col2->cell);
		if (col2->collisionCallback)
			col2->collisionCallback(col1->cell);

		if
		(
			col1->isSensor ||
			col2->isSensor
		)
		return;

		ManifoldResolve manifold;
		manifold.Penetration = (sumRadii - dir.magnitude()) * 0.5f;
		manifold.Normal = dir.normalized();
		manifold.ContactPoint = col1->centerWorld + manifold.Normal * (col1->radius - manifold.Penetration);
		manifold.rb1 = rb1;
		manifold.rb2 = rb2;
		manifolds_.push_back(manifold);
	}
}

bool PhysicsManager::circleVsCircle(Vec2f centerA, float radiusA, Vec2f centerB, float radiusB)
{
	float sumRadii = radiusA + radiusB;
	Vec2f dir = centerB - centerA;
	return dir.magnitude_squared() < sumRadii * sumRadii;
}

void PhysicsManager::resolveCollisions()
{
	for (auto& man : manifolds_)
	{
		if (man.rb1->getInvMass() == 0 && man.rb2->getInvMass() == 0)
		{
			continue;
		}

		Vec2f rv = man.rb2->getVelocity() - man.rb1->getVelocity();
		float velAlongNormal = rv.dot(man.Normal);

		if (velAlongNormal > 0)
		{
			continue;
		}

		float e = 0.1f;

		float j = (1 + e) * velAlongNormal;
		auto contact_vec_1 = man.ContactPoint - man.rb1->getCenterOfMass();
		auto contact_vec_2 = man.ContactPoint - man.rb2->getCenterOfMass();
		auto angular_1 = contact_vec_1.cross(man.Normal);
		angular_1 *= angular_1;
		auto angular_2 = contact_vec_2.cross(man.Normal);
		angular_2 *= angular_2;
		j /= man.rb1->getInvMass() + man.rb2->getInvMass() + angular_1 + angular_2;
		Vec2f impulse = man.Normal * j;

		float overdrive = 10.0f; // let's add some overdrive so that parasites can spread around the map more easily

		man.rb1->addImpulse(impulse * overdrive);
		man.rb2->addImpulse(impulse * -overdrive);
		man.rb1->addTorque(contact_vec_1.cross(impulse));
		man.rb2->addTorque(-contact_vec_2.cross(impulse));

		const float percent = 0.75f;
		const float slop = 0.0f;
		Vec2f correction = man.Normal * percent * (std::max(man.Penetration - slop, 0.0f) / (man.rb1->getInvMass() + man.rb2->getInvMass()));
		man.rb1->addPosCorrection(correction * -1.0f);
		man.rb2->addPosCorrection(correction);
		man.Penetration -= correction.magnitude() * 2;
	}

	manifolds_.clear();
}

void PhysicsManager::squareCast(Vec2f start, Vec2f end, std::vector<AABB*>& out, RigidBody* ignore)
{
	auto& entityGrid = EntityGrid::getInstance();
	Vec2i startGrid = entityGrid.getLocalCoord(start);
	Vec2i endGrid = entityGrid.getLocalCoord(end);

	if (startGrid.x < 0 || startGrid.y < 0 || endGrid.x >= EntityGrid::GridWidth || endGrid.y > EntityGrid::GridHeight)
		return;

	for (unsigned x = startGrid.x; x <= endGrid.x; x++)
	{
		for (unsigned y = startGrid.y; y <= endGrid.y; y++)
		{
			auto& entitiesInSquare = entityGrid.get(x, y);
			for (auto* entity : entitiesInSquare)
			{
				if (!entity)
					break;

				if (entity->wantsToBeDeleted || entity->rigidBody == ignore)
					continue;

				out.push_back(entity);
			}
		}
	}
}

static std::random_device rd; // obtain a random number from hardware
static std::mt19937 gen(rd()); // seed the generator

bool PhysicsManager::findSpawnPosition(AABB* aabb, unsigned maxNearbyEntities, Vec2f& outPos)
{
	auto& entityGrid = EntityGrid::getInstance();
	Vec2i gridMin = entityGrid.getLocalCoord(aabb->bounds.min);
	Vec2i gridMax = entityGrid.getLocalCoord(aabb->bounds.max);

	for (int x = gridMin.x - 1; x <= gridMax.x + 1; x++)
	{
		for (int y = gridMin.y - 1; y <= gridMax.y + 1; y++)
		{
			if ((x < gridMin.x || x > gridMax.x || y < gridMin.y || y > gridMax.y) && x >= 0 && x < EntityGrid::GridWidth && y >= 0 && y < EntityGrid::GridHeight)
			{
				unsigned numEntities = 0;
				auto& entitiesInSquare = entityGrid.get(x, y);

				for (auto* entity : entitiesInSquare)
				{
					if (!entity)
						break;

					if (entity->wantsToBeDeleted || (entity->isCorpse && aabb->isMover))
						continue;

					numEntities++;
				}

				if (numEntities <= maxNearbyEntities)
				{
					float range = EntityGrid::SquareSize * 0.5f;
					outPos = entityGrid.getWorldPos({ x, y }) + Random::floatRange(-range, range);
					return true;
				}
			}
		}
	}

	return false;
}

void PhysicsManager::update(AABB* aabb)
{	
	auto* rigidBody = aabb->rigidBody;
	rigidBody->tick(step_);

	for (CircleCollider* coll : aabb->colliders)
	{
		auto tf =
			TRANSLATE(coll->rigidBody->getPosition().x, coll->rigidBody->getPosition().y) *
			ROTATE(coll->rigidBody->getRotation()) *
			TRANSLATE(coll->centerLocal.x, coll->centerLocal.y) *
			SCALE(coll->radius * 2, coll->radius * 2);

		coll->transform = tf;
		coll->centerWorld = glm::vec2(tf * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	static constexpr float fMax = std::numeric_limits<float>::max();
	static constexpr float fMin = std::numeric_limits<float>::lowest();

	Vec2f min = { fMax, fMax };
	Vec2f max = { fMin, fMin };

	for (const auto* collider : aabb->colliders)
	{
		const Vec2f& pos = collider->centerWorld;

		if (pos.x < min.x)
			min.x = pos.x;
		if (pos.y < min.y)
			min.y = pos.y;
		if (pos.x > max.x)
			max.x = pos.x;
		if (pos.y > max.y)
			max.y = pos.y;
	}

	// TODO this
	/*
	min.x -= Cell::Size * 0.5f;
	min.y -= Cell::Size * 0.5f;
	max.x += Cell::Size * 0.5f;
	max.y += Cell::Size * 0.5f;
	*/

	aabb->bounds.min = min;
	aabb->bounds.max = max;

	Bounds gridBounds = EntityGrid::getInstance().getBounds();
	Vec2f rbPos = rigidBody->getPosition();
	if (aabb->bounds.max.x < gridBounds.min.x)
		rigidBody->setPosition({ rbPos.x + EntityGrid::GridWidth * EntityGrid::SquareSize, rbPos.y });
	if (aabb->bounds.max.y < gridBounds.min.y)
		rigidBody->setPosition({ rbPos.x, rbPos.y + EntityGrid::GridHeight * EntityGrid::SquareSize });
	if (aabb->bounds.min.x > gridBounds.max.x)
		rigidBody->setPosition({ rbPos.x - EntityGrid::GridWidth * EntityGrid::SquareSize, rbPos.y });
	if (aabb->bounds.min.y > gridBounds.max.y)
		rigidBody->setPosition({ rbPos.x, rbPos.y - EntityGrid::GridHeight * EntityGrid::SquareSize });
}

void PhysicsManager::map(AABB* aabb)
{
	EntityGrid::getInstance().add(aabb);
}

bool PhysicsManager::hasValidPos(AABB* aabb)
{
	auto min = aabb->bounds.min;
	auto max = aabb->bounds.max;
	auto pos = EntityGrid::getInstance().getPos();
	return min.x > pos.x && min.y > pos.y && max.x < pos.x + EntityGrid::GridWidth * EntityGrid::SquareSize && max.y < pos.y + EntityGrid::GridHeight * EntityGrid::SquareSize;
}

size_t PhysicsManager::getAABBCount()
{
	return aabbs_.size();
}

PhysicsManager::PhysicsManager()
{

}

void RigidBody::addPosCorrection(Vec2f correction)
{
	correction_ += correction * invMass_;
}

Vec2f RigidBody::getPosition()
{
	return position_;
}

void RigidBody::setPosition(const Vec2f& position)
{
	position_ = position;
}

float RigidBody::getRotation()
{
	return rotation_;
}

void RigidBody::tick(float dt)
{	
	velocity_ = velocity_ * linearFriction_;

	acceleration_ = {0, 0};

	for (const auto& f : forces_)
		acceleration_ += f;

	for (const auto& i : impulses_)
		acceleration_ += i;

	acceleration_ *= invMass_;
	impulses_.clear();
	forces_.clear();
	velocity_ += acceleration_ * dt;
	position_ += velocity_ * dt + correction_;
	correction_ = { 0, 0 };
	centerOfMassWorld_ = position_ + centerOfMassLocal_;

	accelerationAngular_ = 0;

	for (float f : torques)
		accelerationAngular_ += f;

	torques.clear();

	velocityAngular_ += accelerationAngular_ * dt;
	rotation_ += velocityAngular_ * dt;
	velocityAngular_ *= dampingAngular_;
}

Vec2f RigidBody::getVelocity()
{
	return velocity_;
}

void RigidBody::setVelocity(Vec2f velocity)
{
	velocity_ = velocity;
}

void RigidBody::setRotation(float rad)
{
	rotation_ = rad;
}

void RigidBody::setLinearFriction(float friction)
{
	linearFriction_ = friction;
}

Vec2f RigidBody::getCenterOfMass()
{
	return centerOfMassWorld_;
}

CircleCollider::~CircleCollider()
{
}

float RigidBody::getMass()
{
	return mass_;
}

float RigidBody::getInvMass()
{
	return invMass_;
}

void RigidBody::setMass(float mass)
{
	mass_ = mass;
	invMass_ = 1.0f / mass;
	invInertia_ = 0.01f / mass;	
}

void RigidBody::addImpulse(const Vec2f& impulse)
{
	impulses_.push_back(impulse * invMass_);
}

void RigidBody::addTorque(float torque)
{
	torques.push_back(invMass_ * torque);
}

void RigidBody::addForce(const Vec2f& force)
{
	forces_.push_back(force);
}
