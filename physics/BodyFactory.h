#pragma once

#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "ShapeFactory.h"

class btCollisionShape;
class btCollisionObject;
class btTransform;
class btRigidBody;

struct AttachmentInfo
{
    std::string name;
    glm::vec3 position;
    glm::quat orientation;
};

struct BodyCreateInfo
{
    std::string bodyName;
    glm::vec3 position;
    glm::quat orientation;
    glm::vec3 scale;
    glm::vec3 dimensions;
    bool isStatic;
    bool isKinematic;
    float mass;
    float friction;
    float restitution;
    std::string shapeTypeString;
    std::string hull_name;
    bool useMargin;
    float margin;
    uint32_t group;
    uint32_t mask;
    eShapeType shapeType;
    btCollisionShape* shape;
    std::vector<AttachmentInfo> attachments;
};

class BodyFactory
{
public:
	BodyFactory() = delete;
	~BodyFactory() = delete;

	static btRigidBody* CreateRigidBody(float mass, const btTransform& startTransform, btCollisionShape* inShape);

    static btRigidBody* CreateRigidBody(const BodyCreateInfo& createInfo);

    static btCollisionObject* CreateCollisionObject(const BodyCreateInfo& createInfo);
};