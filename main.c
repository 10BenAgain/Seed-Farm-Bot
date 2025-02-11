#include <avr/io.h>
#include <stdint.h>
#include <stddef.h>


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

#define HALF
#define INCREMENT
#define SEEDS_TO_STORE      2000
#define START               2075 // Starting point, approximately 34781 MS (Should hit seed A080)
#define LOAD_INTO_GAME      250  // How many frames it takes to get from save select to overworld
#define WAIT_FOR_SAVE_MENU  350  // How long to hold seed button down for after timer is up to get to save select menu

#define OR_EQ_LS1(x,y)  ((x)|=(1<<(y)));

#define A_BTN   (1 << PORTD2)
#define B_BTN   (1 << PORTD3)
#define PWR_BTN (1 << PORTC1)

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

static inline void PressA(uint32_t duration);
static inline void PressB(uint32_t duration);
static inline void PressPower(uint32_t duration);

volatile uint16_t FrameCounter = 0;

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
    uint32_t increment = 0;
#endif

#ifdef FOREVER
    while(1) {
#else
    size_t i;
    size_t stored = SEEDS_TO_STORE;
    for(i = 0; i < stored; i++) {
#endif
        PressPower(DEFAULT_INTERVAL(150));               /* Hold Power Button for about 2 seconds */
        WaitFrames(DEFAULT_INTERVAL(60));                /* Wait one second */
        PressPower(DEFAULT_INTERVAL(150));               /* Hold Power button */
        WaitFrames(DEFAULT_INTERVAL(400));               /* Wait about 8 seconds, this can probably be shortened */
        PressA(DEFAULT_INTERVAL(1));                     /* Continue into the Main DS menu */
        WaitFrames(DEFAULT_INTERVAL(
                           180));               /* Wait about 3 seconds until we are ready to Press A to start the game */
        PressA(DEFAULT_INTERVAL(1));                     /* Press Start to start the game */

#ifdef INCREMENT
        WaitFrames(DEFAULT_INTERVAL(start) +
                   increment); /* Wait for the intro timer to play out and increment timer by desired frame interval */
#else
        WaitFrames(DEFAULT_INTERVAL(START));             /* Wait for the intro timer to play out */
#endif
        PressA(DEFAULT_INTERVAL(WAIT_FOR_SAVE_MENU));    /* Hold A until save select Menu is ready */
        WaitFrames(DEFAULT_INTERVAL(2));                 /* Wait a short delay before pressing A */
        PressA(DEFAULT_INTERVAL(1));                     /* Select save file*/
        WaitFrames(DEFAULT_INTERVAL(LOAD_INTO_GAME));    /* Wait until recap starts playing */
        PressB(DEFAULT_INTERVAL(1));                     /* Press B to skip recap */
        WaitFrames(DEFAULT_INTERVAL(LOAD_INTO_GAME));    /* Wait until overworld*/
        PressA(DEFAULT_INTERVAL(1));                     /* Talk to sister */
        WaitFrames(DEFAULT_INTERVAL(48));                /* Wait for seed write and dialog box */
        PressB(DEFAULT_INTERVAL(1));                     /* Exit dialog Box */
        WaitFrames(DEFAULT_INTERVAL(30));                /* Wait short interval */

#ifdef INCREMENT
#ifdef HALF
        if (increment < 1) { increment++; }
        else {
            increment = 0;
            start++;
        }
#endif
#ifdef QUARTER
        if (increment < 3) { increment++;} else { increment = 0; start++;}
#endif
#endif
    }
}

static inline void PressA(uint32_t duration) {
    PORTD ^= A_BTN;
    WaitFrames(duration);
    PORTD ^= A_BTN;
}

static inline void PressB(uint32_t duration) {
    PORTD ^= B_BTN;
    WaitFrames(duration);
    PORTD ^= B_BTN;
}

static inline void PressPower(uint32_t duration) {
    PORTC ^= PWR_BTN;
    WaitFrames(duration);
    PORTC ^= PWR_BTN;
}

void WaitFrames(uint32_t frames) {
    FrameCounter = 0;
    RESET_TIMER
    while (FrameCounter < frames){
        if (TIFR1 & (1 << OCF1A)) {
            FrameCounter++;
            OR_EQ_LS1(TIFR1, OCF1A)
        }
    }
}
