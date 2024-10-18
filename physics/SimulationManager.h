#pragma once

// 3rdParty.
#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

class PhysicalContactResolutionCallback;
class PhysicalAssembly;
class PhysicsDebugDrawer;

#define USE_MULTIBODY_WORLD 0

class vec3_to_btVector3 : public glm::vec3
{
public:
	vec3_to_btVector3(const glm::vec3& v) : glm::vec3(v) {}
	vec3_to_btVector3(const btVector3& v) : glm::vec3(v.x(), v.y(), v.z()) {}
	operator btVector3() const { return btVector3(x, y, z); }
};

class quat_to_btQuaternion : public glm::quat
{
public:
	quat_to_btQuaternion(const glm::quat& q) : glm::quat(q) {}
	quat_to_btQuaternion(const btQuaternion& q) : glm::quat(q.w(), q.x(), q.y(), q.z()) {}
	operator btQuaternion() const { return btQuaternion(x, y, z, w); }
};

class btVector3_to_vec3 : public btVector3
{
public:
	btVector3_to_vec3(const btVector3& v) : btVector3(v) {}
	btVector3_to_vec3(const glm::vec3& v) : btVector3(v.x, v.y, v.z) {}
	operator glm::vec3() const { return glm::vec3(x(), y(), z()); }
};

class btTransform_to_mat4x4 : public btTransform
{
public:
	btTransform_to_mat4x4(const btTransform& v) : btTransform(v) {}
	btTransform_to_mat4x4(const glm::mat4x4& mat)
	: btTransform(
		btQuaternion(glm::quat_cast(mat).x, glm::quat_cast(mat).y, glm::quat_cast(mat).z, glm::quat_cast(mat).w),
		btVector3(mat[3][0], mat[3][1], mat[3][2])) {}

	operator glm::mat4x4() const
	{
		glm::mat4x4 mat(1.0f);
		btQuaternion rot = getRotation();
		btVector3 pos = getOrigin();

		glm::quat q(rot.w(), rot.x(), rot.y(), rot.z());
		mat = glm::mat4_cast(q);
		mat[3][0] = pos.x();
		mat[3][1] = pos.y();
		mat[3][2] = pos.z();
		mat[3][3] = 1.0f;

		return mat;
	}
};

class SimulationManager
{
public:
	SimulationManager();
	~SimulationManager() = default;

	void Initialize();
	void Shutdown();
	void Update(float elapsed);

	void StepFrame();
	float& GetSimulationRateModifier();

	void SetIsSimulationEnabled(bool isSimulationEnabled);
	bool GetIsSimulationEnabled() const { return mbIsSimulationEnabled; }

	void AddRigidBody(btRigidBody* rigidBody);
	void RemoveRigidBody(btRigidBody* rigidBody);
	void AddCollider(btCollisionObject* collider);
	void RemoveCollider(btCollisionObject* collider);
	void AddConstraint(btTypedConstraint* constraint);
	void RemoveConstraint(btTypedConstraint* constraint);

	const btRigidBody* PickWorld(glm::vec3 start, glm::vec3 end);
	void AddAssembly(PhysicalAssembly* assembly);
	void RemoveAssembly(PhysicalAssembly* assembly);

	void AddAction(btActionInterface* action);
	void RemoveAction(btActionInterface* action);

	void SetGravity(glm::vec3 gravity);
	glm::vec3 GetGravity() const;
	PhysicsDebugDrawer* GetDebugDraw() const { return mDebugDrawer; }

	btDiscreteDynamicsWorld* GetWorld() const { return mPhysicsWorld; }

private:
	void SetIsDebugRenderingEnabled(bool inbEnableRendering);
	void SetContactResolutionCallback(PhysicalContactResolutionCallback* callback);

	btCollisionConfiguration* mCollisionConfiguration = nullptr;
	btCollisionDispatcher* mDispatcher = nullptr;
#if USE_MULTIBODY_WORLD
	btMultiBodyDynamicsWorld* mPhysicsWorld = nullptr;
	btMultiBodyConstraintSolver* mSolver = nullptr;
#else
	btConstraintSolver* mSolver = nullptr;
	btDiscreteDynamicsWorld* mPhysicsWorld = nullptr;
#endif
	btBroadphaseInterface* mBroadphase = nullptr;
	btGhostPairCallback* mGhostPairCallback = nullptr;

	PhysicsDebugDrawer* mDebugDrawer = nullptr;
	bool mbIsDebugRenderingEnabled = true;
	bool mbIsSimulationEnabled = true;
	float mSimulationRateModifier = 1.0f;
};