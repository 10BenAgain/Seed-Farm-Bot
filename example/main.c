#include <avr/io.h>
#include <stdint.h>

/* These are ridiculous, but I hate the way the OR and XOR characters look */
#define OR_EQ(x,y) ((x)|=(y));
#define XOR_EQ(x,y) ((x)^=(y));

#define PART_FOUR

#ifdef PART_FOUR
#include <avr/interrupt.h>
#endif

/* From Dean Camera's "Newbie's Guide to AVR Timers"
 * http://fabacademy.org/2015/eu/students/gunnarsson.thorarinn_b.b/docs/d7/Timers.pdf
 * This is edited to match the topics discussed but for the Atmega328P
*/

int main(void) {
    /* Change Ports DDRD and DDRB to open on for pins we need. */
    OR_EQ(DDRD, 0xFC) /* B11111100 */
    OR_EQ(DDRB, 0x3F) /* B00111111 */

#ifdef PART_ONE
    /* Set timer1 register TCCR1B with a prescalar of 64 (Doc 15.11.2) */
    OR_EQ(TCCR1B, (1 << CS10)) /* From Table 15-6 */
    OR_EQ(TCCR1B, (1 << CS11))
#endif

#ifdef PART_TWO
    /* Set timer1 register TCCR1B with a prescalar of 256 (Doc 15.11.2) */
    OR_EQ(TCCR1B, (1 << CS12)) /* From Table 15-6 */

     /* Initialize second counter variable */
    uint32_t ElapsedSeconds = 0;
#endif

#ifdef PART_THREE
    /* Set timer1 register TCCR1B with a prescalar of 256 (Doc 15.11.2) */
    OR_EQ(TCCR1B, (1 << CS12)) /* From Table 15-6 */

    /* Set Timer1 control register TCCR1A to clear on compare match (Doc 15.11.1) */
    OR_EQ(TCCR1A, (1 << COM1A1))

    /* Set the compare match value in OCR1A as calculated in Part Two (62500 -1) */
    OCR1A = 62499;
#endif

#ifdef PART_FOUR
    /* Set timer1 register TCCR1B with a prescalar of 256 (Doc 15.11.2) */
    OR_EQ(TCCR1B, (1 << CS12)) /* From Table 15-6 */

    /* Set Timer1 control register TCCR1A to clear on compare match (Doc 15.11.1) */
    OR_EQ(TCCR1A, (1 << COM1A1))

    /* Set the compare match value in OCR1A as calculated in Part Two (62500 -1) */
    OCR1A = 62499;

    /* Enable CTC interrupts for Timer1 (Doc 15.11.8) */
    OR_EQ(TIMSK1, (1 << OCIE1A))

    /* Turns on Global interrupts */
    sei();
#endif
    /* The Arduino will now store the timer value in TCNT1 (Doc 15.11.4) */

    /* Start the loop for the Arduino */
    while(1) {


#ifdef PART_ONE /* Blink at quick interval*/
        /* 1. Goal is to create a delay of 1/20th of a second and the Arduino Nano has a clock speed
         * of 16MHz. So the clock period is 1/16,000,000. To get 1/20th of a second the target
         * is (1/20) / (1/16000000) - 1 = 80,000 - 1 which is too big to fit in the TCNT1 Timer with a
         * prescalar of 1. But with a prescalar of 64, we get (1/20) / (64/16000000) - 1 = 12500 - 1
        */

        /* Check if counter is at our calculated interval */
        if (TCNT1 >= (12500 - 1)) {

            /* Toggle LED */
            XOR_EQ(PORTB, (1 << PORTB5))

            /* Reset the timer to blink */
            TCNT1 = 0;
        }

#endif
#ifdef PART_TWO /* Long Timer Delays*/

        /* 2. The goal here is to have the arduino blink evert one second.
         * The equation this time is (1/1) / (1/16000000) - 1 which is just 16000000 - 1
         * That number obviously wont fit in the 16 bit register so we need to find a
         * prescalar that works. With a prescalar of 256 we get 62500 - 1 which is just right
        */
        if (TCNT1 >= (62500 -1)) {

            /* Reset the timer after 1 second*/
            TCNT1 = 0;

            ElapsedSeconds++;

            /* Check if one minute has elapsed */
            if (ElapsedSeconds == 60) {

                /* Reset the minute timer */
                ElapsedSeconds = 0;

                /* Toggle the LED */
                XOR_EQ(PORTB, (1 << PORTB5))
            }
        }
#endif

#ifdef PART_THREE /* CTC Timer Mode (Clear Timer on Compare) */
    /* The TIFR1 Timer1 Interrupt Flag register will set a flag at bit 1 when TCNT1 value matches
    * the output compare register OCF1A that we set to 62499. Here, we are checking to see if the
    * flag is set which means that the timer has reached its maximum as defined above (Doc 15.11.9)
    */

    /* According to Dean Camera's article, the method for counting in Part Two is inefficient because
     * we are wasting cycles checking the timer's value every time the loop runs. The timer may also pass
     * the desired compare value slightly while processing the loop causing a slight inaccuracy. Using the following
     * method of checking the TIFR1 Compare flag is much more efficient
    */

    if (TIFR1 & (1 << OCF1A)) {

        /* Toggle the LED */
        XOR_EQ(PORTB, (1 << PORTB5))

        /* The Atmega328P documentation says that OCF1A can be cleared by writing a logic one bit to its location.
         * This lets us clear the flag that we are checking to see if one second has elapsed, restarting the loop
         * See 15.11.9 Bit 1 - OCF1A for this information
        */
        TIFR1 = (1 << OCF1A);
    }
#endif
    }
}

#ifdef PART_FOUR
/* Vector value https://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html#gad28590624d422cdf30d626e0a506255f */
ISR(TIMER1_COMPA_vect) {

    /* Toggle the LED */
    XOR_EQ(PORTB, (1 << PORTB5))
}
#endif
