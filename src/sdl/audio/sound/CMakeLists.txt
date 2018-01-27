
include_directories(${SDL_INCLUDE_DIR})

add_library(sdl_extensions_audio_sound CSoundChannel.cpp CSoundChannel.h
                                       CSoundSlot.cpp CSoundSlot.h)

# TODO: Does not work with MacOS. Where is the pkgconfig or cmake script
IF(OGG)
  if(WIN32)
    TARGET_LINK_LIBRARIES(sdl_extensions_audio_sound vorbisfile vorbis ogg)
  else(WIN32)
    TARGET_LINK_LIBRARIES(sdl_extensions_audio_sound vorbis vorbisfile)
  endif(WIN32)
ENDIF(OGG)
