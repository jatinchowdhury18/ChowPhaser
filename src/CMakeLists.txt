target_sources(ChowPhaserMono PRIVATE
    ChowPhaserPlugin.cpp
    LightMeter.cpp)

juce_add_binary_data(BinaryData SOURCES gui.xml)
