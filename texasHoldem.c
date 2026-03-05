void logicFlow() {

    initalizeDeck();
    deckshuffle();

    startingFunds();

    dealHoleCards();

    updateBotHandStrength(0);
    betAction();
    botAction();

    flop();
    printTable(3);
    updateBotHandStrength(3);

    betAction();
    botAction();

    turn();
    printTable(4);
    updateBotHandStrength(4);

    betAction();
    botAction();

    river();
    printTable(5);
    updateBotHandStrength(5);

    betAction();
    botAction();

    betSum = 0;
}
