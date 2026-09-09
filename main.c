#include "ship.h"
#include "battle.h"
#include "fileio.h"

#include <stdio.h>
#include <string.h>


/* Read a valid integer */
static int readInt(
    const char *prompt,
    int minimum,
    int maximum)
{
    int value;
    int result;

    do {
        printf("%s", prompt);

        result =
            scanf("%d", &value);

        while (getchar() != '\n') {
        }

        if (result != 1 ||
            value < minimum ||
            value > maximum) {

            printf(
                "Enter a value from %d to %d.\n",
                minimum,
                maximum
            );
        }

    } while (
        result != 1 ||
        value < minimum ||
        value > maximum
    );

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

        result =
            scanf("%lf", &value);

        while (getchar() != '\n') {
        }

        if (result != 1 ||
            value < minimum ||
            value > maximum) {

            printf(
                "Enter a value from %.2f to %.2f.\n",
                minimum,
                maximum
            );
        }

    } while (
        result != 1 ||
        value < minimum ||
        value > maximum
    );

    return value;
}


/* Read Battleship type */
static char readBattleshipType(void)
{
    char type;

    do {
        printf(
            "Battleship type (U/M/R/S): "
        );

        scanf(" %c", &type);

        while (getchar() != '\n') {
        }

        if (type != 'U' &&
            type != 'M' &&
            type != 'R' &&
            type != 'S') {

            printf(
                "Invalid battleship type.\n"
            );
        }

    } while (
        type != 'U' &&
        type != 'M' &&
        type != 'R' &&
        type != 'S'
    );

    return type;
}


/* Setup battlefield */
static void setupBattlefield(
    Battlefield *field)
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

    type =
        readBattleshipType();

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

    seed =
        (unsigned int)readInt(
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

    /*
     * Part 2 Battleship settings
     */
    field->battleship.firingInterval =
        readDouble(
            "Battleship firing interval TB (1-20): ",
            1.0,
            20.0
        );

    field->battleship.gamma =
        readDouble(
            "Battleship gamma (0.001-0.100): ",
            0.001,
            0.100
        );

    printf(
        "\nBattlefield setup completed.\n"
    );

    displayBattlefield(field);
}


/* Program instructions */
static void showInstructions(void)
{
    printf(
        "\n========== INSTRUCTIONS ==========\n"
    );

    printf(
        "Objective: Battleship tries to destroy Escort ships.\n"
    );

    printf(
        "Part 1-A uses one-hit destruction.\n"
    );

    printf(
        "Part 1-B moves the Battleship through path points.\n"
    );

    printf(
        "Part 1-B Simulation 2 applies a gun angle restriction.\n"
    );

    printf(
        "Part 1-C uses cumulative damage on the Battleship.\n"
    );

    printf(
        "Part 2-A adds Battleship firing delay and attack strategy.\n"
    );

    printf(
        "Part 2-B allows Escort ships to fire repeatedly.\n"
    );

    printf(
        "Part 2-C adds impact-power degradation using gamma.\n"
    );

    printf(
        "Simulation results are saved inside the results folder.\n"
    );
}


/* Start Simulation submenu */
static void simulationMenu(
    Battlefield *field,
    int configured)
{
    int choice;

    if (!configured) {

        printf(
            "\nPlease setup the battlefield first.\n"
        );

        return;
    }

    do {
        printf(
            "\n========== START SIMULATION ==========\n"
        );

        printf(
            "1. Part 1-A\n"
        );

        printf(
            "2. Part 1-B Simulation 1\n"
        );

        printf(
            "3. Part 1-B Simulation 2 - Gun Jam\n"
        );

        printf(
            "4. Part 1-C\n"
        );

        printf(
            "5. Part 2-A\n"
        );

        printf(
            "6. Part 2-B\n"
        );

        printf(
            "7. Part 2-C\n"
        );

        printf(
            "8. Back\n"
        );

        choice =
            readInt(
                "Select: ",
                1,
                8
            );


        /* Part 1-A */
        if (choice == 1) {

            Battlefield copy =
                *field;

            runPart1A(
                &copy,
                0,
                "part1a"
            );
        }


        /* Part 1-B Simulation 1 */
        else if (choice == 2) {

            int points;

            points =
                readInt(
                    "Number of path points (1-20): ",
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


        /* Part 1-B Simulation 2 */
        else if (choice == 3) {

            int points;
            double jamAngle;

            points =
                readInt(
                    "Number of path points (2-20): ",
                    2,
                    20
                );

            jamAngle =
                readDouble(
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


        /* Part 1-C */
        else if (choice == 4) {

            Battlefield copy =
                *field;

            runPart1A(
                &copy,
                1,
                "part1c"
            );
        }


        /* Part 2-A */
        else if (choice == 5) {

            Battlefield copy =
                *field;

            runPart2A(
                &copy
            );
        }


        /* Part 2-B */
        else if (choice == 6) {

            Battlefield copy =
                *field;

            double maximumTime;

            maximumTime =
                readDouble(
                    "Maximum simulation time (10-300 seconds): ",
                    10.0,
                    300.0
                );

            runPart2B(
                &copy,
                maximumTime
            );
        }


        /* Part 2-C */
        else if (choice == 7) {

            Battlefield copy =
                *field;

            double maximumTime;

            maximumTime =
                readDouble(
                    "Maximum simulation time (10-300 seconds): ",
                    10.0,
                    300.0
                );

            runPart2C(
                &copy,
                maximumTime
            );
        }

    } while (choice != 8);
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
        printf(
            "\n========================================\n"
        );

        printf(
            "    ADVANCED NAVAL BATTLE SIMULATOR\n"
        );

        printf(
            "========================================\n"
        );

        printf(
            "1. Start Simulation\n"
        );

        printf(
            "2. Setup\n"
        );

        printf(
            "3. View Instructions\n"
        );

        printf(
            "4. Simulation Statistics\n"
        );

        printf(
            "5. Exit\n"
        );

        choice =
            readInt(
                "Select: ",
                1,
                5
            );


        if (choice == 1) {

            simulationMenu(
                &field,
                configured
            );
        }


        else if (choice == 2) {

            setupBattlefield(
                &field
            );

            configured = 1;
        }


        else if (choice == 3) {

            showInstructions();
        }


        else if (choice == 4) {

            showStatistics();
        }

    } while (choice != 5);

    printf(
        "\nGoodbye.\n"
    );

    return 0;
}
