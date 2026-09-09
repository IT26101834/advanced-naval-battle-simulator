#include "strategy.h"

#include <stdio.h>

/* Check if an Escort can currently hit B */
static int canThreatenBattleship(
    const Battlefield *field,
    int index)
{
    const EscortShip *escort =
        &field->escorts[index];

    HitRecord shot;
    double distance;

    if (!escort->alive) {
        return 0;
    }

    distance = calculateDistance(
        escort->x,
        escort->y,
        field->battleship.x,
        field->battleship.y
    );

    return findShotSolution(
        distance,
        escort->minVelocity,
        escort->maxVelocity,
        escort->minAngle,
        escort->maxAngle,
        &shot
    );
}


/* Compare two possible targets */
static int isBetterTarget(
    const Battlefield *field,
    int first,
    int second)
{
    const EscortShip *a =
        &field->escorts[first];

    const EscortShip *b =
        &field->escorts[second];

    int threatA;
    int threatB;

    double distanceA;
    double distanceB;

    threatA =
        canThreatenBattleship(
            field,
            first
        );

    threatB =
        canThreatenBattleship(
            field,
            second
        );

    /* Priority 1: Escort that can hit B */
    if (threatA != threatB) {
        return threatA > threatB;
    }

    /* Priority 2: Higher impact power */
    if (a->impactPower != b->impactPower) {
        return a->impactPower >
               b->impactPower;
    }

    distanceA = calculateDistance(
        field->battleship.x,
        field->battleship.y,
        a->x,
        a->y
    );

    distanceB = calculateDistance(
        field->battleship.x,
        field->battleship.y,
        b->x,
        b->y
    );

    /* Priority 3: Nearer Escort */
    if (distanceA != distanceB) {
        return distanceA < distanceB;
    }

    /* Priority 4: Lower ID */
    return a->id < b->id;
}


/* Create attack order */
int buildAttackOrder(
    const Battlefield *field,
    int order[])
{
    int used[MAX_ESCORTS] = {0};

    int count = 0;
    int position;
    int i;

    for (position = 0;
         position < field->escortCount;
         position++) {

        int best = -1;

        for (i = 0;
             i < field->escortCount;
             i++) {

            if (!field->escorts[i].alive ||
                used[i]) {

                continue;
            }

            if (best == -1 ||
                isBetterTarget(
                    field,
                    i,
                    best
                )) {

                best = i;
            }
        }

        if (best == -1) {
            break;
        }

        order[count] = best;
        used[best] = 1;
        count++;
    }

    return count;
}
