#pragma once

#include <vector>
#include <memory>

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

	void addChild(std::unique_ptr<Entity> child)
	{
        child->parent = this;
		children.emplace_back(std::move(child));
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
};