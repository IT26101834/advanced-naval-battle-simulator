#ifndef BATTLE_H
#define BATTLE_H

#include "ship.h"

/* Information about one successful shot */
typedef struct {
    int targetId;
    double time;
    double velocity;
    double angle;
} HitRecord;


/* Projectile functions */

double calculateDistance(
    double x1,
    double y1,
    double x2,
    double y2
);

double degreesToRadians(
    double degrees
);

double projectileRange(
    double velocity,
    double angle
);

double calculateFlightTime(
    double distance,
    double velocity,
    double angle
);

int findShotSolution(
    double distance,
    double minVelocity,
    double maxVelocity,
    double minAngle,
    double maxAngle,
    HitRecord *solution
);


/* Part 1 simulations */

void runPart1A(
    Battlefield *field,
    int cumulative,
    const char *label
);

void runPart1B(
    const Battlefield *original,
    int points,
    double jamAngle,
    int cumulative
);


/* Part 2 simulations */

void runPart2A(
    Battlefield *field
);

void runPart2B(
    Battlefield *field,
    double maximumTime
);

void runPart2C(
    Battlefield *field,
    double maximumTime
);

#endif
