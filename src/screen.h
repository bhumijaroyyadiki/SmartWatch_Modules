#pragma once

enum ScreenID {
    SCREEN_TIME,
    SCREEN_STEPS,
    SCREEN_WIFI,
    SCREEN_STOPWATCH,
    SCREEN_HOME
};
void setScreen(ScreenID s);
void showHomeScreen();

extern volatile ScreenID currentScreen;
