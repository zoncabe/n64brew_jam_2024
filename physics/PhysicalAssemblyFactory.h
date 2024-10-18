#pragma once

#include "BodyFactory.h"
#include "ShapeFactory.h"
#include "ConstraintFactory.h"

class PhysicalAssembly;

enum class AssemblyType
{
	Base,
	Ragdoll,
	Vehicle,
	Character,

	Count
};

typedef std::vector<BodyCreateInfo> BodyCreateInfoList;
typedef std::vector<ContraintCreateInfo> ConstraintCreateInfoList;
typedef std::vector<ShapeCreateInfo> ShapeCreateInfoList;
// @TODO: Are these needed here?
//typedef std::vector<btCollisionObject*> CollisionObjectList;
//typedef std::vector<btMotionState*> MotionStateList;
//typedef std::vector<btActionInterface*> ActionList;

struct PhysicalAssemblyCreateInfo
{
	BodyCreateInfoList bodies;
	ConstraintCreateInfoList constraints;
	ShapeCreateInfoList shapes;
};

class PhysicalAssemblyFactory
{
public:
	PhysicalAssemblyFactory() = delete;
	~PhysicalAssemblyFactory() = delete;

	static PhysicalAssembly* CreatePhysicalAssembly(AssemblyType type);

	static PhysicalAssembly* CreatePhysicalAssembly(const PhysicalAssemblyCreateInfo& createInfo);
};