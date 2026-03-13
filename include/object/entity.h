#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include "object/transform.h"

class Entity
{
public:
    virtual ~Entity() = default;
    
    //Scene graph
	std::vector<std::unique_ptr<Entity>> children;
	Entity* parent = nullptr;

	//Space information
	Transform transform;

	// Entity state
	bool EntityDestroyed = false;

	template<typename T, typename... Args>
	T& addChild(Args&&... args)
	{
		static_assert(std::is_base_of_v<Entity, T>, "T must derive from Entity");

		auto child = std::make_unique<T>(std::forward<Args>(args)...);
        child->parent = this;

		T& ref = *child;
		children.emplace_back(std::move(child));

		ref.updateSelfAndChild();
		return ref;
	}

	//Update transform if it was changed
	void updateSelfAndChild()
	{
		if (transform.IsDirty()) {
			forceUpdateSelfAndChild();
			return;
		}
			
		for (auto&& child : children)
		{
			child->updateSelfAndChild();
		}
	}

	//Force update of transform even if local space don't change
	void forceUpdateSelfAndChild()
	{
		if (parent)
			transform.ComputeModelMatrix(parent->transform.GetModelMatrix());
		else
			transform.ComputeModelMatrix();

		for (auto&& child : children)
		{
			child->forceUpdateSelfAndChild();
		}
	}

	void cleanupDestroyed()
	{
		for(auto& child : children)
		{
			child->cleanupDestroyed();
		}
		
		children.erase(
			std::remove_if(children.begin(), children.end(), [this](const std::unique_ptr<Entity>& e){ return e->EntityDestroyed; }), 
			children.end()
		);
	}
};