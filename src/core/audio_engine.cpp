#include <glog/logging.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <stdio.h>

#include "audio_engine.hpp"

AudioEngine::AudioEngine() {
    /* Initialize the SDL audio library */
    SDL_Init(SDL_INIT_AUDIO);

    /* Initialize the variables */
    music = nullptr;
    audio_open = false;

}
AudioEngine::~AudioEngine() {
    if(music != nullptr) {
        Mix_FreeMusic(music);
    }
    if(audio_open) {
        Mix_CloseAudio();
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO); //close down the entire audio system
}

AudioEngine *AudioEngine::get_instance(){
    // Lazy instantiation of the global instance
    static AudioEngine global_instance;

    return &global_instance;
}

void AudioEngine::play_sound_effect(std::string file_location) {
    //TODO: Implement this,
    //Needs to be able to able to play mutliple sound effects at once, maybe you shoul be able to specify what comes 
    //out of each earphone etc.
    //read on how to do this and look at playwave.c
}

void AudioEngine::set_music_volume(int volume) {
    Mix_VolumeMusic(volume); //Control the music volume.
}

int AudioEngine::get_music_volume() {
    return Mix_VolumeMusic(-1);
}

void AudioEngine::play_music(std::string file_location) {
    if(currently_playing.compare(file_location) != 0) { //only change the song if it's not already playing!
        //Initialise audio variables
        int audio_rate;      //output sampling rate (in Hz)
        Uint16 audio_format; //output sample format.
        int audio_channels;  //the number of channels for the output.
        int audio_buffers;   //the chunk size of the output, larger is better for slower machines but means the sound will be delayed more.
        int audio_volume;    //the volume at which you wish to play the music
        int looping;         //wether the music should loop or not


        // Initialize variables
        audio_rate =  MIX_DEFAULT_FREQUENCY; //22050
        audio_format = AUDIO_S16;            //Signed 16-bit samples, in little-endian byte order
        audio_channels = 2;                  //1 = mono, 2 = stereo
        audio_buffers = 4096;                //good value for slower machines and doesn't have too much delay (shouldn't matter too much for music vs sound effects anyway
        audio_volume = get_music_volume();
        looping = -1;                        //loop through the music forever

        //Initialise the audio API, returns -1 on errors so check it initialises correctly
        if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) < 0) {
            LOG(ERROR) << "Couldn't open audio: " << SDL_GetError(); //LOG why Audio couldn't be initialised
            audio_open = false;
            return;
        }
        //Query the auddio device for it's settings.
        Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);

        // Set the music volume
        set_music_volume(audio_volume);
    
        const char* c_file_location = file_location.c_str(); //Convert the file_location to a c string for the SDL API
        music = Mix_LoadMUS(c_file_location); //Load in the music

        if (music == nullptr ) {
            LOG(ERROR) << "Couldn't load " << c_file_location << ": " << SDL_GetError(); //print why music couldn't be loaded to the screen
            Mix_CloseAudio(); //close down the audio system
            audio_open = false;
            return;
        }

        //while(!Mix_FadeOutMusic(1000) && Mix_PlayingMusic()) {
        //	wait for any fades to complete
        //	SDL_Delay(100);
        //} //TODO: implement some sort of music (and screen!) fade

        //Start playing the music.
        if(Mix_FadeInMusic(music, looping, 2000) == -1) {
            LOG(ERROR) << "Mix_FadeInMusic: " << Mix_GetError();
            // well, there's no music, but most games don't break without music...
        }
        currently_playing = file_location;
    }
    return;
}
