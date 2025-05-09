#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define Maxwords 5
#define Maxwordslength 20
#define MaxScoreboard 100
#define scoreboard_file "scoreboard.txt"

typedef struct {
    char categoryName[20];
    char fileName[50]; 
} Category;

typedef struct {
    char name[50];
    int score;
    char category[20];  
} ScoreboardEntry;


typedef struct ScoreboardNode {
    ScoreboardEntry entry;
    struct ScoreboardNode* next;
} ScoreboardNode;


typedef struct CategoryNode {
    Category category;
    struct CategoryNode* next;
} CategoryNode;


typedef struct UsedWordNode {
    char word[Maxwordslength];
    struct UsedWordNode* next;
} UsedWordNode;

ScoreboardNode* scoreboardHead = NULL;
CategoryNode* categoryHead = NULL;
UsedWordNode* usedWordsHead = NULL;

void displayMenu();
void loadScoreboard();
void saveScoreToFile();
void startGame();
void saveScore(int score, const char* category);
void viewScoreboard();
void sortScoreboard();
void printCentered(const char *text, int newLine);
void printHangman(int attempt);
void waitForKeypress();
char* getRandomWord(const char* filename);
void freeScoreboard();
void freeCategories();
void initCategories();
void addCategory(const char* name, const char* filename);
char* getRandomWordWithLength(const char* filename, int minLength, int maxLength);
void addUsedWord(const char* word);
int isWordUsed(const char* word);
void freeUsedWords();
void cleanup();


void insertScore(ScoreboardEntry newEntry) {
    ScoreboardNode* newNode = (ScoreboardNode*)malloc(sizeof(ScoreboardNode));
    newNode->entry = newEntry;
    newNode->next = NULL;

    if (scoreboardHead == NULL) {
        scoreboardHead = newNode;
        return;
    }

    ScoreboardNode* current = scoreboardHead;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newNode;
}

void sortScoreboard() {
    if (scoreboardHead == NULL || scoreboardHead->next == NULL) {
        return;
    }

    ScoreboardNode* current = scoreboardHead;
    ScoreboardNode* index = NULL;
    ScoreboardEntry temp;

    while (current != NULL) {
        index = current->next;
        while (index != NULL) {
            if (current->entry.score < index->entry.score) {
                temp = current->entry;
                current->entry = index->entry;
                index->entry = temp;
            }
            index = index->next;
        }
        current = current->next;
    }
}

void loadScoreboard() {
    FILE* file = fopen(scoreboard_file, "r");
    if (file != NULL) {
        ScoreboardEntry entry;
        char line[256];  
        
        freeScoreboard();
        
        while (fgets(line, sizeof(line), file) != NULL) {
            if (sscanf(line, "%49s %d %19s", entry.name, &entry.score, entry.category) == 3) {
                insertScore(entry);
            }
        }
        fclose(file);
    }
}

void saveScoreToFile() {
    FILE* file = fopen(scoreboard_file, "w");
    if (file != NULL) {
        ScoreboardNode* current = scoreboardHead;
        while (current != NULL) {
            // Format: name score category
            fprintf(file, "%s %d %s\n", 
                current->entry.name, 
                current->entry.score, 
                current->entry.category);
            current = current->next;
        }
        fclose(file);
    } else {
        printf("Error saving scoreboard to file!\n");
    }
}

void viewScoreboard() {
    system("cls");
    printCentered("Scoreboard", 1);
    
    printCentered("Select a Category:", 1);
    int categoryNumber = 1;
    CategoryNode* current = categoryHead;
    while (current != NULL) {
        printf("   %d. %s\n", categoryNumber++, current->category.categoryName);
        current = current->next;
    }
    printf("   %d. All Categories\n", categoryNumber);
    printf("\n");

    printCentered("Enter category number: ", 0);
    int categoryChoice;
    scanf("%d", &categoryChoice);
    getchar();

    system("cls");
    if (categoryChoice == categoryNumber) {
        printCentered("All Categories Scoreboard", 1);
        printf("\n");
        
        current = categoryHead;
        while (current != NULL) {
            printf("Category: %s\n", current->category.categoryName);
            printf("-------------------\n");
            
            ScoreboardNode* scoreNode = scoreboardHead;
            int count = 0;
            while (scoreNode != NULL && count < 10) {
                if (strcmp(scoreNode->entry.category, current->category.categoryName) == 0) {
                    printf("%d. %s - %d points\n", count + 1, scoreNode->entry.name, scoreNode->entry.score);
                    count++;
                }
                scoreNode = scoreNode->next;
            }
            if (count == 0) {
                printf("No scores yet.\n");
            }
            printf("\n");
            current = current->next;
        }
    } else {
        current = categoryHead;
        int count = 1;
        while (current != NULL && count < categoryChoice) {
            current = current->next;
            count++;
        }

        if (current == NULL) {
            printCentered("Invalid category choice.", 1);
        } else {
            printf("Category: %s\n", current->category.categoryName);
            printf("-------------------\n");
            
            ScoreboardNode* scoreNode = scoreboardHead;
            int count = 0;
            while (scoreNode != NULL && count < 10) {
                if (strcmp(scoreNode->entry.category, current->category.categoryName) == 0) {
                    printf("%d. %s - %d points\n", count + 1, scoreNode->entry.name, scoreNode->entry.score);
                    count++;
                }
                scoreNode = scoreNode->next;
            }
            if (count == 0) {
                printf("No scores yet.\n");
            }
        }
    }
    waitForKeypress();
}

void freeScoreboard() {
    ScoreboardNode* current = scoreboardHead;
    while (current != NULL) {
        ScoreboardNode* temp = current;
        current = current->next;
        free(temp);
    }
    scoreboardHead = NULL;
}

void saveScore(int score, const char* category) {
    ScoreboardEntry newEntry;
    printf("\nEnter your name: ");
    fgets(newEntry.name, sizeof(newEntry.name), stdin);
    newEntry.name[strcspn(newEntry.name, "\n")] = '\0';  
    
    int i;
    for (i = 0; newEntry.name[i] != '\0'; i++) {
        if (newEntry.name[i] == ' ') {
            newEntry.name[i] = '_';  
        }
    }
    
    newEntry.score = score;
    strcpy(newEntry.category, category);
    
    loadScoreboard();
    
    insertScore(newEntry);
    sortScoreboard();
    saveScoreToFile();
}

void cleanup() {
    freeScoreboard();
    freeCategories();
    freeUsedWords();
}

int main() {
    
    srand(GetTickCount());
    int choice;
    loadScoreboard();
    
    if (categoryHead == NULL) {
        initCategories();
    }
    
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

    cleanup();
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

void initCategories() {
    Category defaultCategories[] = {
        {"Computers", "Categories/computers.txt"},
        {"Food", "Categories/food.txt"},
        {"Countries", "Categories/countries.txt"},
        {"Animals", "Categories/animals.txt"},
        {"Sports", "Categories/sports.txt"},
        {"Movies", "Categories/movies.txt"},
        {"Programming", "Categories/programming.txt"},
        {"Music", "Categories/music.txt"}
    };

    int numCategories = sizeof(defaultCategories) / sizeof(defaultCategories[0]);
    
    int i;
    for (i = 0; i < numCategories; i++) {
        CategoryNode* newNode = (CategoryNode*)malloc(sizeof(CategoryNode));
        newNode->category = defaultCategories[i];
        newNode->next = categoryHead;
        categoryHead = newNode;
    }
}

void addCategory(const char* name, const char* filename) {
    CategoryNode* newNode = (CategoryNode*)malloc(sizeof(CategoryNode));
    strcpy(newNode->category.categoryName, name);
    strcpy(newNode->category.fileName, filename);
    newNode->next = categoryHead;
    categoryHead = newNode;
}

void freeCategories() {
    CategoryNode* current = categoryHead;
    while (current != NULL) {
        CategoryNode* temp = current;
        current = current->next;
        free(temp);
    }
    categoryHead = NULL;
}

int isVowel(char c) {
    c = tolower(c);
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}

void startGame() {
    system("cls");
    freeUsedWords();
    
    int totalScore = 0;
    int gamesWon = 0;
    int currentGame = 1;
    const int maxGames = 5;
    const int baseScoreMultiplier = 10;

    if (categoryHead == NULL) {
        initCategories();
    }

    system("cls");
    printf("Game %d of %d\n", currentGame, maxGames);
    printf("Current Score: %d\n\n", totalScore);

    printCentered("Select a Category:", 1);
    int categoryNumber = 1;
    CategoryNode* current = categoryHead;
    while (current != NULL) {
        printf("   %d. %s\n", categoryNumber++, current->category.categoryName);
        current = current->next;
    }
    printf("   %d. Back to Main Menu\n", categoryNumber);
    printf("\n");

    printCentered("Enter category number: ", 0);
    int categoryChoice;
    scanf("%d", &categoryChoice);
    getchar();

    if (categoryChoice == categoryNumber) {
        return;  
    }

    current = categoryHead;
    int count = 1;
    while (current != NULL && count < categoryChoice) {
        current = current->next;
        count++;
    }

    if (current == NULL) {
        printCentered("Invalid category choice. Returning to menu...", 1);
        return;
    }

    Category selectedCategory = current->category;

    while (gamesWon < maxGames) {
        system("cls");
        printf("Game %d of %d\n", currentGame, maxGames);
        printf("Current Score: %d\n\n", totalScore);
        printf("Category: %s\n\n", selectedCategory.categoryName);

        int minLength, maxLength;
        if (currentGame <= 2) {
            printf("Difficulty: Easy (1-5 letters)\n");
            minLength = 1;
            maxLength = 5;
        } else if (currentGame <= 4) {
            printf("Difficulty: Medium (6-8 letters)\n");
            minLength = 6;
            maxLength = 8;
        } else {
            printf("Difficulty: Hard (9+ letters)\n");
            minLength = 9;
            maxLength = 20;  
        }
        printf("\n");

        char* chosenWord = getRandomWordWithLength(selectedCategory.fileName, minLength, maxLength);
        if (!chosenWord) {
            printf("Failed to get a word. Exiting...\n");
            return;
        }

        int wordLength = strlen(chosenWord);
        char guessedWord[wordLength + 1];  
        int attemptsLeft = 6;
        char guessedLetters[26];
        int guessedCount = 0;
        int i;
        
        for (i = 0; i < wordLength; i++) {
            guessedWord[i] = (chosenWord[i] == ' ') ? ' ' : '_';
        }
        guessedWord[wordLength] = '\0';

        char availableVowels[] = "aeiou";
        int vowelsLeft = 5;

        while (attemptsLeft > 0) {
            system("cls");
            printf("Game %d of %d\n", currentGame, maxGames);
            printf("Current Score: %d\n\n", totalScore);
            printf("Category: %s\n", selectedCategory.categoryName);
            printHangman(attemptsLeft); 
            
            // Display available vowels
            printf("\nAvailable Vowels: ");
            for (i = 0; i < 5; i++) {
                if (availableVowels[i] != ' ') {
                    printf("%c ", availableVowels[i]);
                }
            }
            printf("\n\n");

            printf("Word: ");
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
            while (getchar() != '\n');  

            int alreadyGuessed = 0;
            for (i = 0; i < guessedCount; i++) {
                if (guessedLetters[i] == guess) {
                    alreadyGuessed = 1;
                    break;
                }
            }

            if (alreadyGuessed) {
                printCentered("You already guessed that letter. Try again.", 1);
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

            if (isVowel(guess)) {
                for (i = 0; i < 5; i++) {
                    if (availableVowels[i] == guess) {
                        availableVowels[i] = ' ';
                        vowelsLeft--;
                        break;
                    }
                }
                continue;
            }

            if (!correctGuess) {
                attemptsLeft--;
            }

            if (strcmp(guessedWord, chosenWord) == 0) {
                system("cls");
                int difficultyMultiplier;
                if (currentGame <= 2) {
                    difficultyMultiplier = 1;  // Easy
                } else if (currentGame <= 4) {
                    difficultyMultiplier = 2;  // Medium
                } else {
                    difficultyMultiplier = 3;  // Hard
                }
                
                int gameScore = attemptsLeft * baseScoreMultiplier * currentGame * difficultyMultiplier;
                totalScore += gameScore;
                gamesWon++;
                
                printCentered("Congratulations! You guessed the word! ", 1);
                printf("\nYou scored: %d points in this game!\n", gameScore);
                printf("Total Score: %d\n", totalScore);
                printf("The word was: %s\n", chosenWord);

                free(chosenWord);
                
                if (gamesWon < maxGames) {
                    printf("\nPress Enter to continue to the next game...");
                    getchar();
                    currentGame++;
                } else {
                    saveScore(totalScore, selectedCategory.categoryName);
                }
                break;
            }
        }

        if (attemptsLeft == 0) {
            system("cls");
            printHangman(0);
            printf("\nGame Over! You lost game %d.\n", currentGame);
            printf("The correct word was: %s\n", chosenWord);
            printf("Your final score: %d\n", totalScore);
            free(chosenWord);
            
            saveScore(totalScore, selectedCategory.categoryName);
            
            waitForKeypress();
            return;
        }
    }

    system("cls");
    printCentered("Congratulations! You completed all 5 games!", 1);
    printf("\nYour final score: %d\n", totalScore);
    
    waitForKeypress();
}

char* getRandomWord(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return NULL;
    }

    int count = 0;
    char buffer[Maxwordslength];
    while (fgets(buffer, Maxwordslength, file) != NULL) {
        count++;
    }
    
    if (count == 0) {
        printf("No words found in file: %s\n", filename);
        fclose(file);
        return NULL;
    }

    int randomIndex = rand() % count;
    

    rewind(file);
 
    int currentIndex = 0;
    char* chosenWord = NULL;
    while (fgets(buffer, Maxwordslength, file) != NULL) {
        if (currentIndex == randomIndex) {
            buffer[strcspn(buffer, "\n")] = '\0'; 
            chosenWord = malloc(strlen(buffer) + 1);
            strcpy(chosenWord, buffer);
            break;
        }
        currentIndex++;
    }
    
    fclose(file);
    return chosenWord;
}

void printHangman(int attemptsLeft) {
    switch (attemptsLeft) {
         case 6:
            printf("\n    +--------+\n    |/       |\n    |        |\n    |\n    |\n    |\n    |\n    |\n==============\n");
            printf("You have all 6 chances left. Let's go!\n");
            break;
        case 5:
            printf("\n    +--------+\n    |/       |\n    |      (•_•)\n    |        \n    |\n    |\n    |\n    |\n==============\n");
            printf("Uh oh! One mistake. Stay focused!\n");
            break;
        case 4:
            printf("\n    +--------+\n    |/       |\n    |      (•_•)\n    |        ||\n    |        ||\n    |\n    |\n    |\n==============\n");
            printf("Keep going! You can still make it!\n");
            break;
        case 3:
            printf("\n    +--------+\n    |/       |\n    |      (•_•)\n    |       /||\n    |      / ||\n    |\n    |\n    |\n==============\n");
            printf("Don't give up! You still have time!\n");
            break;
        case 2:
            printf("\n    +--------+\n    |/       |\n    |      (•_•)\n    |       /||\\\n    |      / || \\\n    |\n    |\n    |\n==============\n");
            printf("Uh oh! Just two chances left! Think carefully!\n");
            break;
        case 1:
            printf("\n    +--------+\n    |/       |\n    |      (•_•)\n    |       /||\\\n    |      / || \\\n    |       /\n    |      /\n    |\n==============\n");
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

char* getRandomWordWithLength(const char* filename, int minLength, int maxLength) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return NULL;
    }

    int count = 0;
    char buffer[Maxwordslength];
    while (fgets(buffer, Maxwordslength, file) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        int len = strlen(buffer);
        if (len >= minLength && len <= maxLength && !isWordUsed(buffer)) {
            count++;
        }
    }
    
    if (count == 0) {
        printf("No unused words found in file matching length criteria: %s\n", filename);
        fclose(file);
        return NULL;
    }

    rewind(file);
    int randomIndex = rand() % count;
    int currentIndex = 0;
    char* chosenWord = NULL;

    while (fgets(buffer, Maxwordslength, file) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        int len = strlen(buffer);
        if (len >= minLength && len <= maxLength && !isWordUsed(buffer)) {
            if (currentIndex == randomIndex) {
                chosenWord = malloc(strlen(buffer) + 1);
                strcpy(chosenWord, buffer);
                addUsedWord(chosenWord);  
                break;
            }
            currentIndex++;
        }
    }
    
    fclose(file);
    return chosenWord;
}

void addUsedWord(const char* word) {
    UsedWordNode* newNode = (UsedWordNode*)malloc(sizeof(UsedWordNode));
    strcpy(newNode->word, word);
    newNode->next = usedWordsHead;
    usedWordsHead = newNode;
}

int isWordUsed(const char* word) {
    UsedWordNode* current = usedWordsHead;
    while (current != NULL) {
        if (strcmp(current->word, word) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void freeUsedWords() {
    UsedWordNode* current = usedWordsHead;
    while (current != NULL) {
        UsedWordNode* temp = current;
        current = current->next;
        free(temp);
    }
    usedWordsHead = NULL;
}




