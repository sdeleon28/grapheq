# GraphEQ build commands

# Configure the build (run once or after CMakeLists.txt changes)
configure:
    cmake -B build

# Build the plugin (configures automatically if needed)
build:
    @if [ ! -d build ]; then just configure; fi
    cmake --build build

# Clean build artifacts
clean:
    rm -rf build

# Full rebuild from scratch
rebuild: clean configure build

# Run the standalone app
run: build
    open build/GraphEQ_artefacts/Standalone/GraphEQ.app

# Install AU plugin to system plugin folder
install-au: build
    mkdir -p ~/Library/Audio/Plug-Ins/Components
    cp -R build/GraphEQ_artefacts/AU/GraphEQ.component ~/Library/Audio/Plug-Ins/Components/

# Install VST3 plugin to system plugin folder
install-vst3: build
    mkdir -p ~/Library/Audio/Plug-Ins/VST3
    cp -R build/GraphEQ_artefacts/VST3/GraphEQ.vst3 ~/Library/Audio/Plug-Ins/VST3/

# Install all plugin formats
install: install-au install-vst3
