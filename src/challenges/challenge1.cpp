#include "main.hpp"
#include "print_debug.hpp"

void dialogue (std::string name, std:string text_to_user) {
    std:cout << name << " : " << text_to_user << std::endl
} 

void run_challenge() {
    //Coordinates are those from tiled
    // ENGINE_TODO: load map for challenge one, walls around the edges of the map
    dialogue ( "Tom",
        "Welcome to Project Zgyote"
        "My name is Tom and I am here to help you learn to move around"
        "Let's get started, press the S key to create a new character" );
    
    // EVENT_TODO: wait until a new charactor is created, don't allow any further characters to be created
    //HOOK s key...
    dialogue ( "Tom",
        "Well Done, you have made your first charactor, this charactor is called Adam"
        "You can move Adam using arrow keys, try moving Adam to the first red square on the path");

    // EVENT_TODO: wait until find_obj(1) == (5,84)
    // ENGINE_TODO: disable manual control

    dialogue ( "Tom",
        "Well Done, now we are going look at a new way of walking");

    // PY_TODO: open text editor window with c1_sample1.py

    dialogue ("Tom",
        "This text is used to control the movement of the Adam"
        "You can edit it, then save it and run it so that Adam follows your commands"
        "Let's begin, by pressing the R key to run the commands");

    // EVENT_TODO: wait until R has been pressed and hence sprite is at (7,84)
    //wait for sprite to move to (7, 84) - tile step on - trigger tile

    dialogue ("Tom",
        "As you can see the move(north) command has moved Adam, north by 1 square"
        "Now we will edit this to get Adam to move to the next red square"
        "Once complete, save the file and press R to the run again";

    // EVENT_TODO: wait until R has been pressed
              //if they run the script and it doesn't work, run again
              //object 1 is the sprite itself - only object
    while (find_obj(1) != Vec2D(12,86)) {
        dialogue ("Tom",
            "That's doesn't seem to have worked"
            "I'll pop you back and you can try again");
        move_object(7,84); //move player back to start
        // EVENT_TODO: wait until script has finished
        //PYTHON: need a way to know if a script is done
    }

    dialogue ("Tom",
        "Well Done, your getting the hang of this now"
        "Next, lets get Adam to next red square"
        "We will have to move Adam 10 times to the east");

    // EVENT_TODO: wait until R has been pressed
    while (find_obj(1) != Vec2D(22,86)) {
        dialogue ("Tom",
            "That's doesn't seem to have worked"
            "I'll pop you back and you can try again");
        move_object(12,86);
        // EVENT_TODO: wait until R has been pressed
    }

    dialogue ("Tom",
        "That was exhausting !"
        "The next red box will require you to move to the east 20 times ..."
        "There's a new command known as a for loop, which we can use to repeat a collection of commands"
        "Type in 'for _ in range (1,20):' and on a new line type move(north)"
        "This will repeat the move(north) command 20 times"
        "Run it and see how it goes" );

    // EVENT_TODO: wait until R has been pressed
    while (find_obj(1) != Vec2D(42,86)) {
        dialogue ("Tom",
            "That's doesn't seem to have worked"
            "I'll pop you back and you can try again");
        move_object(22,86);
        // EVENT_TODO: wait until R has been pressed
    }

    dialogue ("Tom",
        "Excellent Work !"
        "Now, lets introduce another for loop"
        "The next target is north by 30 and east by 10" );

    // EVENT_TODO: wait until R has been pressed
    while (find_obj(1) != Vec2D(52,56)) {
        dialogue ("Tom",
            "That's doesn't seem to have worked"
            "I'll pop you back and you can try again");
        move_object(42,86);
        // EVENT_TODO: wait until R has been pressed
    }

    dialogue ("Tom",
        "Well Done, but we're not through the woods yet"

    // EVENT_TODO: wait until R has been pressed
    while (find_obj(1) != Vec2D(92,56)) {
        dialogue ("Tom",
            "That's doesn't seem to have worked"
            "I'll pop you back and you can try again");
        move_object(52,56);
        // EVENT_TODO: wait until R has been pressed
    }         

    dialogue ("Tom",
        "Well Done, you've finished challenge 1");

}
