cmake_minimum_required(VERSION 3.15)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment target")
project(ChowPhaser VERSION 1.1.1)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(modules)

# juce_set_vst2_sdk_path(C:/SDKs/VST_SDK/VST2_SDK/)

set(JUCE_FORMATS AU VST3 Standalone)

# Build LV2 only on Linux
if(UNIX AND NOT APPLE)
    message(STATUS "Building LV2 plugin format")
    list(APPEND JUCE_FORMATS LV2)
endif()

# Build VST2 is SDK set
if(TARGET juce_vst2_sdk)
    message(STATUS "Building VST2 plugin format")
    list(APPEND JUCE_FORMATS VST)
endif()

juce_add_plugin(ChowPhaserMono
    COMPANY_NAME chowdsp
    PLUGIN_MANUFACTURER_CODE Chow
    PLUGIN_CODE Cph1
    FORMATS ${JUCE_FORMATS}
    ProductName "ChowPhaser Mono"
    LV2_URI https://github.com/jatinchowdhury18/ChowPhaserMono
)

juce_add_plugin(ChowPhaserStereo
    COMPANY_NAME chowdsp
    PLUGIN_MANUFACTURER_CODE Chow
    PLUGIN_CODE Cph2
    FORMATS ${JUCE_FORMATS}
    ProductName "ChowPhaser Stereo"
    LV2_URI https://github.com/jatinchowdhury18/ChowPhaserStereo
)

juce_generate_juce_header(ChowPhaserMono)
juce_generate_juce_header(ChowPhaserStereo)
add_subdirectory(src)
add_subdirectory(res)

if(ASIOSDK_DIR)
    message(STATUS "Using ASIO SDK from ${ASIOSDK_DIR}")
    target_include_directories(juce_plugin_modules PUBLIC ${ASIOSDK_DIR}/common)
    target_compile_definitions(juce_plugin_modules PUBLIC JUCE_ASIO=1)
endif()

target_link_libraries(ChowPhaserMono PRIVATE
    BinaryData
    juce_plugin_modules
)

target_link_libraries(ChowPhaserStereo PRIVATE
    BinaryData
    juce_plugin_modules
)

# we need these flags for notarization on MacOS
option(MACOS_RELEASE "Set build flags for MacOS Release" OFF)
if(MACOS_RELEASE)
    message(STATUS "Setting MacOS release flags...")
    set_target_properties(ChowPhaserMono_Standalone PROPERTIES
        XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
    set_target_properties(ChowPhaserStereo_Standalone PROPERTIES
        XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
endif()
