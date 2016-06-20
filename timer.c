#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

const int NUMBERS[] = {
    0b00111111,
    0b00000011,
    0b01101100,
    0b01100110,
    0b01010011,
    0b01110110,
    0b01111110,
    0b00110011,
    0b01111111,
    0b01110111
};

int setup(void) {
    // setup ports
    DDRC = 0b00111111;
    DDRD = 0b10000011;
    // setup timer0
    TCCR0 = 0b10; // set to clk/8
    TCNT0 = 0; // reset counter
    // setup timer1
    TCCR1A = 0b00000011;
    TCCR1B = 0b00011101;
    OCR1A = 15625; // 16 * 10**6 / 1024

    TIMSK = 1 + (1 << 4) ; // set overflow and timer1 comp interrupt
    // setup external interrupts
    MCUCR = 0b00001111; // rising edge interrupt
    GICR = 0b11000000; // set int0, int1 interrupt
}

char get_pin_select(char seg) {
    return 1 << ((seg % 2) + 6);
}

void set_digit(char val, char seg) {
    char digit = NUMBERS[val % 10];
    char select_pin = get_pin_select(seg);
    PORTC = digit & 0b0011111;
    PORTD = ((digit >> 6) & 1) + select_pin;
}

void clear_digit(char seg) {
    PORTC = 0;
    PORTD = get_pin_select(seg);
}

char is_run = 0;
char is_setup = 1;
char timer = 0;

ISR(INT0_vect) { // start/reset
    if (is_setup) {
        is_run = 1;
        is_setup = 0;
    } else if (is_run) {
        is_run = 0;
        is_setup = 1;
        timer = 0;
    }
}

ISR(INT1_vect) { // add/stop/resume
    if (is_setup) {
        timer += 5;
    } else if (is_run) {
        is_run = 0;
    } else {
        is_run = 1;
    }
}

int state = 0;

ISR(TIMER0_OVF_vect) {
    switch (state) {
        case 0: set_digit(timer / 10, 0); break;
        case 1: clear_digit(0); break;
        case 2: set_digit(timer, 1); break;
        case 3: clear_digit(1); break;
    }
    ++state;
    if (state >= 4) {
        state = 0;
    }
}

ISR(TIMER1_COMPA_vect) {
    timer += 1;
}

int main(void) {
    setup();
    sleep_enable();
    sei();
    for (;;) {
        sleep_cpu();
    }
    return 0;
}
