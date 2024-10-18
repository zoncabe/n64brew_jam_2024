#pragma once

// Local.
#include "../util/JsonUtil.h"
#include "PhysicalAssemblyFactory.h"

class PhysicalAssemblySerializer : public JsonSerializer<PhysicalAssemblyCreateInfo>
{
public:
    PhysicalAssemblySerializer() = default;
    virtual ~PhysicalAssemblySerializer() = default;

protected:
    virtual void Serialize(nlohmann::json& parentNode, PhysicalAssemblyCreateInfo& instance, bool isReading) override;
};