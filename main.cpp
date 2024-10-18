#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#include "screen/screen.h"
#include "control/controls.h"
#include "time/time.h"

#include "physics/physics.h"

#include "camera/camera.h"
#include "camera/camera_states.h"
#include "camera/camera_control.h"

#include "actor/actor.h"
#include "actor/actor_states.h"
#include "actor/actor_motion.h"
#include "actor/actor_control.h"
#include "actor/actor_animation.h"

#include "scene/scenery.h"

#include "game_states.h"

#include "util/JsonUtil.h"

#include "physics/PhysicalAssemblySerializer.h"

#define TEST_SAVE_FILE_LOADING 1
#define TEST_PHYSICAL_ASSEMBLY_LOADING 1

#ifdef TEST_SAVE_FILE_LOADING
struct SaveGameData
{
	std::string playerName;
	int highScore;
};

class SaveGameSerializer : public JsonSerializer<SaveGameData>
{
public:
	SaveGameSerializer() = default;
	virtual ~SaveGameSerializer() = default;

protected:
	virtual void Serialize(nlohmann::json& parentNode, SaveGameData& instance, bool isReading) override
	{
		JsonUtil::SerializeString(parentNode["playerName"], instance.playerName, isReading);
		JsonUtil::Serialize(parentNode["highScore"], instance.highScore, isReading);
	}
};
#endif

int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

	dfs_init(DFS_DEFAULT_LOCATION);
	rdpq_init();

	Screen screen;
	screen_init(&screen);

	ControllerData control;
	joypad_init();

	TimeData timing;
	time_init(&timing);

#ifdef TEST_SAVE_FILE_LOADING
	SaveGameData saveData = {};
	static const char* kSaveFilePath = "rom:/saveGameDataTest.json";
	SaveGameSerializer serializer;
	if (serializer.Load(kSaveFilePath, saveData))
	{
		debugf("\nLoaded HighScores JSON successfully:\n%s\t%d\n", saveData.playerName.c_str(), saveData.highScore);
	}
#endif

#ifdef TEST_PHYSICAL_ASSEMBLY_LOADING
	
	PhysicalAssemblyCreateInfo createInfo = {};
	static const char* kLevelFilePath = "rom:/testLevel.json";
	PhysicalAssemblySerializer assemblySerializer;
	if (assemblySerializer.Load(kLevelFilePath, createInfo))
	{
		int numBodies = (int)createInfo.bodies.size();
		int numConstraints = (int)createInfo.constraints.size();
		int numShapes = (int)createInfo.shapes.size();
		debugf("\nLoaded level successfully:\nNumBodies:\t%d\nNumConstraints:\t%d\nNumShapes:\t%d\n", numBodies, numConstraints, numShapes);
	}
#endif

	uint8_t game_state = GAMEPLAY;
	game_setState(game_state, &screen, &timing, &control);

	return 0;  
}
