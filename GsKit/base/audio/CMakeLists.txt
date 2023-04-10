


add_library(GsKit_base_audio STATIC
    Audio.cpp
    Audio.h
    Mixer.cpp
    CAudioResources.cpp
    CAudioResources.h
    base/COPLEmulator.cpp
    base/COPLEmulator.h
    base/dbopl.cpp
    base/dbopl.h
    base/Sampling.cpp
    base/Sampling.h
    music/CMusic.cpp
    music/CMusic.h
    music/CIMFPlayer.cpp
    music/CIMFPlayer.h
    music/CMusicPlayer.cpp
    music/CMusicPlayer.h
    sound/CSoundChannel.cpp
    sound/CSoundChannel.h
    sound/CSoundSlot.cpp
    sound/CSoundSlot.h
)

target_link_libraries(GsKit_base_audio GsKit_base_interface)
