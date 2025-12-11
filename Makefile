.PHONY: all build clean test docs help configure run

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
	@echo "  run          - Build and run the application"
	@echo "  clean        - Remove build directory"
	@echo "  test         - Run test suite"
	@echo "  docs         - Build documentation"
	@echo "  bundle       - Bundle Qt frameworks (macOS only)"
	@echo ""
	@echo "Variables:"
	@echo "  CMAKE_BUILD_TYPE - Build type (Debug|Release), default: Release"
	@echo ""
	@echo "Examples:"
	@echo "  make                    # Build in Release mode"
	@echo "  make CMAKE_BUILD_TYPE=Debug  # Build in Debug mode"
	@echo "  make run                # Build and run the application"
	@echo "  make test               # Run tests"

configure:
	@mkdir -p $(BUILD_DIR)
	@UNAME=$$(uname 2>/dev/null || echo "Unknown"); \
	if [ "$$UNAME" = "Darwin" ]; then \
		QT_DIR="$$HOME/Qt/6.9.3/macos/lib/cmake/Qt6"; \
	elif [ "$$UNAME" = "Linux" ]; then \
		QT_DIR="$$HOME/Qt/6.9.3/gcc_64/lib/cmake/Qt6"; \
	elif echo "$$UNAME" | grep -q "MINGW\|MSYS" || [ -n "$$WINDIR" ]; then \
		QT_DIR="$$HOME/Qt/6.9.3/msvc2019_64/lib/cmake/Qt6"; \
		if [ ! -d "$$QT_DIR" ]; then \
			QT_DIR="$$HOME/Qt/6.9.3/mingw_64/lib/cmake/Qt6"; \
		fi; \
	fi; \
	cmake -B $(BUILD_DIR) -S . \
		-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) \
		-DBUILD_SHARED_LIBS=ON \
		-DBUILD_TESTS=$(if $(BUILD_TESTS),$(BUILD_TESTS),OFF) \
		-DQt6_DIR="$$QT_DIR"

build: configure
	cmake --build $(BUILD_DIR) --config $(CMAKE_BUILD_TYPE)

run: build
	@echo "Running Codexium Magnus..."
	@UNAME=$$(uname 2>/dev/null || echo "Unknown"); \
	if [ "$$UNAME" = "Darwin" ]; then \
		APP_PATH="$(BUILD_DIR)/src/codexium-magnus/codexium-magnus.app"; \
		if [ -d "$$APP_PATH" ]; then \
			open "$$APP_PATH"; \
		else \
			echo "Error: App bundle not found at $$APP_PATH"; \
			exit 1; \
		fi; \
	elif [ "$$UNAME" = "Linux" ]; then \
		EXE_PATH="$(BUILD_DIR)/src/codexium-magnus/codexium-magnus"; \
		if [ -f "$$EXE_PATH" ]; then \
			"$$EXE_PATH"; \
		else \
			echo "Error: Executable not found at $$EXE_PATH"; \
			exit 1; \
		fi; \
	elif echo "$$UNAME" | grep -q "MINGW\|MSYS"; then \
		EXE_PATH="$(BUILD_DIR)/src/codexium-magnus/codexium-magnus.exe"; \
		if [ -f "$$EXE_PATH" ]; then \
			"$$EXE_PATH"; \
		else \
			echo "Error: Executable not found at $$EXE_PATH"; \
			exit 1; \
		fi; \
	elif [ "$$OS" = "Windows_NT" ] || [ -n "$$WINDIR" ] || [ -n "$$COMSPEC" ]; then \
		EXE_PATH=""; \
		if [ -f "$(BUILD_DIR)/src/codexium-magnus/$(CMAKE_BUILD_TYPE)/codexium-magnus.exe" ]; then \
			EXE_PATH="$(BUILD_DIR)/src/codexium-magnus/$(CMAKE_BUILD_TYPE)/codexium-magnus.exe"; \
		elif [ -f "$(BUILD_DIR)/src/codexium-magnus/codexium-magnus.exe" ]; then \
			EXE_PATH="$(BUILD_DIR)/src/codexium-magnus/codexium-magnus.exe"; \
		fi; \
		if [ -n "$$EXE_PATH" ] && [ -f "$$EXE_PATH" ]; then \
			"$$EXE_PATH"; \
		else \
			echo "Error: Executable not found. Searched:"; \
			echo "  $(BUILD_DIR)/src/codexium-magnus/$(CMAKE_BUILD_TYPE)/codexium-magnus.exe"; \
			echo "  $(BUILD_DIR)/src/codexium-magnus/codexium-magnus.exe"; \
			echo ""; \
			echo "Note: On Windows with Visual Studio generator, use:"; \
			echo "  cmake --build $(BUILD_DIR) --target codexium-magnus --config $(CMAKE_BUILD_TYPE)"; \
			echo "  $(BUILD_DIR)/src/codexium-magnus/$(CMAKE_BUILD_TYPE)/codexium-magnus.exe"; \
			exit 1; \
		fi; \
	else \
		echo "Unknown operating system: $$UNAME"; \
		echo "Please run the executable manually:"; \
		echo "  macOS:   open $(BUILD_DIR)/src/codexium-magnus/codexium-magnus.app"; \
		echo "  Linux:   $(BUILD_DIR)/src/codexium-magnus/codexium-magnus"; \
		echo "  Windows: $(BUILD_DIR)/src/codexium-magnus/$(CMAKE_BUILD_TYPE)/codexium-magnus.exe"; \
		exit 1; \
	fi

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