.PHONY: compile rebuild rebuild-debug rebuild-release test webassembly webassembly-preview

BUILD_DIR ?= build
SOURCE_DIR ?= .
CPP_COMPILER ?= g++
C_COMPILER ?= gcc
BUILD_TYPE ?= Release
CLEAN ?= 0

compile:
	if test "$(CLEAN)" -eq "1" && test -d "$(BUILD_DIR)"; then rm -rf "$(BUILD_DIR)"; fi
	cmake -B "$(BUILD_DIR)" -S "$(SOURCE_DIR)" \
			-DCMAKE_CXX_COMPILER="$(CPP_COMPILER)" \
			-DCMAKE_C_COMPILER="$(C_COMPILER)" \
			-DCMAKE_BUILD_TYPE="$(BUILD_TYPE)"
	cmake --build "$(BUILD_DIR)" --config "$(BUILD_TYPE)"
rebuild:
	$(MAKE) compile CLEAN=1
rebuild-debug:
	$(MAKE) rebuild BUILD_TYPE=Debug
rebuild-release:
	$(MAKE) rebuild BUILD_TYPE=Release

test:
	if ! test -d "$(BUILD_DIR)"; then "$(MAKE)" rebuild-release; fi
	GTEST_COLOR=1 ctest --test-dir "$(BUILD_DIR)" --build-config "$(BUILD_TYPE)" --output-on-failure --no-tests=error

WEBASSEMBLY_BUILD_DIR ?= build-wasm
WEBASSEMBLY_DIST_DIR ?= $(WEBASSEMBLY_BUILD_DIR)/dist
WEBASSEMBLY_PREVIEW_PORT ?= 8000
WEB_DIR ?= external/web
WEB_PAGE_DIR ?= $(WEB_DIR)/page

# Builds a hostable directory: the two Emscripten outputs beside a page that runs them.
webassembly:
	if test "$(CLEAN)" -eq "1" && test -d "$(WEBASSEMBLY_BUILD_DIR)"; then rm -rf "$(WEBASSEMBLY_BUILD_DIR)"; fi
	emcmake cmake -B "$(WEBASSEMBLY_BUILD_DIR)" -S "$(SOURCE_DIR)" -DCMAKE_BUILD_TYPE=Release
	cmake --build "$(WEBASSEMBLY_BUILD_DIR)" --target cpp_warships
	mkdir -p "$(WEBASSEMBLY_DIST_DIR)"
	cp "$(WEBASSEMBLY_BUILD_DIR)/executables/game/cpp_warships.js" \
		"$(WEBASSEMBLY_BUILD_DIR)/executables/game/cpp_warships.wasm" \
		"$(WEBASSEMBLY_DIST_DIR)/"
	cp -R "$(WEB_PAGE_DIR)/." "$(WEBASSEMBLY_DIST_DIR)/"

webassembly-preview: webassembly
	python3 "$(WEB_DIR)/serve.py" "$(WEBASSEMBLY_DIST_DIR)" "$(WEBASSEMBLY_PREVIEW_PORT)"
