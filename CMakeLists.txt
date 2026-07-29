cmake_minimum_required(VERSION 3.22)

project(Dream_EQ VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(JUCE)

juce_add_plugin(Dream_EQ
    COMPANY_NAME "DreamCompany"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE D991
    PLUGIN_CODE D991
    FORMATS Standalone
    PRODUCT_NAME "Dream_EQ")

juce_generate_juce_header(Dream_EQ)

target_sources(Dream_EQ
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginEditor.cpp
        src/filter.cpp)
        
target_compile_definitions(Dream_EQ
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(Dream_EQ
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
