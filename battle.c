#include <stdlib.h>

#include "battle.h"
#include "fileio.h"
#include "strategy.h"

#include <math.h>
#include <stdio.h>

#define GRAVITY 9.81
#define PI 3.14159265358979323846


/* Convert degrees to radians */
double degreesToRadians(double degrees)
{
    return degrees * PI / 180.0;
}


/* Calculate distance between two ships */
double calculateDistance(
    double x1,
    double y1,
    double x2,
    double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;

    return sqrt(dx * dx + dy * dy);
}


/* Calculate projectile range */
double projectileRange(
    double velocity,
    double angle)
{
    double radians;

    radians = degreesToRadians(angle);

    return (
        velocity * velocity *
        sin(2.0 * radians)
    ) / GRAVITY;
}


/* Calculate approximate flight time */
double calculateFlightTime(
    double distance,
    double velocity,
    double angle)
{
    double radians;
    double horizontalSpeed;

    radians = degreesToRadians(angle);

    horizontalSpeed =
        velocity * cos(radians);

    if (horizontalSpeed <= 0.0) {
        return -1.0;
    }

    return distance / horizontalSpeed;
}


/* Find a possible projectile shot */
int findShotSolution(
    double distance,
    double minVelocity,
    double maxVelocity,
    double minAngle,
    double maxAngle,
    HitRecord *solution)
{
    double velocity;
    double angle;

    for (velocity = minVelocity;
         velocity <= maxVelocity;
         velocity += 1.0) {

        for (angle = minAngle;
             angle <= maxAngle;
             angle += 1.0) {

            double range =
                projectileRange(
                    velocity,
                    angle
                );

            if (range >= distance) {

                solution->velocity =
                    velocity;

                solution->angle =
                    angle;

                solution->time =
                    calculateFlightTime(
                        distance,
                        velocity,
                        angle
                    );

                return 1;
            }
        }
    }

    return 0;
}


/* Check whether an Escort can hit Battleship */
static int escortCanHitBattleship(
    const Battlefield *field,
    int index,
    HitRecord *solution)
{
    const EscortShip *escort;
    double distance;

    escort = &field->escorts[index];

    if (!escort->alive) {
        return 0;
    }

    distance =
        calculateDistance(
            escort->x,
            escort->y,
            field->battleship.x,
            field->battleship.y
        );

    solution->targetId = 0;

    return findShotSolution(
        distance,
        escort->minVelocity,
        escort->maxVelocity,
        escort->minAngle,
        escort->maxAngle,
        solution
    );
}


/* Check whether Battleship can hit an Escort */
static int battleshipCanHitEscort(
    const Battlefield *field,
    int index,
    double minAngle,
    HitRecord *solution)
{
    const EscortShip *escort;
    double distance;

    escort = &field->escorts[index];

    if (!escort->alive) {
        return 0;
    }

    distance =
        calculateDistance(
            field->battleship.x,
            field->battleship.y,
            escort->x,
            escort->y
        );

    solution->targetId =
        escort->id;

    return findShotSolution(
        distance,
        1.0,
        field->battleship.maxVelocity,
        minAngle,
        89.0,
        solution
    );
}


/* Count alive Escort ships */
static int countAliveEscorts(
    const Battlefield *field)
{
    int i;
    int count = 0;

    for (i = 0;
         i < field->escortCount;
         i++) {

        if (field->escorts[i].alive) {
            count++;
        }
    }

    return count;
}


/* Clear a result file */
static void clearResultFile(
    const char *filename)
{
    FILE *file;

    ensureResultsDirectory();

    file = fopen(filename, "w");

    if (file != NULL) {
        fclose(file);
    }
}


/* ================================================= */
/* PART 1-A AND PART 1-C                             */
/* ================================================= */

void runPart1A(
    Battlefield *field,
    int cumulative,
    const char *label)
{
    int i;
    int hitCount = 0;

    double battleTime = 0.0;

    HitRecord hit;

    char initialFile[100];
    char hitsFile[100];
    char finalFile[100];

    snprintf(
        initialFile,
        sizeof(initialFile),
        "results/%s_initial.txt",
        label
    );

    snprintf(
        hitsFile,
        sizeof(hitsFile),
        "results/%s_hits.txt",
        label
    );

    snprintf(
        finalFile,
        sizeof(finalFile),
        "results/%s_final.txt",
        label
    );

    ensureResultsDirectory();

    saveBattlefield(
        field,
        initialFile,
        "Initial Battlefield"
    );

    clearResultFile(hitsFile);


    /*
     * Part 1-A:
     * One Escort hit can destroy B.
     * Earliest successful shell is selected.
     */
    if (!cumulative) {

        int attackerIndex = -1;

        double earliestTime = 0.0;

        HitRecord earliestHit;

        for (i = 0;
             i < field->escortCount;
             i++) {

            if (escortCanHitBattleship(
                    field,
                    i,
                    &hit)) {

                if (attackerIndex == -1 ||
                    hit.time < earliestTime) {

                    attackerIndex = i;
                    earliestTime = hit.time;
                    earliestHit = hit;
                }
            }
        }

        if (attackerIndex != -1) {

            field->battleship.alive = 0;
            field->battleship.health = 0.0;

            earliestHit.targetId =
                field->escorts[
                    attackerIndex
                ].id;

            saveHit(
                hitsFile,
                &earliestHit,
                "Escort"
            );

            printf(
                "\nBattleship was destroyed by Escort %d.\n",
                field->escorts[
                    attackerIndex
                ].id
            );
        }
    }


    /*
     * Part 1-C:
     * Escort damage is cumulative.
     */
    if (cumulative) {

        for (i = 0;
             i < field->escortCount;
             i++) {

            if (escortCanHitBattleship(
                    field,
                    i,
                    &hit)) {

                field->battleship
                    .cumulativeDamage +=
                    field->escorts[i]
                    .impactPower;

                field->battleship
                    .health -=
                    field->escorts[i]
                    .impactPower;

                hit.targetId =
                    field->escorts[i].id;

                saveHit(
                    hitsFile,
                    &hit,
                    "Escort"
                );

                if (field->battleship
                        .health <= 0.0) {

                    field->battleship
                        .health = 0.0;

                    field->battleship
                        .alive = 0;

                    printf(
                        "\nBattleship destroyed by cumulative damage.\n"
                    );

                    break;
                }
            }
        }
    }


    /* Battleship attacks if it survived */
    if (field->battleship.alive) {

        for (i = 0;
             i < field->escortCount;
             i++) {

            if (battleshipCanHitEscort(
                    field,
                    i,
                    1.0,
                    &hit)) {

                field->escorts[i]
                    .alive = 0;

                field->escorts[i]
                    .health = 0.0;

                hitCount++;

                if (hit.time > battleTime) {
                    battleTime = hit.time;
                }

                saveHit(
                    hitsFile,
                    &hit,
                    "Battleship"
                );
            }
        }

        printf(
            "\nBattleship destroyed %d Escort ships.\n",
            hitCount
        );

        printf(
            "Battle duration: %.2f seconds\n",
            battleTime
        );

        if (cumulative) {

            printf(
                "Battleship health: %.2f\n",
                field->battleship.health
            );

            printf(
                "Cumulative damage: %.2f\n",
                field->battleship
                    .cumulativeDamage
            );
        }
    }


    saveBattlefield(
        field,
        finalFile,
        "Final Battlefield"
    );
}


/* ================================================= */
/* PART 1-B                                          */
/* ================================================= */

void runPart1B(
    const Battlefield *original,
    int points,
    double jamAngle,
    int cumulative)
{
    Battlefield field;

    int i;
    int p;

    const char *simulationName;

    field = *original;

    /*
     * Same seed creates the same path
     * for Simulation 1 and Simulation 2.
     */
    srand(original->seed);

    if (jamAngle > 0.0) {
        simulationName = "sim2";
    }
    else {
        simulationName = "sim1";
    }

    printf(
        "\n--- Part 1-B %s ---\n",
        simulationName
    );


    for (p = 0;
         p < points &&
         field.battleship.alive;
         p++) {

        char filename[150];
        char title[100];


        /* Generate Battleship path position */
        field.battleship.x =
            (double)(
                rand() %
                ((int)field.size + 1)
            );

        field.battleship.y =
            (double)(
                rand() %
                ((int)field.size + 1)
            );

        printf(
            "\nIteration %d - B position (%.2f, %.2f)\n",
            p + 1,
            field.battleship.x,
            field.battleship.y
        );


        /* Escort ships attack Battleship */
        for (i = 0;
             i < field.escortCount;
             i++) {

            HitRecord hit;

            if (escortCanHitBattleship(
                    &field,
                    i,
                    &hit)) {

                if (cumulative) {

                    field.battleship
                        .health -=
                        field.escorts[i]
                        .impactPower;

                    field.battleship
                        .cumulativeDamage +=
                        field.escorts[i]
                        .impactPower;

                    if (field.battleship
                            .health <= 0.0) {

                        field.battleship
                            .health = 0.0;

                        field.battleship
                            .alive = 0;

                        break;
                    }
                }
                else {

                    field.battleship
                        .health = 0.0;

                    field.battleship
                        .alive = 0;

                    break;
                }
            }
        }


        /*
         * Battleship attacks if it survived.
         */
        if (field.battleship.alive) {

            for (i = 0;
                 i < field.escortCount;
                 i++) {

                HitRecord hit;

                double minimumAngle = 1.0;

                /*
                 * Simulation 2 gun jam.
                 * Jam is applied halfway through path.
                 */
                if (jamAngle > 0.0 &&
                    p >= points / 2) {

                    minimumAngle =
                        jamAngle;
                }

                if (battleshipCanHitEscort(
                        &field,
                        i,
                        minimumAngle,
                        &hit)) {

                    field.escorts[i]
                        .alive = 0;

                    field.escorts[i]
                        .health = 0.0;
                }
            }
        }


        /*
         * Save every iteration.
         * Simulation 1 and 2 use
         * different filenames.
         */
        snprintf(
            filename,
            sizeof(filename),
            "results/part1b_%s_iteration_%d.txt",
            simulationName,
            p + 1
        );

        snprintf(
            title,
            sizeof(title),
            "Part 1-B %s Iteration %d",
            simulationName,
            p + 1
        );

        saveBattlefield(
            &field,
            filename,
            title
        );


        if (!field.battleship.alive) {

            printf(
                "Battleship destroyed at iteration %d.\n",
                p + 1
            );

            break;
        }
    }


    printf(
        "\nPart 1-B %s finished.\n",
        simulationName
    );
}


/* ================================================= */
/* PART 2-A                                          */
/* ================================================= */

void runPart2A(
    Battlefield *field)
{
    int order[MAX_ESCORTS];

    int escortFired[MAX_ESCORTS] =
        {0};

    int count;
    int i;
    int j;

    double currentTime = 0.0;

    char line[200];


    clearResultFile(
        "results/part2a.txt"
    );


    count =
        buildAttackOrder(
            field,
            order
        );


    appendText(
        "results/part2a.txt",
        "PART 2-A ATTACK ORDER\n"
    );


    printf(
        "\n--- Part 2-A ---\n"
    );

    printf(
        "Battleship attack order: "
    );


    for (i = 0;
         i < count;
         i++) {

        printf(
            "%d ",
            field->escorts[
                order[i]
            ].id
        );

        snprintf(
            line,
            sizeof(line),
            "Target %d: Escort %d\n",
            i + 1,
            field->escorts[
                order[i]
            ].id
        );

        appendText(
            "results/part2a.txt",
            line
        );
    }


    printf("\n");


    for (i = 0;
         i < count &&
         field->battleship.alive;
         i++) {

        int index =
            order[i];

        HitRecord shot;


        if (!field->escorts[index]
                .alive) {

            continue;
        }


        if (battleshipCanHitEscort(
                field,
                index,
                1.0,
                &shot)) {

            field->escorts[index]
                .alive = 0;

            field->escorts[index]
                .health = 0.0;

            field->battleship
                .firingCount++;

            snprintf(
                line,
                sizeof(line),
                "Time %.2f: B destroyed Escort %d\n",
                currentTime,
                field->escorts[index].id
            );

            appendText(
                "results/part2a.txt",
                line
            );

            printf(
                "Time %.2f: B destroyed Escort %d\n",
                currentTime,
                field->escorts[index].id
            );
        }


        currentTime +=
            field->battleship
                .firingInterval;


        /*
         * Remaining Escorts can attack once.
         */
        for (j = 0;
             j < field->escortCount;
             j++) {

            HitRecord escortShot;


            if (!field->escorts[j]
                    .alive ||
                escortFired[j]) {

                continue;
            }


            if (escortCanHitBattleship(
                    field,
                    j,
                    &escortShot)) {

                escortFired[j] = 1;

                field->battleship
                    .health -=
                    field->escorts[j]
                    .impactPower;

                field->battleship
                    .cumulativeDamage +=
                    field->escorts[j]
                    .impactPower;


                if (field->battleship
                        .health <= 0.0) {

                    field->battleship
                        .health = 0.0;

                    field->battleship
                        .alive = 0;

                    printf(
                        "Battleship destroyed.\n"
                    );

                    break;
                }
            }
        }
    }


    saveBattlefield(
        field,
        "results/part2a_final.txt",
        "Part 2-A Final Battlefield"
    );


    printf(
        "Part 2-A finished at %.2f seconds.\n",
        currentTime
    );
}


/* ================================================= */
/* PART 2-B                                          */
/* ================================================= */

void runPart2B(
    Battlefield *field,
    double maximumTime)
{
    double currentTime = 0.0;

    double nextBFire = 0.0;

    double nextEscortFire[
        MAX_ESCORTS
    ] = {0.0};

    int i;

    char line[200];


    clearResultFile(
        "results/part2b.txt"
    );


    printf(
        "\n--- Part 2-B ---\n"
    );


    while (
        currentTime <= maximumTime &&
        field->battleship.alive &&
        countAliveEscorts(field) > 0) {


        /* Battleship firing event */
        if (currentTime >= nextBFire) {

            int order[MAX_ESCORTS];

            int count;
            int position;


            count =
                buildAttackOrder(
                    field,
                    order
                );


            for (position = 0;
                 position < count;
                 position++) {

                int index =
                    order[position];

                HitRecord shot;


                if (battleshipCanHitEscort(
                        field,
                        index,
                        1.0,
                        &shot)) {

                    field->escorts[index]
                        .alive = 0;

                    field->escorts[index]
                        .health = 0.0;

                    field->battleship
                        .firingCount++;

                    snprintf(
                        line,
                        sizeof(line),
                        "Time %.2f: B destroyed Escort %d\n",
                        currentTime,
                        field->escorts[index].id
                    );

                    appendText(
                        "results/part2b.txt",
                        line
                    );

                    break;
                }
            }


            nextBFire +=
                field->battleship
                    .firingInterval;
        }


        /* Escort firing events */
        for (i = 0;
             i < field->escortCount;
             i++) {

            HitRecord shot;


            if (!field->escorts[i]
                    .alive) {

                continue;
            }


            if (currentTime >=
                nextEscortFire[i]) {


                if (escortCanHitBattleship(
                        field,
                        i,
                        &shot)) {

                    field->battleship
                        .health -=
                        field->escorts[i]
                        .impactPower;

                    field->battleship
                        .cumulativeDamage +=
                        field->escorts[i]
                        .impactPower;

                    field->escorts[i]
                        .firingCount++;


                    snprintf(
                        line,
                        sizeof(line),
                        "Time %.2f: Escort %d hit B\n",
                        currentTime,
                        field->escorts[i].id
                    );

                    appendText(
                        "results/part2b.txt",
                        line
                    );


                    if (field->battleship
                            .health <= 0.0) {

                        field->battleship
                            .health = 0.0;

                        field->battleship
                            .alive = 0;

                        break;
                    }
                }


                nextEscortFire[i] +=
                    field->escorts[i]
                        .firingInterval;
            }
        }


        currentTime += 1.0;
    }


    saveBattlefield(
        field,
        "results/part2b_final.txt",
        "Part 2-B Final Battlefield"
    );


    printf(
        "Part 2-B finished at %.2f seconds.\n",
        currentTime
    );

    printf(
        "Battleship health: %.2f\n",
        field->battleship.health
    );
}


/* ================================================= */
/* PART 2-C                                          */
/* ================================================= */

void runPart2C(
    Battlefield *field,
    double maximumTime)
{
    double currentTime = 0.0;

    double nextBFire = 0.0;

    double nextEscortFire[
        MAX_ESCORTS
    ] = {0.0};

    int i;

    char line[220];


    clearResultFile(
        "results/part2c.txt"
    );


    printf(
        "\n--- Part 2-C ---\n"
    );


    while (
        currentTime <= maximumTime &&
        field->battleship.alive &&
        countAliveEscorts(field) > 0) {


        /*
         * Battleship firing with
         * impact power degradation.
         */
        if (currentTime >= nextBFire) {

            int order[MAX_ESCORTS];

            int count;
            int position;


            count =
                buildAttackOrder(
                    field,
                    order
                );


            for (position = 0;
                 position < count;
                 position++) {

                int index =
                    order[position];

                HitRecord shot;


                if (battleshipCanHitEscort(
                        field,
                        index,
                        1.0,
                        &shot)) {


                    field->battleship
                        .currentImpactPower =
                        exp(
                            -field->battleship
                                .gamma *
                            field->battleship
                                .firingCount
                        );


                    field->escorts[index]
                        .health -=
                        field->battleship
                        .currentImpactPower;


                    field->battleship
                        .firingCount++;


                    if (field->escorts[index]
                            .health <= 0.0) {

                        field->escorts[index]
                            .health = 0.0;

                        field->escorts[index]
                            .alive = 0;
                    }


                    snprintf(
                        line,
                        sizeof(line),
                        "Time %.2f: B fired at Escort %d, impact %.3f, remaining health %.3f\n",
                        currentTime,
                        field->escorts[index].id,
                        field->battleship
                            .currentImpactPower,
                        field->escorts[index]
                            .health
                    );


                    appendText(
                        "results/part2c.txt",
                        line
                    );

                    break;
                }
            }


            nextBFire +=
                field->battleship
                    .firingInterval;
        }


        /*
         * Escort firing with
         * impact degradation.
         */
        for (i = 0;
             i < field->escortCount;
             i++) {

            HitRecord shot;


            if (!field->escorts[i]
                    .alive) {

                continue;
            }


            if (currentTime >=
                nextEscortFire[i]) {


                if (escortCanHitBattleship(
                        field,
                        i,
                        &shot)) {


                    field->escorts[i]
                        .currentImpactPower =
                        field->escorts[i]
                            .impactPower *
                        exp(
                            -field->escorts[i]
                                .gamma *
                            field->escorts[i]
                                .firingCount
                        );


                    field->battleship
                        .health -=
                        field->escorts[i]
                        .currentImpactPower;


                    field->battleship
                        .cumulativeDamage +=
                        field->escorts[i]
                        .currentImpactPower;


                    field->escorts[i]
                        .firingCount++;


                    snprintf(
                        line,
                        sizeof(line),
                        "Time %.2f: Escort %d hit B, impact %.3f, B health %.3f\n",
                        currentTime,
                        field->escorts[i].id,
                        field->escorts[i]
                            .currentImpactPower,
                        field->battleship
                            .health
                    );


                    appendText(
                        "results/part2c.txt",
                        line
                    );


                    if (field->battleship
                            .health <= 0.0) {

                        field->battleship
                            .health = 0.0;

                        field->battleship
                            .alive = 0;

                        break;
                    }
                }


                nextEscortFire[i] +=
                    field->escorts[i]
                        .firingInterval;
            }
        }


        currentTime += 1.0;
    }


    saveBattlefield(
        field,
        "results/part2c_final.txt",
        "Part 2-C Final Battlefield"
    );


    printf(
        "Part 2-C finished at %.2f seconds.\n",
        currentTime
    );

    printf(
        "Battleship health: %.3f\n",
        field->battleship.health
    );
}
