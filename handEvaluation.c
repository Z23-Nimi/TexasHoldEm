#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "texasHoldem.h"

int compareCards(const void *a, const void *b) {
    const Card *c1 = (const Card *)a;
    const Card *c2 = (const Card *)b;

    return c1->rank - c2->rank;
}

int compareHandValue(HandValue a, HandValue b) {
    if (a.rank != b.rank)
        return a.rank - b.rank;

    for (int i = 0; i < 5; i++) {
        if (a.values[i] != b.values[i])
            return a.values[i] - b.values[i];
    }

    return 0;
}

HandValue evaluate5CardHand(Card hand[5]) {
    HandValue hv;
    memset(hv.values, 0, sizeof(hv.values));

    int rankCount[15] = {0};
    int suitCount[4] = {0};

    for (int i = 0; i < 5; i++) {
        rankCount[hand[i].rank]++;
        suitCount[hand[i].suit]++;
    }

    int pairs[2] = {0};
    int pairCount = 0;
    int three = 0, four = 0;

    for (int i = 14; i >= 2; i--) {
        if (rankCount[i] == 4) four = i;
        else if (rankCount[i] == 3) three = i;
        else if (rankCount[i] == 2) pairs[pairCount++] = i;
    }

    int flush = 0;
    for (int i = 0; i < 4; i++) {
        if (suitCount[i] == 5) flush = 1;
    }

    int straight = 0;
    int highStraight = 0;

    for (int i = 14; i >= 5; i--) {
        if (rankCount[i] && rankCount[i-1] && rankCount[i-2] &&
            rankCount[i-3] && rankCount[i-4]) {
            straight = 1;
            highStraight = i;
            break;
        }
    }

    if (!straight && rankCount[14] && rankCount[2] &&
        rankCount[3] && rankCount[4] && rankCount[5]) {
        straight = 1;
        highStraight = 5;
    }

    int sorted[5], idx = 0;
    for (int i = 14; i >= 2; i--) {
        for (int j = 0; j < rankCount[i]; j++) {
            sorted[idx++] = i;
        }
    }

    if (straight && flush) {
        if (highStraight == 14) hv.rank = Royal_Flush;
        else {
            hv.rank = Straight_Flush;
            hv.values[0] = highStraight;
        }
    }
    else if (four) {
        hv.rank = Four_Of_Kind;
        hv.values[0] = four;

        for (int i = 0; i < 5; i++) {
            if (sorted[i] != four) {
                hv.values[1] = sorted[i];
                break;
            }
        }
    }
    else if (three && pairCount >= 1) {
        hv.rank = Full_House;
        hv.values[0] = three;
        hv.values[1] = pairs[0];
    }
    else if (flush) {
        hv.rank = Flush;
        for (int i = 0; i < 5; i++) {
            hv.values[i] = sorted[i];
        }
    }
    else if (straight) {
        hv.rank = Straight;
        hv.values[0] = highStraight;
    }
    else if (three) {
        hv.rank = Three_Of_Kind;
        hv.values[0] = three;

        int k = 1;
        for (int i = 0; i < 5; i++) {
            if (sorted[i] != three) {
                hv.values[k++] = sorted[i];
            }
        }
    }
    else if (pairCount == 2) {
        hv.rank = Two_Pair;

        int highPair = pairs[0];
        int lowPair = pairs[1];

        hv.values[0] = highPair;
        hv.values[1] = lowPair;

        for (int i = 0; i < 5; i++) {
            if (sorted[i] != highPair && sorted[i] != lowPair) {
                hv.values[2] = sorted[i];
                break;
            }
        }
    }
    else if (pairCount == 1) {
        hv.rank = One_Pair;
        hv.values[0] = pairs[0];

        int k = 1;
        for (int i = 0; i < 5; i++) {
            if (sorted[i] != pairs[0]) {
                hv.values[k++] = sorted[i];
            }
        }
    }
    else {
        hv.rank = High_Card;
        for (int i = 0; i < 5; i++) {
            hv.values[i] = sorted[i];
        }
    }

    return hv;
}

HandValue evaluateBestHand(Card cards[], int totalCards) {
    HandValue best;
    best.rank = High_Card;
    memset(best.values, 0, sizeof(best.values));

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

                        HandValue current = evaluate5CardHand(hand);

                        if (compareHandValue(current, best) > 0)
                            best = current;
                    }
                }
            }
        }
    }
    return best;
}

float evaluateHandStrength(Card hole[2], Card board[], int boardCount, int simulations) {
    int wins = 0, ties = 0;

    for (int sim = 0; sim < simulations; sim++) {

        Card simDeck[52];
        memcpy(simDeck, deck, sizeof(deck));

        int simDeckLen = deckLength;

        Card oppHole[2] = {
            simDeck[rand() % simDeckLen],
            simDeck[rand() % simDeckLen]
        };

        Card fullBoard[5];
        memcpy(fullBoard, board, boardCount * sizeof(Card));

        for (int i = boardCount; i < 5; i++) {
            fullBoard[i] = simDeck[rand() % simDeckLen];
        }

        Card playerCards[7] = {
            hole[0], hole[1],
            fullBoard[0], fullBoard[1], fullBoard[2],
            fullBoard[3], fullBoard[4]
        };

        Card oppCards[7] = {
            oppHole[0], oppHole[1],
            fullBoard[0], fullBoard[1], fullBoard[2],
            fullBoard[3], fullBoard[4]
        };

        HandValue playerBest = evaluateBestHand(playerCards, 7);
        HandValue oppBest = evaluateBestHand(oppCards, 7);

        int result = compareHandValue(playerBest, oppBest);

        if (result > 0) wins++;
        else if (result == 0) ties++;
    }

    return (wins + ties * 0.5f) / simulations;
}
