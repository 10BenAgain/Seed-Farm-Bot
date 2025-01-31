#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stddef.h>

#define OR  |=
#define XOR ^=

#define OR_EQ_LS1(x,y)  ((x)|=(1<<(y)));
#define XOR_EQ_LS1(x,y) ((x)^=(1<<(y)));

#define A_BTN   (1 << PORTD2)
#define B_BTN   (1 << PORTD3)
#define LFT_BTN (1 << PORTD6)
#define RGT_BTN (1 << PORTD7)
#define UP_BTN  (1 << PORTB0)
#define DWN_BTN (1 << PORTB1)
#define L_BTN   (1 << PORTB2)
#define R_BTN   (1 << PORTB3)
#define SEL_BTN (1 << PORTB4)

/* When flashing the board, PORTCB5 (Pin 13) is used for some purpose unknown to me
 * Instead of figuring out what, start will be mapped to PORTC0 instead so when
 * flashing the board while in-game, the start menu doesn't open rapidly
*/
#define SRT_BTN (1 << PORTC0)
#define PWR_BTN (1 << PORTC1)

#define RELEASE_ALL_BUTTONS (PORTD XOR PORTD); \
                            (PORTB XOR PORTB); \
                            (PORTC XOR PORTC);

/* Amount of frames approximately it takes to get from file select to recap */
#define LOAD_INTO_GAME 160

/* Amount of frames approximately it takes to get from holding seed button to file select */
#define WAIT_FOR_SAVE_MENU 236

/* 59.65549883230804 FPS = 16.76291405778042 MS = 0.016762914057780419624 Seconds
 * Target period is 0.016762914057780419624
 * Rounding down to 0.016762 should be accurate enough,
 * and it fits nicely with a prescalar of 8
 * 0.016762 * 2000000 = 33524
*/

#define F_INTERVAL 33524
#define RESET_TIMER (TCNT1 = 0);

/* Modes for splitting delay interval */
#define DEFAULT     1
#define HALF        2  // 16762
#define QUARTER     4  // 8381
#define SEVENTEENTH 17 // 1972

/* Calculate the time in standard frames (16.762) intervals */
#define DEFAULT_INTERVAL(x) ((x)*IntervalFactor)

/* Set compare register value 'mode' since we want to count half, quarter, etc frames*/
const uint32_t IntervalFactor = DEFAULT;

/* Value stored in the compare register */
const uint32_t COMPARE_INTERVAL = F_INTERVAL / IntervalFactor; 

/* Global variable for storing elapsed time in frame intervals */
static volatile uint32_t FrameCounter = 0;

typedef enum {
    A,
    B,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    L,
    R,
    SELECT,
    START,
    POWER
} Button;

void RebootAndGetToMenu();
void PressButton(Button btn, uint32_t duration);
void WaitFrames(uint32_t frames);

int main(void) {
    /* Change Ports DDRD, DDRB, and DDRC to open on for pins we need. */
    DDRD OR 0xFC; /* B11111100 */
    DDRB OR 0x3F; /* B00111111 */
    DDRC OR 0x3;  /* B00000011 */

    /* Set timer1 register TCCR1B with a prescalar of 8 (Doc 15.11.2)
     * Clear TCCR1A and set TCCR1b to CTC mode
    */
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11);

    /* Set the compare match value in OCR1A as calculated in F_INTERVAL divided by interval mode */
    OCR1A = COMPARE_INTERVAL;

    /* Enable CTC interrupts for Timer1 (Doc 15.11.8) */
    TIMSK1 OR (1 << OCIE1A);

    sei();
    /* The Arduino will store the timer value in TCNT1 (Doc 15.11.4) */

    /* Setup values for game */
    uint32_t start = 2500;

    uint32_t counter = 0;

    Button seedButton = A;

#ifdef FOREVER
    while(1) {
#else
        size_t i;
#define STORE 5 // How many seeds to store in file
        for(i = 0; i < STORE; i++) {
#endif
    /* Your code goes here */
            /* Reboot the console and navigate to game selection menu */
            RebootAndGetToMenu();

            WaitFrames(100);

            PressButton(A, DEFAULT_INTERVAL(1));
            WaitFrames(DEFAULT_INTERVAL(20));

            /* Press A to start the game */
            PressButton(A, DEFAULT_INTERVAL(1));

            /* Wait for the intro timer to play out */
            WaitFrames(DEFAULT_INTERVAL(start) + counter);

            /* Press and hold seed button */
            PressButton(seedButton, DEFAULT_INTERVAL(WAIT_FOR_SAVE_MENU));

            /* Wait for short delay */
            WaitFrames(DEFAULT_INTERVAL(2));

            /* Select save file*/
            PressButton(A, DEFAULT_INTERVAL(1));

            /* Wait until recap starts playing */
            WaitFrames(DEFAULT_INTERVAL(LOAD_INTO_GAME));

            /* Skip recap */
            PressButton(B, DEFAULT_INTERVAL(1));

            /* Wait until overworld*/
            WaitFrames(DEFAULT_INTERVAL(LOAD_INTO_GAME));

            /* Talk to sister */
            PressButton(A, DEFAULT_INTERVAL(1));

            /* Wait for seed write and dialog box */
            WaitFrames(DEFAULT_INTERVAL(48));

            /* Exit dialog Box */
            PressButton(B, DEFAULT_INTERVAL(1));

            /* Wait short interval */
            WaitFrames(DEFAULT_INTERVAL(30));

            /* Walk down to avoid somehow clicking on sister again */
            PressButton(LEFT, DEFAULT_INTERVAL(70));

            // counter++;
    }
}

/* Increment the global timer on Timer1 Compare match */
ISR(TIMER1_COMPA_vect) {
        FrameCounter++;
    }

void PressButton(Button btn, uint32_t duration) {
    switch (btn) {
    case A:      PORTD XOR A_BTN; break;
    case B:      PORTD XOR B_BTN; break;
    case LEFT:   PORTD XOR LFT_BTN; break;
    case RIGHT:  PORTD XOR RGT_BTN; break;
    case UP:     PORTB XOR UP_BTN; break;
    case DOWN:   PORTB XOR DWN_BTN; break;
    case L:      PORTB XOR L_BTN; break;
    case R:      PORTB XOR R_BTN; break;
    case SELECT: PORTB XOR SEL_BTN; break;
    case START:  PORTB XOR SRT_BTN; break;
    case POWER:  PORTC XOR PWR_BTN; break;
    default: break;
    }

    WaitFrames(duration);

    RELEASE_ALL_BUTTONS
}

void WaitFrames(uint32_t frames) {
    FrameCounter = 0;
    RESET_TIMER
    while (FrameCounter < frames){
        // Let ISR increment timer
    };
}

void RebootAndGetToMenu() {
    /* Hold Power Button for about 2 seconds */
    PressButton(POWER, DEFAULT_INTERVAL(150));

    /* Wait one second */
    WaitFrames(DEFAULT_INTERVAL(60));

    /* Hold Power button */
    PressButton(POWER, DEFAULT_INTERVAL(150));

    /* Wait about 8 seconds, this can probably be shortened */
    WaitFrames(DEFAULT_INTERVAL(400));

    PressButton(A, DEFAULT_INTERVAL(1));

    /* Wait about 3 seconds until we are ready to Press A to start the game */
    WaitFrames(DEFAULT_INTERVAL(180));
}