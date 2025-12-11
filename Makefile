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
		-DQt6_DIR=$$HOME/Qt/6.9.3/macos/lib/cmake/Qt6

build: configure
	cmake --build $(BUILD_DIR) --config $(CMAKE_BUILD_TYPE)

clean:
	rm -rf $(BUILD_DIR)

test: build
	cd $(BUILD_DIR) && ctest --output-on-failure

docs:
	cd docs && make all