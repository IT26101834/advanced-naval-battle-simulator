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

/* Save battlefield information to a text file */
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

    fprintf(file, "Battleship\n");

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
        "State: %s\n",
        field->battleship.alive
            ? "alive"
            : "destroyed"
    );

    fprintf(file, "\nEscort Ships\n");

    for (i = 0; i < field->escortCount; i++) {

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
            "Impact Power: %.2f\n",
            escort->impactPower
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

/* Add text to an existing result file */
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

/* Save details about a successful hit */
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

/* Statistics will be completed in Commit 5 */
void showStatistics(void)
{
    printf("\nSimulation statistics will be added later.\n");
}
