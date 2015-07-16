#include <glog/logging.h>
#include <iostream>
#include "game_init.hpp"


using namespace std;

int main(int argc, char *argv[]) {

    game_init(argc, argv);
//	switch (argc) {
//		default:
//			std::cout << "Usage: " << argv[0] << " [EDITOR] [MAP]" << std::endl;
//			return 1;
//
//		// The lack of break statements is not an error!!!
//		case 3:
//			//map_path = std::string(argv[2]);
//		case 2:
//			Engine::set_editor(argv[1]);
//		case 1:
//			break;
//	}

    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    /// CREATE GLOBAL OBJECTS

    return 0;
}
