#include <fenv.h>

// 3rdParty.
#include <btBulletDynamicsCommon.h>

#include <libdragon.h>

// Local.
#include "BodyFactory.h"

btRigidBody* BodyFactory::CreateRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape)
{
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
	motionState->setWorldTransform(startTransform);

	// Rigidbody is dynamic if and only if mass is non zero, otherwise static.
	btVector3 localInertia = {};
	if (mass > 0.0f)
	{
		shape->calculateLocalInertia(mass, localInertia);
	}

	btRigidBody::btRigidBodyConstructionInfo bodyInfo(mass, motionState, shape, localInertia);
	btRigidBody* rigidBody = new btRigidBody(bodyInfo);

	// @TODO: Utilize the rigid body userdata pointer to link back to the parent entity/physics component.
	//rigidBody->setUserPointer(parentEntity);

	return rigidBody;
}

btRigidBody* BodyFactory::CreateRigidBody(const BodyCreateInfo& createInfo)
{
	//btAssert((!createInfo.shape || createInfo.shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	btQuaternion orientation(createInfo.orientation.x, createInfo.orientation.y, createInfo.orientation.z, createInfo.orientation.w);
	btVector3 pos(createInfo.position.x, createInfo.position.y, createInfo.position.z);
	btTransform transform(orientation, pos);
	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	motionState->setWorldTransform(transform);

	float modifiedMass = createInfo.mass;
	if (createInfo.isStatic)
	{
		modifiedMass = 0.0f;
	}

	btVector3 localInertia = {};
	//if (createInfo.shapeType != eShapeType::Mesh)
	if (modifiedMass != 0.0f)
	{
		btCollisionShape* shape = createInfo.shape;
		shape->calculateLocalInertia(modifiedMass, localInertia);
	}

	btRigidBody::btRigidBodyConstructionInfo bodyInfo(modifiedMass, motionState, createInfo.shape, localInertia);
	bodyInfo.m_restitution = createInfo.restitution;
	bodyInfo.m_friction = createInfo.friction;

	btRigidBody* rigidBody = new btRigidBody(bodyInfo);

	// @TODO: Utilize the rigid body userdata pointer to link back to the parent entity/physics component.
	//rigidBody->setUserPointer(parentEntity);

	return rigidBody;
}

btCollisionObject* BodyFactory::CreateCollisionObject(const BodyCreateInfo& createInfo)
{
	btQuaternion orientation(createInfo.orientation.x, createInfo.orientation.y, createInfo.orientation.z, createInfo.orientation.w);
	btVector3 pos(createInfo.position.x, createInfo.position.y, createInfo.position.z);
	btTransform transform(orientation, pos);
	//btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	//motionState->setWorldTransform(transform);

	btCollisionObject* pOb = new btCollisionObject();
	pOb->setWorldTransform(transform);
	pOb->setCollisionShape(createInfo.shape);

	// @TODO: Utilize the rigid body userdata pointer to link back to the parent entity/physics component.
	//rigidBody->setUserPointer(parentEntity);

	return pOb;
}