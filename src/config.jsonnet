//This is the config file for the game, it is a jsonnet file for ease of use.
{

	"files": {
		"game_folder": "../game",
		//"level_location": "/test_world/test_level/test_one",
		"level_location": "/test_world/yingischallenged/main",
		"full_level_location" : self.game_folder + "/levels" + self.level_location,
		"player_scripts": self.game_folder + "/player_scripts",
		"object_location": self.game_folder + "/objects"
		
	},
	"layers": { //define layer properties
		"special_layer_name": "SpecialLayer" //The name of the layer which provides special properties to tiles on the map.
	},

	//define constants for rendering sizes
	"gui_constants": {
		"button_width": 0.08,
		"button_height": 0.12,

		//multiplying factors for converting between window sizes and gui scales
		"x_scale": 1.0/660.0,
		"y_scale": 1.0/285.0,
		
		//the maximum number of sprite heads displayed on the screen		
		"button_max": 5,
		//the distance between two buttons
		"button_spacing": 0.08
	}
}
