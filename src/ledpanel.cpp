#include "./../code/hardware/ledpanel.hpp"

//Define LEDSet Object with set LED pins.
LEDSet<5> LEDPanel({LED_BUILTIN, LED_RED, LED_GREEN, LED_WHITE, LED_BLUE});