#include "header.h"

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

int power(int base, int exp) {
    int out = 1;
    for(int i = 0; i < exp; i++) {
        out *= base;
    }
    return out;
}

int lower_8(int num) {
    num -= num%8;
    return num/8;
}

int int_round(double num) {
    if (double_mod(num) < (1/2)) {
        return (int)(num - double_mod(num));
    } else {
        return (int)(num - double_mod(num)) + 1;
    }
}

double double_mod(double num) {
    return num - (int)(num/1) * 1;
}