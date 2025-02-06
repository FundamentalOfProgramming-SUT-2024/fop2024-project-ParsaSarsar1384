#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdio.h> 
#include <ncursesw/ncurses.h>  // Ensure you include the wide ncurses header
#include <locale.h>
#include <wchar.h>
#include <unistd.h>     
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
char** list_directories(int* FolderNum);
void boxPrint(int live,char message[],int time);
typedef struct {  
    int x;  
    int y;  
} mokh; 
typedef struct{
    int jan;
    int time;
    char message[300];
} Info;
typedef struct{
    int type;
    int janAdd;
    mokh place;
} Food;
typedef struct{
    int type;
    mokh place;
} Weapon;
int goldNum=0;
int goldNumTake=0;
Info datas={30,0,"Message Box ..."};
Weapon weapons[200];
Food foods[200];
mokh golds[200];
int weaponNum=0;
mokh firstWin;
mokh lastWin;
int end = 0;
void movePlayer(mokh* player, char* last, int ch,int color) {  
    int new_x = player->x;  
    int new_y = player->y;  
    int event=1;
    if (ch == 'w') { // Move up  
        new_y--;  
    } else if (ch == 'a') { // Move left  
        new_x--;  
    } else if (ch == 's') { // Move down  
        new_y++;  
    } else if (ch == 'd') { // Move right  
        new_x++;  
    } else if (ch == 259) { // Up-right  
        new_y--;  
        new_x++;  
    } else if (ch == 260) { // Up-left  
        new_y--;  
        new_x--;  
    } else if (ch == 258) { // Down-left  
        new_y++;  
        new_x--;  
    } else if (ch == 261) { // Down-right  
        new_y++;  
        new_x++;  
    }  
    else
        event=0;
    if ((new_x > firstWin.x && new_x < lastWin.x) && (new_y > firstWin.y && new_y < lastWin.y))
    {
        clear();
        mvprintw(20,20,"You Win!");
        getch();
        end = 1;
        
    }
    
    if (event)
    {
        char next_char = mvinch(new_y, new_x);  
        if (next_char=='O')
        {
            strcpy(datas.message,"You Hit a Column!");
        }
        else if (next_char=='F')
        {
            strcpy(datas.message,"You Hit Fire!");

            datas.jan--;
        }
        else if (next_char =='^')
        {
            // boxPrint(datas.jan,"You Hit a trap!",datas.time);
            strcpy(datas.message,"You Hit a trap!");
            datas.jan--;
            next_char ='.';
        }
        else if(next_char!='|' && next_char!='_' && next_char!=' ' && next_char!='+' && next_char!='.'){
                 next_char ='.'; 
                // boxPrint(datas.jan,"You grab a Weapon!",datas.time);
                strcpy(datas.message,"You grab a Weapon!");
                for (int i = 0; i < goldNum; i++)
                {
                    if (golds[i].y == new_y && golds[i].x == new_x)
                    {  
                        strcpy(datas.message,"You got a Gold!");
                        break;
                        goldNumTake++;
                    }
                }
                
                
        }
            
        if (next_char != '|' && next_char!='_' && next_char!='O' && next_char !='F') {  
            mvprintw(player->y, player->x, "%c", *last);  
            *last = mvinch(player->y, player->x);  
            attron(COLOR_PAIR(color));  
            mvprintw(new_y, new_x, "@");  
            player->x = new_x;  
            player->y = new_y;  
            attroff(COLOR_PAIR(color));  
            *last = next_char;
        }    
    }
}  
char** list_directories(int* FolderNum) { 
    char **list = malloc(8);

    struct dirent *entry;  
    DIR *dp = opendir(".");  
    int i=0;
    while ((entry = readdir(dp)) != NULL) {   
        if (entry->d_name[0] == '.') {  
            continue;  
        }  
        char full_path[1024];  
        snprintf(full_path, sizeof(full_path), "%s",entry->d_name);  
        struct stat statbuf;  
        if (stat(full_path, &statbuf) == -1) {  
            perror("stat");  
            continue;  
        }  
        if (S_ISDIR(statbuf.st_mode)) {
            list = (char**)realloc(list,((i+1))*8);
            *(list + i ) = malloc(8); 
            strcpy(list[i++],full_path); 
        }  
    }  
    *FolderNum = i;
    closedir(dp);
    return list;  
}  
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
void boxPrint(int live,char message[],int time){
    attron(COLOR_PAIR(1));
    char liveN[500];
    for (int i = 0; i < live/5; i++)
    {
        strcat(liveN,"\U0001F377");
    }
    
    mvprintw(2,100,"______________________________________________________________________");
    mvprintw(3,100,"|                                                                     |"); 
    mvprintw(4,100,"|                           Game info                                 |"); 
    mvprintw(5,100,"|                                                                     |"); 
    mvprintw(6,100,"______________________________________________________________________");
    mvprintw(7,100,"|                                                                     |"); 
    mvprintw(8,100,"|                                                                     |"); 
    mvprintw(9,100,"|     Live :                                                          |"); 
    mvprintw(10,100,"|                    %-18s                                     |",liveN);  
    mvprintw(11,100,"|                                                                     |"); 
    mvprintw(12,100,"|                                                                     |"); 
    mvprintw(13,100,"|     Time:                                                           |"); 
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(3));
    mvprintw(14,100,"|                    %-4d Second                                      |",time); 
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(1));
    mvprintw(15,100,"|                                                                     |"); 
    mvprintw(16,100,"|                                                                     |"); 
    mvprintw(17,100,"|                                                                     |"); 
    mvprintw(18,100,"|     Message:                                                        |"); 
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(3));
    mvprintw(19,100,"|                    %-22s                           |",message); 
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(1));
    mvprintw(20,100,"|                                                                     |"); 
    mvprintw(21,100,"|                                                                     |"); 
    mvprintw(22,100,"|                                                                     |"); 
    mvprintw(23,100,"______________________________________________________________________");
    attroff(COLOR_PAIR(1));
}
int main() {
    srand(time(0));
    setlocale(LC_ALL, "");
    initscr();  // Initialize the ncurses environment
    start_color();  // Enable color functionality
    init_pair(1, COLOR_RED, COLOR_BLACK); // First color pair (for the first row)  
    init_pair(2, COLOR_GREEN, COLOR_BLACK); // Second color pair (for the second row)  
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_GREEN);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    keypad(stdscr, TRUE);  // Enable keypad for arrow keys
    noecho();  // Disable echoing of input characters
    curs_set(0);  // Hide the cursor
    // goto game;
    first:char* levels[] = {"Register", "Sign-in", "Pre-Game","Setting","Profile", "Exit"};
    int op;
        printw("%ld",sizeof("\u2694"));

    while (1) {
        op = menu("--- Rogue ---", 6, levels);
        echo();
        curs_set(1);
        int lineCount;
        char **lines = readFromFile("main.txt", &lineCount);
        lines  = split(lines[0],',',&lineCount);
        char music[30];
        if (op == 0) {  // Register option
            int temp2;
             char **lines = readFromFile("main.txt", &temp2);
             char **array = split(lines[0],',',&temp2);
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
            char filepath[100],filepath2[100];
            snprintf(filepath, sizeof(filepath), "%s/login.txt", value[0]);
            snprintf(filepath2, sizeof(filepath2), "%s/game_info.txt", value[0]);
            // Open the file for writing
             FILE *file2 = fopen(filepath, "w");
             FILE *fileInfo = fopen(filepath2, "w");
             if (fileInfo)
             {
                int tedad_bazi = rand()%51;
                fprintf(fileInfo, "%d\n%d\n%d",tedad_bazi * (rand()%11 + 10),tedad_bazi*(rand()%(6) + 4),tedad_bazi);
                fclose(fileInfo);
             }
             
            if (file2) {
                fprintf(file2, "%s\n%s", value[1], value[2]);
                fclose(file2);  // Don't forget to close the file after writing
            }
            mvprintw(20,1,"Press any Key to Back To Main Menu...");
            getch();  // Wait for user input before continuing
            curs_set(0);
            FILE *file = fopen("main.txt", "w"); 
            
            fprintf(file, "%s,%s,%s,%s", value[0],array[1],array[2],array[3]);
            fclose(file);
            for (int i = 0; i < 3; i++) {
                free(value[i]);
            }
            free(value);
        }
        else if (op==1)
        {
            char *options[] = {"Login As Guest","Login","Forgot MY Password","Exit"};
            curs_set(0);
            int temp2;
            char **lines = readFromFile("main.txt", &temp2);
            char **array = split(lines[0],',',&temp2);
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
                    char temp[100]="";
                    strcat(temp,value[0]);
                    strcat(temp,",");
                    strcat(temp,array[1]);
                    strcat(temp,",");
                    strcat(temp,array[2]);
                    strcat(temp,",");
                    strcat(temp,array[3]);
                    writeToFile("main.txt",temp);
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
            char *options[] = {"New Game","Resume Game","Settings","Scoreboard","Exit"};
            curs_set(0);
            int opGame = menu("--- Pre Game Menu ---", 5, options);
            if (opGame == 0)
            {
                // game:
                noecho();
                clear();
                int temp2;
                char **lines = readFromFile("main.txt", &temp2);
                char **array = split(lines[0],',',&temp2);
            int x = 99;  
            int y = 33;  
            typedef struct {  
                int width;  
                int height;  
                int start_x;  
                int start_y;  
                mokh dor[3];  // Doors array
                mokh hallways[30]; 
            } Room;  
            Room rooms[7]; 
            int roomSide[3][3] = {0}; 
            int a = rand() % 4 + 6;  
            for (int i = 0; i < a; i++) {  
                rooms[i].start_x = rand() % x;  
                rooms[i].start_y = rand() % y;  
                int grid_x = (rooms[i].start_x < x / 3) ? 1 : (rooms[i].start_x < x * 2 / 3) ? 2 : 3;  
                int grid_y = (rooms[i].start_y < y / 3) ? 1 : (rooms[i].start_y < y * 2 / 3) ? 2 : 3;   
                if (roomSide[grid_x - 1][grid_y - 1] == 0) {  
                    roomSide[grid_x - 1][grid_y - 1] = 1;  
                } else {  
                    i--;  
                    continue;  
                }  
                rooms[i].start_x = (rooms[i].start_x / (x / 3)) * (x / 3) + rand() % 3 + 1;  
                rooms[i].start_y = (rooms[i].start_y / (y / 3)) * (y / 3) + rand() % 3 + 1;  
                rooms[i].width = rand() % (x / 9) + x / 9;  
                rooms[i].height = rand() % (y / 9) + y / 7 + 2;  
                int numDoors = rand() % 3 + 1;  
            }
            int goldRoom=-1;
            int majonRoom=-1;
            datas.jan = 30;
            datas.time = 0;
            mokh player = (mokh){rooms[0].start_x + rand()%(rooms[0].width - 2) + 1,rooms[0].start_y + rand()%(rooms[0].height - 2) + 1};  
            for (int j = 0; j < a; j++) {
                if (goldRoom==-1)
                {
                    goldRoom = rand()%(a-1) + 1;
                }
                if (majonRoom==-1 && majonRoom!=goldRoom)
                {
                    majonRoom = rand()%(a-1) + 1;
                }
                if (goldRoom == j)
                {
                    attron(COLOR_PAIR(5));
                }
                if (majonRoom == j)
                {
                    attron(COLOR_PAIR(3));
                }
                for (int i = 1; i < rooms[j].height - 1; i++)
                    for (int k = 1; k < rooms[j].width - 1; k++)
                        mvprintw(rooms[j].start_y + i, rooms[j].start_x + k, ".");  
                if (goldRoom == j)
                {
                    for (int i = 0; i < 6 + rand()%3; i++)
                    {
                        int  y,x;
                        y = rooms[j].start_y + rand()%(rooms[j].height - 4) + 1;
                        x = rooms[j].start_x + rand()%(rooms[j].width - 4) + 1;
                        mvprintw(y,x,"\u2605");
                        golds[goldNum++] = (mokh){y,x};
                    }
                    attroff(COLOR_PAIR(5));
                    firstWin = (mokh){rooms[j].start_y,rooms[j].start_x};
                    lastWin = (mokh){rooms[j].start_y + rooms[j].height,rooms[j].start_x + rooms[j].width};
                }
                if (majonRoom == j)
                {
                    for (int i = 0; i < 6 + rand()%3; i++)
                    {
                        int  y,x;
                        y = rooms[j].start_y + rand()%(rooms[j].height - 4) + 1;
                        x = rooms[j].start_x + rand()%(rooms[j].width - 4) + 1;
                        mvprintw(y,x,"\U0001F37E");
                        // golds[goldNum++] = (mokh){y,x};
                    }
                    attroff(COLOR_PAIR(3));
                }
                for (int i = 0; i < rand()%3; i++)
                {
                    int  y,x;
                    y = rooms[j].start_y + rand()%(rooms[j].height - 2) + 1;
                    x = rooms[j].start_x + rand()%(rooms[j].width - 2) + 1;
                    mvprintw(y,x,"O");
                }
                for (int i = 0; i < 1 + rand()%3; i++)
                    {
                        int  y,x;
                        y = rooms[j].start_y + rand()%(rooms[j].height - 4) + 1;
                        x = rooms[j].start_x + rand()%(rooms[j].width - 4) + 1;
                        mvprintw(y,x,"\u2605");
                        mvprintw(y,x+1,".");
                    }
                for (int i = 0; i < rand()%3; i++)
                {
                    int  y,x;
                    y = rooms[j].start_y + rand()%(rooms[j].height - 4) + 1;
                    x = rooms[j].start_x + rand()%(rooms[j].width - 4) + 1;
                    attron(COLOR_PAIR(2));
                    char* xxx = "\u2694";
                    mvprintw(y,x,"%s",xxx);
                    mvprintw(y,x+1,".");
                    attroff(COLOR_PAIR(2));
                    weapons[weaponNum++].place = (mokh){x,y};
                    weapons[weaponNum].type = 1;
                }
                for (int i = 0; i < rand()%4; i++)
                {
                    int  y,x;
                    y = rooms[j].start_y + rand()%(rooms[j].height - 4) + 1;
                    x = rooms[j].start_x + rand()%(rooms[j].width - 4) + 1;
                    attron(COLOR_PAIR(2));
                    char* xxx = "\U0001F5E1";
                    mvprintw(y,x,"%s",xxx);
                    mvprintw(y,x+1,".");
                    attroff(COLOR_PAIR(2));
                    weapons[weaponNum++].place = (mokh){x,y};
                    weapons[weaponNum].type = 2;
                }
                for (int i = 0; i < rand()%3; i++)
                {
                    int  y,x;
                    y = rooms[j].start_y + rand()%(rooms[j].height - 2) + 1;
                    x = rooms[j].start_x + rand()%(rooms[j].width - 2) + 1;
                    attron(COLOR_PAIR(2));
                    mvprintw(y,x,"F");
                    attroff(COLOR_PAIR(2));
                }
                 for (int i = 0; i < rand()%3; i++)
                {
                    int  y,x;
                    y = rooms[j].start_y + rand()%(rooms[j].height - 2) + 1;
                    x = rooms[j].start_x + rand()%(rooms[j].width - 2) + 1;
                    attron(COLOR_PAIR(1));
                    mvprintw(y,x,"^");
                    attroff(COLOR_PAIR(1));
                } 
                if (goldRoom == j)
                {
                    attron(COLOR_PAIR(5));
                }
                if (majonRoom == j)
                {
                    attron(COLOR_PAIR(3));
                }
                for (int i = 1; i < rooms[j].width - 1; i++) {  
                    mvprintw(rooms[j].start_y, rooms[j].start_x + i, "_");  
                    mvprintw(rooms[j].start_y + rooms[j].height - 1, rooms[j].start_x + i, "_");  
                }  
                // رسم دیوارهای چپ و راست  
                for (int i = 1; i < rooms[j].height; i++) {  
                    mvprintw(rooms[j].start_y + i, rooms[j].start_x, "|");  
                    mvprintw(rooms[j].start_y + i, rooms[j].start_x + rooms[j].width - 1, "|");  
                }
                 int b = rand()%(rooms[j].height - 1) + 1;
                int s = rand()%(rooms[j].width - 1) + 1;
                int c = rand()%2;
                mvprintw(rooms[j].start_y + b*c,rooms[j].start_x + s*(c-1)*(c-1), "+"); 
                if (goldRoom == j)
                {
                    attroff(COLOR_PAIR(5));
                }  
                if (majonRoom == j)
                {
                    attroff(COLOR_PAIR(3));
                }    
            }
            
            
            attron(COLOR_PAIR(strToInt(array[2])+1));
            char last='.';
            mvprintw(player.y,player.x,"@");
            attroff(COLOR_PAIR(strToInt(array[2])+1));
            int timeFirst = time(NULL);
            int seconds=0;
            boxPrint(datas.jan,datas.message,0);
            while(1){
                if (end == 1)
                {
                   mvprintw(100,20,"You Win!");
                    getch();
                    // goto first;
                    end=0;
                }
                
                if (time(0) - timeFirst >= 1)
                {
                    seconds += time(0) - timeFirst;
                    if (seconds >= 12/(1+strToInt(array[1])) + 5)
                    {   
                        datas.jan-= seconds/(12/(1+strToInt(array[1])) + 5);
                        seconds=0;
                    }
                    if (datas.jan <= 0)
                    {
                        clear();
                        mvprintw(30,40,"YOU LOSE !  Press Ecs key to Exit");
                    }
                    boxPrint(datas.jan,datas.message,datas.time +=time(0) - timeFirst);
                    timeFirst = time(NULL);
                }
                int ch = getch();
                 movePlayer(&player, &last, ch,strToInt(array[2])+1); 
                if (ch == 27)
                {
                    goto first;
                    datas= (Info){30,0,"Message Box ..."};
                } 
            }
          


            getch();
            }
            else if (opGame == 2)
            {
                clear();
                 mvprintw(9,58,"         +-------------------------------------+");
                mvprintw(10,58,"         |                Poison               |");
                mvprintw(11,58,"         +-------------------------------------+");
                mvprintw(12,58,"         |  1-      : \U0001F37E                       |");
                mvprintw(13,58,"         |  2-      : \U0001F377                       |");
                mvprintw(14,58,"         |  3-      : \U0001F37C                       |");
                mvprintw(15,58,"         +-------------------------------------+");

                mvprintw(16+3,58,"         +-------------------------------------+");
                mvprintw(17+3,58,"         |                Weapon               |");
                mvprintw(18+3,58,"         +-------------------------------------+");
                mvprintw(19+3,58,"         |  1-      :  \u2692                       |");
                mvprintw(20+3,58,"         |  2-      :  \U0001F5E1                       |");
                mvprintw(21+3,58,"         |  3-      :  \U0001FA84                      |");
                mvprintw(22+3,58,"         |  4-      :  \u27B3                       |");
                mvprintw(23+3,58,"         |  5-      :  \u2694                       |");
                mvprintw(24+3,58,"         +-------------------------------------+");
                getch();
            }
            else if (opGame == 3)
            {
                clear();
                 int x,temp10;
                char** temp = list_directories(&x);
                typedef struct{
                    char* username;
                    int score;
                    int gold;
                    int play;
                } User;
                User users[x+2];
                for (int i = 0; i < x; i++)
                {
                    char* path=malloc(24);
                    strcpy(path,temp[i]);
                    strcat(path,"/game_info.txt");
                    char **lines = readFromFile(path, &temp10);
                    users[i] = (User){temp[i],atoi(lines[0]),atoi(lines[1]),atoi(lines[2])};
                }
                int i, j;
                User temp3;  
                for (i = 0; i < x - 1; i++) {  
                    for (j = 0; j < x - i - 1; j++) {  
                        // If the element found is greater than the next element, swap them  
                        if (users[j].score < users[j + 1].score) {  
                            temp3 = users[j];  
                            users[j] = users[j + 1];  
                            users[j + 1] = temp3;  
                        }  
                    }  
                }
                char titles[3][30] = {"Goat","Champ","Hero"};
                mvprintw(0,45," ______________________________________________________________________________________");
                mvprintw(1,45,"|                              Score Game Best of Bests !!!                             |");
                mvprintw(2,45," _____________________________________________________________________________________");
                mvprintw(4,45,"|# |  Username               | Total Score  | Gold Number | Experience   | Game Number  |");
                mvprintw(5,45," ______________________________________________________________________________________");
                int lineCount = 0;
                char **lines = readFromFile("main.txt", &lineCount);
                lines  = split(lines[0],',',&lineCount);
                for (int i = 0; i < x; i++) {  
                char level[20] = "Normal";
                    char nikname[5] = "-----"; 
                if (users[i].play > 5 && users[i].play <= 10)
                {
                    strcpy(level,"Good");
                }
                else if (users[i].play > 10 && users[i].play <= 15)
                {
                    strcpy(level,"Nice");
                }
                else if (users[i].play > 15 && users[i].play <= 20)
                {
                    strcpy(level,"Legend");
                }
                else{
                    strcpy(level,"Monster");
                }
                if (strcmp(users[i].username,lines[0])==0)
                    attron(COLOR_PAIR(4));
                 if (i == 0) {  
                    attron(COLOR_PAIR(1)); // First row red 
                    strcpy(nikname,titles[i]);
                    attron(A_BOLD);
                    if (strcmp(users[i].username,lines[0])==0)
                    attron(COLOR_PAIR(4));
                    mvprintw(i+6,45,"|%-2d|  %-12s (%5s) \U0001F947| %-8d     | %-8d    | %-12s | %-12d |", i + 1, users[i].username,nikname, users[i].score, users[i].gold, level,users[i].play);  
                } else if (i == 1) {  
                    strcpy(nikname,titles[i]);
                    attron(A_BOLD);  
                    attron(COLOR_PAIR(2)); // Second row green 
                    if (strcmp(users[i].username,lines[0])==0)
                    attron(COLOR_PAIR(4));
                    mvprintw(i+6,45,"|%-2d|  %-12s (%5s) \U0001F948| %-8d     | %-8d    | %-12s | %-12d |", i + 1, users[i].username,nikname, users[i].score, users[i].gold, level,users[i].play);  
 
                } else if (i == 2) {  
                    strcpy(nikname,titles[i]);
                    attron(A_BOLD);  
                    attron(COLOR_PAIR(3)); // Third row blue 
                    if (strcmp(users[i].username,lines[0])==0)
                    attron(COLOR_PAIR(4)); 
                    mvprintw(i+6,45,"|%-2d|  %-12s (%5s) \U0001F949| %-8d     | %-8d    | %-12s | %-12d |", i + 1, users[i].username,nikname, users[i].score, users[i].gold, level,users[i].play);  
                } 
                else
                    mvprintw(i+6,45,"|%-2d|  %-12s (%5s)   | %-8d     | %-8d    | %-12s | %-12d |", i + 1, users[i].username,nikname, users[i].score, users[i].gold, level,users[i].play);  
                // Turn off color after printing  
                if (i < 3) {  
                    attroff(COLOR_PAIR(i + 1));  
                    attroff(A_BOLD); 
                }  
                if (strcmp(users[i].username,lines[0])==0)
                {
                    attroff(COLOR_PAIR(4)); 
                }   
                
              }  
              mvprintw(6+x,45," ______________________________________________________________________________________");
            }
            refresh();
            getch();
            
        }
        else if (op==3)
        {
            // random_colors(300,700);
             int temp2;
             int settingChange = menu("--- Setting ---",4, (char*[]){"Game Level","Player Color","Song","See Current Setting"});
             char **lines = readFromFile("main.txt", &temp2);
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
            file = fopen("main.txt", "w");
            fprintf(file,"%s,%s,%s,%s",array[0],array[1],array[2],array[3]);
            fclose(file);
        }
        else if (op==4)
        {
            clear();
            int lineCount;
            char **lines = readFromFile("main.txt", &lineCount);
            lines  = split(lines[0],',',&lineCount);
            char* path=malloc(24);
            strcpy(path,lines[0]);
            strcat(path,"/game_info.txt");
            char **game = readFromFile(path, &lineCount);
            strcpy(path,lines[0]);
            strcat(path,"/login.txt");
            game[0][strlen(game[0])-1] = '\0';
            game[1][strlen(game[1])-1] = '\0';
            game[2][strlen(game[2])-1] = '\0';
            char **login = readFromFile(path, &lineCount);
                attron(A_BOLD);
                mvprintw(0,51,"  /######  /##   /## /##   /## /#######     /##     /##  /######  /##   /##");
                mvprintw(1,51," /##__  ##| ##  | ##| ### | ##| ##__  ##   | ##    | ## /##__  ##| ### | ##");
                mvprintw(2,51,"| ##  \\ ##| ##  | ##| ####| ##| ##  \\ ##   | ##    | ##| ##  \\ ##| ####| ##");
                mvprintw(3,51,"| ##  | ##| ##  | ##| ## ## ##| ##  | ##   | ##    | ##| ##  | ##| ## ## ##");
                mvprintw(4,51,"| ##  | ##| ##  | ##| ##  ####| ##  | ##   | ##    | ##| ##  | ##| ##  ####");
                mvprintw(5,51,"| ##  | ##| ##  | ##| ##\\  ###| ##  | ##   | ##    | ##| ##  | ##| ##\\  ###");
                mvprintw(6,51,"|  ######/|  ######/| ## \\  ##| #######/   |  ######/ |  ######/| ## \\  ##");
                mvprintw(7,51," \\______/  \\______/ |__/  \\__/|_______/     \\______/   \\______/ |__/  \\__/");
                attroff(A_BOLD);

                mvprintw(9,58,"         +-------------------------------------+");
                mvprintw(10,58,"         |           LOGIN DETAILS             |");
                mvprintw(11,58,"         +-------------------------------------+");
                mvprintw(12,58,"         |  Username:  %-24s|", lines[0]);
                mvprintw(13,58,"         |  Password:  %-24s|", login[0]);
                mvprintw(14,58,"         |  Gmail:     %-24s|", login[1]);
                mvprintw(15,58,"         +-------------------------------------+");

                mvprintw(18,58,"         +-------------------------------------+");
                mvprintw(19,58,"         |             GAME STATS              |");
                mvprintw(20,58,"         +-------------------------------------+");
                mvprintw(21,58,"         |  Score:         %-20s|", game[0]);
                mvprintw(22,58,"         |  Gold:          %-20s|", game[1]);
                mvprintw(23,58,"         |  Game Number:   %-20s|", game[2]);
                mvprintw(24,58,"         +-------------------------------------+");
                
                mvprintw(28,60,"            [ PRESS ANY KEY TO CONTINUE ]\n");
                refresh();
            getch();
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
