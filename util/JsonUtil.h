#pragma once

// C++.
#include <fstream>
#include <iomanip>
#include <string>

// 3rd Party.
#include "../lib/json/json.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

class JsonUtil
{
public:
	template <typename T>
	static void Serialize(nlohmann::json& node, T& value, bool isReading)
	{
		if (isReading)
		{
			// Make sure that the node exists before attempting to read it.
			if (!node.is_null())
			{
				value = node.get<T>();
			}
			else
			{
				printf("Attempted to read a node but it doesn't exist!\n");
			}
		}
		else
		{
			node = value;
		}
	}

	static void SerializeString(nlohmann::json& node, std::string& str, bool isReading);

	static void SerializeQuaternion(nlohmann::json& node, glm::quat& quat, bool isReading);

	static void SerializeVec2(nlohmann::json& node, glm::vec2& vec, bool isReading);

	static void SerializeVec3(nlohmann::json& node, glm::vec3& vec, bool isReading);

	static void SerializeVec4(nlohmann::json& node, glm::vec4& vec, bool isReading);

	static void SerializeMat4x4(nlohmann::json& node, glm::mat4x4& mat, bool isReading);

	template <typename T>
	using TypeSerializerCallback = void(*)(nlohmann::json& node, T& item, bool isReading);

	template <typename T>
	static void SerializeVector(nlohmann::json& node, std::vector<T>& items, bool isReading, TypeSerializerCallback<T> callback)
	{
		if (isReading)
		{
			size_t numItems = node.size();
			for (size_t itemIndex = 0; itemIndex < numItems; ++itemIndex)
			{
				T newItem = {};
				callback(node[itemIndex], newItem, isReading);
				items.push_back(newItem);
			}

		}
		else
		{
			for (size_t itemIndex = 0; itemIndex < items.size(); ++itemIndex)
			{
				callback(node[itemIndex], items[itemIndex], isReading);
			}
		}
	}

private:
	JsonUtil() = delete;
	~JsonUtil() = delete;
};

template <typename T>
class JsonSerializer
{
public:
	JsonSerializer() = default;
	virtual ~JsonSerializer() = default;

	void Save(const std::string& filePath, T& instance)
	{
		nlohmann::json document;

		Serialize(document, instance, false);

		std::ofstream fileStream(filePath.c_str());
		fileStream << std::setw(4) << document;
	}

	bool Load(const std::string& filePath, T& instance)
	{
		bool result = false;

		std::ifstream fileStream(filePath.c_str());
		if (fileStream.good())
		{
			nlohmann::json document;
			fileStream >> document;

			Serialize(document, instance, true);

			result = true;
		}

		return result;
	}

protected:
	virtual void Serialize(nlohmann::json& parentNode, T& instance, bool isReading) = 0;
};