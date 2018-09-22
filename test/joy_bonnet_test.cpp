#include "joyBonnetConstants.h"
#include "joyBonnet.h"

using namespace std;

int main(void)
{
    // sets up the wiringPi library
    if (wiringPiSetupGpio() < 0)
    {
        fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
        return 1;
    }

    JoyBonnet &joy = JoyBonnet::Instance();

    joy.addHandler(X_BUTTON_PIN, { [=](){ cout << "X Button!" << endl; return; }});
    joy.addHandler(Y_BUTTON_PIN, { [=](){ cout << "Y Button!" << endl; return; }});
    joy.addHandler(A_BUTTON_PIN, { [=](){ cout << "A Button!" << endl; return; }});
    joy.addHandler(B_BUTTON_PIN, { [=](){ cout << "B Button!" << endl; return; }});
    joy.addHandler(START_BUTTON_PIN, { [=](){ cout << "Start Button!" << endl; return; }});
    joy.addHandler(SELECT_BUTTON_PIN, { [=](){ cout << "Select Button!" << endl; return; }});
    joy.addHandler(PLAYER_1_BUTTON_PIN, { [=](){ cout << "Player 1 Button!" << endl; return; }});
    joy.addHandler(PLAYER_2_BUTTON_PIN, { [=](){ cout << "Player 2 Button!" << endl; return; }});

    cout << "Main thread:" << this_thread::get_id() << endl;

    // display counter value every second.
    while (1)
    {
        // int x = joy.read_joystick_x();
        // int y = joy.read_joystick_y();
        // cout << "(X,Y) -> [" << x << "," << y << "]" << endl;

        // int xCoord, yCoord;
        // tie(xCoord, yCoord) = joy.read_joystick_coords();

        // cout << "Tuple -> [" << xCoord << "," << yCoord << "]" << endl;

        // cout << print_direction(joy.read_joystick_direction()) << endl;
        delay(100); // wait 1 second
    }
    
    return 0;
}
