#include <string>

class AudioEngine {
public:
    ///
    /// Getter for the main global audio manager.
    /// @return a reference to the global audio manager
    ///
    static AudioEngine* get_instance();

    void play_song(std::string file_location);

private:

    AudioEngine();
    ~AudioEngine();
    
    AudioEngine(AudioEngine const&) = delete;
    void operator=(AudioEngine const&) = delete;

};
