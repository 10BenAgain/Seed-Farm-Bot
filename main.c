#include <avr/io.h>
#include <stdint.h>
#include <stddef.h>

#include "btn.h"

/* How Frame Interval is calculated
 * 59.65549883230804 FPS = 16.76291405778042 MS = 0.016762914057780419624 Seconds
 * Target period is 0.016762914057780419624
 * Rounding down to 0.016762 should be accurate enough,
 * and it fits nicely with a prescalar of 8 (16 MHz or 16000000 Hz)
 * New clock speed = 0.016762 * (16000000 / 8 )
 * 0.016762 * 2000000 = 33524
 * Now we just need to check every time the TCNT1 register matches the calculated value.
 * To increment the target seed timer by smaller intervals, we can simply divide the
 * result interval by even divisors (33524 / 2, 4) where 2 = 16762 and 4 = 8381
*/

#define INCREMENT
#define STARTUP_R

// How many frames it takes to get from save select to over world
#define LOAD_INTO_GAME      250

// How long to hold seed button down for after timer is up to get to save select menu
#define WAIT_FOR_SAVE_MENU  350

#define OR_EQ_LS1(x,y)  ((x)|=(1<<(y)));

#ifdef DEFAULT
    #define F_INTERVAL 33524
    #define DEFAULT_INTERVAL(x) ((x))
#endif

#ifdef HALF
    #define F_INTERVAL 16762
    #define DEFAULT_INTERVAL(x) ((x)*2)
#endif

#ifdef QUARTER
    #define F_INTERVAL 8381
    #define DEFAULT_INTERVAL(x) ((x)*4)
#endif

#define RESET_TIMER (TCNT1 = 0);

void WaitFrames(uint32_t frames);

void PressA(uint32_t duration);
void PressB(uint32_t duration);

__attribute__((unused)) void PressR(uint32_t duration);
void PressPower(uint32_t duration);

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
        PressPower(DEFAULT_INTERVAL(150));               /* Hold Power Button for about 2 seconds                                */
        WaitFrames(DEFAULT_INTERVAL(60));                /* Wait one second                                                      */
        PressPower(DEFAULT_INTERVAL(150));               /* Hold Power button                                                    */
        WaitFrames(DEFAULT_INTERVAL(400));               /* Wait about 8 seconds, this can probably be shortened                 */
        PressA(DEFAULT_INTERVAL(1));                     /* Continue into the Main DS menu                                       */
        WaitFrames(DEFAULT_INTERVAL(180));               /* Wait about 3 seconds until we are ready to Press A to start the game */
        PressA(DEFAULT_INTERVAL(1));                     /* Press Start to start the game                                        */
            #ifdef STARTUP_R
                    PORTD ^= R_BTN;
            #endif

    #ifdef INCREMENT
            WaitFrames(DEFAULT_INTERVAL(start));         /* Wait for the intro timer to play out and increment timer by desired frame interval */
        #if defined HALF || defined QUARTER
                WaitFrames(increment);
        #endif
    #else
            WaitFrames(DEFAULT_INTERVAL(START));         /* Wait for the intro timer to play out */
    #endif

    #ifdef SEED_BUTTON_L
            PressL(DEFAULT_INTERVAL(WAIT_FOR_SAVE_MENU));    /* Hold L until save select Menu is ready  */
    #endif

    #ifdef SEED_BUTTON_START
            PressStart(DEFAULT_INTERVAL(WAIT_FOR_SAVE_MENU)); /* Hold Start until save select Menu is ready  */
    #endif

    #ifdef SEED_BUTTON_A
            PressA(DEFAULT_INTERVAL(WAIT_FOR_SAVE_MENU)); /* Hold A until save select Menu is ready  */
    #endif
        WaitFrames(DEFAULT_INTERVAL(2));                  /* Wait a short delay before pressing A    */
            #ifdef STARTUP_R
                    PORTD ^= R_BTN;
            #endif
        PressA(DEFAULT_INTERVAL(1));                      /* Select save file                        */
        WaitFrames(DEFAULT_INTERVAL(LOAD_INTO_GAME));     /* Wait until recap starts playing         */
        PressB(DEFAULT_INTERVAL(1));                      /* Press B to skip recap                   */
        WaitFrames(DEFAULT_INTERVAL(LOAD_INTO_GAME));     /* Wait until overworld                    */
        PressA(DEFAULT_INTERVAL(1));                      /* Talk to sister                          */
        WaitFrames(DEFAULT_INTERVAL(48));                 /* Wait for seed write and dialog box      */
        PressB(DEFAULT_INTERVAL(1));                      /* Exit dialog Box                         */
        WaitFrames(DEFAULT_INTERVAL(30));                 /* Wait short interval                     */

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

void
PressA(uint32_t duration) {
    PORTD ^= A_BTN;
    WaitFrames(duration);
    PORTD ^= A_BTN;
}

void
PressB(uint32_t duration) {
    PORTD ^= B_BTN;
    WaitFrames(duration);
    PORTD ^= B_BTN;
}

__attribute__((unused)) void
PressR(uint32_t duration) {
    PORTD ^= R_BTN;
    WaitFrames(duration);
    PORTD ^= R_BTN;
}

#ifdef SEED_BUTTON_L
    void
    PressL(uint32_t duration) {
        PORTD ^= L_BTN;
        WaitFrames(duration);
        PORTD ^= L_BTN;
}
#endif

#ifdef SEED_BUTTON_START
    void
    PressStart(uint32_t duration) {
        PORTD ^= ST_BTN;
        WaitFrames(duration);
        PORTD ^= ST_BTN;
    }
#endif

void
PressPower(uint32_t duration) {
    PORTC ^= PWR_BTN;
    WaitFrames(duration);
    PORTC ^= PWR_BTN;
}

void
WaitFrames(uint32_t frames) {
    uint32_t counter = 0;
    RESET_TIMER
    while (counter < frames){
        if (TIFR1 & (1 << OCF1A)) {
            counter++;
            OR_EQ_LS1(TIFR1, OCF1A)
        }
    }
}
