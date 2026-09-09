#ifndef STRATEGY_H
#define STRATEGY_H

#include "battle.h"

/* Build Battleship attack order */
int buildAttackOrder(
    const Battlefield *field,
    int order[]
);

#endif
