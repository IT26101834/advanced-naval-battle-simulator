#include "ship.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Generate random decimal value */
static double randomBetween(double minimum,
                            double maximum)
{
    return minimum +
           (maximum - minimum) *
           ((double)rand() / RAND_MAX);
}

/* Return Escort impact power */
double escortImpactPower(const char *type)
{
    if (strcmp(type, "EA") == 0) {
        return 0.08;
    }

    if (strcmp(type, "EB") == 0) {
        return 0.06;
    }

    if (strcmp(type, "EC") == 0) {
        return 0.07;
    }

    if (strcmp(type, "ED") == 0) {
        return 0.05;
    }

    return 0.04;
}

/* Return angle range for Escort type */
double escortAngleRange(const char *type)
{
    if (strcmp(type, "EA") == 0) {
        return 20.0;
    }

    if (strcmp(type, "EB") == 0) {
        return 30.0;
    }

    if (strcmp(type, "EC") == 0) {
        return 25.0;
    }

    if (strcmp(type, "ED") == 0) {
        return 50.0;
    }

    return 70.0;
}

/* Return Battleship name */
const char *battleshipName(char type)
{
    switch (type) {

        case 'U':
            return "USS Iowa (BB-61)";

        case 'M':
            return "MS King George V";

        case 'R':
            return "Richelieu";

        case 'S':
            return "Sovetsky Soyuz-class";

        default:
            return "Unknown";
    }
}

/* Create Battleship and Escort ships */
void initializeBattlefield(
    Battlefield *field,
    double size,
    int escortCount,
    char battleshipType,
    double battleshipX,
    double battleshipY,
    double battleshipMaxVelocity,
    unsigned int seed)
{
    const char types[][3] = {
        "EA",
        "EB",
        "EC",
        "ED",
        "EE"
    };

    int i;

    srand(seed);

    field->size = size;
    field->seed = seed;
    field->escortCount = escortCount;

    /* Battleship setup */
    field->battleship.type =
        battleshipType;

    field->battleship.x =
        battleshipX;

    field->battleship.y =
        battleshipY;

    field->battleship.maxVelocity =
        battleshipMaxVelocity;

    field->battleship.health = 1.0;

    field->battleship.cumulativeDamage = 0.0;

    field->battleship.alive = 1;

    /* Part 2 default values */
    field->battleship.firingInterval = 5.0;
    field->battleship.firingCount = 0;
    field->battleship.gamma = 0.02;
    field->battleship.currentImpactPower = 1.0;

    /* Escort ship setup */
    for (i = 0; i < escortCount; i++) {

        EscortShip *escort =
            &field->escorts[i];

        double angleRange;

        escort->id = i + 1;

        strcpy(
            escort->type,
            types[rand() % 5]
        );

        escort->x =
            randomBetween(0.0, size);

        escort->y =
            randomBetween(0.0, size);

        escort->minVelocity =
            randomBetween(15.0, 30.0);

        escort->maxVelocity =
            randomBetween(45.0, 80.0);

        if (strcmp(escort->type, "EA") == 0) {

            escort->maxVelocity =
                battleshipMaxVelocity * 1.2;
        }

        angleRange =
            escortAngleRange(escort->type);

        escort->minAngle =
            randomBetween(
                1.0,
                90.0 - angleRange
            );

        escort->maxAngle =
            escort->minAngle +
            angleRange;

        escort->impactPower =
            escortImpactPower(
                escort->type
            );

        escort->health = 1.0;

        escort->alive = 1;

        /* Part 2 default values */
        escort->firingCount = 0;

        escort->currentImpactPower =
            escort->impactPower;

        if (strcmp(escort->type, "EA") == 0) {
            escort->firingInterval = 6.0;
            escort->gamma = 0.05;
        }
        else if (strcmp(escort->type, "EB") == 0) {
            escort->firingInterval = 7.0;
            escort->gamma = 0.06;
        }
        else if (strcmp(escort->type, "EC") == 0) {
            escort->firingInterval = 8.0;
            escort->gamma = 0.07;
        }
        else if (strcmp(escort->type, "ED") == 0) {
            escort->firingInterval = 9.0;
            escort->gamma = 0.08;
        }
        else {
            escort->firingInterval = 10.0;
            escort->gamma = 0.09;
        }
    }
}

/* Display battlefield information */
void displayBattlefield(
    const Battlefield *field)
{
    int i;

    printf(
        "\nBattlefield: %.2f x %.2f | Seed: %u\n",
        field->size,
        field->size,
        field->seed
    );

    printf(
        "Battleship %c (%s) at (%.2f, %.2f), Vmax %.2f\n",
        field->battleship.type,
        battleshipName(
            field->battleship.type
        ),
        field->battleship.x,
        field->battleship.y,
        field->battleship.maxVelocity
    );

    printf(
        "Battleship Health: %.2f | Damage: %.2f\n",
        field->battleship.health,
        field->battleship.cumulativeDamage
    );

    printf(
        "B firing interval: %.2f | Gamma: %.3f | Impact: %.2f\n",
        field->battleship.firingInterval,
        field->battleship.gamma,
        field->battleship.currentImpactPower
    );

    printf(
        "%-4s %-3s %-15s %-15s %-15s %-8s %-8s\n",
        "ID",
        "Ty",
        "Position",
        "Velocity",
        "Angles",
        "Power",
        "State"
    );

    for (i = 0;
         i < field->escortCount;
         i++) {

        const EscortShip *escort =
            &field->escorts[i];

        printf(
            "%-4d %-3s "
            "(%5.1f,%5.1f) "
            "%5.1f-%-5.1f "
            "%5.1f-%-5.1f "
            "%-8.2f %s\n",

            escort->id,
            escort->type,

            escort->x,
            escort->y,

            escort->minVelocity,
            escort->maxVelocity,

            escort->minAngle,
            escort->maxAngle,

            escort->impactPower,

            escort->alive
                ? "alive"
                : "destroyed"
        );
    }
}
