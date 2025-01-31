#include <dirent.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdio.h>  // Include for file operations

// Define Constants
#define MAX_INPUT_LENGTH 80
#define MAX_FIELDS 5
#define FORM_WIDTH 50

// Function Declarations
void input(char* title, char* value);
void random_colors();
short int menu(char* title, int opNum, char* list[]);
char** form(char* data[], int inputNum, int pos);
void draw_border(int height, int width);
int validEmail(const char *email);
int validPass(char *password);
int directory_exists(const char *path);
char* generateRandomPassword();
char** split(char str[], char character, int* count);
char** readFromFile(const char *filePath, int *lineCount);
void writeToFile(const char *filePath, const char *data);
// Check if directory exists
int directory_exists(const char *path) {
    DIR *dir = opendir(path);
    if (dir) {
        closedir(dir);
        return 1;  // Directory exists
    }
    return 0;  // Directory does not exist
}
void writeToFile(const char *filePath, const char *data) {
    FILE *file = fopen(filePath, "w"); 
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }
    fprintf(file, "%s", data);
    fclose(file);
}
char** readFromFile(const char *filePath, int *lineCount) {
    FILE *file = fopen(filePath, "r");  
    char **lines = malloc(sizeof(char*) * 10);  
    int count = 0;  
    size_t bufferSize = 100;  
    char *buffer = malloc(bufferSize);  
    while (fgets(buffer, bufferSize, file)) {
        if (count >= bufferSize) {
            bufferSize *= 2;  
            buffer = realloc(buffer, bufferSize);
        }
        lines[count] = malloc(strlen(buffer) + 1);
        strcpy(lines[count], buffer);  
        count++;     
        if (count % 10 == 0)
            lines = realloc(lines, sizeof(char*) * (count + 10));
    }
    fclose(file);
    *lineCount = count;
    free(buffer);
    return lines;  
}
char** split(char str[], char character, int* count) {  
    char *myPtr = strtok(str, (char[]){character, '\0'});  
    char **result = malloc(1000 * sizeof(char*));   
    int x = 0;  
    while (myPtr != NULL) {  
        result[x] = malloc(strlen(myPtr) + 1);   
        strcpy(result[x], myPtr);  
        myPtr = strtok(NULL, (char[]){character, '\0'});  
        x++;  
    }  
    *count = x;   
    return result;   
} 
void clearFun(int x1, int y1, int x2, int y2) {
    for (int i = x1; i <= x2; i++) {
        for (int j = y1; j <= y2; j++) {
            move(i, j);  // Move cursor to position (i, j)
            printw(" ");  // Print a space to clear the area
        }
    }
}

int validEmail(const char *email) {
    int at_found = 0;
    int dot_found = 0;
    int at_pos = -1;
    int dot_pos = -1;
    if (strlen(email) < 5) {
        return 0;
    }
    for (int i = 0; email[i] != '\0'; i++) {
        if (email[i] == '@') {
            at_found = 1;
            at_pos = i;
        }
        if (at_found && email[i] == '.') {
            dot_found = 1;
            dot_pos = i;
        }
    }
    return (at_found && dot_found && at_pos < dot_pos);
}

// Password validation
int validPass(char *password) {
    if (strlen(password) < 7)
        return 0;
    int t=0;
    int check[3];
    check[0]=0; check[1]=0; check[2]=0;
    for (int i = 0; i < strlen(password); i++) {
        if (check[0]== 0 && password[i]>='a' && password[i]<='z') {
            ++t;
            check[0] = 1;
        }
        if (check[1]== 0 && password[i]>='A' && password[i]<='Z') {
            ++t;
            check[1] = 1;
        }
        if (check[2]== 0 && password[i]>='0' && password[i]<='9') {
            ++t;
            check[2] = 1;
        }
    }
    return (t == 3);
}

// Generate a random password
char* generateRandomPassword() {
    srand(time(0));
    int length = rand() % 6 + 10; 
    char *password = malloc(11);
    const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char digits[] = "0123456789";
    const char all_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    
    password[0] = upper[rand() % strlen(upper)];
    password[1] = upper[rand() % strlen(upper)] + 32;  // Add lowercase letter
    password[2] = digits[rand() % strlen(digits)];
    
    for (int i = 3; i < length; i++) {
        password[i] = all_chars[rand() % strlen(all_chars)];
    }

    // Shuffle the password
    for (int i = 0; i < length; i++) {
        int j = rand() % length;
        char temp = password[i];
        password[i] = password[j];
        password[j] = temp;
    }
    
    password[length] = '\0';
    return password;
}

// Set random colors and color pairs
void random_colors(int start,int domain)
{
    for (int i = 1; i <= 7; ++i)
    {
        int r = rand() % (start+1) + domain;
        int g = rand() % (start+1) + domain;
        int b = rand() % (start+1) + domain;
        init_color(i, r, g, b);
        init_pair(i, i, COLOR_BLACK);
    }
}

// Menu function for displaying options
short int menu(char* title, int opNum, char* list[]) {
    int choice = 0;
    while (1) {
        clear();
        move(0, COLS / 2 - 35);
        printw("+--------------------------------------------------+\n");
        move(1, COLS / 2 - 35);
        printw("|                   %-31s|\n", title);  // Title centered
        move(2, COLS / 2 - 35);
        printw("+--------------------------------------------------+\n");
        move(3, COLS / 2 - 35);
        for (int i = 0; i < opNum; ++i) {
            if (i == choice) {
                attron(A_REVERSE); // Highlight selected option
            }
            move(i + 4, COLS / 2 - 35);
            printw("|                %d- %-31s|\n", i + 1, list[i]);
            if (i == choice) {
                attroff(A_REVERSE);
            }
        }
        move(opNum + 4, COLS / 2 - 35);
        printw("+--------------------------------------------------+\n");

        int ch = getch();
        if (ch == KEY_UP) {
            choice = (choice == 0) ? opNum - 1 : choice - 1;
        } else if (ch == KEY_DOWN) {
            choice = (choice == opNum - 1) ? 0 : choice + 1;
        } else if (ch == 27) {  // Escape key
            return -1;
        } else if (ch == 10) {  // Enter key
            return choice;
        }
    }
}

// Draw form border
void draw_border(int height, int width) {
    for (int i = 0; i < height; ++i) {
        move(i, 0);
        if (i == 0 || i == height - 1) {
            for (int j = 0; j < width; ++j) printw("_");
        } else {
            printw("\\");
            for (int j = 1; j < width - 1; ++j) printw(" ");
            printw("\\");
        }
    }
}
int strToInt(char* a){
    if (strcmp(a,"0")==0)
    {
        return 0;
    }
    else if (strcmp(a,"1")==0)
    {
        return 1;
    }
    else if (strcmp(a,"2")==0)
    {
        return 2;
    }
    
}
int main() {
    srand(time(0));
    initscr();  // Initialize the ncurses environment
    start_color();  // Enable color functionality
    random_colors(500,500);  // Initialize random colors

    keypad(stdscr, TRUE);  // Enable keypad for arrow keys
    noecho();  // Disable echoing of input characters
    curs_set(0);  // Hide the cursor

    first:char* levels[] = {"Register", "Sign-in", "New Game", "Continue Last Game","Setting", "Exit"};
    int op;

    while (1) {
        op = menu("--- Rogue ---", 6, levels);
        echo();
        curs_set(1);
        if (op == 0) {  // Register option
            char* data[] = {"Username", "Password", "Email"};
            char** value = calloc(3, sizeof(char*));
            for (int i = 0; i < 3; i++) {
                value[i] = calloc(MAX_INPUT_LENGTH, sizeof(char));
            }
            clear();
            int height = 12, width = FORM_WIDTH;
            draw_border(height, width);
            username_REG:
              // Redraw form border after clearing
            move(2, 2);
            printw(" %-40s", data[0]);  // Field label
            move(4, 2);
            printw("Enter: ");
            move(4, 10);  // Position cursor for input
            getnstr(value[0], MAX_INPUT_LENGTH);
            move(4, 10 + strlen(value[0]));
            printw("                      ");  // Clear extra chars
            if (directory_exists(value[0])) {
                attron(COLOR_PAIR(1));  // Use red color for error
                mvprintw(10, 10, "The Username is Already Exist!");
                attroff(COLOR_PAIR(1));  // Reset to default color
                mvprintw(4, 10, "                       ");
                goto username_REG;
            }
            refresh();
            draw_border(height, width);  // Redraw form border after clearing

            move(4, 2);
            printw("shuffle Password? (if yes Press Tab Button): ");
            int ch = getch();
            if (ch == 9) {
                value[1] = generateRandomPassword();
                clear();
                draw_border(height, width);
                goto email_REG;
            }
            draw_border(height,width);
            password_REG:
            move(2, 2);
            printw(" %-40s", data[1]);  // Field label
            move(4, 2);
            printw("Enter: ");
            move(4, 10);  // Position cursor for input
            noecho();
            getnstr(value[1], MAX_INPUT_LENGTH);
            echo();
            move(4, 10 + strlen(value[1]));
            printw("                      ");  // Clear extra chars
            if (!validPass(value[1])) {
                attron(COLOR_PAIR(1));  // Use red color for error
                mvprintw(10, 10, "Password Invalid!");
                attroff(COLOR_PAIR(1));  // Reset to default color
                mvprintw(4, 10, "                       ");
                goto password_REG;
            }
            refresh();
            draw_border(height, width);

            email_REG:
            move(2, 2);
            printw(" %-40s", data[2]);  // Field label
            move(4, 2);
            printw("Enter: ");
            move(4, 10);  // Position cursor for input
            getnstr(value[2], MAX_INPUT_LENGTH);
            move(4, 10 + strlen(value[2]));
            printw("                      ");  // Clear extra chars
            if (!validEmail(value[2])) {
                attron(COLOR_PAIR(1));  // Use red color for error
                mvprintw(10, 10, "Email Invalid!");
                attroff(COLOR_PAIR(1));  // Reset to default color
                mvprintw(4, 10, "                       ");
                goto email_REG;
            }
            refresh();  

            // Clear the screen and print final registration data in green
            clear();
            attron(COLOR_PAIR(2));  // Use green color for final data
            printw("+------------------------------------------+\n");
            printw("|          Registration Successful!        |\n");
            printw("+------------------------------------------+\n\n");
            for (int i = 0; i < 3; i++) {
                printw("%-15s: %-20s\n", data[i], value[i]);
            }
            attroff(COLOR_PAIR(2));  // Reset to default color

            // Create directory if it does not exist
            mkdir(value[0], 0700);

            // Correct file path creation
            char filepath[100];
            snprintf(filepath, sizeof(filepath), "%s/login.txt", value[0]);

            // Open the file for writing
            FILE *file = fopen(filepath, "w");
            if (file) {
                fprintf(file, "%s\n%s", value[1], value[2]);
                fclose(file);  // Don't forget to close the file after writing
            }
            mvprintw(20,1,"Press any Key to Back To Main Menu...");
            getch();  // Wait for user input before continuing
            curs_set(0);
            writeToFile("main/main.txt",value[0]);
            for (int i = 0; i < 3; i++) {
                free(value[i]);
            }
            free(value);
        }
        else if (op==1)
        {
            char *options[] = {"Login As Guest","Login","Forgot MY Password","Exit"};
            curs_set(0);
            int opLogin = menu("--- Login ---", 4, options);
            curs_set(1);
            if (opLogin==0)
            {
                // go to the game without any word
            }
            else if (opLogin == 1) {
            // Login form
            char **value = calloc(2, sizeof(char*));  // Only need username and password
            value[0] = malloc(MAX_INPUT_LENGTH * sizeof(char));  // Allocate space for username
            value[1] = malloc(MAX_INPUT_LENGTH * sizeof(char));  // Allocate space for password

            int height = 12, width = FORM_WIDTH;
            draw_border(height, width);

            username_LOG:
            // Redraw form border after clearing
            move(2, 2);
            printw(" %-40s", "Username");  // Field label
            move(4, 2);
            printw("Enter: ");
            move(4, 10);  // Position cursor for input
            getnstr(value[0], MAX_INPUT_LENGTH);  // Read username
            move(4, 10 + strlen(value[0]));
            printw("                      ");  // Clear extra chars

            // Validate if directory exists for the username
            if (!directory_exists(value[0])) {
                attron(COLOR_PAIR(1));  // Use red color for error
                mvprintw(10, 10, "The Username does Not Exist!");
                attroff(COLOR_PAIR(1));  // Reset to default color
                mvprintw(4, 10, "                       ");
                goto username_LOG;
            }

            refresh();
            draw_border(height, width);

            password_LOG:
            move(2, 2);
            printw(" %-40s", "Password");  // Field label
            move(4, 2);
            printw("Enter: ");
            move(4, 10);  // Position cursor for input
            noecho();
            getnstr(value[1], MAX_INPUT_LENGTH);  // Read password
            echo();
            move(4, 10 + strlen(value[1]));
            printw("                      ");  // Clear extra chars
            char filepath[100];
            snprintf(filepath, sizeof(filepath), "%s/login.txt", value[0]);
            int lineCount = 0;
            char **lines = readFromFile(filepath, &lineCount);
            lines[0][strlen(lines[0])-1] = '\0';
            if (lines != NULL && lineCount >= 2) {
                if (strcmp(value[1], lines[0]) == 0) {  // Check if password matches
                    mvprintw(10, 10, "Login Successful!");
                    writeToFile("main/main.txt",value[0]);
                } else {
                    attron(COLOR_PAIR(1));
                    mvprintw(10, 10, "Invalid Password!");
                    attroff(COLOR_PAIR(1));
                }
                // Free memory used by lines
                for (int i = 0; i < lineCount; i++) {
                    free(lines[i]);
                }
                free(lines);
            }

            refresh();

            // Wait for any key press
            getch();
            free(value[0]);
            free(value[1]);
            free(value);
        }

            else if (opLogin==2)
            {
                int height = 12, width = FORM_WIDTH;
                char username[100];
                draw_border(height, width);
                username_FORGET:
                // Redraw form border after clearing
                move(2, 2);
                printw(" %-40s", "Username");  // Field label
                move(4, 2);
                printw("Enter: ");
                move(4, 10);  // Position cursor for input
                getnstr(username, MAX_INPUT_LENGTH);  // Read username
                move(4, 10 + strlen(username));
                printw("                      ");  // Clear extra chars

                // Validate if directory exists for the username
                if (!directory_exists(username)) {
                    attron(COLOR_PAIR(1));  // Use red color for error
                    mvprintw(10, 10, "The Username does Not Exist!");
                    attroff(COLOR_PAIR(1));  // Reset to default color
                    mvprintw(4, 10, "                       ");
                    goto username_FORGET;
                }
                char filepath[100];
                strcpy(filepath,username);
                strcat(filepath,"/login.txt");
                // snprintf(filepath, sizeof(filepath), "%s/login.txt", username);
                int lineCount = 0;
                char **lines = readFromFile(filepath, &lineCount); 
                clear();
                move(0,0);
                attron(COLOR_PAIR(2));
                printw("+------------------------------------------+\n");
            printw("|            Password got found!            |\n");
            printw("+------------------------------------------+\n\n");
            printw("        Password: %s",lines[0]);
            mvprintw(10,1,"Press any Key to Back To Main Menu...");
            attroff(COLOR_PAIR(2));
                refresh();
            } 
            else if (opLogin==3)
                goto first;
            
            
            getch();
        }
        else if (op==2)
        {
            /* code */
        }
        else if (op==4)
        {
            random_colors(300,700);
             int temp2;
             int settingChange = menu("--- Setting ---",4, (char*[]){"Game Level","Player Color","Song","See Current Setting"});
             char **lines = readFromFile("main/main.txt", &temp2);
             char **array = split(lines[0],',',&temp2);
             char data[4][100] = {"Active Username","Game Level","Player Color","Song"};
             char *levelArr[100]={"Easy","Medium","Hard"};
             char *colorArr[100]={"Red","Green","Blue"};
             char *songArr[100]={"song1.mp3","song2.mp3","song3.mp3"};
             if (settingChange==3){
                clear();
                move(0,0);
                printw("+------------------------------------------+\n");
                printw("|           Setting Informations!          |\n");
                printw("+------------------------------------------+\n\n");
                printw("%-15s: %-20s\n", data[0], array[0]);
                printw("%-15s: %s\n", data[1], levelArr[strToInt(array[1])]);
                printw("%-15s: %s\n", data[2], colorArr[strToInt(array[2])]);
                printw("%-15s: %s\n", data[3], songArr[strToInt(array[3])]);
                getch();
             }
            if (settingChange==0)
            {
                int level = menu("Level",3,levelArr);
                snprintf(array[1], sizeof(array[1]), "%d", level);
            }
            else if (settingChange==1)
            {
                int color = menu("Color",3,colorArr);
                snprintf(array[2], sizeof(array[2]), "%d", color);
            }
            else if (settingChange==2)
            {
                int song = menu("Song",3,songArr);
                snprintf(array[3], sizeof(array[3]), "%d", song);
            }
            FILE *file;
            file = fopen("main/main.txt", "w");
            fprintf(file,"%s,%s,%s,%s",array[0],array[1],array[2],array[3]);
            fclose(file);
        }
        else if (op==5)
        {
            clear();
            break;
        }
        
    }   
    getch();  // Wait for user input before closing the screen
    endwin();  // Clean up and close ncurses

    return 0;
}