// 3rdParty.
#include <glm/gtc/type_ptr.hpp>
#include <libdragon.h>

// Local.
#include "JsonUtil.h"

static const char* s_COMPONENT_X = "x";
static const char* s_COMPONENT_Y = "y";
static const char* s_COMPONENT_Z = "z";
static const char* s_COMPONENT_W = "w";
static const char* s_COMPONENTS = "m";

void JsonUtil::SerializeString(nlohmann::json& node, std::string& str, bool isReading)
{
	if (isReading)
	{
		if (!node.is_null())
		{
			str = node.get<std::string>();
		}
		else
		{
			debugf("Failed to read JSON string.\n");
		}
	}
	else
	{
		node = str;
	}
}

void JsonUtil::SerializeQuaternion(nlohmann::json& node, glm::quat& quat, bool isReading)
{
	bool isNodeNull = node.is_null();

	if (isReading)
	{
		if (!isNodeNull)
		{
			quat.x = node[s_COMPONENT_X];
			quat.y = node[s_COMPONENT_Y];
			quat.z = node[s_COMPONENT_Z];
			quat.w = node[s_COMPONENT_W];
		}
		else
		{
			debugf("Failed to read JSON vec3.\n");
		}
	}
	else
	{
		node[s_COMPONENT_X] = quat.x;
		node[s_COMPONENT_Y] = quat.y;
		node[s_COMPONENT_Z] = quat.z;
		node[s_COMPONENT_W] = quat.w;
	}
}

void JsonUtil::SerializeVec2(nlohmann::json& node, glm::vec2& vec, bool isReading)
{
	bool isNodeNull = node.is_null();

	if (isReading)
	{
		if (!isNodeNull)
		{
			vec.x = node[s_COMPONENT_X];
			vec.y = node[s_COMPONENT_Y];
		}
		else
		{
			debugf("Failed to read JSON vec3.\n");
		}
	}
	else
	{
		node[s_COMPONENT_X] = vec.x;
		node[s_COMPONENT_Y] = vec.y;
	}
}

void JsonUtil::SerializeVec3(nlohmann::json& node, glm::vec3& vec, bool isReading)
{
	bool isNodeNull = node.is_null();

	if (isReading)
	{
		if (!isNodeNull)
		{
			vec.x = node[s_COMPONENT_X];
			vec.y = node[s_COMPONENT_Y];
			vec.z = node[s_COMPONENT_Z];
		}
		else
		{
			debugf("Failed to read JSON vec3.\n");
		}
	}
	else
	{
		node[s_COMPONENT_X] = vec.x;
		node[s_COMPONENT_Y] = vec.y;
		node[s_COMPONENT_Z] = vec.z;
	}
}

void JsonUtil::SerializeVec4(nlohmann::json& node, glm::vec4& vec, bool isReading)
{
	bool isNodeNull = node.is_null();

	if (isReading)
	{
		if (!isNodeNull)
		{
			vec.x = node[s_COMPONENT_X];
			vec.y = node[s_COMPONENT_Y];
			vec.z = node[s_COMPONENT_Z];
			vec.w = node[s_COMPONENT_W];
		}
		else
		{
			debugf("Failed to read JSON vec3.\n");
		}
	}
	else
	{
		node[s_COMPONENT_X] = vec.x;
		node[s_COMPONENT_Y] = vec.y;
		node[s_COMPONENT_Z] = vec.z;
		node[s_COMPONENT_W] = vec.w;
	}
}

void JsonUtil::SerializeMat4x4(nlohmann::json& node, glm::mat4x4& mat, bool isReading)
{
	bool isNodeNull = node.is_null();

	if (isReading)
	{
		if (!isNodeNull)
		{
			float* values = glm::value_ptr(mat);
			for (size_t index = 0; index < 16; index++)
			{
				values[index] = node[s_COMPONENTS][index];
			}
		}
		else
		{
			debugf("Failed to read JSON mat4x4.\n");
		}
	}
	else
	{
		const float* values = glm::value_ptr(mat);
		for (size_t index = 0; index < 16; index++)
		{
			node[s_COMPONENTS][index] = values[index];
		}
	}
}