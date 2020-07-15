target_sources(ChowPhaserMono PRIVATE
    ChowPhaserPlugin.cpp
    SingleChannelPhaser.cpp)

target_sources(ChowPhaserStereo PRIVATE
    ChowPhaserStereo.cpp
    SingleChannelPhaser.cpp)

add_subdirectory(gui)
