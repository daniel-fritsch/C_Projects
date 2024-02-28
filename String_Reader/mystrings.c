#include <stdio.h>
// Program for looking through a file and printing readable strings from the ASCII characters 32-126
// useful for extracting human-readable information from a file.

int main(int argc, char *argv[]){
    // check if an argument was provided
    if (argc != 2){
            printf("Error, an argument was not provided");
            return 1;
    }

    // open file and check if valid
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
            printf("Invalid file argument: the file you provided was not valid.");
            return 1;
    }

    // variable declarations for string analysis
    int characterItem; //character to be analyzed (number becuase we are cheching ASCII values)
    int counter = 0; //counter to see how many successive characters we have for string determination
    char stringCheck[4]; //space to put the 4 characters we need in case we determine that we have found a string
    int stringCondition = 0; //this tells the program that we are currently in a string 

    // loop
    while ((characterItem = fgetc(file)) != EOF) {
        if (characterItem >= 32 && 126 >= characterItem) {
            if (stringCondition == 4) {
                printf("%c", characterItem); //if we know that we are in a string we can just print the charac>
            } else {
                stringCheck[counter] = characterItem;
                counter++;
            }
        } else {
            counter = 0;
            if (stringCondition == 4){
                printf("\n"); // if we are in a string we are now at its end so we must print a newline charac>
                stringCondition = 0;
            }
        }

        // if the counter is at 4 that means we have found a string. This if statement makes sure
        // we print the first 4 characters of the string before continuing on to other possible characters.
        if (counter == 4 && stringCondition != 4) {
            for(int i=0; i<counter; i++){
                printf("%c", stringCheck[i]);
            }
            stringCondition = 4; // set stringCondition to 4 so the program knows we are currently in string
        }
    }

    // close file and exit gracefully
    fclose(file);
    return 0;
}
