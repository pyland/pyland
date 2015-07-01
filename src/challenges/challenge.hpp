#ifndef CHALLENGE_H
#define CHALLENGE_H

#include "python_embed_headers.hpp"

#include <boost/python.hpp>
#include <glm/vec2.hpp>
#include <string>
#include <vector>

#include "animation_frames.hpp"
#include "dispatcher.hpp"
#include "lifeline.hpp"
#include "sprite_switcher.hpp"
#include "walkability.hpp"

class ChallengeData;
class Map;

class Challenge {
protected:
	Map* map;
	Lifeline esc_callback;
	SpriteSwitcher* sprite_switcher;

	std::vector<int> map_object_ids;
public:

	Challenge(ChallengeData* _challenge_data);

	virtual ~Challenge();

	ChallengeData* challenge_data;
	Dispatcher<int> event_finish;
	std::vector<int> sprite_ids;
	std::vector<int> assistant_ids;

	virtual void start() = 0;
	virtual void finish() = 0;

	///
	/// Runs the challenge by calling all the needed subsystems
	///
	void run();

	///
	/// Makes a sprite and adds it to the object manager and the current map
	/// @param position the (x, y) position of the sprite on the map
	/// @param name the name of the sprite
	/// @param walkability the walkability properties of the sprite
	/// @param sheet_id the id of the image of the sprite in its sprite sheet
	/// @param sheet_name the name of the sprite sheet
	/// @return the sprite's id
	///
	int make_sprite(glm::vec2 position,
					std::string name,
					Walkability walkability,
					AnimationFrames tiles,
					std::string start_frame);

	///
	/// Makes a map object and adds it to the object manager and the current map
	/// @return the sprite's id
	///
	int make_map_object(glm::vec2 position,
						std::string name,
						Walkability walkability,
						AnimationFrames frames,
						std::string start_frame,
						bool cuttable=false,
						bool findable=true);

    static int id_type(int id);
	virtual boost::python::object read_message(int id) const;
};

#endif
