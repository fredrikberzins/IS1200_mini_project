#include "header.h"

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

