.PHONY: all build clean test docs help configure

# Build directory
BUILD_DIR = build
CMAKE_BUILD_TYPE ?= Release

# Default target
all: configure build

help:
	@echo "Codexium Magnus Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Configure and build (default)"
	@echo "  configure    - Run CMake configuration"
	@echo "  build        - Build the project"
	@echo "  clean        - Remove build directory"
	@echo "  test         - Run test suite"
	@echo "  docs         - Build documentation"
	@echo ""
	@echo "Variables:"
	@echo "  CMAKE_BUILD_TYPE - Build type (Debug|Release), default: Release"
	@echo ""
	@echo "Examples:"
	@echo "  make                    # Build in Release mode"
	@echo "  make CMAKE_BUILD_TYPE=Debug  # Build in Debug mode"
	@echo "  make test               # Run tests"

configure:
	@mkdir -p $(BUILD_DIR)
	cmake -B $(BUILD_DIR) -S . \
		-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) \
		-DBUILD_SHARED_LIBS=ON \
		-DBUILD_TESTS=$(if $(BUILD_TESTS),$(BUILD_TESTS),OFF) \
		-DQt6_DIR=$$HOME/Qt/6.9.3/macos/lib/cmake/Qt6

build: configure
	cmake --build $(BUILD_DIR) --config $(CMAKE_BUILD_TYPE)

clean:
	rm -rf $(BUILD_DIR)

test: build
	cd $(BUILD_DIR) && ctest --output-on-failure

docs:
	cd docs && make all

# macOS: Bundle Qt frameworks into .app (requires macdeployqt)
bundle: build
	@if [ "$$(uname)" = "Darwin" ]; then \
		MACDEPLOYQT=$$HOME/Qt/6.9.3/macos/bin/macdeployqt; \
		if [ -f "$$MACDEPLOYQT" ]; then \
			echo "Bundling Qt frameworks..."; \
			$$MACDEPLOYQT $(BUILD_DIR)/src/codexium-magnus/codexium-magnus.app -always-overwrite; \
			echo "App bundle ready: $(BUILD_DIR)/src/codexium-magnus/codexium-magnus.app"; \
		else \
			echo "Error: macdeployqt not found at $$MACDEPLOYQT"; \
			echo "App bundle created but Qt frameworks not bundled."; \
			echo "Location: $(BUILD_DIR)/src/codexium-magnus/codexium-magnus.app"; \
		fi \
	else \
		echo "bundle target is macOS-only"; \
	fi