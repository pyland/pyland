#ifndef CHALLENGE_H
#define CHALLENGE_H

#include <string>

class Challenge {
    public:
        // TODO: Make the editor part of the core
        // engine, not the challenge class
        Challenge(std::string editor);
        virtual ~Challenge() = default;

        virtual void start() = 0;
        virtual void finish() = 0;

        // TODO: Make this function part of the core
        // engine, not the challenge class
        static void dialogue (std::string name, std::string text_to_user);

    private:
        std::string editor;
};

#endif