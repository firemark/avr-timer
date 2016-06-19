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
    // setup timer
    TCCR0 = 0b101; // set to clk/1024
    TCNT0 = 0; // reset counter
    TIMSK = 1; // set overflow interrupt
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
    } else if (is_run) {
        is_run = 0;
        timer = 0;
    }
}

ISR(INT1_vect) { // add/stop
    if (is_setup) {
        timer += 5;
    } else if (is_run) {
        is_run = 0;
    } else {
        is_run = 1;
    }
}

int cycles = 0;

ISR(TIM0_OVF_vect) {
    if (cycles++ == 61) {
        cycles = 0;
    } else {
        timer += 1;
    }
}

int main(void) {
    setup();
    for (;;) {
        set_digit(timer / 10, 0);
        _delay_ms(2);
        clear_digit(2);

        set_digit(timer, 1);
        _delay_ms(2);
        clear_digit(2);
    }
    return 0;
}
