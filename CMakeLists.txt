cmake_minimum_required(VERSION 3.15)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment target")
project(ChowPhaser VERSION 1.1.1)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(modules)

# juce_set_vst2_sdk_path(C:/SDKs/VST_SDK/VST2_SDK/)
# include_directories(C:/SDKs/asiosdk2.3/common)

juce_add_plugin(ChowPhaserMono
    COMPANY_NAME chowdsp
    PLUGIN_MANUFACTURER_CODE Chow
    PLUGIN_CODE Cph1
    FORMATS AU VST3 Standalone #VST
    ProductName "ChowPhaser Mono")

juce_add_plugin(ChowPhaserStereo
    COMPANY_NAME chowdsp
    PLUGIN_MANUFACTURER_CODE Chow
    PLUGIN_CODE Cph2
    FORMATS AU VST3 Standalone #VST
    ProductName "ChowPhaser Stereo")

juce_generate_juce_header(ChowPhaserMono)
juce_generate_juce_header(ChowPhaserStereo)
add_subdirectory(src)
add_subdirectory(res)

target_compile_definitions(ChowPhaserMono
    PUBLIC
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    FOLEYS_SHOW_GUI_EDITOR_PALLETTE=0
    FOLEYS_ENABLE_BINARY_DATA=1)

target_compile_definitions(ChowPhaserStereo
    PUBLIC
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    FOLEYS_SHOW_GUI_EDITOR_PALLETTE=0
    FOLEYS_ENABLE_BINARY_DATA=1)

target_link_libraries(ChowPhaserMono PRIVATE
    BinaryData
    juce::juce_audio_utils
    foleys_gui_magic)

target_link_libraries(ChowPhaserStereo PRIVATE
    BinaryData
    juce::juce_audio_utils
    foleys_gui_magic)

# we need these flags for notarization on MacOS
option(MACOS_RELEASE "Set build flags for MacOS Release" OFF)
if(MACOS_RELEASE)
    message(STATUS "Setting MacOS release flags...")
    set_target_properties(ChowPhaserMono_Standalone PROPERTIES
        XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
    set_target_properties(ChowPhaserStereo_Standalone PROPERTIES
        XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
endif()
