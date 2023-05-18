#pragma once

using entity = uint32_t;

struct IComponent
{
	enum class Type : size_t
	{
		RigidBody,
		CircleCollider,
		COUNT
	};

	bool IsAlive = false;
	Type T;
};

#define NUM_COMPONENTS static_cast<size_t>(IComponent::Type::COUNT)

class Registry
{
public:
	static Registry& get();
	entity createEntity();
	void add(entity e, IComponent* cmp);
	void remove(entity e, IComponent* cmp);
	const std::array<IComponent*, NUM_COMPONENTS>& getComponents(entity e) const;
	size_t size();

private:
	std::vector<std::array<IComponent*, NUM_COMPONENTS>> entities_;
};

template<typename T, size_t maxComponents>
class ComponentSys
{
public:
	static ComponentSys<T, maxComponents>& get()
	{
		static ComponentSys<T> instance;
		return instance;
	}

	T& addComponent(entity e)
	{
		Registry::get().add(e, &components_[e]);
		components_[e].IsAlive = true;
		return components_[e];
	}

	void deleteComponent(entity e)
	{
		Registry::get().remove(e, &components_[e]);
		components_[e].IsAlive = false;
	}

	T* getComponent(entity e)
	{
		return components_[e].IsAlive ? &components_[e] : nullptr;
	}

	void update()
	{
	}

	auto& getComponents() const
	{
		return components_;
	}

protected:
	std::array<T, maxComponents> components_;

private:
	ComponentSys() = default;
};
