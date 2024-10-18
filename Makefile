ROM_NAME=game
BUILD_DIR=build

include $(N64_INST)/include/n64.mk
include $(T3D_INST)/t3d.mk

N64_CFLAGS += -std=gnu2x
N64_INCLUDEDIR += -I./lib -I./lib/glm

src = main.cpp util/JsonUtil.cpp

assets_png = $(wildcard assets/*.png)
assets_gltf = $(wildcard assets/*.glb)
assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite))) \
			  $(addprefix filesystem/,$(notdir $(assets_ttf:%.ttf=%.font64))) \
			  $(addprefix filesystem/,$(notdir $(assets_gltf:%.glb=%.t3dm)))

all: $(ROM_NAME).z64

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"


filesystem/%.t3dm: assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) "$<" $@ --base-scale=1
	$(N64_BINDIR)/mkasset -c 2 -o filesystem $@

collision_files = $(wildcard assets/levels/*.json)
collision_assets = $(addprefix filesystem/,$(notdir $(collision_files:%.json=%.json)))
filesystem/%.json: $(collision_files)
	@mkdir -p $(dir $@)
	@echo "    [Collision JSON] $@"
	@cp $< $@

$(BUILD_DIR)/$(ROM_NAME).dfs: $(assets_conv) $(collision_assets)
$(BUILD_DIR)/$(ROM_NAME).elf: $(src:%.c=$(BUILD_DIR)/%.o) $(src:%.cpp=$(BUILD_DIR)/%.o)

$(ROM_NAME).z64: N64_ROM_TITLE="N64BREW2024GAME"
$(ROM_NAME).z64: $(BUILD_DIR)/$(ROM_NAME).dfs

clean:
	rm -rf $(BUILD_DIR) *.z64
	rm -rf filesystem

build_lib:
	rm -rf $(BUILD_DIR) *.z64
	make -C $(T3D_INST)
	make all

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
