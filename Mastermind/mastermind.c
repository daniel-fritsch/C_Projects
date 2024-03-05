#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    // setting random number generator
    srand((unsigned int)time(NULL));

    //introduction
    printf("Welcome to Mastermind! \n");


    // user play input query
    char playGame[10];
    printf("Would you like to play: ");
    scanf("%10s", playGame);


    //while loop for playing the game, continues as long as the user wishes to keep playing
    while(playGame[0] == 'y' || playGame[0] == 'Y'){
        // user information statement
        printf("The color options are red, orange, yellow, green, purple and blue. \n");
        printf("Enter your guesses as a series of 4 lowercase letters based on these colors. \n");
        printf("\n");

        // generating random numbers to create the set of 4 colors. 
        int low = 1;
        int high = 6;
        int answerKey[4];
        answerKey[0] = rand() % (high - low + 1) + low;
        answerKey[1] = rand() % (high - low + 1) + low;
        answerKey[2] = rand() % (high - low + 1) + low;
        answerKey[3] = rand() % (high - low + 1) + low;

        // a copy of answer key is made to restore the answerKey after comparisons are made within the game, necessary for smooth running
        char answerKeyCopy[4];
        for (int i = 0; i<4; i++){
            answerKeyCopy[i] = answerKey[i];
        }

        // necessary variables
        int length = 4;
        int rounds = 1;

        //while loop for playing an individual game, consisting of 10 rounds or user victory
        while (rounds <= 10) {
            //chagnes are made to answer key through the comparisons, this restores them to what they should be based on the copy created. 
            for(int i = 0; i<length; i++) {
                answerKey[i] = answerKeyCopy[i];
            }

            // user guess input(if invalid the game ends and the user is given option to play again)
            char guess[5];
            printf("Enter guess number %d (ex: rgrp): ", rounds);
            scanf("%4s", guess);
            

            // I convert the letters to numbers to handle them more comfortably
            int colorSet[4];
            int checkInvalidInput = 0;
            for (int i = 0; i < length; i++) {
                if (guess[i] == 'r') {
                    colorSet[i] = 1;
                } else if (guess[i] == 'o') {
                    colorSet[i] = 2;
                } else if (guess[i] == 'y') {
                    colorSet[i] = 3;
                } else if (guess[i] == 'g') {
                    colorSet[i] = 4;
                } else if (guess[i] == 'b') {
                    colorSet[i] = 5;
                } else if (guess[i] == 'p') {
                    colorSet[i] = 6;
                } else {
                    checkInvalidInput = 1;
                }
            }
            // if the output is not right this is where the program breaks to the outer whle loop
            if (checkInvalidInput == 1) {
                printf("error: invalid input. \n");
                checkInvalidInput = 0;
                break;
            }


            // comparison
            int rightPlace = 0;
            int wrongGuesses[4];
            for (int i = 0; i < length; i++) {
                if (colorSet[i] == answerKey[i]) {
                    rightPlace++;
                    wrongGuesses[i] = 8;
                    answerKey[i] = 9;
                } else {
                    wrongGuesses[i] = colorSet[i];
                }
            }
            int inc = 0;
            for (int i = 0; i < length; i++) {
                for (int j = 0; j < length; j++) {
                    if (answerKey[j] == wrongGuesses[i]) {
                        inc++;
                        answerKey[j] = 9;
                        break;
                    }
                }
            }

            // victory condition announcement 
            printf("Colors in the correct place: %d\n", rightPlace);
            if (rightPlace == 4) {
                printf("You guessed correctly! \n");
                break;
            }
            // user information output on how many colors are correct but in wrong position.
            printf("Collors correct but in wrong position: %d\n", inc);
            printf("\n");

            rounds++;
        }
        //game over
        printf("Game is Over. \n");
        printf("Correct combination: ");
        for (int i = 0; i<length; i++){
            if (answerKeyCopy[i] == 1){
                printf("r");
            } else if(answerKeyCopy[i] == 2){
                printf("o");
            } else if(answerKeyCopy[i] == 3){
                printf("y");
            } else if(answerKeyCopy[i] == 4){
                printf("g");
            } else if(answerKeyCopy[i] == 5){
                printf("b");
            } else if(answerKeyCopy[i] == 6){
                printf("p");
            }
        }
        printf("\n");

        //outer while loop continuation condition
        printf("Would you like to keep playing? : ");
        scanf("%10s", playGame);
    }
    return 0;
}
