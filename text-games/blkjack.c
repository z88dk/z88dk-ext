/*
 * Very simple BLACKJACK (21) game
 * Compile command cc blkjack -fop
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define	DECK	52
#define	SUITES	4

//extern unsigned RAND_SEED;

//#pragma output osca_notimer

/*
 * Keep track of money and bet, play game & adjust total
 */
main()
{
	unsigned money, bet;
	char inbuf[11];

	//get_time(&money, &bet, &RAND_SEED);
	//RAND_SEED += (bet*60) + (money*3600);
	srand (clock());

	money = 1000;
	for(;;) {
		printf("\nYou have %u dollars.\nHow much would you like to bet?", money);
		fgets(inbuf, sizeof(inbuf)-1, stdin);
		// Workaround to a clock driven randomize on z88dk
		//RAND_SEED += clock();
		srand (clock());
		if(!(bet = atoi(inbuf))) {
			printf("No bet - No game!\n");
			return; }
		if(bet > money) {
			printf("You don't have that much money!");
			continue; }
		if(!(money += blackjack() ? bet : -bet)) {
			printf("You went BUST!\n");
			return; } }
}

/*
 * Play a single 21 game
 */	
blackjack()
{
	int deck[DECK], i, j, t, player_total, dealer_total, next_card;

	player_total = dealer_total = next_card = 0;

	/* Deal a deck of cards */
	for(i=0; i < DECK; ++i)
		deck[i] = i;
	for(i=0; i < DECK; ++i) {
		t = deck[i];
		//deck[i] = deck[j = random(DECK)];
		deck[i] = deck[j = (double)rand()*DECK/RAND_MAX];
		deck[j] = t; }

	/* Accept player cards */
	for(;;) {
		printf("Player: %-4u", player_total);
		if(player_total > 21) {
			printf("You LOSE!\n");
			return 0; }
		if(get_input("Another card (Y/N)", "YN") == 'N')
			break;
		show_card(i = deck[next_card++]);
		if(!(i %= (DECK/SUITES))) {
			if(get_input("1 or Ten?", "1T") == 'T')
				i = 9; }
		player_total += (i > 9) ? 10 : i+1; }

	/* Play dealer */
	for(;;) {
		printf("Dealer: %-4u", dealer_total);
		if(dealer_total > 21) {
			printf("You WIN!\n");
			return 1; }
		if(dealer_total >= player_total) {
			printf("Dealer wins!\n");
			return 0; }
		show_card(i = deck[next_card++]);
		if(!(i %= (DECK/SUITES))) {
			t = dealer_total + 10;
			if((t < 22) && ((t >= player_total) || (t < 10)))
				i = 9;
			printf("Dealer choses: %u\n", i+1); }
		dealer_total += (i > 9) ? 10 : i+1; }
}

/*
 * Get input character with prompt, and validate
 */
get_input(prompt, allowed)
	char prompt[], allowed[];
{
	int i;
	char buffer[50], *ptr, c;

	for(;;) {
		printf("%s?", prompt);
		fgets(ptr = buffer, sizeof(buffer)-1, stdin);
		while(isspace(c = toupper(*ptr)))
			++ptr;
		for(i = 0; allowed[i]; ++i)
			if(c == allowed[i])
				return c;
		printf("Huh?\n"); }
}

/*
 * Display a card
 */
show_card(card)
	int card;
{
	static char *suites[] = { "Hearts", "Diamonds", "Clubs", "Spades" };
	static char *cards[] = { "Ace", "Two", "Three", "Four", "Five", "Six",
		"Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King" };

	printf("%s of %s\n", cards[card % (DECK/SUITES)], suites[card / (DECK/SUITES)]);
}
