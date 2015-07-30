#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <glog/logging.h>

#include <stdio.h>

#include "audio_engine.hpp"

AudioEngine::AudioEngine() {
}
AudioEngine::~AudioEngine() {
}

AudioEngine *AudioEngine::get_instance(){
    // Lazy instantiation of the global instance
    static AudioEngine global_instance;

    return &global_instance;
}

void AudioEngine::play_song(std::string file_location) {
    /* Initialize the SDL audio library */
    SDL_Init(SDL_INIT_AUDIO);

    static int audio_open = 0;
    static Mix_Music *music = nullptr;
    static int next_track = 0;

    int audio_rate;
    Uint16 audio_format;
    int audio_channels;
    int audio_buffers;
    int audio_volume = MIX_MAX_VOLUME;
    int looping = 0;
    int interactive = 0;
    int rwops = 0;
    int i;

    /* Initialize variables */
    audio_rate = 22050;
    audio_format = AUDIO_S16;
    audio_channels = 2;
    audio_buffers = 4096;

    Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers);
    Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);

    audio_open = 1;

    /* Set the music volume */
    Mix_VolumeMusic(audio_volume);

    music = Mix_LoadMUS("../game/music/test.ogg");

    if ( music == NULL ) {
            fprintf(stderr, "Couldn't load %s: %s\n",
                "../game/music/test.ogg", SDL_GetError());
            //CleanUp(2);
        }

    Mix_FadeInMusic(music,looping,2000);
    LOG(INFO) << file_location;
    LOG(INFO) << "TESTING TESTING";
    /*
    while ( !next_track && (Mix_PlayingMusic() || Mix_PausedMusic()) ) {
        if(interactive)
            SDL_Delay(100);
        else
            SDL_Delay(100);
    }
    Mix_FreeMusic(music);
    */
    return;
}
