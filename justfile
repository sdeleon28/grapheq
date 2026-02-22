# GraphEQ build commands

# Debug (default)
configure:
    cmake -B build

build:
    @if [ ! -d build ]; then just configure; fi
    cmake --build build

# Release
configure-release:
    cmake -B build-release -DCMAKE_BUILD_TYPE=Release

build-release:
    @if [ ! -d build-release ]; then just configure-release; fi
    cmake --build build-release

# Deploy (no build dependency — just copies)
deploy-au:
    mkdir -p ~/Library/Audio/Plug-Ins/Components
    cp -R build/GraphEQ_artefacts/AU/GraphEQ.component ~/Library/Audio/Plug-Ins/Components/

deploy-vst3:
    mkdir -p ~/Library/Audio/Plug-Ins/VST3
    cp -R build/GraphEQ_artefacts/VST3/GraphEQ.vst3 ~/Library/Audio/Plug-Ins/VST3/

deploy: deploy-au deploy-vst3

deploy-release-au:
    mkdir -p ~/Library/Audio/Plug-Ins/Components
    cp -R build-release/GraphEQ_artefacts/AU/GraphEQ.component ~/Library/Audio/Plug-Ins/Components/

deploy-release-vst3:
    mkdir -p ~/Library/Audio/Plug-Ins/VST3
    cp -R build-release/GraphEQ_artefacts/VST3/GraphEQ.vst3 ~/Library/Audio/Plug-Ins/VST3/

deploy-release: deploy-release-au deploy-release-vst3

# Combined workflows
build-and-deploy: build deploy
rebuild: clean configure build deploy

# Clean
clean:
    rm -rf build build-release

# Run standalone
run: build
    open build/GraphEQ_artefacts/Standalone/GraphEQ.app
