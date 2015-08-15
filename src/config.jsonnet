//This is the config file for the game, it is a jsonnet file for ease of use.
{
	"game_settings": {
		"language": "pyrate"
		//"language": "english"
	},

	"special_methods": {
		"walk_on" : "b" //TODO explain this!!!!
	},
	"files": {
		"game_folder": "../game",
		"level_folder": self.game_folder+"/levels/",
		//"level_location": "/test_world/test_level/test_one",
		//"level_location": "/test_world/yingischallenged/main",
		//"level_location": "/intro",
		"world_location": "/test_world/yingischallenged/",
		"level_location": "/test_world/yingischallenged/main",
		"full_level_location" : "/test_world/yingischallenged/main",
		//"full_level_location" : "/intro",
		//"full_level_location" : self.game_folder + "/levels" + self.level_location,
		"full_world_location" : self.game_folder + "/levels" + self.world_location,
		"player_scripts": self.game_folder + "/player_scripts",
		"object_location": self.game_folder + "/objects",
		"font_location": self.game_folder + "/fonts",
		"dialogue_font": self.font_location + "/Ubuntu-R.ttf"
		
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
		"left_x_offset": 0.00,
		"right_x_offset": 0.91,
		"bottom_y_offset": 0.00,
		"top_y_offset": 0.87,				

		//for placing the title of a menu
		"title_x_offset": 0.50,
		"title_y_offset": 0.90,
		"pyguide_title_x_offset": 0.25,
		"pyguide_title_y_offset": 0.9,

		//for placing an item on the menu
		"menu_x_offset": 0.03,
		"menu_y_offset": 0.75,
		"menu_height": 1.0,
		"menu_width": 1.0,
		"menu_spacing": 1.0,	
		
		//the sizes of the notification box
		"notification_width": 1.00,
		"notification_height": 0.30,

		//the parameters for notification text
		"notification_text_width": 0.90,
		"notification_text_height": 0.40, 
		"notification_text_x": 0.03,
		"notification_text_y": 0.70,
		"notification_text_buffer": 80,

		//the parameters for notification button (next, back etc)
		"notification_button_width": 0.50,
		"notification_button_height": 1.00, 
		"notification_button_x": 0.90,
		"notification_button_y": 0.35,

		//the parameters for the py help text box 
		"py_help_width": 0.50,
		"py_help_height": 0.90, 
		"py_help_x": 0.45,
		"py_help_y": 0.05,

		//the parameters for the py guide commands on the menu 
		"py_help_item_width": 0.30,
		"py_help_item_height": 0.30, 
		"py_help_item_x": 0.00,
		"py_help_item_y": 0.65,
		"py_help_item_spacing": 0.15,

		//the parameters for the py help text which explains commands
		"py_help_text_width": 0.90,
		"py_help_text_height": 0.50, 
		"py_help_text_x": 0.05,
		"py_help_text_y": 0.80,
		"py_help_text_buffer": 500,

		//the parameters for the buttons accompanying the text box
		"py_help_button_width": 0.10,
		"py_help_button_height": 0.10, 
		"py_help_button_x": 0.80,
		"py_help_button_y": 0.15,
		"py_help_button_spacing": 0.70,

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
		"number": 2,
		"0": "move_east(): This moves the present character towards the right by one step. This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step. This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step. This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step. This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step. This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step. This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step. This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step. This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step. This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.This moves the present character towards the right by one step.",
		"1": "print(args): THIS prints the argument args to the terminal"		
	}
}
