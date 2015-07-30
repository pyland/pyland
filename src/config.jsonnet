//This is the config file for the game, it is a jsonnet file for ease of use.
{

	"files": {
		"game_folder": "../game",
		//"level_location": "/test_world/test_level/test_one",
		"level_location": "/test_world/new_challenge/main",
		"full_level_location" : self.game_folder + "/levels" + self.level_location
	},
	"layers": { //define layer properties
		"special_layer_name": "SL1" //The name of the layer which provides special properties to tiles on the map.
	}
}
