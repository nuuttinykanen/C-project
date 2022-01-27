#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Time {
    int hour;
    int minute;
    int second;
} Time;

typedef struct Driver {
    char* surname;
    char* team;
    struct Time time;
} Driver;

typedef struct Scoreboard {
    struct Driver *driver;
    struct Scoreboard *next; 
} Scoreboard;

int A(Scoreboard *board, char *surname, char *team) {
    Scoreboard *og = board;
    while(board && board->driver != NULL) {
        if(strcmp(board->driver->surname, surname) == 0) {
            printf("Driver %c%s%c is already in the database.\n", '"', surname, '"');
            return 0;
        }
        board = board->next;
    }
    board = og;
    // Create new driver
    Driver *new_driver = malloc(sizeof(struct Driver));
    new_driver->surname = malloc(sizeof(char) * (strlen(surname) + 1));
    new_driver->team = malloc(sizeof(char) * (strlen(team) + 1));
    if(!new_driver || !new_driver->surname || !new_driver->team) {
        fprintf(stderr, "Error in allocating memory");
        return 0;
    }
    strcpy(new_driver->surname, surname);
    strcpy(new_driver->team, team);
    
    Time new_time = {
        .hour = 0,
        .minute = 0,
        .second = 0
    };
    new_driver->time = new_time;

    // Find out if board only contains one driver or more.
    if(!board->driver) { 
        board->next = NULL;
        board->driver = new_driver;
    }
    else {
        // If more than one driver, create a new Scoreboard for linked list.
        Scoreboard *new_board = malloc(sizeof(struct Scoreboard));
        if(!new_board) {
           fprintf(stderr, "Error in allocating memory");
           return 0;
        }
        new_board->driver = new_driver;
        new_board->next = NULL;
        while(board->next) board = board->next;
        board->next = new_board; 
    }
    board = og;
    return 1;
}; 

int compare_time(const void *a, const void *b) {
    Driver driver_a = *((Driver*)a);
    Driver driver_b = *((Driver*)b);
    Time time_a = driver_a.time;
    Time time_b = driver_b.time;

    if(time_a.hour > time_b.hour) return 1;
    else if(time_a.hour < time_b.hour) return -1;
    else if(time_a.minute > time_b.minute) return 1;
    else if(time_a.minute < time_b.minute) return -1;
    else if(time_a.second > time_b.second) return 1;
    else if(time_a.second < time_b.second) return -1;
    else return 0;
};

void L(Scoreboard *board) {
    if(!board->driver) return;
    Scoreboard *og = board;
    // Get length of linked list.
    int length = 0;
    while(board) {
        length++;
        board = board->next;
    }
    board = og;
    // Set up a pointer array of Drivers.
    int i = 0;
    Driver driver_list[length]; 
    while(board) {
        driver_list[i] = *(board->driver);
        board = board->next;
        i++;
    }
    board = og;
    // Sort 
    qsort(driver_list, length, sizeof(struct Driver), compare_time);
    // Print
    for(int j = 0; j < length; j++) {
        printf("%s %s %d %d %d\n", driver_list[j].surname, driver_list[j].team, driver_list[j].time.hour, driver_list[j].time.minute, driver_list[j].time.second);
    }
    fprintf(stdout, "SUCCESS\n");
};

int U(Scoreboard *board, char *surname, int hour, int minute, int second) {
    if(hour < 0) {
        printf("Hour cannot be negative.\n");
        return 0;
    }
    if(minute > 59 || minute < 0) {
        printf("Minute cannot be negative or greater than 59.\n");
        return 0;
    }
    if(second > 59 || second < 0) {
        printf("Second cannot be negative or greater than 59.\n");
        return 0;
    }
    Scoreboard *og = board;
    // Move through linked list and find matching surname.
    while(board && board->driver != NULL && strcmp(board->driver->surname, surname) != 0) board = board->next;
    if(!board || !board->driver) {
        printf("Error: requested driver does not exist.\n");
        board = og;
        return 0;
    }
    if(!strcmp(board->driver->surname, surname)) {
        board->driver->time.hour += hour;
        board->driver->time.minute += minute;
        board->driver->time.second += second;
        while(board->driver->time.second > 59) {
            board->driver->time.minute++;
            int diff = board->driver->time.second - 60;
            board->driver->time.second = diff;
        }
        while(board->driver->time.minute > 59) {
            board->driver->time.hour++;
            int diff = board->driver->time.minute - 60;
            board->driver->time.minute = diff;
        }
    }
    else printf("Error: requested driver does not exist.\n");
    board = og;
    return 1;
};

int W(Scoreboard *board, char *filename) {
    FILE *file = fopen(filename, "w");
    if(!file) {
        fprintf(stderr, "Cannot open file %s for reading.\n", filename);
        return 0;
    }
    if(!board->driver) {
        fclose(file);
        return 0;
    }
    Scoreboard *og = board;
    int length = 0;
    while(board) {
        length++;
        board = board->next;
    }
    board = og;
    int i = 0;
    Driver driver_list[length]; 
    while(board) {
        driver_list[i] = *(board->driver);
        board = board->next;
        i++;
    }
    board = og;

    for(int j = 0; j < length; j++) {
        if(fputs(driver_list[j].surname, file) < 0) {
            fprintf(stderr, "ERROR IN WRITING SURNAME!");
        }
        fprintf(file, " ");
        if(fputs(driver_list[j].team, file) < 0) {
            fprintf(stderr, "ERROR IN WRITING TEAM!");
        }
        fprintf(file, " %d ", driver_list[j].time.hour);
        fprintf(file, "%d ", driver_list[j].time.minute);
        fprintf(file, "%d\n", driver_list[j].time.second);
    }
    fclose(file);
    return 1;
}

int O(Scoreboard *board, char *filename) {
    FILE *file = fopen(filename, "r");
    if(!file) {
        fprintf(stderr, "Cannot open file %s for reading.\n", filename);
        return 0;
    }
    Scoreboard *og = board;
    int i = 0;
    while(board && board->driver) {
        free(board->driver->surname);
        free(board->driver->team);
        free(board->driver);
        Scoreboard *this_board = board;
        board = board->next;
        if(i != 0) free(this_board);
        i++;
    }
    board = og;
    board->driver = NULL;

    char next_line[1000];
    while(fgets(next_line, 1000, file)) {
        unsigned int name_length = 0;
        unsigned int team_length = 0;
        int first = 0;
        int second = 0;
        int i = 0;
        while(!second) {
            if(next_line[i] == ' ') {
                if(!first) first = 1;
                else second = 1;
            }
            if(!first) name_length++;
            else team_length++; 
            i++;
        }
        char* surname = malloc(sizeof(char) * (name_length + 1));
        char* team = malloc(sizeof(char) * (team_length + 1));
        if(!surname || !team) {
            fprintf(stderr, "Error in allocating memory");
            return 0;
        }
        int hours;
        int minutes;
        int seconds;  
        sscanf(next_line, "%s %s %d %d %d", surname, team, &hours, &minutes, &seconds);
        A(board, surname, team);
        U(board, surname, hours, minutes, seconds);
        free(surname);
        free(team);
    }
    fclose(file);
    return 1;
}

void Q(Scoreboard *board) {
    if(!board->driver) {
        free(board);
        return;
    }
    while(board) {
        free(board->driver->surname);
        free(board->driver->team);
        free(board->driver);
        Scoreboard *og_board = board;
        board = board->next;
        free(og_board);
    }
    free(board);
    return;
}

int main(void) {
   Scoreboard *board = malloc(sizeof(struct Scoreboard));
   if(!board) {
       fprintf(stderr, "Error in allocating memory");
       return 0;
   }
   board->driver = NULL;
   while(1) {
       char input[1000];
       if(!fgets(input, 1000, stdin) || feof(stdin) != 0 || ferror(stdin) != 0) return 1;
       switch(input[0]) {
           case 'Q': {
               Q(board);
               fprintf(stdout, "SUCCESS\n");
               return 1;
           }
           case 'A': {
               unsigned int name_length = 0;
               unsigned int team_length = 0;
               int first = 0;
               int i = 2;
               while(input[i] != '\0') {
                   if(input[i] == ' ') first = 1;
                   if(!first) name_length++;
                   else team_length++; 
                   i++;
               }
               char sign;
               char* surname = malloc(sizeof(char) * (name_length + 1));
               char* team = malloc(sizeof(char) * (team_length + 1));
               if(!surname || !team) {
                   fprintf(stderr, "Error in allocating memory");
                   return 0;
               }
               if(sscanf(input, "%c %s %s", &sign, surname, team) == 3) {
                   if(A(board, surname, team)) fprintf(stdout, "SUCCESS\n");
               }
               else fprintf(stdout, "A should be followed by exactly 2 arguments.\n");
               free(surname);
               free(team);
               break;
           }
           case 'L': {
               L(board);
               break;
           }
           case 'U': {
               unsigned int name_length = 0;
               int i = 2;
               while(input[i] != ' ' && input[i] != '\0') {
                   name_length++;
                   i++;
               }
               char sign;
               char* surname = malloc(sizeof(char) * (name_length + 1));
               if(!surname) {
                   fprintf(stderr, "Error in allocating memory");
                   return 0;
               }
               int hour;
               int minute;
               int second;
               if(sscanf(input, "%c %s %d %d %d", &sign, surname, &hour, &minute, &second) == 5) {
                   if(U(board, surname, hour, minute, second)) fprintf(stdout, "SUCCESS\n");
               }
               else fprintf(stdout, "U should be followed by exactly 4 arguments.\n");
               free(surname);
               break;             
           }
           case 'W':
           case 'O': {
               unsigned int name_length = 0;
               int i = 2;
               while(input[i] != ' ' && input[i] != '\0') {
                   name_length++;
                   i++;
               }
               char sign;
               char* filename = malloc(sizeof(char) * (name_length + 1));
               if(!filename) {
                   fprintf(stderr, "Error in allocating memory");
                   return 0;
               }
               if(sscanf(input, "%c %s", &sign, filename) == 2) {
                    if(input[0] == 'W') {
                        if(W(board, filename)) fprintf(stdout, "SUCCESS\n");
                    }
                    else if(O(board, filename)) fprintf(stdout, "SUCCESS\n");
               }
               free(filename);
               break;
           }
           default: printf("Invalid command %c\n", input[0]);
       }
   }
};
