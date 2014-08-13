#ifndef CHALLENGE_BASE
#define CHALLENGE_BASE


///
/// The base challenge class
///
class ChallengeBase {
    Sprite* player;
    Map* map;
    
public:
    ChallengeBase(std::string map_name);
    
    ///
    /// Get the map this challenge is using
    /// @return the map
    ///
    Map* get_map() { return map: }
    
    ///
    /// Set the map
    /// @param _map the map to be used for this challenge base
    ///
    void set_map(Map* _map) { map = _map; }
    
};

#endif
