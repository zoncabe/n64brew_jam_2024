ROM_NAME=game
BUILD_DIR=build

include $(N64_INST)/include/n64.mk
include $(T3D_INST)/t3d.mk

N64_CFLAGS += -std=gnu2x
N64_INCLUDEDIR += -I./lib -I./lib/glm -I./lib/kodachrome -I./lib/micro-ui -I/opt/libdragon/include/bullet
N64_LIBS += -lkodachrome \
			-lmicro-ui

src = main.cpp $(wildcard physics/*.cpp util/*.cpp lib/kodachrome/*.c lib/micro-ui/*.c)

assets_png = $(wildcard assets/*.png)
assets_gltf = $(wildcard assets/*.glb)
assets_ttf = $(wildcard assets/*.ttf)
assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite))) \
              $(addprefix filesystem/,$(notdir $(assets_ttf:%.ttf=%.font64))) \
              $(addprefix filesystem/,$(notdir $(assets_gltf:%.glb=%.t3dm)))

all: $(ROM_NAME).z64

# Create build directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)/lib

# Convert TTF assets to font64
filesystem/%.font64: assets/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) $(MKFONT_FLAGS) -s 9 -o filesystem "$<"

# Convert PNG assets to sprites
filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

# Convert GLB assets to T3D models
filesystem/%.t3dm: assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) "$<" $@ --base-scale=1
	$(N64_BINDIR)/mkasset -c 2 -o filesystem $@

# Wildcard for collision files
collision_files = $(wildcard assets/levels/*.json)
collision_assets = $(addprefix filesystem/,$(notdir $(collision_files)))

# Wildcard for dialog files
dialog_files = $(wildcard assets/dialogs/*.json)
dialog_assets = $(addprefix filesystem/,$(notdir $(dialog_files)))

# Rule to copy collision files
filesystem/%.json: assets/levels/%.json
	@mkdir -p $(dir $@)
	@echo "    [Collision JSON] $@"
	@cp $< $@

# Rule to copy dialog files
filesystem/%.json: assets/dialogs/%.json
	@mkdir -p $(dir $@)
	@echo "    [Dialog JSON] $@"
	@cp $< $@

$(BUILD_DIR)/$(ROM_NAME).dfs: $(assets_conv) $(collision_assets) $(dialog_assets)
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

.PHONY: all clean build_lib
