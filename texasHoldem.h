#ifndef TEXAS_HOLDEM_H
#define TEXAS_HOLDEM_H

#include <stdlib.h>

typedef struct {
    int rank;
    int suit;
} Card;

typedef enum {
    High_Card,
    One_Pair,
    Two_Pair,
    Three_Of_Kind,
    Straight,
    Flush,
    Full_House,
    Four_Of_Kind,
    Straight_Flush,
    Royal_Flush
} handRank;

extern const char *handRankNames[];

typedef struct {
    handRank rank;
    int values[5];
} HandValue;

extern Card deck[52];
extern int deckLength;

int compareCards(const void *a, const void *b);
int compareHandValue(HandValue a, HandValue b);
HandValue evaluate5CardHand(Card hand[5]);
HandValue evaluateBestHand(Card cards[], int totalCards);
float evaluateHandStrength(Card hole[2], Card board[], int boardCount, int simulations);

#endif
