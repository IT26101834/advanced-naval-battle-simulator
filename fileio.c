#include "fileio.h"

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>


/* Create results folder if it does not exist */
int ensureResultsDirectory(void)
{
    if (mkdir("results", 0775) == 0 ||
        errno == EEXIST) {

        return 1;
    }

    printf("Cannot create results directory.\n");

    return 0;
}


/* Save complete battlefield information */
int saveBattlefield(
    const Battlefield *field,
    const char *filename,
    const char *title)
{
    FILE *file;
    int i;

    if (!ensureResultsDirectory()) {
        return 0;
    }

    file = fopen(filename, "w");

    if (file == NULL) {
        printf("Cannot open %s\n", filename);
        return 0;
    }

    fprintf(file, "%s\n", title);

    fprintf(
        file,
        "Canvas Size: %.2f x %.2f\n",
        field->size,
        field->size
    );

    fprintf(
        file,
        "Seed: %u\n\n",
        field->seed
    );


    /* Battleship information */
    fprintf(file, "BATTLESHIP\n");

    fprintf(
        file,
        "Type: %c\n",
        field->battleship.type
    );

    fprintf(
        file,
        "Position: (%.2f, %.2f)\n",
        field->battleship.x,
        field->battleship.y
    );

    fprintf(
        file,
        "Maximum Velocity: %.2f\n",
        field->battleship.maxVelocity
    );

    fprintf(
        file,
        "Health: %.3f\n",
        field->battleship.health
    );

    fprintf(
        file,
        "Cumulative Damage: %.3f\n",
        field->battleship.cumulativeDamage
    );

    fprintf(
        file,
        "Firing Interval: %.2f\n",
        field->battleship.firingInterval
    );

    fprintf(
        file,
        "Firing Count: %d\n",
        field->battleship.firingCount
    );

    fprintf(
        file,
        "Gamma: %.4f\n",
        field->battleship.gamma
    );

    fprintf(
        file,
        "Current Impact Power: %.4f\n",
        field->battleship.currentImpactPower
    );

    fprintf(
        file,
        "State: %s\n",
        field->battleship.alive
            ? "alive"
            : "destroyed"
    );


    /* Escort ship information */
    fprintf(file, "\nESCORT SHIPS\n");

    for (i = 0;
         i < field->escortCount;
         i++) {

        const EscortShip *escort =
            &field->escorts[i];

        fprintf(
            file,
            "\nID: %d\n",
            escort->id
        );

        fprintf(
            file,
            "Type: %s\n",
            escort->type
        );

        fprintf(
            file,
            "Position: (%.2f, %.2f)\n",
            escort->x,
            escort->y
        );

        fprintf(
            file,
            "Velocity: %.2f - %.2f\n",
            escort->minVelocity,
            escort->maxVelocity
        );

        fprintf(
            file,
            "Angles: %.2f - %.2f\n",
            escort->minAngle,
            escort->maxAngle
        );

        fprintf(
            file,
            "Initial Impact Power: %.3f\n",
            escort->impactPower
        );

        fprintf(
            file,
            "Health: %.3f\n",
            escort->health
        );

        fprintf(
            file,
            "Firing Interval: %.2f\n",
            escort->firingInterval
        );

        fprintf(
            file,
            "Firing Count: %d\n",
            escort->firingCount
        );

        fprintf(
            file,
            "Gamma: %.4f\n",
            escort->gamma
        );

        fprintf(
            file,
            "Current Impact Power: %.4f\n",
            escort->currentImpactPower
        );

        fprintf(
            file,
            "State: %s\n",
            escort->alive
                ? "alive"
                : "destroyed"
        );
    }

    fclose(file);

    return 1;
}


/* Add normal text to a result file */
int appendText(
    const char *filename,
    const char *text)
{
    FILE *file;

    if (!ensureResultsDirectory()) {
        return 0;
    }

    file = fopen(filename, "a");

    if (file == NULL) {
        return 0;
    }

    fputs(text, file);

    fclose(file);

    return 1;
}


/* Save one successful shot */
int saveHit(
    const char *filename,
    const HitRecord *hit,
    const char *attacker)
{
    FILE *file;

    if (!ensureResultsDirectory()) {
        return 0;
    }

    file = fopen(filename, "a");

    if (file == NULL) {
        return 0;
    }

    fprintf(
        file,
        "%s target=%d time=%.3f velocity=%.3f angle=%.3f\n",
        attacker,
        hit->targetId,
        hit->time,
        hit->velocity,
        hit->angle
    );

    fclose(file);

    return 1;
}


/* Display one saved text file */
static int displayResultFile(
    const char *filename)
{
    FILE *file;
    char line[300];

    file = fopen(filename, "r");

    if (file == NULL) {
        return 0;
    }

    printf(
        "\n------------------------------------\n"
    );

    printf(
        "File: %s\n",
        filename
    );

    printf(
        "------------------------------------\n"
    );

    while (fgets(
               line,
               sizeof(line),
               file) != NULL) {

        printf("%s", line);
    }

    fclose(file);

    return 1;
}


/* Load previous simulation results */
void showStatistics(void)
{
    int found = 0;

    printf(
        "\n========== SIMULATION STATISTICS ==========\n"
    );

    found += displayResultFile(
        "results/part1a_final.txt"
    );

    found += displayResultFile(
        "results/part1c_final.txt"
    );

    found += displayResultFile(
        "results/part2a_final.txt"
    );

    found += displayResultFile(
        "results/part2b_final.txt"
    );

    found += displayResultFile(
        "results/part2c_final.txt"
    );

    if (found == 0) {

        printf(
            "No saved simulation results available.\n"
        );
    }
}
