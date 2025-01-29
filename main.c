#include <avr/io.h>
#include <stdint.h>

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
#define SRT_BTN (1 << PORTB5)
#define PWR_BTN (1 << PORTC5)

#define RELEASE_ALL_BUTTONS (PORTD ^= PORTD); \
                            (PORTB ^= PORTB); \
                            (PORTC ^= PORTC);

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
static uint32_t FrameCounter = 0;

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
    DDRC OR 0x1;  /* B00000001 */

    /* Set timer1 register TCCR1B with a prescalar of 8 (Doc 15.11.2) */
    OR_EQ_LS1(TCCR1B, CS11) /* From Table 15-6 */

    /* Set Timer1 control register TCCR1A to clear on compare match (Doc 15.11.1) */
    OR_EQ_LS1(TCCR1A, COM1A1)

    /* Set the compare match value in OCR1A as calculated in F_INTERVAL divided by interval mode */
    OCR1A = COMPARE_INTERVAL;

    /* The Arduino will store the timer value in TCNT1 (Doc 15.11.4) */

    /* Setup values for game */
    uint32_t start = 2088;

    uint32_t counter = 0;

    Button seedButton = A;

    int i = 2000;

    while(1) {

        while(i--) {
            /* Reboot the console and navigate to game selection menu */
            RebootAndGetToMenu();

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
            PressButton(DOWN, DEFAULT_INTERVAL(70));

            counter++;
        }

    }
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
    do {
        if (TIFR1 & (1 << OCF1A)) {
            FrameCounter++;
            TIFR1 = (1 << OCF1A);
        }
    } while (frames < FrameCounter);
}

void RebootAndGetToMenu() {
    /* Hold Power Button for about 2 seconds */
    PressButton(POWER, DEFAULT_INTERVAL(150));

    /* Wait one second */
    WaitFrames(DEFAULT_INTERVAL(60));

    /* Hold Power button */
    PressButton(POWER, DEFAULT_INTERVAL(150));

    /* Wait about 8 seconds, this can probably be shortened */
    WaitFrames(DEFAULT_INTERVAL(470));

    PressButton(A, DEFAULT_INTERVAL(1));

    /* Wait about 3 seconds until we are ready to Press A to start the game */
    WaitFrames(DEFAULT_INTERVAL(180));
}




