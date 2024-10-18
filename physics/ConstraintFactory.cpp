// 3rdParty.
#include <btBulletDynamicsCommon.h>

// Local.
#include "ConstraintFactory.h"
#include "SimulationManager.h"
#include "../util/MathUtil.h"

// NOTE: Keep this declaration in sync with the header.
// It also needs to match the formatting that the python exporter uses.
static const char* kConstraintTypeStrings[] =
{
    "FIXED", // @TODO: Is this needed?
    "POINT", // @TODO: Is this needed?
    "HINGE",
    "SLIDER",
    "PISTON",
    "GENERIC",
    "GENERIC_SPRING",
    "MOTOR", // @TODO: Is this needed?

    "Count",
};

eConstraintType ConstraintTypeStringToType(const std::string& typeString)
{
    eConstraintType result = eConstraintType::Fixed;

    for (size_t index = 0; index < static_cast<size_t>(eConstraintType::Count); index++)
    {
        if (typeString.compare(kConstraintTypeStrings[index]) == 0)
        {
            result = static_cast<eConstraintType>(index);
        }
    }

    return result;
}

// 
static const glm::vec3 kHingeAxis = glm::vec3(0.0f, 0.0f, 1.0f);
static const glm::vec3 kMotorAxis = glm::vec3(1.0f, 0.0f, 0.0f);

btTypedConstraint* ConstraintFactory::CreateConstraint(const ContraintCreateInfo& createInfo)
{
    btTypedConstraint* result = nullptr;

    eConstraintType type = ConstraintTypeStringToType(createInfo.constraintType);
    switch (type)
    {
        case eConstraintType::Fixed:
        {
            btTransform localA(quat_to_btQuaternion(createInfo.orientationOffsetA), vec3_to_btVector3(createInfo.translationOffsetA));
            btTransform localB(quat_to_btQuaternion(createInfo.orientationOffsetB), vec3_to_btVector3(createInfo.translationOffsetB));

            btFixedConstraint* fixedConstraint = new btFixedConstraint(*createInfo.object1Body, *createInfo.object2Body, localA, localB);
        
            result = fixedConstraint;
        }
        break;
        case eConstraintType::Hinge:
        {
            btTransform localA(quat_to_btQuaternion(createInfo.orientationOffsetA), vec3_to_btVector3(createInfo.translationOffsetA));
            btTransform localB(quat_to_btQuaternion(createInfo.orientationOffsetB), vec3_to_btVector3(createInfo.translationOffsetB));
            btHingeConstraint* newHinge = new btHingeConstraint(*createInfo.object1Body, *createInfo.object2Body, localA, localB);

            // Assume that the hinge rotation axis is around Z.
            auto transformed = MathUtil::TransformVector(kHingeAxis, createInfo.orientation);;
            btVector3 test = vec3_to_btVector3(transformed);
            newHinge->setAxis(test);

            if (createInfo.useLimitAng[ConstraintAxis::Z])
            {
                newHinge->setLimit(createInfo.limitAngLower[ConstraintAxis::Z], createInfo.limitAngUpper[ConstraintAxis::Z]);
            }

            result = newHinge;
        }
        break;
        case eConstraintType::Generic:
        {
            btTransform localA(quat_to_btQuaternion(createInfo.orientationOffsetA), vec3_to_btVector3(createInfo.translationOffsetA));
            btTransform localB(quat_to_btQuaternion(createInfo.orientationOffsetB), vec3_to_btVector3(createInfo.translationOffsetB));

            btGeneric6DofConstraint* newConstraint = new btGeneric6DofConstraint(*createInfo.object1Body, *createInfo.object2Body, localA, localB, false);

            // Set the lower and upper values for the angular limits.
            newConstraint->setAngularLowerLimit(btVector3(createInfo.limitAngLower[ConstraintAxis::X], createInfo.limitAngLower[ConstraintAxis::Z], createInfo.limitAngLower[ConstraintAxis::Y]));
            newConstraint->setAngularUpperLimit(btVector3(createInfo.limitAngUpper[ConstraintAxis::X], createInfo.limitAngUpper[ConstraintAxis::Z], createInfo.limitAngUpper[ConstraintAxis::Y]));

            // Set the lower and upper values for the linear limits.
            newConstraint->setLinearLowerLimit(btVector3(createInfo.limitLinearLower[ConstraintAxis::X], createInfo.limitLinearLower[ConstraintAxis::Z], createInfo.limitLinearLower[ConstraintAxis::Y]));
            newConstraint->setLinearUpperLimit(btVector3(createInfo.limitLinearUpper[ConstraintAxis::X], createInfo.limitLinearUpper[ConstraintAxis::Z], createInfo.limitLinearUpper[ConstraintAxis::Y]));

            result = newConstraint;
            //btHingeConstraint* newHinge = new btHingeConstraint(*createInfo.object1Body, *createInfo.object2Body, localA, localB);
        }
        break;
        case eConstraintType::GenericSpring:
        {
            btTransform localA(quat_to_btQuaternion(createInfo.orientationOffsetA), vec3_to_btVector3(createInfo.translationOffsetA));
            btTransform localB(quat_to_btQuaternion(createInfo.orientationOffsetB), vec3_to_btVector3(createInfo.translationOffsetB));

            btGeneric6DofSpringConstraint* newConstraint = new btGeneric6DofSpringConstraint(*createInfo.object1Body, *createInfo.object2Body, localA, localB, false);

            // Set the lower and upper values for the angular limits.
            newConstraint->setAngularLowerLimit(btVector3(createInfo.limitAngLower[ConstraintAxis::X], createInfo.limitAngLower[ConstraintAxis::Z], createInfo.limitAngLower[ConstraintAxis::Y]));
            newConstraint->setAngularUpperLimit(btVector3(createInfo.limitAngUpper[ConstraintAxis::X], createInfo.limitAngUpper[ConstraintAxis::Z], createInfo.limitAngUpper[ConstraintAxis::Y]));

            // Linear
            newConstraint->enableSpring(ConstraintAxis::X, createInfo.useSpring[ConstraintAxis::X]);
            newConstraint->setStiffness(ConstraintAxis::X, createInfo.stiffness[ConstraintAxis::X]);
            newConstraint->setDamping(ConstraintAxis::X, createInfo.damping[ConstraintAxis::X]);

            newConstraint->enableSpring(ConstraintAxis::Z, createInfo.useSpring[ConstraintAxis::Y]);
            newConstraint->setStiffness(ConstraintAxis::Z, createInfo.stiffness[ConstraintAxis::Y]);
            newConstraint->setDamping(ConstraintAxis::Z, createInfo.damping[ConstraintAxis::Y]);

            newConstraint->enableSpring(ConstraintAxis::Y, createInfo.useSpring[ConstraintAxis::Z]);
            newConstraint->setStiffness(ConstraintAxis::Y, createInfo.stiffness[ConstraintAxis::Z]);
            newConstraint->setDamping(ConstraintAxis::Y, createInfo.damping[ConstraintAxis::Z]);

            // Angular
            newConstraint->enableSpring(ConstraintAxis::X + 3, createInfo.useSpringAng[ConstraintAxis::X]);
            newConstraint->setStiffness(ConstraintAxis::X + 3, createInfo.stiffnessAng[ConstraintAxis::X]);
            newConstraint->setDamping(ConstraintAxis::X + 3, createInfo.dampingAng[ConstraintAxis::X]);

            newConstraint->enableSpring(ConstraintAxis::Z + 3, createInfo.useSpringAng[ConstraintAxis::Y]);
            newConstraint->setStiffness(ConstraintAxis::Z + 3, createInfo.stiffnessAng[ConstraintAxis::Y]);
            newConstraint->setDamping(ConstraintAxis::Z + 3, createInfo.dampingAng[ConstraintAxis::Y]);

            newConstraint->enableSpring(ConstraintAxis::Y + 3, createInfo.useSpringAng[ConstraintAxis::Z]);
            newConstraint->setStiffness(ConstraintAxis::Y + 3, createInfo.stiffnessAng[ConstraintAxis::Z]);
            newConstraint->setDamping(ConstraintAxis::Y + 3, createInfo.dampingAng[ConstraintAxis::Z]);


            // Set the lower and upper values for the linear limits.
            newConstraint->setLinearLowerLimit(btVector3(createInfo.limitLinearLower[ConstraintAxis::X], createInfo.limitLinearLower[ConstraintAxis::Z], createInfo.limitLinearLower[ConstraintAxis::Y]));
            newConstraint->setLinearUpperLimit(btVector3(createInfo.limitLinearUpper[ConstraintAxis::X], createInfo.limitLinearUpper[ConstraintAxis::Z], createInfo.limitLinearUpper[ConstraintAxis::Y]));
            
            result = newConstraint;
        }
        break;
        case eConstraintType::Motor:
        {
        #define ENABLE_MOTORS 1
        #if ENABLE_MOTORS
            btTransform localA(quat_to_btQuaternion(createInfo.orientationOffsetA), vec3_to_btVector3(createInfo.translationOffsetA));
            btTransform localB(quat_to_btQuaternion(createInfo.orientationOffsetB), vec3_to_btVector3(createInfo.translationOffsetB));
            btHingeConstraint* newHinge = new btHingeConstraint(*createInfo.object1Body, *createInfo.object2Body, localA, localB);

            auto transformed = MathUtil::TransformVector(kMotorAxis, createInfo.orientation);;
            btVector3 test = vec3_to_btVector3(transformed);
            newHinge->setAxis(test);

            //newHinge->setAxis(btVector3(0.0f, 1.0f, 0.0f));
            newHinge->enableAngularMotor(createInfo.useAngularMotor, createInfo.angularMotorTargetVelocity, createInfo.angularMotorMaxImpulse);

            /*
            // Assume that the hinge rotation axis is around Y.
            // @TODO: Is there a better way to be sure about this?
            btVector3 kHingeAxis = btVector3(0.0f, 1.0f, 0.0f);
            newHinge->setAxis(kHingeAxis);
            if (createInfo.useLimitAng[ConstraintAxis::Z])
            {
                newHinge->setLimit(createInfo.limitAngLower[ConstraintAxis::Z], createInfo.limitAngUpper[ConstraintAxis::Z]);
            }
            */

            result = newHinge;
#endif
        }
        break;
        default:
            assert(false);
            break;
    }

    if (createInfo.useBreakingThreshold)
    {
        result->setBreakingImpulseThreshold(createInfo.breakingThreshold);
    }

    return result;
}