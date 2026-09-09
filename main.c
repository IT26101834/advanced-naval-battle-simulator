#include "ship.h"
#include "battle.h"
#include "fileio.h"

#include <stdio.h>
#include <string.h>

/* Read a valid integer */
static int readInt(const char *prompt, int minimum, int maximum)
{
    int value;
    int result;

    do {
        printf("%s", prompt);
        result = scanf("%d", &value);

        while (getchar() != '\n') {
        }

        if (result != 1 ||
            value < minimum ||
            value > maximum) {

            printf("Enter a value from %d to %d.\n",
                   minimum, maximum);
        }

    } while (result != 1 ||
             value < minimum ||
             value > maximum);

    return value;
}

/* Read a valid decimal value */
static double readDouble(
    const char *prompt,
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

/* Read battleship type */
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

            printf("Invalid battleship type.\n");
        }

    } while (type != 'U' &&
             type != 'M' &&
             type != 'R' &&
             type != 'S');

    return type;
}

/* Setup battlefield */
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

/* Instructions */
static void showInstructions(void)
{
    printf("\n--- Instructions ---\n");
    printf("1. Setup the battlefield first.\n");
    printf("2. Part 1-A uses one-hit destruction.\n");
    printf("3. Part 1-B moves the battleship through path points.\n");
    printf("4. Part 1-C uses cumulative Escort damage.\n");
}

/* Simulation menu */
static void simulationMenu(
    Battlefield *field,
    int configured)
{
    int choice;
    int points;
    double jamAngle;

    if (!configured) {
        printf("\nPlease setup the battlefield first.\n");
        return;
    }

    do {
        printf("\n--- Part 1 Simulations ---\n");
        printf("1. Part 1-A\n");
        printf("2. Part 1-B Simulation 1\n");
        printf("3. Part 1-B Simulation 2 - Gun Jam\n");
        printf("4. Part 1-C\n");
        printf("5. Back\n");

        choice = readInt("Select: ", 1, 5);

        if (choice == 1) {

            Battlefield copy = *field;

            runPart1A(
                &copy,
                0,
                "part1a"
            );
        }
        else if (choice == 2) {

            points = readInt(
                "Number of path points: ",
                1,
                20
            );

            runPart1B(
                field,
                points,
                0.0,
                0
            );
        }
        else if (choice == 3) {

            points = readInt(
                "Number of path points: ",
                2,
                20
            );

            jamAngle = readDouble(
                "Minimum angle after gun jam (1-29): ",
                1.0,
                29.0
            );

            runPart1B(
                field,
                points,
                jamAngle,
                0
            );
        }
        else if (choice == 4) {

            Battlefield copy = *field;

            runPart1A(
                &copy,
                1,
                "part1c"
            );
        }

    } while (choice != 5);
}

int main(void)
{
    Battlefield field;

    int configured = 0;
    int choice;

    memset(
        &field,
        0,
        sizeof(field)
    );

    do {
        printf("\n====================================\n");
        printf("   Advanced Naval Battle Simulator\n");
        printf("====================================\n");

        printf("1. Setup Battlefield\n");
        printf("2. Show Battlefield\n");
        printf("3. Start Part 1 Simulation\n");
        printf("4. Instructions\n");
        printf("5. Exit\n");

        choice = readInt(
            "Select: ",
            1,
            5
        );

        if (choice == 1) {

            setupBattlefield(&field);
            configured = 1;

        }
        else if (choice == 2) {

            if (configured) {
                displayBattlefield(&field);
            }
            else {
                printf("Complete setup first.\n");
            }

        }
        else if (choice == 3) {

            simulationMenu(
                &field,
                configured
            );

        }
        else if (choice == 4) {

            showInstructions();
        }

    } while (choice != 5);

    printf("Goodbye.\n");

    return 0;
}
