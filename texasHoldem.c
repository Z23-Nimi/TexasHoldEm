
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
#include <unistd.h>
#include <signal.h>

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

typedef struct {
    handRank rank;
    int values[5];
} HandValue;

const char *rankStrength[] = {"","","2","3","4","5","6","7","8","9","10","J","Q","K","A"};
const char *suitNames[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
int deckLength = 52;
int burnIndex = 0;
float collegeFund = 0;
float betSum = 0;
int randoAction = 0;
int action = 0;
float betAmount = 0;
int alarm_count = 0;
float botHandStrength = 0.0;

Card deck[52];
Card table[5];
Card burnPile[52];
Card playerHand[2];
Card botHand[2];

void reRaiseBot();
void reRaisePlayer();
void playerResponse();

void initalizeDeck() {
    int index = 0;
    for (int suit = 0; suit < 4; suit++) {
        for (int rank = 2; rank < 15; rank++) {
            deck[index].rank = rank;
            deck[index].suit = suit;
            index++;
        }
    }
}

void deckshuffle() {
    srand(time(NULL));
    for (int i = 52 - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

void randoBot(int x) {
    for (int i = 0; i < 1; i++) {
        randoAction = rand() % x + 1;
    }
}

Card drawCard() {
    deckLength -= 1;
    return deck[deckLength];
}

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

    // Count multiples
    for (int i = 14; i >= 2; i--) {
        if (rankCount[i] == 4) four = i;
        else if (rankCount[i] == 3) three = i;
        else if (rankCount[i] == 2) pairs[pairCount++] = i;
    }

    // Check flush
    int flush = 0;
    for (int i = 0; i < 4; i++) {
        if (suitCount[i] == 5) flush = 1;
    }

    // Check straight
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

    // Wheel: A-2-3-4-5
    if (!straight && rankCount[14] && rankCount[2] && rankCount[3] &&
        rankCount[4] && rankCount[5]) {
        straight = 1;
        highStraight = 5;
    }

    // Sort descending for kickers
    int sorted[5], idx = 0;
    for (int i = 14; i >= 2; i--) {
        for (int j = 0; j < rankCount[i]; j++) {
            sorted[idx++] = i;
        }
    }

    // ===== HAND RANKING =====

    if (straight && flush) {
        if (highStraight == 14) {
            hv.rank = Royal_Flush;
        } else {
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
    int wins = 0;
    int ties = 0;
    int losses = 0;

    for (int sim = 0; sim < simulations; sim++) {

        Card simDeck[52];
        memcpy(simDeck, deck, sizeof(deck));

        int simDeckLen = deckLength;

        Card oppHole[2];

        oppHole[0] = simDeck[rand() % simDeckLen];
        oppHole[1] = simDeck[rand() % simDeckLen];

        Card fullBoard[5];

        for (int i = 0; i < boardCount; i++)
            fullBoard[i] = board[i];

        int index = 0;

        while (boardCount + index < 5) {
            fullBoard[boardCount + index] = simDeck[rand() % simDeckLen];
            index++;
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

        if (playerBest.rank > oppBest.rank) wins++;
        else if (playerBest.rank < oppBest.rank) losses++;
        else ties++;
    }

    return (wins + ties * 0.5) / simulations;
}

void handle_alarm(int sig) {
    if (sig == SIGALRM) {
        printf("\nTime's up! The bot has called the clock on you.\n");
        printf("A reasonable amount of time has passed, you have 3 minutes or your hand will be considered dead.\n");
        if (alarm_count >= 1) {
            printf("Your hand has been declared dead due to inactivity. Better luck next time!\n");
            betSum = 0;
            exit(0);
        }

        alarm (180);
        alarm_count++;
    }
}

int betAction() {
    int first_timer = 0;
    printf ("What would you like to do? (1) Bet, (2) Fold, (3) Check\n");
    scanf ("%d", &action);

    for (int i = 0; i < 1; i++) {
        first_timer = rand() % 119 + 61;
    }

    alarm (first_timer); 

    switch (action) {
        case 1:
            printf ("How much would you like to bet?\n");
            scanf ("%f", &betAmount);

            if (betAmount > collegeFund) {
                printf ("You don't have enough money to bet that amount. Please try again you horrible father.\n");
            } else {
                collegeFund -= betAmount;
                betSum += betAmount;
                printf ("You bet $%.2f. Your remaining college fund is $%.2f.\n", betAmount, collegeFund);
                printf ("Your total bet is now $%.2f.\n", betSum);
            }
            break;

        case 2:
            printf ("You folded. Better luck next time!\n");
            return 1;
        case 3:
            printf ("You checked. Maybe the bot will bet? Let's find out!\n");
            break;
        default:
            printf ("Invalid action. Please try again.\n");
    }
    //Use alarm to let the bot call clock on a random amount of time after 3 minutes
    random();
    return 0;
}

void reRaiseBot() {

    printf ("\033[1mThe bot is thinking...\n\033[0m\n");
    float strong = 0.80;
    if (botHandStrength >= strong) {

        printf("The bot raises your bet!\n");

        if (betAmount *1.3 > collegeFund) {
            betAmount = collegeFund;
        } 
        else {
        betAmount *= 1.3;
        betSum += betAmount;
        }

        printf ("The bot raises by $%.2f. Your total bet is now $%.2f.\n", betAmount, betSum);
        playerResponse();
    }

    else {
        printf("The bot calls your bet of $%.2f.\n", betAmount);
        betSum += betAmount;
    }
}

void playerResponse() {
    printf("Would you like to call, fold, or raise? (1) Call, (2) Fold, (3) Raise\n");
    scanf("%d", &action);
    switch (action){
    case 1:
        printf("You call.\n");
        betSum += betAmount;
        collegeFund -= betAmount;
        break;
    case 2:
        printf("You fold.\n");
        betSum = 0;
        break;
    case 3:
        reRaisePlayer();
        break;
    default:
        printf("Invalid action. Please try again.\n");
        break;
    }

}

void reRaisePlayer() {
    printf("You raise.\n");
    betSum += betAmount;
    collegeFund -= betAmount;
    while (1) {
        printf("How much would you like to raise?\n");
        scanf("%f", &betAmount);

        if (betAmount > collegeFund) {
            printf ("You don't have enough money to bet that amount. Please try again you horrible father.\n");
        } 
        else {
            collegeFund -= betAmount;
            betSum += betAmount;
            break;
        }
    }

    printf("You raise by $%.2f. Your total bet is now $%.2f. Your remaining college fund is $%.2f.\n", betAmount, betSum, collegeFund);

    if (collegeFund == 0) {
        printf ("\033[1m Damn you must hate your kid.\n\033[0m\n");
    }

    reRaiseBot();
}

void botAction() {
    float strong = 0.70;
    float weak = 0.35;
    printf ("\033[1mThe bot is thinking...\n\033[0m\n");
    randoBot(3);
    sleep(randoAction);
    randoBot(10);
    if (action == 1 || action == 3) {
        switch (randoAction) {
        case 1:
            printf("The bot is sweating profusely.\n");
            break;
        case 2:
            printf("The bot is fidgeting with its hands.\n");
            break;
        case 3:
            printf("The bot is staring intently at the table.\n");
            break;
        case 4:
            printf("The bot looking at your cards.\n");
            break;
        case 5:
            printf("The bot is calculating your defeat.\n");
            break;
        case 6:
            printf("The bot is muttering to itself.\n");
            break;
        case 7:
            printf("The bot is trying to read your tells.\n");
            break;
        case 8:
            printf ("The bot is trying to look under the deck.\n");
            break;
        case 9:
            printf ("The bot is trying to hack into the casino's security system.\n");
            break;
        case 10:
            printf ("The bot is making a house of cards.\n");
        }
        
        if (botHandStrength >= strong) {

            if (action == 1) {
                printf("The bot raises your bet!\n");
                betAmount *= 1.5;
                betSum += betAmount;
            }

            else if (action == 3) {
                randoBot(7);
                betAmount = (collegeFund / randoAction);
                printf("The bot bets $%.2f.\n", betAmount);
                betSum += betAmount;
            }

            playerResponse();
            
        }

        else if (botHandStrength <= weak && betAmount >= (collegeFund / 3)) {

            if (action == 1) {
                printf("The bot folds.\n");
                collegeFund += betSum;
            }

            else if (action == 3) {
                printf("The bot checks.\n");
            }
        }

        else {
            if (betAmount > 0) {
                printf("The bot calls your bet of $%.2f.\n", betAmount);
                betSum += betAmount;
            } else {
                printf("The bot checks.\n");
            }
        }
        printf ("\nThe bot has made a move.\n");
    }
    else {
        printf ("The bot gloats in your defeat.\n");
    }
}

void updateBotHandStrength(int boardCount) {
    botHandStrength = evaluateHandStrength(botHand, table, boardCount, 1000);
}

void printCards(int boardCount) {
    printf("\nYour Cards:\n");

    for (int i = 0; i < 2; i++) {
        printf("%s of %s\n", rankStrength[playerHand[i].rank], suitNames[playerHand[i].suit]);
    }

    printf("\nCommunity Cards:\n");

    for (int i = 0; i < boardCount; i++) {
        printf("%s of %s\n",
            rankStrength[table[i].rank],
            suitNames[table[i].suit]);
    }

    printf("\n");
}

void dealHoleCards() {
    playerHand[0] = drawCard();
    playerHand[1] = drawCard();

    botHand[0] = drawCard();
    botHand[1] = drawCard();

    printf("Your cards: %s of %s, %s of %s\n",
        rankStrength[playerHand[0].rank], suitNames[playerHand[0].suit],
        rankStrength[playerHand[1].rank], suitNames[playerHand[1].suit]);
}

void printHoleCards(Card hand[2]) {
    printf("%s of %s, %s of %s",
        rankStrength[hand[0].rank], suitNames[hand[0].suit],
        rankStrength[hand[1].rank], suitNames[hand[1].suit]);
}

void flop() {
    for (int i = 0; i < 3; i++) {
        table[i] = drawCard();
    }
}

void turn() {
    table[3] = drawCard();
}

void river() {
    table[4] = drawCard();
}

void burnCard() {
    burnPile[burnIndex] = drawCard();
    burnIndex++;
}

void showdown() {

    printf("\n===== SHOWDOWN =====\n");

    printf("Your hand: ");
    printHoleCards(playerHand);
    printf("\n");

    printf("Bot hand: ");
    printHoleCards(botHand);
    printf("\n\n");

    Card playerCards[7] = {
        playerHand[0], playerHand[1],
        table[0], table[1], table[2],
        table[3], table[4]
    };

    Card botCards[7] = {
        botHand[0], botHand[1],
        table[0], table[1], table[2],
        table[3], table[4]
    };

    HandValue playerBest = evaluateBestHand(playerCards, 7);
    HandValue botBest = evaluateBestHand(botCards, 7);

    printf("Your best hand: %s\n", handRankNames[playerBest.rank]);
    printf("Bot best hand: %s\n\n", handRankNames[botBest.rank]);

    int result = compareHandValue(playerBest, botBest);

    if (result > 0) {
        printf("You win the pot of $%.2f!\n", betSum);
        collegeFund += betSum;
    }
    else if (result < 0) {
        printf("The bot wins the pot of $%.2f.\n", betSum);
    }
    else {
        printf("It's a tie! The pot is split.\n");
        collegeFund += betSum / 2;
    }

    printf("Your remaining college fund: $%.2f\n", collegeFund);
}

void startingFunds(){
    printf ("Welcome to the poker game!\n");
    printf ("How much of your son's college fund do you want to bring in?\n");
    printf ("$");
    scanf ("%f", &collegeFund);
    printf ("You have brought in $%.2f of your son's college fund.\n", collegeFund);
}

void logicFlow() {

    initalizeDeck();
    deckshuffle();

    dealHoleCards();

    updateBotHandStrength(0);
    int n = betAction();
    if (n == 1) {
        return;
    }
    botAction();
    betAmount = 0;

    burnCard();
    flop();
    printCards(3);
    updateBotHandStrength(3);

    n = betAction();
    if (n == 1) {
        return;
    }
    botAction();
    betAmount = 0;

    burnCard();
    turn();
    printCards(4);
    updateBotHandStrength(4);

    n = betAction();
    if (n == 1) {
        return;
    }
    botAction();
    betAmount = 0;

    burnCard();
    river();
    printCards(5);
    updateBotHandStrength(5);

    showdown();

    betSum = 0;
}

int main() {
    startingFunds();
    int choice;
    while (1) {
        logicFlow();
        printf("Would you like to:\n1. Play Again\n2. End\n");
        scanf("%d", &choice);
        if (choice == 2) {
            return 0;
        }
    }
}

// Kickers, re raising
