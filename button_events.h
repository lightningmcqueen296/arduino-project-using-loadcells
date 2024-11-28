
class ButtonEvents {
    static const long hold_time = 1000; //
    long button_held_time;
    long button_last_pressed_time;
    int buttonPin;
    bool buttonLastState;
    bool buttonPrevState;
public:
    ButtonEvents (int pinNo);
    void toggleStatus();
    void setHoldState();
    void resetHoldState();
    bool eventIsClicked();
    bool eventIsCommitted();
    bool triggered(String eventType);
    String getEvents();
};	