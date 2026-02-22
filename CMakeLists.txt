cmake_minimum_required(VERSION 3.22)
project(GraphEQ VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(External/JUCE)

juce_add_plugin(GraphEQ
    COMPANY_NAME "GraphEQ"
    PLUGIN_MANUFACTURER_CODE Grfq
    PLUGIN_CODE Greq
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "GraphEQ"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
)

target_sources(GraphEQ PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_compile_definitions(GraphEQ PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(GraphEQ
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
