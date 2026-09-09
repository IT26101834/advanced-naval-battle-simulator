#ifndef FILEIO_H
#define FILEIO_H

#include "battle.h"

int ensureResultsDirectory(void);

int saveBattlefield(
    const Battlefield *field,
    const char *filename,
    const char *title
);

int appendText(
    const char *filename,
    const char *text
);

int saveHit(
    const char *filename,
    const HitRecord *hit,
    const char *attacker
);

void showStatistics(void);

#endif
