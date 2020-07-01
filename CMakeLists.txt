cmake_minimum_required(VERSION 3.15)
project(ChowPhaser VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(modules)

juce_set_vst2_sdk_path(C:/SDKs/VST_SDK/VST2_SDK/)

juce_add_plugin(ChowPhaserMono
    COMPANY_NAME chowdsp
    PLUGIN_MANUFACTURER_CODE chow
    PLUGIN_CODE Cph1
    FORMATS AU VST3 VST Standalone
    ProductName "ChowPhaser Mono")

juce_generate_juce_header(ChowPhaserMono)
add_subdirectory(src)

target_compile_definitions(ChowPhaserMono
    PUBLIC
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    FOLEYS_SHOW_GUI_EDITOR_PALLETTE=0)

target_link_libraries(ChowPhaserMono PRIVATE
    BinaryData
    juce::juce_audio_utils
    foleys_gui_magic)
