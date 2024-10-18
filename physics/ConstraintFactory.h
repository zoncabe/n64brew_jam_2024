#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class btTypedConstraint;

enum class eConstraintType
{
    Fixed,
    Point,
    Hinge,
    Slider,
    Piston,
    Generic,
    GenericSpring,
    Motor,

    Count
};

enum ConstraintAxis
{
    X,
    Y,
    Z
};

static const char* kAxes[] = { "x", "y", "z" };

eConstraintType ConstraintTypeStringToType(const std::string& typeString);

struct ContraintCreateInfo
{
    // All constraints use the following fields
    std::string name;
    std::string constraintType;
    glm::quat orientation;
    bool enabled;
    bool disableCollisions;
    float breakingThreshold;
    bool useBreakingThreshold;
    bool useOverrideSolverIterations;
    uint32_t solverIterations;
    std::string object1;
    btRigidBody* object1Body;
    glm::vec3 translationOffsetA;
    glm::quat orientationOffsetA;
    std::string object2;
    btRigidBody* object2Body;
    glm::vec3 translationOffsetB;
    glm::quat orientationOffsetB;

    // Generic
    bool useLimitAng[3];
    float limitAngLower[3];
    float limitAngUpper[3];
    bool useLimitLinear[3];
    float limitLinearLower[3];
    float limitLinearUpper[3];
    bool useAngularMotor;
    float angularMotorTargetVelocity;
    float angularMotorMaxImpulse;
    bool useLinearMotor;
    float linearMotorTargetVelocity;
    float linearMotorMaxImpulse;

    // Generic spring
    bool useSpring[3];
    float stiffness[3];
    float damping[3];
    bool useSpringAng[3];
    float stiffnessAng[3];
    float dampingAng[3];
};

class ConstraintFactory
{
public:
    ConstraintFactory() = delete;
	~ConstraintFactory() = delete;

    static btTypedConstraint* CreateConstraint(const ContraintCreateInfo& createInfo);
};