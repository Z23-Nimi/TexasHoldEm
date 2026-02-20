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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void betAction(float *collegeFund) {
    int action;
    printf ("What would you like to do? (1) Bet, (2) Fold, (3) Check\n");
    scanf ("%d", &action);

    switch (action) {
        case 1:
            printf ("How much would you like to bet?\n");
            float betAmount;
            scanf ("%f", &betAmount);
            if (betAmount > *collegeFund) {
                printf ("You don't have enough money to bet that amount. Please try again you horrible father.\n");
            } else {
                *collegeFund -= betAmount;
                printf ("You bet $%.2f. Your remaining college fund is $%.2f.\n", betAmount, *collegeFund);
            }

            //Bot action here, raise call or fold

            break;
        case 2:
            printf ("You folded. Better luck next time!\n");
            break;
        case 3:
            printf ("You checked. Maybe the bot will bet? Let's find out!\n");

            //Bot action here (raise or check)
            //Bot notes: make sure to shit talk on 7 2 or nuts

            break;
        default:
            printf ("Invalid action. Please try again.\n");
    }
}

int main() {


    return 0;
}
