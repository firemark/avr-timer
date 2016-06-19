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
    DDRC = 0b00111111;
    DDRD = 0b10000011;
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
char timer = 0;

ISR(INT0_vect) { // start/reset
    if (is_run) {
        is_run = 0;
        timer = 0;
    } else {
        is_run = 1;
    }
}

ISR(INT1_vect) { // add/stop
    if (is_run) {
        timer += 5;
    } else {
        is_run = 0;
    }
}

ISR(TIM0_OVF_vect) {
    //todo
}

int main(void) {
    setup();
    for(;;){
        set_digit(timer / 10, 0);
        _delay_ms(5);
        set_digit(timer, 1);
        _delay_ms(5);
    }
    return 0;
}
