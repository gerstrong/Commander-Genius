
include_directories(${SDL_INCLUDE_DIR})


add_library(sdl_extensions_audio_music CIMFPlayer.cpp CIMFPlayer.h
                                       CMusic.cpp CMusic.h
                                       CMusicPlayer.cpp CMusicPlayer.h
                                       COGGPlayer.cpp COGGPlayer.h)

                                     
# TODO: Does not work with MacOS. Where is the pkgconfig or cmake script
IF(OGG)
  if(WIN32)
    TARGET_LINK_LIBRARIES(sdl_extensions_audio_music vorbisfile vorbis ogg)
  else(WIN32)
    TARGET_LINK_LIBRARIES(sdl_extensions_audio_music vorbis vorbisfile)
  endif(WIN32)
ENDIF(OGG)
                                     
IF(TREMOR)
  TARGET_LINK_LIBRARIES(sdl_extensions_audio_music vorbisidec)
ENDIF(TREMOR)
