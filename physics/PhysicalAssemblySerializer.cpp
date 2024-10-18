// C++.
#include <fstream>
#include <sstream>

#include <libdragon.h>

// Local.
#include "BodyFactory.h"
#include "ConstraintFactory.h"
#include "ShapeFactory.h"
#include "SimulationManager.h"
#include "PhysicalAssembly.h"
#include "PhysicalAssemblyFactory.h"
#include "PhysicalAssemblySerializer.h"

void SerializeMeshVertex(nlohmann::json& node, glm::vec3& element, bool isReading)
{
    JsonUtil::SerializeVec3(node, element, isReading);
}

void SerializeMeshIndex(nlohmann::json& node, uint32_t& element, bool isReading)
{
    JsonUtil::Serialize(node, element, isReading);
}

void SerializeMesh(nlohmann::json& node, ShapeCreateInfo& meshCreateInfo, bool isReading)
{
    JsonUtil::SerializeString(node["mesh_name"], meshCreateInfo.name, isReading);
    JsonUtil::SerializeVector(node["mesh_verts"], meshCreateInfo.meshCreateInfo.vertices, isReading, SerializeMeshVertex);
    JsonUtil::SerializeVector(node["mesh_indices"], meshCreateInfo.meshCreateInfo.indices, isReading, SerializeMeshIndex);
    meshCreateInfo.type = eShapeType::Mesh;
}

void SerializeConvexHull(nlohmann::json& node, ShapeCreateInfo& hullCreateInfo, bool isReading)
{
    JsonUtil::SerializeString(node["hull_name"], hullCreateInfo.name, isReading);
    JsonUtil::SerializeVector(node["hull_points"], hullCreateInfo.meshCreateInfo.vertices, isReading, SerializeMeshVertex);
    hullCreateInfo.type = eShapeType::Hull;
}

static const char* s_STR_ATTACHMENT_NAME = "name";
static const char* s_STR_ATTACHMENT_POSITION = "position";
static const char* s_STR_ATTACHMENT_ORIENTATION = "orientation";

void SerializeAttachmentInfo(nlohmann::json& node, AttachmentInfo& attachment, bool isReading)
{
    JsonUtil::SerializeString(node[s_STR_ATTACHMENT_NAME], attachment.name, isReading);
    JsonUtil::SerializeVec3(node[s_STR_ATTACHMENT_POSITION], attachment.position, isReading);
    JsonUtil::SerializeQuaternion(node[s_STR_ATTACHMENT_ORIENTATION], attachment.orientation, isReading);
}

void SerializeRigidBody(nlohmann::json& node, BodyCreateInfo& bodyCreateInfo, bool isReading)
{
    debugf("Parsing rigidbody\n");
    JsonUtil::SerializeString(node["name"], bodyCreateInfo.bodyName, isReading);
    JsonUtil::SerializeVec3(node["location"], bodyCreateInfo.position, isReading);
    JsonUtil::SerializeQuaternion(node["quaternion"], bodyCreateInfo.orientation, isReading);
    JsonUtil::SerializeVec3(node["scale"], bodyCreateInfo.scale, isReading);
    JsonUtil::SerializeVec3(node["dimensions"], bodyCreateInfo.dimensions, isReading);
    JsonUtil::Serialize(node["static"], bodyCreateInfo.isStatic, isReading);
    JsonUtil::Serialize(node["kinematic"], bodyCreateInfo.isKinematic, isReading);
    JsonUtil::Serialize(node["mass"], bodyCreateInfo.mass, isReading);
    JsonUtil::Serialize(node["friction"], bodyCreateInfo.friction, isReading);
    JsonUtil::Serialize(node["restitution"], bodyCreateInfo.restitution, isReading);

    if (isReading)
    {
        bodyCreateInfo.dimensions = bodyCreateInfo.dimensions / 2.0f;
    }

    // Read the collision shape string. Convert it to the proper shape type, and then create a new shape.
    JsonUtil::SerializeString(node["collision_shape"], bodyCreateInfo.shapeTypeString, isReading);
    
    if (bodyCreateInfo.shapeTypeString.compare("CONVEX_HULL") == 0)
    {
        JsonUtil::SerializeString(node["hull_name"], bodyCreateInfo.hull_name, isReading);
        bodyCreateInfo.shapeType = eShapeType::Hull;
    }
    else if (bodyCreateInfo.shapeTypeString.compare("MESH") == 0)
    {
        JsonUtil::SerializeString(node["mesh_name"], bodyCreateInfo.hull_name, isReading);
        bodyCreateInfo.shapeType = eShapeType::Mesh;
    }
    else if (bodyCreateInfo.shapeTypeString.compare("BOX") == 0)
    {
        bodyCreateInfo.shapeType = eShapeType::Box;
    }
    else if (bodyCreateInfo.shapeTypeString.compare("CYLINDER") == 0)
    {
        bodyCreateInfo.shapeType = eShapeType::Cylinder;
    }
    else if (bodyCreateInfo.shapeTypeString.compare("SPHERE") == 0)
    {
        bodyCreateInfo.shapeType = eShapeType::Sphere;
    }
    else if (bodyCreateInfo.shapeTypeString.compare("CAPSULE") == 0)
    {
        bodyCreateInfo.shapeType = eShapeType::Capsule;
    }
    else if (bodyCreateInfo.shapeTypeString.compare("CONE") == 0)
    {
        bodyCreateInfo.shapeType = eShapeType::Cone;
    }
    else
    {
        assert(false);
        printf("WOW!!!");
    }

    JsonUtil::Serialize(node["use_margin"], bodyCreateInfo.useMargin, isReading);
    JsonUtil::Serialize(node["collision_margin"], bodyCreateInfo.margin, isReading);
    JsonUtil::Serialize(node["group"], bodyCreateInfo.group, isReading);
    JsonUtil::Serialize(node["mask"], bodyCreateInfo.mask, isReading);
    JsonUtil::SerializeVector(node["attachments"], bodyCreateInfo.attachments, isReading, SerializeAttachmentInfo);
}

void SerializeLinearLimits(nlohmann::json& node, ContraintCreateInfo& constraintCreateInfo, bool isReading, ConstraintAxis axis)
{
    std::stringstream useLimitLinear;
    useLimitLinear << "use_limit_lin_" << kAxes[axis];
    JsonUtil::Serialize(node[useLimitLinear.str().c_str()], constraintCreateInfo.useLimitLinear[axis], isReading);

    std::stringstream limitLower;
    limitLower << "limit_lin_" << kAxes[axis] << "_lower";
    JsonUtil::Serialize(node[limitLower.str().c_str()], constraintCreateInfo.limitLinearLower[axis], isReading);

    std::stringstream limitUpper;
    limitUpper << "limit_lin_" << kAxes[axis] << "_upper";
    JsonUtil::Serialize(node[limitUpper.str().c_str()], constraintCreateInfo.limitLinearUpper[axis], isReading);
}

void SerializeAngleLimits(nlohmann::json& node, ContraintCreateInfo& constraintCreateInfo, bool isReading, ConstraintAxis axis)
{
    std::stringstream useLimitAngle;
    useLimitAngle << "use_limit_ang_" << kAxes[axis];
    JsonUtil::Serialize(node[useLimitAngle.str().c_str()], constraintCreateInfo.useLimitAng[axis], isReading);

    std::stringstream limitLower;
    limitLower << "limit_ang_" << kAxes[axis] << "_lower";
    JsonUtil::Serialize(node[limitLower.str().c_str()], constraintCreateInfo.limitAngLower[axis], isReading);

    std::stringstream limitUpper;
    limitUpper << "limit_ang_" << kAxes[axis] << "_upper";
    JsonUtil::Serialize(node[limitUpper.str().c_str()], constraintCreateInfo.limitAngUpper[axis], isReading);
}

void SerializeLinearSpringConfig(nlohmann::json& node, ContraintCreateInfo& constraintCreateInfo, bool isReading, ConstraintAxis axis)
{
    std::stringstream useSpringStream;
    useSpringStream << "use_spring_" << kAxes[axis];
    JsonUtil::Serialize(node[useSpringStream.str().c_str()], constraintCreateInfo.useSpring[axis], isReading);

    std::stringstream springStiffness;
    springStiffness << "spring_stiffness_" << kAxes[axis];
    JsonUtil::Serialize(node[springStiffness.str().c_str()], constraintCreateInfo.stiffness[axis], isReading);

    std::stringstream springDamping;
    springDamping << "spring_damping_" << kAxes[axis];
    JsonUtil::Serialize(node[springDamping.str().c_str()], constraintCreateInfo.damping[axis], isReading);
}

void SerializeAngularSpringConfig(nlohmann::json& node, ContraintCreateInfo& constraintCreateInfo, bool isReading, ConstraintAxis axis)
{
    std::stringstream useSpringStream;
    useSpringStream << "use_spring_ang_" << kAxes[axis];
    JsonUtil::Serialize(node[useSpringStream.str().c_str()], constraintCreateInfo.useSpringAng[axis], isReading);

    std::stringstream springStiffness;
    springStiffness << "spring_stiffness_ang_" << kAxes[axis];
    JsonUtil::Serialize(node[springStiffness.str().c_str()], constraintCreateInfo.stiffnessAng[axis], isReading);

    std::stringstream springDamping;
    springDamping << "spring_damping_ang_" << kAxes[axis];
    JsonUtil::Serialize(node[springDamping.str().c_str()], constraintCreateInfo.dampingAng[axis], isReading);
}

void SerializeConstraint(nlohmann::json& node, ContraintCreateInfo& constraintCreateInfo, bool isReading)
{
    JsonUtil::SerializeString(node["name"], constraintCreateInfo.name, isReading);
    JsonUtil::SerializeString(node["type"], constraintCreateInfo.constraintType, isReading);
    JsonUtil::Serialize(node["enabled"], constraintCreateInfo.enabled, isReading);
    JsonUtil::Serialize(node["disable_collisions"], constraintCreateInfo.disableCollisions, isReading);
    JsonUtil::Serialize(node["breaking_threshold"], constraintCreateInfo.breakingThreshold, isReading);
    JsonUtil::Serialize(node["use_breaking"], constraintCreateInfo.useBreakingThreshold, isReading);
    JsonUtil::Serialize(node["use_override_solver_iterations"], constraintCreateInfo.useOverrideSolverIterations, isReading);
    JsonUtil::Serialize(node["solver_iterations"], constraintCreateInfo.solverIterations, isReading);
    JsonUtil::SerializeQuaternion(node["quaternion"], constraintCreateInfo.orientation, isReading);
    JsonUtil::SerializeString(node["object1"], constraintCreateInfo.object1, isReading);
    JsonUtil::SerializeVec3(node["translation_offset_a"], constraintCreateInfo.translationOffsetA, isReading);
    JsonUtil::SerializeQuaternion(node["rotation_offset_a"], constraintCreateInfo.orientationOffsetA, isReading);
    JsonUtil::SerializeString(node["object2"], constraintCreateInfo.object2, isReading);
    JsonUtil::SerializeVec3(node["translation_offset_b"], constraintCreateInfo.translationOffsetB, isReading);
    JsonUtil::SerializeQuaternion(node["rotation_offset_b"], constraintCreateInfo.orientationOffsetB, isReading);

    eConstraintType type = ConstraintTypeStringToType(constraintCreateInfo.constraintType);

    if (type == eConstraintType::Hinge)
    {
        // Hinges are only concerned with a Z axis rotation.
        SerializeAngleLimits(node, constraintCreateInfo, isReading, ConstraintAxis::Z);
    }

    if (type == eConstraintType::Motor)
    {
        // Angular motor settings.
        JsonUtil::Serialize(node["use_motor_ang"], constraintCreateInfo.useAngularMotor, isReading);
        JsonUtil::Serialize(node["motor_ang_target_velocity"], constraintCreateInfo.angularMotorTargetVelocity, isReading);
        JsonUtil::Serialize(node["motor_ang_max_impulse"], constraintCreateInfo.angularMotorMaxImpulse, isReading);

        // Linear motor settings.
        JsonUtil::Serialize(node["use_motor_lin"], constraintCreateInfo.useLinearMotor, isReading);
        JsonUtil::Serialize(node["motor_lin_target_velocity"], constraintCreateInfo.linearMotorTargetVelocity, isReading);
        JsonUtil::Serialize(node["motor_lin_max_impulse"], constraintCreateInfo.linearMotorMaxImpulse, isReading);
    }

    if (type == eConstraintType::Generic || type == eConstraintType::GenericSpring)
    {
        SerializeLinearLimits(node, constraintCreateInfo, isReading, ConstraintAxis::X);
        SerializeLinearLimits(node, constraintCreateInfo, isReading, ConstraintAxis::Y);
        SerializeLinearLimits(node, constraintCreateInfo, isReading, ConstraintAxis::Z);

        SerializeAngleLimits(node, constraintCreateInfo, isReading, ConstraintAxis::X);
        SerializeAngleLimits(node, constraintCreateInfo, isReading, ConstraintAxis::Y);
        SerializeAngleLimits(node, constraintCreateInfo, isReading, ConstraintAxis::Z);

        if (type == eConstraintType::GenericSpring)
        {
            SerializeLinearSpringConfig(node, constraintCreateInfo, isReading, ConstraintAxis::X);
            SerializeLinearSpringConfig(node, constraintCreateInfo, isReading, ConstraintAxis::Y);
            SerializeLinearSpringConfig(node, constraintCreateInfo, isReading, ConstraintAxis::Z);

            SerializeAngularSpringConfig(node, constraintCreateInfo, isReading, ConstraintAxis::X);
            SerializeAngularSpringConfig(node, constraintCreateInfo, isReading, ConstraintAxis::Y);
            SerializeAngularSpringConfig(node, constraintCreateInfo, isReading, ConstraintAxis::Z);
        }
    }
}

void PhysicalAssemblySerializer::Serialize(nlohmann::json& parentNode, PhysicalAssemblyCreateInfo& instance, bool isReading)
{
    debugf("Parsing assembly\n");
    JsonUtil::SerializeVector(parentNode["convex_hulls"], instance.shapes, isReading, SerializeConvexHull);
    JsonUtil::SerializeVector(parentNode["meshes"], instance.shapes, isReading, SerializeMesh);
    JsonUtil::SerializeVector(parentNode["rigid_bodys"], instance.bodies, isReading, SerializeRigidBody);
    JsonUtil::SerializeVector(parentNode["constraints"], instance.constraints, isReading, SerializeConstraint);
    // @TODO:
    // Read/write the colliders
    // Read/write the motionstates
    // Read/write the actions
    // ETC.
}