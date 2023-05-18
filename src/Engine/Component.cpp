#include "../pch.h"
#include "Component.h"

Registry& Registry::get()
{
	static Registry registry;
	return registry;
}

entity Registry::createEntity()
{
	auto arr = std::array<IComponent*, NUM_COMPONENTS>();
	arr.fill(nullptr);
	entities_.push_back(arr);
	return entities_.size() - 1;
}

void Registry::add(entity e, IComponent* cmp)
{
	entities_[e][static_cast<size_t>(cmp->T)] = cmp;
}

void Registry::remove(entity e, IComponent* cmp)
{
	entities_[e][static_cast<size_t>(cmp->T)] = nullptr;
}

const std::array<IComponent*, NUM_COMPONENTS>& Registry::getComponents(entity e) const
{
	return entities_[e];
}

size_t Registry::size()
{
	return entities_.size();
}
