#include <avr/io.h>
#include <stdint.h>
#include <stddef.h>

#include "btn.h"
#include "vals.h"

#define INCREMENT

// How many frames it takes to get from save select to over world
#define LOAD_INTO_GAME      250

// How long to hold seed button down for after timer is up to get to save select menu
#define WAIT_FOR_SAVE_MENU  350

#define OR_EQ_LS1(x,y)  ((x)|=(1<<(y)));
#define RESET_TIMER (TCNT1 = 0);

void WaitFrames(uint32_t frames);

void PressA(uint32_t duration);
void PressB(uint32_t duration);
void PressX(uint32_t duration);
void PressHome(uint32_t duration);

#ifdef SEED_BUTTON_L
    void PressL(uint32_t duration);
#endif

#ifdef SEED_BUTTON_START
    void PressStart(uint32_t duration);
#endif

int main(void) {
    /* Board setup */
    DDRD |= 0xFC;   // B11111100
    DDRB |= 0x3F;   // B00111111
    DDRC |= 0x3;    // B00000011

    TCCR1A = 0;

    OR_EQ_LS1(TCCR1B, WGM12)
    OR_EQ_LS1(TCCR1B, CS11) // Prescaler 8

    OCR1A = F_INTERVAL;

    OR_EQ_LS1(TIMSK1, OCIE1A)

#ifdef INCREMENT
    uint32_t start = START;
    #if defined HALF || defined QUARTER
        uint32_t increment = 0;
    #endif
#endif

#ifdef FOREVER
    while(1) {
#else
    for (size_t j = 0; j < SEEDS_TO_STORE; j++) {
#endif
        WaitFrames(_DI(180));                    // Small delay while on HOME MENU
        PressA(_DI(10));                         // Press A to launch GBA NSO
        WaitFrames(_DI(680));                    // Wait to get into launcher
        PressA(_DI(10));                         // Launch the game
            #ifdef STARTUP_R
                    PORTD ^= R_BTN;
            #endif

    #ifdef INCREMENT
            WaitFrames(_DI(start));               // Wait for the intro timer to play out and increment timer by desired frame interval */
        #if defined HALF || defined QUARTER
                WaitFrames(increment);
        #endif
    #else
            WaitFrames(_DI(START));              // Wait for the intro timer to play out
    #endif

    #ifdef SEED_BUTTON_L
            PressL(_DI(WAIT_FOR_SAVE_MENU));     // Hold L until save select Menu is ready
    #endif

    #ifdef SEED_BUTTON_START
            PressStart(_DI(WAIT_FOR_SAVE_MENU)); // Hold Start until save select Menu is ready
    #endif

    #ifdef SEED_BUTTON_A
            PressA(_DI(WAIT_FOR_SAVE_MENU));     // Hold A until save select Menu is ready
    #endif
        WaitFrames(_DI(2));                      // Wait a short delay before pressing A
            #ifdef STARTUP_R
                    PORTD ^= R_BTN;
            #endif
        PressA(_DI(10));                         // Select save file
        WaitFrames(_DI(LOAD_INTO_GAME));         // Wait until recap starts playing
        PressB(_DI(10));                         // Press B to skip recap
        WaitFrames(_DI(LOAD_INTO_GAME));         // Wait until overworld
        PressA(_DI(10));                         // Talk to sister
        WaitFrames(_DI(148));                    // Wait for seed write and dialog box
        PressB(_DI(10));                         // Exit dialog Box
        WaitFrames(_DI(130));                    // Wait short interval
        PressHome(_DI(10));                      // Exit Game
        WaitFrames(_DI(480));                    // Wait till home screen
        PressX(_DI(10));                         // Press X to get close menu
        WaitFrames(_DI(130));                    // Wait a short time
        PressA(_DI(10));                         // Press A to exit the game
        WaitFrames(_DI(180));                    // Wait for the game to exit

#ifdef INCREMENT
    #ifdef DEFAULT
            start++;
    #endif

    #ifdef HALF
            if (increment < 1) { increment++; }
            else {
                increment = 0;
                start++;
            }
    #endif

    #ifdef QUARTER
            if (increment < 3) { increment++;}
            else {
                increment = 0;
                start++;
            }
    #endif

#endif
    }
}

void PressA(uint32_t duration)
{
    PORTD ^= A_BTN;
    WaitFrames(duration);
    PORTD ^= A_BTN;
}

void PressB(uint32_t duration)
{
    PORTD ^= B_BTN;
    WaitFrames(duration);
    PORTD ^= B_BTN;
}

void PressX(uint32_t duration)
{
    PORTD ^= X_BTN;
    WaitFrames(duration);
    PORTD ^= X_BTN;
}

void PressHome(uint32_t duration)
{
    PORTD ^= HOME_BTN;
    WaitFrames(duration);
    PORTD ^= HOME_BTN;
}

__attribute__((unused))
void PressR(uint32_t duration)
{
    PORTD ^= R_BTN;
    WaitFrames(duration);
    PORTD ^= R_BTN;
}

#ifdef SEED_BUTTON_L
__attribute__((unused))
void PressL(uint32_t duration) {
        PORTD ^= L_BTN;
        WaitFrames(duration);
        PORTD ^= L_BTN;
}
#endif

#ifdef SEED_BUTTON_START
__attribute__((unused))
void PressStart(uint32_t duration) {
        PORTD ^= ST_BTN;
        WaitFrames(duration);
        PORTD ^= ST_BTN;
    }
#endif

void WaitFrames(uint32_t frames) {
    uint32_t counter = 0;
    RESET_TIMER
    while (counter < frames){
        if (TIFR1 & (1 << OCF1A)) {
            counter++;
            OR_EQ_LS1(TIFR1, OCF1A)
        }
    }
}
