
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

Card deck[52];
Card table[5];
Card burnPile[52];

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

void betAction() {

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
            float betAmount;
            scanf ("%f", &betAmount);

            if (betAmount > collegeFund) {
                printf ("You don't have enough money to bet that amount. Please try again you horrible father.\n");
            } else {
                collegeFund -= betAmount;
                betSum += betAmount;
                printf ("You bet $%.2f. Your remaining college fund is $%.2f.\n", betAmount, collegeFund);
                printf ("Your total bet is now $%.2f.\n", betSum);
                break;
            }

        case 2:
            printf ("You folded. Better luck next time!\n");
            exit(0);
        case 3:
            printf ("You checked. Maybe the bot will bet? Let's find out!\n");
            break;
        default:
            printf ("Invalid action. Please try again.\n");
    }
    //Use alarm to let the bot call clock on a random amount of time after 3 minutes
    random();
}

void botAction() {
    printf ("\033[1mThe bot is thinking...\n\033[0m\n");
    randoBot(3);
    sleep(randoAction);
    randoBot(10);
    if (action == 1 || action == 3) {
        switch (randoAction) {
        case 1:
            printf("The bot is sweating profusely");
            break;
        case 2:
            printf("The bot is fidgeting with its hands");
            break;
        case 3:
            printf("The bot is staring intently at the table");
            break;
        case 4:
            printf("The bot looking at your cards");
            break;
        case 5:
            printf("The bot is calculating your defeat");
            break;
        case 6:
            printf("The bot is muttering to itself");
            break;
        case 7:
            printf("The bot is trying to read your tells");
            break;
        case 8:
            printf ("The bot is trying to look under the deck");
        case 9:
            printf ("The bot is trying to hack into the casino's security system");
            break;
        case 10:
            printf ("The bot is making a house of cards");
        }
        
        botActionDos();
        printf ("\nThe bot has made a move.\n");
    }
    else {
        printf ("The bot gloats in your defeat");
    }
}

void botActionDos() {
    //hand strenght high enough, bet or raise
    if (botHandStrength >= x) {
        if (action == 1) {
        raise;
        }
        else if (action == 3) {
            randoBot(7);
            betAmount = (collegeFund / randoAction);
            printf("The bot bets $%.2f.\n", betAmount);
            betSum += betAmount;
        }
    }
    else if (botHandStrength <= x && betAmount >= (collegeFund / 3)) {
        if (action == 1) {
        printf("R.I.P.");
        printf("The bot folds.");
        collegeFund += betSum;
        }
        else if (action == 3) {
        printf("The bot checks.");
        }
    }
    else {
        printf("The bot calls your bet of $%.2f.\n", betAmount);
        betSum += betAmount;
    }

}

void flop() {
    for (int i = 0; i < 3; i++) {
        table[i] = drawCard();
    }
}

void river() {
    for (int i = 3; i < 5; i++) {
        table[i] = drawCard();
    }
}

void burnCard() {
    burnPile[burnIndex] = drawCard();
    burnIndex++;
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
    startingFunds();
    betAction();
    botAction();
    flop();
    betAction();
    botAction();
    turn();
    betAction();
    botAction();
    river();
    betAction(); 
    botAction();
    //Showdown
    betSum = 0;

}

int main() {
    logicFlow();
    return 0;
}

//Need to identify bot hand and strength
//Need to identify player hand and strength
//Bot cannot do things until hand strenght has been identified
//Showdown
// When folding (currently exit command), need to reset program with new college fund. (maybe add finction that specifically says "add funds?" to make it feel more continuouss)
