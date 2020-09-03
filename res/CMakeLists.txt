juce_add_binary_data(BinaryData SOURCES
    gui.xml
    stereo_gui.xml
    knob.svg
    pointer.svg
    RobotoCondensed-Bold.ttf
    RobotoCondensed-Regular.ttf
    )

# Need to build BinaryData with -fPIC flag on Linux
set_target_properties(BinaryData PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)
