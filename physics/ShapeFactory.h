#pragma once

// C++.
#include <cstdint>
#include <string>
#include <vector>

// 3rdParty.
#include <glm/vec3.hpp>

class btCollisionShape;

enum class eShapeType
{
    Plane,
    Box,
    Cone,
    Capsule,
    Cylinder,
    Sphere,
    Hull,
    Mesh,

    Count,
};

static const std::vector<std::string> kShapeTypes =
{
    "Plane",
    "Box",
    "Cone",
    "Capsule",
    "Cylinder",
    "Sphere",
    "Hull",
    "Mesh",
};

typedef std::vector<glm::vec3> VertexList;
typedef std::vector<uint32_t> IndexList;

struct ShapeCreateInfo
{
    struct
    {
        glm::vec3 normal;
        float distance;
    } planeInfo;
    struct
    {
        glm::vec3 dimensions;
    } boxInfo;
    struct
    {
        float radius;
        float height;
    } coneInfo;
    struct
    {
        float radius;
        float height;
    } capsuleInfo;
    struct
    {
        glm::vec3 halfExtents;
    } cylinderInfo;
    struct
    {
        float radius;
    } sphereInfo;
    struct
    {
        VertexList vertices;
        IndexList indices;
    } meshCreateInfo;

    eShapeType type;
    std::string name;
};

class ShapeFactory
{
public:
    ShapeFactory() = delete;
    ~ShapeFactory() = delete;

    static btCollisionShape* CreateCollisionShape(const ShapeCreateInfo& createInfo);

    static eShapeType TypeFromString(const char* typeString);
};