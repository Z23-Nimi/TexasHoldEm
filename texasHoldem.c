/*
Sean Keenan
William De Anda
03/27/2026
Simulate a game of Texas Hold'Em against the computer, includes betting
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DECK_SIZE 52

typedef struct {
    int rank;
    int suit;
} Card;

const char *rankStrength[] = {"","","2","3","4","5","6","7","8","9","10","J","Q","K","A"};

void initalizeDeck(Card deck[]) {
    int index = 0;
    for (int suit = 0; suit < 4; suit++) {
        for (int rank = 2; rank < 15; rank++) {
            deck[index].rank = rank;
            deck[index].suit = suit;
            index++;
        }
    }
}

int main() {


    return 0;
}