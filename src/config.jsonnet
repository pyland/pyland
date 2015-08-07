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

	//define layer properties
	"layers": {
		"special_layer_name": "SpecialLayer" //The name of the layer which provides special properties to tiles on the map.

	},

	//define constants for rendering sizes
	"scales": {
		
		//multiplying factors for converting between window sizes and gui scales
		"x_scale": 1.0/660.0,
		"y_scale": 1.0/285.0,		

		//for placing the buttons on top of the screen
		"left_x_offset": 0.0,
		"right_x_offset": 0.91,
		"bottom_y_offset": 0.0,
		"top_y_offset": 0.87,				

		//for placing the title of a menu
		"title_x_offset": 0.5,
		"title_y_offset": 0.9,

		//for placing an item on the menu
		"menu_x_offset": 0.0,
		"menu_y_offset": 0.75,		
		
		//the sizes of the buttons on top of the screen
		"notification_width": 1.0,
		"notification_height": 0.3,

		//the parameters for notification text
		"notification_text_width": 0.9,
		"notification_text_height": 0.4, 
		"notification_text_x": 0.03,
		"notification_text_y": 0.7,

		//the parameters for notification button (next, back etc)
		"notification_button_width": 0.5,
		"notification_button_height": 1.0, 
		"notification_button_x": 0.9,
		"notification_button_y": 0.35,

		//the parameters for the py help text box which explains commands
		"py_help_width": 0.5,
		"py_help_height": 0.5, 
		"py_help_x": 0.5,
		"py_help_y": 0.0,

		//the sizes of the buttons on top of the screen
		"button_width": 0.08,
		"button_height": 0.12,

		//the distance between two buttons
		"horizontal_button_spacing": 0.08,
		"vertical_button_spacing": 0.10,

		//the maximum number of sprite heads displayed on the screen		
		"button_max": 5
	},

	//define the text to be displayed for different PyGuide apis
	
	// the commands are indexed, so that the guide can read them in by number
	// while putting them in here, use the following syntax-
	// "index": "pyguide api: description"
	
	"pyguide_apis": {
		"1": "move_east(): This moves the present character towards the right by one step",
		"2": "print(args): This prints the argument args to the terminal"		
	}
}
