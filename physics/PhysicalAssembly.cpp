// Local.
#include "PhysicalAssembly.h"

void PhysicalAssembly::Initialize()
{
	InitializeImpl();

	InitializeShapes();
	InitializeColliders();
	InitializeBodies();
	InitializeConstraints();
	InitializeActions();
}

void PhysicalAssembly::AddShape(const std::string& name, btCollisionShape* shape)
{
	m_shapeNameMap[name] = shape;
	m_shapes.push_back(shape);
}

btCollisionShape* PhysicalAssembly::GetShapeFromName(const std::string& shapeName) const
{
	btCollisionShape* result = nullptr;
	
	auto res = m_shapeNameMap.find(shapeName);
	if (res != m_shapeNameMap.end())
	{
		result = res->second;
	}

	return result;
}

void PhysicalAssembly::AddBody(const std::string& name, btRigidBody* body)
{
	body->setUserPointer(this);

	m_bodyNameMap[name] = body;
	m_rigidBodies.push_back(body);
}

void PhysicalAssembly::AddCollider(const std::string& name, btCollisionObject* body)
{
	body->setUserPointer(this);

	m_bodyNameMap[name] = body;
	m_colliders.push_back(body);
}

btCollisionObject* PhysicalAssembly::GetBodyFromName(const std::string& bodyName) const
{
	btCollisionObject* result = nullptr;

	auto res = m_bodyNameMap.find(bodyName);
	if (res != m_bodyNameMap.end())
	{
		result = res->second;
	}

	return result;
}

bool PhysicalAssembly::GetBodyName(const btRigidBody* body, std::string& name) const
{
	for (auto bodyIter = m_bodyNameMap.begin(); bodyIter != m_bodyNameMap.end(); bodyIter++)
	{
		if (bodyIter->second == body)
		{
			name = bodyIter->first;
			return true;
		}
	}
	return false;
}

void PhysicalAssembly::AddConstraint(const std::string& name, btTypedConstraint* constraint)
{
	m_constraintNameMap[name] = constraint;
	m_constraints.push_back(constraint);
}

btTypedConstraint* PhysicalAssembly::GetConstraintFromName(const std::string& constraintName) const
{
	btTypedConstraint* result = nullptr;

	auto res = m_constraintNameMap.find(constraintName);
	if (res != m_constraintNameMap.end())
	{
		result = res->second;
	}

	return result;
}

bool PhysicalAssembly::GetConstraintName(btTypedConstraint* constraint, std::string& name) const
{
	for (auto constraintIter = m_constraintNameMap.begin(); constraintIter != m_constraintNameMap.end(); constraintIter++)
	{
		if (constraintIter->second == constraint)
		{
			name = constraintIter->first;
			return true;
		}
	}
	return false;
}