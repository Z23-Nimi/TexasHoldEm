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
#include "texasHoldem.h"

const char *rankStrength[] = {"","","2","3","4","5","6","7","8","9","10","J","Q","K","A"};
const char *suitNames[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
int deckLength = 52;
int burnIndex = 0;
float collegeFund = 0;
float betSum = 0;
int randoAction = 0;
int action = 0;
float betAmount = 0;
float botHandStrength = 0.0;

Card deck[52];
Card table[5];
Card burnPile[52];
Card playerHand[2];
Card botHand[2];

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

void reRaiseBot();
void reRaisePlayer();
void playerResponse();

void initalizeDeck() {
    deckLength = 52;
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

int betAction() {
    int first_timer = 0;
    printf ("What would you like to do? (1) Bet, (2) Fold, (3) Check\n");
    scanf ("%d", &action);

    for (int i = 0; i < 1; i++) {
        first_timer = rand() % 119 + 61;
    }

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
    float strong = 0.65;
    float weak = 0.35;
    printf ("\033[1mThe bot is thinking...\n\033[0m\n");
    randoBot(3);
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
                printf ("The bot raises by $%.2f. Your total bet is now $%.2f.\n", betAmount, betSum);
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

        else if (botHandStrength <= weak) {

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

    printf("DEBUG: %f\n", botHandStrength);
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
