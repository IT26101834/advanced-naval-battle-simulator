#include <stdlib.h>
#include "battle.h"
#include "fileio.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

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

/* Projectile range formula */
double projectileRange(
    double velocity,
    double angle)
{
    double radians = degreesToRadians(angle);

    return (
        velocity * velocity *
        sin(2.0 * radians)
    ) / GRAVITY;
}

/* Approximate flight time */
double calculateFlightTime(
    double distance,
    double velocity,
    double angle)
{
    double radians = degreesToRadians(angle);
    double horizontalSpeed =
        velocity * cos(radians);

    if (horizontalSpeed <= 0.0) {
        return -1.0;
    }

    return distance / horizontalSpeed;
}

/* Find a simple valid projectile solution */
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

/* Check whether one Escort can hit B */
static int escortCanHitBattleship(
    const Battlefield *field,
    int index,
    HitRecord *solution)
{
    const EscortShip *escort =
        &field->escorts[index];

    double distance;

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

/* Check whether B can hit an Escort */
static int battleshipCanHitEscort(
    const Battlefield *field,
    int index,
    double minAngle,
    HitRecord *solution)
{
    const EscortShip *escort =
        &field->escorts[index];

    double distance;

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

/* Part 1-A and Part 1-C */
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

    /* Clear previous hit file */
    {
        FILE *file = fopen(hitsFile, "w");

        if (file != NULL) {
            fclose(file);
        }
    }

    /*
     * Escort attacks first.
     * Part 1-A: one hit sinks B.
     * Part 1-C: damage is cumulative.
     */
    for (i = 0;
         i < field->escortCount;
         i++) {

        if (escortCanHitBattleship(
                field,
                i,
                &hit)) {

            if (cumulative == 0) {

                field->battleship.alive = 0;
                field->battleship.health = 0.0;

                printf(
                    "\nBattleship was destroyed by Escort %d.\n",
                    field->escorts[i].id
                );

                hit.targetId =
                    field->escorts[i].id;

                saveHit(
                    hitsFile,
                    &hit,
                    "Escort"
                );

                break;
            }
            else {

                field->battleship.cumulativeDamage +=
                    field->escorts[i].impactPower;

                field->battleship.health -=
                    field->escorts[i].impactPower;

                hit.targetId =
                    field->escorts[i].id;

                saveHit(
                    hitsFile,
                    &hit,
                    "Escort"
                );

                if (field->battleship.health <= 0.0) {

                    field->battleship.health = 0.0;
                    field->battleship.alive = 0;

                    printf(
                        "\nBattleship was destroyed after cumulative damage.\n"
                    );

                    break;
                }
            }
        }
    }

    /*
     * If B is still alive,
     * attack all reachable Escorts.
     */
    if (field->battleship.alive) {

        for (i = 0;
             i < field->escortCount;
             i++) {

            if (battleshipCanHitEscort(
                    field,
                    i,
                    1.0,
                    &hit)) {

                field->escorts[i].alive = 0;
                field->escorts[i].health = 0.0;

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
                "Battleship remaining health: %.2f\n",
                field->battleship.health
            );

            printf(
                "Cumulative damage: %.2f\n",
                field->battleship.cumulativeDamage
            );
        }
    }

    saveBattlefield(
        field,
        finalFile,
        "Final Battlefield"
    );
}

/* Part 1-B path simulation */
void runPart1B(
    const Battlefield *original,
    int points,
    double jamAngle,
    int cumulative)
{
    Battlefield field;
    int i;
    int p;

    field = *original;

    printf("\n--- Part 1-B Path Simulation ---\n");

    for (p = 0;
         p < points &&
         field.battleship.alive;
         p++) {

        /* Generate simple random path point */
        field.battleship.x =
            (double)(rand() %
            ((int)field.size + 1));

        field.battleship.y =
            (double)(rand() %
            ((int)field.size + 1));

        printf(
            "\nIteration %d - B position (%.2f, %.2f)\n",
            p + 1,
            field.battleship.x,
            field.battleship.y
        );

        /*
         * Escort attacks.
         * Destroyed escorts stay destroyed.
         */
        for (i = 0;
             i < field.escortCount;
             i++) {

            HitRecord hit;

            if (escortCanHitBattleship(
                    &field,
                    i,
                    &hit)) {

                if (cumulative) {

                    field.battleship.health -=
                        field.escorts[i].impactPower;

                    field.battleship.cumulativeDamage +=
                        field.escorts[i].impactPower;

                    if (field.battleship.health <= 0.0) {

                        field.battleship.health = 0.0;
                        field.battleship.alive = 0;

                        break;
                    }
                }
                else {

                    field.battleship.alive = 0;
                    field.battleship.health = 0.0;

                    break;
                }
            }
        }

        if (!field.battleship.alive) {

            printf(
                "Battleship destroyed at iteration %d.\n",
                p + 1
            );

            break;
        }

        /*
         * B attacks Escorts.
         * jamAngle > 0 means gun is restricted.
         */
        for (i = 0;
             i < field.escortCount;
             i++) {

            HitRecord hit;
            double minimumAngle = 1.0;

            if (jamAngle > 0.0 &&
                p >= points / 2) {

                minimumAngle = jamAngle;
            }

            if (battleshipCanHitEscort(
                    &field,
                    i,
                    minimumAngle,
                    &hit)) {

                field.escorts[i].alive = 0;
                field.escorts[i].health = 0.0;
            }
        }

        {
            char filename[120];

            snprintf(
                filename,
                sizeof(filename),
                "results/part1b_iteration_%d.txt",
                p + 1
            );

            saveBattlefield(
                &field,
                filename,
                "Part 1-B Iteration"
            );
        }
    }

    printf("\nPart 1-B simulation finished.\n");
}
