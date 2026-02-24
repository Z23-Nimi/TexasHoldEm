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

const char *handRankNames[] = {
    "High Card",
    "One Pair",
    "Two Pair",
    "Three of a Kind",
    "Straight",
    "Flush",
    "Full House",
    "Four of a Kind",
    "Straight Flush",
    "Royal Flush"
};

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

int compareCards(const void *a, const void *b) {
    const Card *c1 = (const Card *)a;
    const Card *c2 = (const Card *)b;

    return c1->rank - c2->rank;
}

handRank evaluate5CardHand(Card hand[5]) {
    int rankCount[15] = {0};
    int suitCount[4] = {0};
    
    for (int i = 0; i < 5; i++) {
        rankCount[hand[i].rank]++;
        suitCount[hand[i].suit]++;
    }

    int pairs = 0, three = 0, four = 0;
    int flush = 0, straight = 0, royal = 0;

    // Check multiples
    for (int i = 2; i <= 14; i++) {
        if (rankCount[i] == 4) four++;
        if (rankCount[i] == 3) three++;
        if (rankCount[i] == 2) pairs++;
    }

    // Check flush
    for (int i = 0; i < 4; i++) {
        if (suitCount[i] == 5) flush = 1;
    }

    // Check straight
    for (int i = 2; i <= 10; i++) {
        if (rankCount[i] &&
            rankCount[i+1] &&
            rankCount[i+2] &&
            rankCount[i+3] &&
            rankCount[i+4]) {
            straight = 1;
            if (i == 10) {
                royal = 1;
            }
        }
    }

    // Special case: A-2-3-4-5
    if (rankCount[14] && rankCount[2] && rankCount[3] &&
        rankCount[4] && rankCount[5]) {
        straight = 1;
    }

    if (straight && flush && royal) return Royal_Flush;
    if (straight && flush) return Straight_Flush;
    if (four) return Four_Of_Kind;
    if (three && pairs) return Full_House;
    if (flush) return Flush;
    if (straight) return Straight;
    if (three) return Three_Of_Kind;
    if (pairs == 2) return Two_Pair;
    if (pairs == 1) return One_Pair;
    return High_Card;
}

handRank evaluateBestHand(Card cards[], int totalCards) {
    handRank best = High_Card;

    for (int i = 0; i < totalCards - 4; i++) {
        for (int j = i+1; j < totalCards - 3; j++) {
            for (int k = j+1; k < totalCards - 2; k++) {
                for (int l = k+1; l < totalCards - 1; l++) {
                    for (int m = l+1; m < totalCards; m++) {

                        Card hand[5] = {
                            cards[i], cards[j],
                            cards[k], cards[l], cards[m]
                        };

                        qsort(hand, 5, sizeof(Card), compareCards);

                        handRank current = evaluate5CardHand(hand);

                        if (current > best)
                            best = current;
                    }
                }
            }
        }
    }
    return best;
}

void betAction(float collegeFund) {
    int action;
    printf ("What would you like to do? (1) Bet, (2) Fold, (3) Check\n");
    scanf ("%d", &action);

    switch (action) {
        case 1:
            printf ("How much would you like to bet?\n");
            float betAmount;
            scanf ("%f", &betAmount);
            if (betAmount > collegeFund) {
                printf ("You don't have enough money to bet that amount. Please try again you horrible father.\n");
            } else {
                collegeFund -= betAmount;
                printf ("You bet $%.2f. Your remaining college fund is $%.2f.\n", betAmount, collegeFund);
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
    float collegeFund;
    int action;

    printf ("Welcome to the poker game!\n");
    printf ("How much of your son's college fund do you want to bring in?\n");
    printf ("$");
    scanf ("%f", &collegeFund);
    printf ("You have brought in $%.2f of your son's college fund.\n", collegeFund);

    //flop here

    printf ("The flop is: 10 of hearts, 7 of clubs, and 2 of diamonds.\n"); //Will be replaced by random stuff later
    betAction(collegeFund);
    
    //After finishing bet, if bot has best possible hand on board, say "I have the nuts!"

    return 0;
}
