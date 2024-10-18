#pragma once

// C++.
#include <map>
#include <vector>

// Local.
#include "SimulationManager.h"
#include "PhysicalAssemblyFactory.h"

typedef std::vector<btRigidBody*> RigidBodyList;
typedef std::vector<btCollisionObject*> CollisionObjectList;
typedef std::vector<btTypedConstraint*> ConstraintList;
typedef std::vector<btMotionState*> MotionStateList;
typedef std::vector<btCollisionShape*> CollisionShapeList;
typedef std::vector<btActionInterface*> ActionList;

class PhysicalAssembly
{
public:
    PhysicalAssembly() = default;
    virtual ~PhysicalAssembly() = default;

	RigidBodyList& GetRigidBodies() { return m_rigidBodies; }
	CollisionObjectList& GetColliders() { return m_colliders; }
	ConstraintList& GetConstraints() { return m_constraints; }
	MotionStateList& GetMotionStates() { return m_motionStates; }
	CollisionShapeList& GetShapes() { return m_shapes; }
	ActionList& GetActions() { return m_actions; }

	void Initialize();

	void AddShape(const std::string& name, btCollisionShape* shape);
	btCollisionShape* GetShapeFromName(const std::string& shapeName) const;

	void AddCollider(const std::string& name, btCollisionObject* body);

	void AddBody(const std::string& name, btRigidBody* body);
	btCollisionObject* GetBodyFromName(const std::string& bodyName) const;
	bool GetBodyName(const btRigidBody* body, std::string& name) const;

	void AddConstraint(const std::string& name, btTypedConstraint* constraint);
	btTypedConstraint* GetConstraintFromName(const std::string& constraintName) const;
	bool GetConstraintName(btTypedConstraint* constraint, std::string& name) const;

	// @TODO: This should be moved to something like a shared 'PhysicalAssemblyClass' object.
	void SetCreateInfo(const PhysicalAssemblyCreateInfo& assemblyCreateInfo)
	{
		m_assemblyCreateInfo = assemblyCreateInfo;
	}

	// @TODO: This should be moved to something like a shared 'PhysicalAssemblyClass' object.
	const PhysicalAssemblyCreateInfo& GetCreateInfo() const { return m_assemblyCreateInfo; }

protected:

	virtual void InitializeImpl() {}
	virtual void InitializeShapes() {}
	virtual void InitializeBodies() {}
	virtual void InitializeColliders() {}
	virtual void InitializeConstraints() {}
	virtual void InitializeActions() {}

	RigidBodyList m_rigidBodies;
	CollisionObjectList m_colliders;
	ConstraintList m_constraints;
	MotionStateList m_motionStates;
	CollisionShapeList m_shapes;
	ActionList m_actions;

	typedef std::map<std::string, btCollisionObject*> BodyNameMap;
	BodyNameMap m_bodyNameMap;
	
	typedef std::map<std::string, btCollisionShape*> ShapeNameMap;
	ShapeNameMap m_shapeNameMap;

	typedef std::map<std::string, btTypedConstraint*> ConstraintNameMap;
	ConstraintNameMap m_constraintNameMap;

	// @TODO: This should be moved to something like a shared 'PhysicalAssemblyClass' object,
	// since it really only needs to be stored once for each unique assembly.
	// It's here for now to make things quicker to build out.
	PhysicalAssemblyCreateInfo m_assemblyCreateInfo = {};
};