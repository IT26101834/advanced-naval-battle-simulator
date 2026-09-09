#ifndef SHIP_H
#define SHIP_H

#define MAX_ESCORTS 100

/* Battleship data */
typedef struct {
    char type;

    double x;
    double y;

    double maxVelocity;

    double health;
    double cumulativeDamage;

    /* Part 2 values */
    double firingInterval;
    int firingCount;
    double gamma;
    double currentImpactPower;

    int alive;
} Battleship;


/* Escort ship data */
typedef struct {
    int id;
    char type[3];

    double x;
    double y;

    double minVelocity;
    double maxVelocity;

    double minAngle;
    double maxAngle;

    double impactPower;

    double health;

    /* Part 2 values */
    double firingInterval;
    int firingCount;
    double gamma;
    double currentImpactPower;

    int alive;
} EscortShip;


/* Complete battlefield */
typedef struct {
    double size;

    unsigned int seed;

    Battleship battleship;

    EscortShip escorts[MAX_ESCORTS];

    int escortCount;
} Battlefield;


/* Function declarations */

void initializeBattlefield(
    Battlefield *field,
    double size,
    int escortCount,
    char battleshipType,
    double battleshipX,
    double battleshipY,
    double battleshipMaxVelocity,
    unsigned int seed
);

void displayBattlefield(
    const Battlefield *field
);

double escortImpactPower(
    const char *type
);

double escortAngleRange(
    const char *type
);

const char *battleshipName(
    char type
);

#endif
