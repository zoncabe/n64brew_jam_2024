// Local.
#include "SimulationManager.h"
#include "PhysicalAssembly.h"

#define ENABLE_DEBUG_DRAW 0
#if ENABLE_DEBUG_DRAW
#include "PhysicsDebugDrawer.h"
#endif

#define SAFE_DELETE(x) delete x; x = nullptr;

SimulationManager::SimulationManager()
{
}

void SimulationManager::AddRigidBody(btRigidBody* rigidBody)
{
    assert(rigidBody != nullptr);
    if (rigidBody != nullptr)
    {
        mPhysicsWorld->addRigidBody(rigidBody);
    }
}

void SimulationManager::RemoveRigidBody(btRigidBody* rigidBody)
{
    assert(rigidBody != nullptr);
    if (rigidBody != nullptr)
    {
        mPhysicsWorld->removeRigidBody(rigidBody);
    }
}

void SimulationManager::AddCollider(btCollisionObject* collider)
{
    assert(collider != nullptr);
    if (collider != nullptr)
    {
        mPhysicsWorld->addCollisionObject(collider, btBroadphaseProxy::StaticFilter);
    }
}

void SimulationManager::RemoveCollider(btCollisionObject* collider)
{
    assert(collider != nullptr);
    if (collider != nullptr)
    {
        mPhysicsWorld->removeCollisionObject(collider);
    }
}

void SimulationManager::AddConstraint(btTypedConstraint* constraint)
{
    //assert(constraint != nullptr);
    if (constraint != nullptr)
    {
        bool disableConstraintsBetweenLinkedBodies = true;
        mPhysicsWorld->addConstraint(constraint, disableConstraintsBetweenLinkedBodies);
    }
}

void SimulationManager::RemoveConstraint(btTypedConstraint* constraint)
{
    assert(constraint != nullptr);
    if (constraint != nullptr)
    {
        mPhysicsWorld->removeConstraint(constraint);
    }
}

const btRigidBody* SimulationManager::PickWorld(glm::vec3 start, glm::vec3 end)
{
    const btRigidBody* result = nullptr;

    auto from = vec3_to_btVector3(start);
    auto to = vec3_to_btVector3(end);
    btCollisionWorld::ClosestRayResultCallback callback(from, to);

    mPhysicsWorld->rayTest(from, to, callback);

    if (callback.m_collisionObject)
    {
        PhysicalAssembly* assembly = static_cast<PhysicalAssembly*>(callback.m_collisionObject->getUserPointer());

        std::string bodyName;
        assembly->GetBodyName(static_cast<const btRigidBody*>(callback.m_collisionObject), bodyName);

        
        //Util::LogLine("Picked '%s'",  bodyName.c_str());

        result = static_cast<const btRigidBody*>(callback.m_collisionObject);
    }

    return result;
}

void SimulationManager::AddAssembly(PhysicalAssembly* assembly)
{
    assert(assembly != nullptr);
    if (assembly != nullptr)
    {
        // Add all of the assembly's collision objects to the world.
        const std::vector<btCollisionObject*>& assemblyColliders = assembly->GetColliders();
        for (size_t colliderIndex = 0; colliderIndex < assemblyColliders.size(); colliderIndex++)
        {
            AddCollider(assemblyColliders[colliderIndex]);
        }

        // Add all of the assembly's rigid bodies to the world.
        const std::vector<btRigidBody*>& assemblyRigidBodies = assembly->GetRigidBodies();
        for (size_t bodyIndex = 0; bodyIndex < assemblyRigidBodies.size(); bodyIndex++)
        {
            AddRigidBody(assemblyRigidBodies[bodyIndex]);
        }

        // Add all of the assembly's constraints to the world.
        const std::vector<btTypedConstraint*>& assemblyConstraints = assembly->GetConstraints();
        for (size_t constraintIndex = 0; constraintIndex < assemblyConstraints.size(); constraintIndex++)
        {
            AddConstraint(assemblyConstraints[constraintIndex]);
        }

        // Add all of the assembly's actions.
        const std::vector<btActionInterface*>& assemblyActions = assembly->GetActions();
        for (size_t actionIndex = 0; actionIndex < assemblyActions.size(); actionIndex++)
        {
            AddAction(assemblyActions[actionIndex]);
        }
    }
}

void SimulationManager::RemoveAssembly(PhysicalAssembly* assembly)
{
    assert(assembly != nullptr);
    if (assembly != nullptr)
    {
        // Remove all of the assembly's collision objects from the world.
        const std::vector<btCollisionObject*>& assemblyColliders = assembly->GetColliders();
        for (size_t colliderIndex = 0; colliderIndex < assemblyColliders.size(); colliderIndex++)
        {
            RemoveCollider(assemblyColliders[colliderIndex]);
        }

        // Remove all of the assembly's rigid bodies to the world.
        const std::vector<btRigidBody*>& assemblyRigidBodies = assembly->GetRigidBodies();
        for (size_t bodyIndex = 0; bodyIndex < assemblyRigidBodies.size(); bodyIndex++)
        {
            RemoveRigidBody(assemblyRigidBodies[bodyIndex]);
        }

        // Remove all of the assembly's constraints to the world.
        const std::vector<btTypedConstraint*>& assemblyConstraints = assembly->GetConstraints();
        for (size_t constraintIndex = 0; constraintIndex < assemblyConstraints.size(); constraintIndex++)
        {
            RemoveConstraint(assemblyConstraints[constraintIndex]);
        }

        // Remove all of the assembly's actions to the world.
        const std::vector<btActionInterface*>& assemblyActions = assembly->GetActions();
        for (size_t actionIndex = 0; actionIndex < assemblyActions.size(); actionIndex++)
        {
            RemoveAction(assemblyActions[actionIndex]);
        }
    }
}

void SimulationManager::AddAction(btActionInterface* action)
{
    mPhysicsWorld->addAction(action);
}

void SimulationManager::RemoveAction(btActionInterface* action)
{
    mPhysicsWorld->removeAction(action);
}

#define ENABLE_COLLISION_SOUNDS 0

void SimulatorNearCallback(btBroadphasePair& collisionPair, btCollisionDispatcher& dispatcher, const btDispatcherInfo& dispatchInfo)
{
    btCollisionDispatcher::defaultNearCallback(collisionPair, dispatcher, dispatchInfo);
}

void SimulationManager::Initialize()
{
    // Initialize everything.
    mCollisionConfiguration = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
    mDispatcher->setNearCallback(SimulatorNearCallback);
    mBroadphase = new btDbvtBroadphase();
    mGhostPairCallback = new btGhostPairCallback();
    auto overlappingPairCache = mBroadphase->getOverlappingPairCache();
    if (overlappingPairCache != nullptr)
    {
        overlappingPairCache->setInternalGhostPairCallback(mGhostPairCallback);
    }

#if USE_MULTIBODY_WORLD
    btMultiBodyConstraintSolver* solver = new btMultiBodyConstraintSolver();
    mSolver = solver;
    mPhysicsWorld = new btMultiBodyDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
#else
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    mSolver = solver;

    mPhysicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
#endif

    // @TODO: Don't do this in non-debug/final builds
#if ENABLE_DEBUG_DRAW
    mDebugDrawer = new PhysicsDebugDrawer();
    SetIsDebugRenderingEnabled(true);
#endif
}

void SimulationManager::SetGravity(glm::vec3 gravity)
{
    mPhysicsWorld->setGravity(vec3_to_btVector3(gravity));
}

glm::vec3 SimulationManager::GetGravity() const
{
    return btVector3_to_vec3(mPhysicsWorld->getGravity());
}

void SimulationManager::Shutdown()
{
#if ENABLE_DEBUG_DRAW
    mPhysicsWorld->setDebugDrawer(nullptr);
    SAFE_DELETE(mDebugDrawer);
#endif
    SAFE_DELETE(mPhysicsWorld);
    SAFE_DELETE(mSolver);
    SAFE_DELETE(mBroadphase);
    SAFE_DELETE(mGhostPairCallback);
    SAFE_DELETE(mDispatcher);
    SAFE_DELETE(mCollisionConfiguration);
}

bool step1Frame = false;

//////////////////////////////////////////////////////////////////////////
void SimulationManager::Update(float inTimeDelta)
{
    if (mbIsSimulationEnabled || step1Frame)
    {
        if (step1Frame)
        {
            inTimeDelta = 1.0f / 60.0f;
            step1Frame = false;
        }

        static const int kMaxSubsteps = 0;
        static const float kFixedTimestamp = 1 / 60.0f;

        float simulationTimestep = inTimeDelta * mSimulationRateModifier;

        static const int kNumSubSteps = 10;
        float substepTimespan = simulationTimestep / kNumSubSteps;

        for (int substepIndex = 0; substepIndex < kNumSubSteps; substepIndex++)
        {
            mPhysicsWorld->stepSimulation(substepTimespan, kMaxSubsteps, kFixedTimestamp);
        }
    }

    //mPhysicsWorld->debugDrawWorld();
}

void SimulationManager::StepFrame()
{
    step1Frame = true;
}

float& SimulationManager::GetSimulationRateModifier()
{
    return mSimulationRateModifier;
}

void SimulationManager::SetIsSimulationEnabled(bool isSimulationEnabled)
{
    mbIsSimulationEnabled = isSimulationEnabled;
}

void SimulationManager::SetIsDebugRenderingEnabled(bool inbEnableRendering)
{
    mbIsDebugRenderingEnabled = inbEnableRendering;

#if ENABLE_DEBUG_DRAW
    if (mbIsDebugRenderingEnabled)
    {
        mPhysicsWorld->setDebugDrawer(mDebugDrawer);
    }
    else
    {
        mPhysicsWorld->setDebugDrawer(nullptr);
    }
#endif
}

void SimulationManager::SetContactResolutionCallback(PhysicalContactResolutionCallback* callback)
{
    assert(callback != nullptr);
    if (callback != nullptr)
    {
        //mPhysicsWorld->contactTest()
    }
}