#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

///
/// An abstraction of how the Audio is run. Uses SDLmixer underneath to provide sound
/// and music playing capabilities.
///
class AudioEngine {
public:
    ///
    /// Getter for the main global audio manager.
    /// @return a reference to the global audio manager
    ///
    static AudioEngine* get_instance();

    ///
    /// Play the song in the location given.
    /// @param file_location
    ///     The file location of the song that to be played. (Has to be an Ogg Vorbis file)
    void play_music(std::string file_location);

    ///
    /// Set the music volume anywhere between 0 and 128
    /// @param volume
    ///     The volume you wish to set the music to.
    void set_music_volume(int volume);

    int get_music_volume();

private:

    AudioEngine();
    ~AudioEngine();
    
    AudioEngine(AudioEngine const&) = delete;
    void operator=(AudioEngine const&) = delete;

    Mix_Music *music; //a pointer to the music object
    bool audio_open;  //if the audio player is currently open.

};
