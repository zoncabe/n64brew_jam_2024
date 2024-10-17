BUILD_DIR = build

include $(N64_INST)/include/n64.mk
include $(T3D_INST)/t3d.mk

N64_CFLAGS += -std=gnu2x

src = main.c \
	lib/kodachrome/kodachrome.c \
	lib/micro-ui/microui.c \
	lib/micro-ui/microuiN64.c

assets_png = $(wildcard assets/*.png)
assets_gltf = $(wildcard assets/*.glb)
assets_ttf = $(wildcard assets/*.ttf)
assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite))) \
              $(addprefix filesystem/,$(notdir $(assets_ttf:%.ttf=%.font64))) \
              $(addprefix filesystem/,$(notdir $(assets_gltf:%.glb=%.t3dm)))

all: game.z64

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

$(BUILD_DIR)/game.dfs: $(assets_conv)

# Build object files for source files in lib/
$(BUILD_DIR)/lib/%.o: lib/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "    [CC] $<"
	$(N64_CC) $(N64_CFLAGS) -c $< -o $@

# Build object files for main.c
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "    [CC] $<"
	$(N64_CC) $(N64_CFLAGS) -c $< -o $@

# Link the ELF file
$(BUILD_DIR)/game.elf: $(src:%.c=$(BUILD_DIR)/%.o)

# Generate the ROM file
game.z64: N64_ROM_TITLE="Tiny3D - Model"
game.z64: $(BUILD_DIR)/game.dfs

# Include generated dependencies
-include $(wildcard $(BUILD_DIR)/*.d) $(wildcard $(BUILD_DIR)/lib/*.d)

clean:
	rm -rf $(BUILD_DIR) *.z64
	rm -rf filesystem

build_lib:
	rm -rf $(BUILD_DIR) *.z64
	make -C $(T3D_INST)
	make all

.PHONY: all clean build_lib
