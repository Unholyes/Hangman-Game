#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define Maxwords 5
#define Maxwordslength 20
#define MaxScoreboard 10
#define scoreboard_file "scoreboard.txt"
typedef struct {
    char categoryName[20];
   	char fileName[50]; 
} Category;

typedef struct {
    char name[50];
    int score;
} ScoreboardEntry;
ScoreboardEntry scoreboard[MaxScoreboard];
int scoreboardSize = 0;

void displayMenu();
void loadScoreboard();
void saveScoreToFile();
void startGame();
void saveScore(int score);
void viewScoreboard();
void sortScoreboard();
void printCentered(const char *text, int newLine);
void printHangman(int attempt);
void waitForKeypress();
char* getRandomWord(const char* filename);

int main() {
    int choice;
    loadScoreboard();
    do {
        displayMenu();
        printf("\n");
        printCentered("Enter your choice:",0);
        fflush(stdout);  
        scanf(" %d", &choice);
        getchar(); 

        switch (choice) {
            case 1:
                startGame();
                break;
            case 2:
                viewScoreboard();
                break;
            case 3:
                printCentered("Exiting game. Goodbye!",1);
                break;
            default:
                printCentered("Invalid choice! Please try again.",1);
        }
    } while (choice != 3);

    return 0;
}


void displayMenu() {
    system("cls"); 

    printf("\n");
    printCentered("======================================",1);
    printCentered("  _    _                                         ",1);
    printCentered(" | |  | |                                        ",1);
    printCentered(" | |__| | __ _ _ __   __ _ _ __ ___   __ _ _ __  ",1);
    printCentered(" |  __  |/ _` | '_ \\ / _` | '_ ` _ \\ / _` | '_ \\ ",1);
    printCentered(" | |  | | (_| | | | | (_| | | | | | | (_| | | | |",1);
    printCentered(" |_|  |_|\\__,_|_| |_|\\__, |_| |_| |_|\\__,_|_| |_|",1);
    printCentered("                     __/ |                      ",1);
    printCentered("                    |___/                       ",1);
    printCentered("======================================",1);
    
    printf("\n");
    printCentered("1. Start Game",1);
    printCentered("2. View Scoreboard",1);
    printCentered("3. Exit",1);
    printf("\n");
}

void printCentered(const char *text, int newLine) { 
    int columns = 80;  
    int textLength = strlen(text);
    int padding = (columns - textLength) / 2;
    if (padding < 0) padding = 0;

 
    printf("%*s%s", padding, "", text);  

    
    if (newLine) {
        printf("\n");
    }
}


void startGame() {
    system("cls");


    Category categories[] = { 
    {"Computers", "Categories/computers.txt"},
    {"Food", "Categories/food.txt"},
    {"Countries", "Categories/countries.txt"},
    {"Animals", "Categories/animals.txt"},
    {"Sports", "Categories/sports.txt"},
    {"Movies", "Categories/movies.txt"},
    {"Programming", "Categories/programming.txt"},
    {"Music", "Categories/music.txt"}
};


    int numCategories = sizeof(categories) / sizeof(categories[0]);
    int categoryChoice;

    
    printCentered("Select a Category:",1);
    fflush(stdout);   
    int i;
    for (i = 0; i < numCategories; i++) {
        printf("   %d. %s\n", i + 1, categories[i].categoryName);
    }
    printf("\n");

    printCentered("Enter category number: ",0);
    scanf("%d", &categoryChoice);
    getchar();  

    
    if (categoryChoice < 1 || categoryChoice > numCategories) {
        printCentered("Invalid category choice. Returning to menu...",1);
        return;
    }

   
    Category selectedCategory = categories[categoryChoice - 1];
    char* chosenWord = getRandomWord(categories[categoryChoice - 1].fileName);
if (!chosenWord) {
    printf("Failed to get a word. Exiting...\n");
    return;
}

    
    int wordLength = strlen(chosenWord);
    char guessedWord[wordLength + 1];  
    int attemptsLeft = 6;  
    char guessedLetters[26];  
    int guessedCount = 0;
    
   
    for (i = 0; i < wordLength; i++) {
        guessedWord[i] = (chosenWord[i] == ' ') ? ' ' : '_';  
    }
    guessedWord[wordLength] = '\0'; 

    
    while (attemptsLeft > 0) {
        system("cls");
        printHangman(attemptsLeft); 

        
        printf("Category: ");
        printf(selectedCategory.categoryName);
        printf("\n\n");

       
        printf("\nWord: ");
        for (i = 0; i < wordLength; i++) {
            printf("%c ", guessedWord[i]);
        }
        printf("\n\n");

        
        printf("Guessed Letters: ");
        for (i = 0; i < guessedCount; i++) {
            printf("%c ", guessedLetters[i]);
        }
        printf("\n");

       
        printf("\nEnter a letter: ");
        char guess;
        scanf(" %c", &guess);
        getchar();  

        
        int alreadyGuessed = 0;
        for (i = 0; i < guessedCount; i++) {
            if (guessedLetters[i] == guess) {
                alreadyGuessed = 1;
                break;
            }
        }

        if (alreadyGuessed) {
            printCentered("You already guessed that letter. Try again.",1);
            getchar();
            continue;
        }

        guessedLetters[guessedCount++] = guess;  

        
        int correctGuess = 0;
        for (i = 0; i < wordLength; i++) {
            if (chosenWord[i] == guess) {
                guessedWord[i] = guess;
                correctGuess = 1;
            }
        }

       
        if (!correctGuess) {
            attemptsLeft--;
        }

       
        if (strcmp(guessedWord, chosenWord) == 0) {
            system("cls");
            printCentered("Congratulations! You guessed the word! ",1);

          
            int score = attemptsLeft * 10;
            printf("\nYou scored: %d points!\n", score);
            printf("The word was: %s\n", chosenWord);
            
            free(chosenWord);  
             char playerName[50];
            printf("Enter your name: ");
            fgets(playerName, sizeof(playerName), stdin);
            playerName[strcspn(playerName, "\n")] = '\0'; 

            saveScore(score);
            waitForKeypress();
            return;
        }
    }

    
    system("cls");
    printHangman(0);
    printf("\nThe correct word was: %s\n", chosenWord);
    
    free(chosenWord);  
    waitForKeypress();
}


char* getRandomWord(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return NULL;
    }

    char words[Maxwords][Maxwordslength];
    int count = 0;

    while (count < Maxwords && fgets(words[count], Maxwordslength, file)) {
        // Remove newline character
        words[count][strcspn(words[count], "\n")] = '\0';
        count++;
    }

    fclose(file);

    if (count == 0) {
        printf("No words found in file: %s\n", filename);
        return NULL;
    }

    // Pick a random word
    int randomIndex = rand() % count;
    char* chosenWord = malloc(strlen(words[randomIndex]) + 1);
    strcpy(chosenWord, words[randomIndex]);

    return chosenWord;
}
void printHangman(int attemptsLeft) {
    switch (attemptsLeft) {
        case 6:
            printf("\n    +--------+\n    |/       |\n    |        |\n    |\n    |\n    |\n    |\n    |\n==============\n");
            printf("You have all 6 chances left. Let's go!\n");
            break;
        case 5:
            printf("\n    +--------+\n    |/       |\n    |      (�_�)\n    |        \n    |\n    |\n    |\n    |\n==============\n");
            printf("Uh oh! One mistake. Stay focused!\n");
            break;
        case 4:
            printf("\n    +--------+\n    |/       |\n    |      (�_�)\n    |        ||\n    |        ||\n    |\n    |\n    |\n==============\n");
            printf("Keep going! You can still make it!\n");
            break;
        case 3:
            printf("\n    +--------+\n    |/       |\n    |      (�_�)\n    |       /||\n    |      / ||\n    |\n    |\n    |\n==============\n");
            printf("Don't give up! You still have time!\n");
            break;
        case 2:
            printf("\n    +--------+\n    |/       |\n    |      (�_�)\n    |       /||\\\n    |      / || \\\n    |\n    |\n    |\n==============\n");
            printf("Uh oh! Just two chances left! Think carefully!\n");
            break;
        case 1:
            printf("\n    +--------+\n    |/       |\n    |      (�_�)\n    |       /||\\\n    |      / || \\\n    |       /\n    |      /\n    |\n==============\n");
            printf("Last chance! Believe in yourself!\n");
            break;
        case 0:
            printf("\n    +--------+\n    |/       |\n    |      (x_x)\n    |       /||\\\n    |      / || \\\n    |       / \\\n    |      /   \\\n    |\n==============\n");
            printf("Oh no! Game Over. Better luck next time!\n");
            break;
    }
}

void waitForKeypress() {
    printf("\nPress Enter to return to the main menu...");
    getchar();
}
void saveScore(int score) {
    if (scoreboardSize < MaxScoreboard) {
        printf("Enter your name: ");
        fgets(scoreboard[scoreboardSize].name, sizeof(scoreboard[scoreboardSize].name), stdin);
        scoreboard[scoreboardSize].name[strcspn(scoreboard[scoreboardSize].name, "\n")] = '\0'; // Remove newline
        scoreboard[scoreboardSize].score = score;
        scoreboardSize++;
    } else {
        printf("Scoreboard is full!\n");
    }
    
    sortScoreboard();
    saveScoreToFile();  // Save to file after updating the scoreboard
}


void sortScoreboard() {
    // Sort the scoreboard in descending order of scores
    int i;
    int j;
    for (i = 0; i < scoreboardSize - 1; i++) {
        for (j = i + 1; j < scoreboardSize; j++) {
            if (scoreboard[i].score < scoreboard[j].score) {
                ScoreboardEntry temp = scoreboard[i];
                scoreboard[i] = scoreboard[j];
                scoreboard[j] = temp;
            }
        }
    }
}
void loadScoreboard() {
    FILE* file = fopen(scoreboard_file, "r");
    if (file != NULL) {
        while (fscanf(file, "%49s %d\n", scoreboard[scoreboardSize].name, &scoreboard[scoreboardSize].score) == 2) {
            scoreboardSize++;
        }
        fclose(file);
    }
}
void saveScoreToFile() {
	int i;
    FILE* file = fopen(scoreboard_file, "w");
    if (file != NULL) {
        for ( i = 0; i < scoreboardSize; i++) {
            fprintf(file, "%s %d\n", scoreboard[i].name, scoreboard[i].score);
        }
        fclose(file);
    } else {
        printf("Error saving scoreboard to file!\n");
    }
}

void viewScoreboard() {
	int i;
    system("cls");
    printCentered("Scoreboard", 1);

    // Load the scoreboard if it's not already loaded
    if (scoreboardSize == 0) {
        loadScoreboard(); // Load scores from the file
    }

    if (scoreboardSize == 0) {
        printCentered("No scores yet.", 1);
    } else {
        for ( i = 0; i < scoreboardSize; i++) {
            printf("%d. %s - %d points\n", i + 1, scoreboard[i].name, scoreboard[i].score);
        }
    }
    waitForKeypress();
}





