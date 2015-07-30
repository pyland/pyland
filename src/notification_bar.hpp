#ifndef NOTIFICATION_BAR_H
#define NOTIFICATION_BAR_H

#include <memory>
#include <string>

#include "button.hpp"
#include "lifeline.hpp"
#include "notification_stack.hpp"

class Text;

enum class Direction {NEXT, PREVIOUS};

class NotificationBar {
private:
    Notification notification_stack;
    Text* notification_text;

    std::shared_ptr<Button> forward_button;

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

    ///
    /// Clears the notifications
    ///
    void clear_text();

    ///
    /// Add a notification to the bar
    /// @param text_to_display the notification
    ///
    void add_notification(std::string text_to_display);
};

#endif
