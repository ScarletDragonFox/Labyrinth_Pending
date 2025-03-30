cmake_minimum_required(VERSION 3.21...3.31)

project(soloud
    VERSION 2020.2.7
    LANGUAGES C CXX
    HOMEPAGE_URL "https://github.com/jarikomppa/soloud"
    DESCRIPTION "Free, easy, portable audio engine for games")

set( SOLOUD_SOURCE_FILES 
    soloud/src/backend/miniaudio/soloud_miniaudio.cpp

    soloud/src/c_api/soloud_c.cpp

    soloud/src/core/soloud.cpp
    soloud/src/core/soloud_audiosource.cpp
    soloud/src/core/soloud_bus.cpp
    soloud/src/core/soloud_core_3d.cpp
    soloud/src/core/soloud_core_basicops.cpp
    soloud/src/core/soloud_core_faderops.cpp
    soloud/src/core/soloud_core_filterops.cpp
    soloud/src/core/soloud_core_getters.cpp
    soloud/src/core/soloud_core_setters.cpp
    soloud/src/core/soloud_core_voicegroup.cpp
    soloud/src/core/soloud_core_voiceops.cpp
    soloud/src/core/soloud_fader.cpp
    soloud/src/core/soloud_fft.cpp
    soloud/src/core/soloud_fft_lut.cpp
    soloud/src/core/soloud_file.cpp
    soloud/src/core/soloud_filter.cpp
    soloud/src/core/soloud_misc.cpp
    soloud/src/core/soloud_queue.cpp
    soloud/src/core/soloud_thread.cpp

    soloud/src/audiosource/ay/chipplayer.cpp
    soloud/src/audiosource/ay/sndbuffer.cpp
    soloud/src/audiosource/ay/sndchip.cpp
    soloud/src/audiosource/ay/sndrender.cpp
    soloud/src/audiosource/ay/soloud_ay.cpp
    soloud/src/audiosource/monotone/soloud_monotone.cpp
    soloud/src/audiosource/noise/soloud_noise.cpp

    soloud/src/audiosource/openmpt/soloud_openmpt.cpp
    soloud/src/audiosource/openmpt/soloud_openmpt_dll.c # this uses a DLL and will not work in wasm


    soloud/src/audiosource/sfxr/soloud_sfxr.cpp
    soloud/src/audiosource/speech/darray.cpp
    soloud/src/audiosource/speech/klatt.cpp
    soloud/src/audiosource/speech/resonator.cpp
    soloud/src/audiosource/speech/soloud_speech.cpp
    soloud/src/audiosource/speech/tts.cpp
    soloud/src/audiosource/tedsid/sid.cpp
    soloud/src/audiosource/tedsid/soloud_tedsid.cpp
    soloud/src/audiosource/tedsid/ted.cpp
    soloud/src/audiosource/vic/soloud_vic.cpp
    soloud/src/audiosource/vizsn/soloud_vizsn.cpp
    soloud/src/audiosource/wav/dr_impl.cpp
    soloud/src/audiosource/wav/soloud_wav.cpp
    soloud/src/audiosource/wav/soloud_wavstream.cpp
    soloud/src/audiosource/wav/stb_vorbis.c

    soloud/src/filter/soloud_bassboostfilter.cpp
    soloud/src/filter/soloud_biquadresonantfilter.cpp
    soloud/src/filter/soloud_dcremovalfilter.cpp
    soloud/src/filter/soloud_duckfilter.cpp
    soloud/src/filter/soloud_echofilter.cpp
    soloud/src/filter/soloud_eqfilter.cpp
    soloud/src/filter/soloud_fftfilter.cpp
    soloud/src/filter/soloud_flangerfilter.cpp
    soloud/src/filter/soloud_freeverbfilter.cpp
    soloud/src/filter/soloud_lofifilter.cpp
    soloud/src/filter/soloud_robotizefilter.cpp
    soloud/src/filter/soloud_waveshaperfilter.cpp

    #openmpt/include/stb_vorbis/stb_vorbis.c
)



add_library(soloud STATIC ${SOLOUD_SOURCE_FILES})
target_include_directories(soloud PUBLIC soloud/include)
target_compile_definitions(soloud PUBLIC WITH_MINIAUDIO)





