// Local.
//#include "Character.h"
#include "PhysicalAssemblyFactory.h"
#include "PhysicalAssembly.h"
//#include "Ragdoll.h"

#include <libdragon.h>

PhysicalAssembly* PhysicalAssemblyFactory::CreatePhysicalAssembly(AssemblyType type)
{
    PhysicalAssembly* result = nullptr;

    switch (type)
    {
    //case AssemblyType::Ragdoll:
        //result = new RagDoll();
        //break;
  //  case AssemblyType::Character:
        //result = new Character();
        //break;
    case AssemblyType::Vehicle:
        // @TODO: Need to return a new vehicle instance.
        assert(false);
        break;
    case AssemblyType::Base:
    default:
        result = new PhysicalAssembly();
        break;
    }

    return result;
}

PhysicalAssembly* PhysicalAssemblyFactory::CreatePhysicalAssembly(const PhysicalAssemblyCreateInfo& createInfo)
{
    PhysicalAssembly* assembly = CreatePhysicalAssembly(AssemblyType::Base);

    // Create all shapes.
    for (size_t shapeIndex = 0; shapeIndex < createInfo.shapes.size(); shapeIndex++)
    {
        btCollisionShape* shape = ShapeFactory::CreateCollisionShape(createInfo.shapes[shapeIndex]);
        assembly->AddShape(createInfo.shapes[shapeIndex].name, shape);
    }

    // Create all rigid bodies, including primitive shapes as necessary.
    for (size_t bodyIndex = 0; bodyIndex < createInfo.bodies.size(); bodyIndex++)
    {
        btCollisionObject* body = nullptr;
        
        btCollisionShape* bodyShape = nullptr;
        BodyCreateInfo cr = createInfo.bodies[bodyIndex];

        bool isMassZero = cr.isStatic;

        float customEpsilon = 0.1f;
        if (fabs(cr.mass) <= customEpsilon)
        {
            isMassZero = true;
        }

        if (!createInfo.bodies[bodyIndex].hull_name.empty())
        {
            bodyShape = assembly->GetShapeFromName(createInfo.bodies[bodyIndex].hull_name);
            cr.shape = bodyShape;

            debugf("dungus\n");
            body = BodyFactory::CreateRigidBody(cr);
        }
        else
        {
            // We need to create a primitive shape for the rigid body.
            ShapeCreateInfo shapeInfo = {};
            shapeInfo.type = cr.shapeType;

            // @TODO: Finish filling out below.
            switch (cr.shapeType)
            {
            case eShapeType::Plane:
            {
                assert(false);
            }
            break;
            case eShapeType::Box:
            {
                shapeInfo.boxInfo.dimensions = cr.dimensions;
            }
            break;
            case eShapeType::Cone:
            {
                assert(false);
            }
            break;
            case eShapeType::Capsule:
            {
                shapeInfo.capsuleInfo = { cr.dimensions.x, cr.dimensions.y };
            }
            break;
            case eShapeType::Cylinder:
            {
                shapeInfo.cylinderInfo.halfExtents = cr.dimensions;
            }
            break;
            case eShapeType::Sphere:
            {
                shapeInfo.sphereInfo.radius = cr.dimensions.x;
            }
            break;
            default:
                assert(false);
                break;
            }

            bodyShape = ShapeFactory::CreateCollisionShape(shapeInfo);
            assembly->AddShape(cr.bodyName, bodyShape);

            // Replace the shape with the newly created shape primitive.
            cr.shape = bodyShape;
        }

        if (isMassZero)
        {
                debugf("collider\n");
            body = BodyFactory::CreateCollisionObject(cr);
            assembly->AddCollider(cr.bodyName, body);
        }
        else
        {
            debugf("failed on %s\n", cr.bodyName.c_str());
            body = BodyFactory::CreateRigidBody(cr);
            assembly->AddBody(cr.bodyName, static_cast<btRigidBody*>(body));
        }
    }

    /*

    // Create all constraints.
    for (size_t constraintIndex = 0; constraintIndex < createInfo.constraints.size(); constraintIndex++)
    {
        ContraintCreateInfo currentCreateInfo = createInfo.constraints[constraintIndex];

        // Get pointers to the two bodies being constrained together.
        btRigidBody* object1 = assembly->GetBodyFromName(currentCreateInfo.object1);
        btRigidBody* object2 = assembly->GetBodyFromName(currentCreateInfo.object2);

        currentCreateInfo.object1Body = object1;
        currentCreateInfo.object2Body = object2;
        
        btTypedConstraint* constraint = ConstraintFactory::CreateConstraint(currentCreateInfo);
        assembly->AddConstraint(currentCreateInfo.name, constraint);
    }

    */

    return assembly;
}