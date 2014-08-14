#ifndef NOTIFICATION_BAR_H
#define NOTIFICATION_BAR_H

#include "button.hpp"
#include "lifeline.hpp"
#include "notification_stack.hpp"
#include "text_font.hpp"
#include "text.hpp"

enum class Direction {NEXT, PREVIOUS};

class NotificationBar {
    private:
        Notification notification_stack;
        Text* notification_text;

        int forward_button_id;
        int backward_button_id;

        std::unique_ptr<Text> backward_text;
        std::unique_ptr<Text> forward_text;
        Lifeline text_box;

        void move_notification(Direction direction);

        ///
        /// update buttons depending on the notification_stach flags
        ///
        void hide_buttons();

    public:

        NotificationBar();
        ~NotificationBar();

        ///
        /// generate the backward and forward navigation button text for the notification bar 
        /// TODO: remove_this when GUI fonts is done
        ///
        void text_displayer();


        void add_notification(std::string text_to_display);
};

#endif