#include "ship.h"

#include <stdio.h>
#include <string.h>

/* Read a valid integer from the user */
static int readInt(const char *prompt, int minimum, int maximum)
{
    int value;
    int result;

    do {
        printf("%s", prompt);
        result = scanf("%d", &value);

        while (getchar() != '\n') {
        }

        if (result != 1 || value < minimum || value > maximum) {
            printf("Enter a value from %d to %d.\n",
                   minimum, maximum);
        }

    } while (result != 1 ||
             value < minimum ||
             value > maximum);

    return value;
}

/* Read a valid decimal value */
static double readDouble(const char *prompt,
                         double minimum,
                         double maximum)
{
    double value;
    int result;

    do {
        printf("%s", prompt);
        result = scanf("%lf", &value);

        while (getchar() != '\n') {
        }

        if (result != 1 ||
            value < minimum ||
            value > maximum) {

            printf("Enter a value from %.2f to %.2f.\n",
                   minimum, maximum);
        }

    } while (result != 1 ||
             value < minimum ||
             value > maximum);

    return value;
}

/* Read Battleship type */
static char readBattleshipType(void)
{
    char type;

    do {
        printf("Battleship type (U/M/R/S): ");
        scanf(" %c", &type);

        while (getchar() != '\n') {
        }

        if (type != 'U' &&
            type != 'M' &&
            type != 'R' &&
            type != 'S') {

            printf("Invalid type.\n");
        }

    } while (type != 'U' &&
             type != 'M' &&
             type != 'R' &&
             type != 'S');

    return type;
}

/* Get battlefield data from user */
static void setupBattlefield(Battlefield *field)
{
    double size;
    int count;
    char type;
    double x;
    double y;
    double maxVelocity;
    unsigned int seed;

    size = readDouble(
        "Canvas size D (10-1000): ",
        10.0,
        1000.0
    );

    count = readInt(
        "Number of Escort ships (1-100): ",
        1,
        MAX_ESCORTS
    );

    type = readBattleshipType();

    x = readDouble(
        "Battleship X position: ",
        0.0,
        size
    );

    y = readDouble(
        "Battleship Y position: ",
        0.0,
        size
    );

    maxVelocity = readDouble(
        "Battleship maximum shell velocity (1-200): ",
        1.0,
        200.0
    );

    seed = (unsigned int)readInt(
        "Random seed (1-999999): ",
        1,
        999999
    );

    initializeBattlefield(
        field,
        size,
        count,
        type,
        x,
        y,
        maxVelocity,
        seed
    );

    printf("\nBattlefield setup completed.\n");

    displayBattlefield(field);
}

/* Show simple instructions */
static void instructions(void)
{
    printf("\nBattlefield Setup\n");
    printf("Setup the battlefield and inspect ship data.\n");
    printf("Battle simulation will be added later.\n");
}

int main(void)
{
    Battlefield field;
    int configured = 0;
    int choice;

    memset(&field, 0, sizeof(field));

    do {
        printf("\n====================================\n");
        printf("   Advanced Naval Battle Simulator\n");
        printf("====================================\n");

        printf("1. Setup Battlefield\n");
        printf("2. Show Battlefield\n");
        printf("3. Instructions\n");
        printf("4. Exit\n");

        choice = readInt("Select: ", 1, 4);

        if (choice == 1) {

            setupBattlefield(&field);
            configured = 1;

        }
        else if (choice == 2) {

            if (configured == 1) {
                displayBattlefield(&field);
            }
            else {
                printf("Complete setup first.\n");
            }

        }
        else if (choice == 3) {

            instructions();
        }

    } while (choice != 4);

    printf("Goodbye.\n");

    return 0;
}
