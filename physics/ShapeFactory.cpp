// 3rdParty.
#include <btBulletCollisionCommon.h>

// Local.
#include "ShapeFactory.h"

#define USE_CONVEX_DECOMPOSITION_SHAPE 0

glm::vec3 bulletToGlm(const btVector3& v)
{
    return glm::vec3(v.getX(), v.getY(), v.getZ());
}

btVector3 glmToBullet(const glm::vec3& v)
{
    return btVector3(v.x, v.y, v.z);
}


btCollisionShape* ShapeFactory::CreateCollisionShape(const ShapeCreateInfo& createInfo)
{
    btCollisionShape* collisionShape = nullptr;

    switch (createInfo.type)
    {
    case eShapeType::Plane:
        collisionShape = new btStaticPlaneShape(glmToBullet(createInfo.planeInfo.normal), createInfo.planeInfo.distance);
        break;
    case eShapeType::Box:
        collisionShape = new btBoxShape(glmToBullet(createInfo.boxInfo.dimensions));
        break;
    case eShapeType::Cone:
        collisionShape = new btConeShape(createInfo.coneInfo.radius, createInfo.coneInfo.height);
        break;
    case eShapeType::Capsule:
        collisionShape = new btCapsuleShape(createInfo.capsuleInfo.radius, createInfo.capsuleInfo.height);
        break;
    case eShapeType::Cylinder:
        collisionShape = new btCylinderShape(glmToBullet(createInfo.cylinderInfo.halfExtents));
        break;
    case eShapeType::Sphere:
        collisionShape = new btSphereShape(createInfo.sphereInfo.radius);
        break;
    case eShapeType::Hull:
    {
        const VertexList& verts = createInfo.meshCreateInfo.vertices;

        btConvexHullShape* shape = new btConvexHullShape();

        for (size_t i = 0; i < verts.size(); i++)
        {
            shape->addPoint(glmToBullet(verts[i]));
        }

        collisionShape = shape;
    }
    break;
    case eShapeType::Mesh:
    {
        btTriangleMesh* newMesh = new btTriangleMesh();

        const VertexList& verts = createInfo.meshCreateInfo.vertices;
        const IndexList& indices = createInfo.meshCreateInfo.indices;

        size_t numTriangles = static_cast<int>(indices.size()) / 3;
        for (size_t i = 0; i < numTriangles; i++)
        {
            size_t index = i * 3;

            newMesh->addTriangle(glmToBullet(verts[indices[index + 0]]),
                glmToBullet(verts[indices[index + 1]]),
                glmToBullet(verts[indices[index + 2]]));
        }

#if USE_CONVEX_DECOMPOSITION_SHAPE
        btVector3 meshScale = btVector3(1.0f, 1.0f, 1.0f);
        btGImpactConvexDecompositionShape* decompositionShape = new btGImpactConvexDecompositionShape(newMesh, meshScale);
        decompositionShape->updateBound();

        collisionShape = decompositionShape;
#else
        collisionShape = new btBvhTriangleMeshShape(newMesh, true);
#endif
    }
    break;
    default:
        // I dunno what to do
        assert(false);
        break;
    }

    return collisionShape;
}

eShapeType ShapeFactory::TypeFromString(const char* typeString)
{
    eShapeType result = eShapeType::Count;

    for (int typeIndex = 0; typeIndex < static_cast<int>(eShapeType::Count); typeIndex++)
    {
        if (strcmp(typeString, kShapeTypes[typeIndex].c_str()) == 0)
        {
            result = static_cast<eShapeType>(typeIndex);
            break;
        }
    }

    return result;
}