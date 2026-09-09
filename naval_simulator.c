#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_ESCORTS 50
#define GRAVITY 9.81
#define PI 3.14159265358979323846

typedef struct {
    int id;
    char type;
    double x;
    double y;
    double minVelocity;
    double maxVelocity;
    double minAngle;
    double maxAngle;
    double impactPower;
    int alive;
} EscortShip;

typedef struct {
    char type;
    double x;
    double y;
    double maxVelocity;
    int alive;
} BattleShip;

typedef struct {
    double size;
    int escortCount;
    BattleShip battleship;
    EscortShip escorts[MAX_ESCORTS];
} Battlefield;

typedef struct {
    int possible;
    double velocity;
    double angle;
    double flightTime;
} ShotSolution;

static double degreesToRadians(double degrees) {
    return degrees * PI / 180.0;
}

static double calculateDistance(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

static double projectileRange(double velocity, double angle) {
    return velocity * velocity * sin(2.0 * degreesToRadians(angle)) / GRAVITY;
}

static ShotSolution findShotSolution(double distance, double minVelocity,
                                     double maxVelocity, double minAngle,
                                     double maxAngle) {
    ShotSolution best = {0, 0.0, 0.0, 0.0};
    double angle;

    if (distance <= 0.0 || minVelocity <= 0.0 || maxVelocity < minVelocity) {
        return best;
    }

    for (angle = minAngle; angle <= maxAngle + 0.001; angle += 0.5) {
        double sine = sin(2.0 * degreesToRadians(angle));
        double requiredVelocity;
        double velocity;

        if (sine <= 0.0 || projectileRange(maxVelocity, angle) < distance) {
            continue;
        }
        requiredVelocity = sqrt(distance * GRAVITY / sine);
        velocity = requiredVelocity < minVelocity ? minVelocity : requiredVelocity;
        if (velocity <= maxVelocity) {
            double time = distance / (velocity * cos(degreesToRadians(angle)));
            if (!best.possible || time < best.flightTime) {
                best.possible = 1;
                best.velocity = velocity;
                best.angle = angle;
                best.flightTime = time;
            }
        }
    }
    return best;
}

static void setEscortType(EscortShip *escort, char type) {
    escort->type = type;
    switch (type) {
        case 'A':
            escort->impactPower = 0.08;
            escort->maxAngle = 20.0;
            break;
        case 'B':
            escort->impactPower = 0.06;
            escort->maxAngle = 30.0;
            break;
        case 'C':
            escort->impactPower = 0.07;
            escort->maxAngle = 25.0;
            break;
        case 'D':
            escort->impactPower = 0.05;
            escort->maxAngle = 50.0;
            break;
        default:
            escort->type = 'E';
            escort->impactPower = 0.04;
            escort->maxAngle = 70.0;
            break;
    }
}

static int readIntInRange(const char *prompt, int minimum, int maximum) {
    int value;
    while (printf("%s", prompt), scanf("%d", &value) != 1 ||
           value < minimum || value > maximum) {
        printf("Please enter a value from %d to %d.\n", minimum, maximum);
        while (getchar() != '\n') {
        }
    }
    return value;
}

static double readDoubleInRange(const char *prompt, double minimum, double maximum) {
    double value;
    while (printf("%s", prompt), scanf("%lf", &value) != 1 ||
           value < minimum || value > maximum) {
        printf("Please enter a value from %.2f to %.2f.\n", minimum, maximum);
        while (getchar() != '\n') {
        }
    }
    return value;
}

static void setupBattlefield(Battlefield *field) {
    int i;
    const char types[] = {'A', 'B', 'C', 'D', 'E'};

    field->size = readDoubleInRange("Enter canvas size D: ", 1.0, 1000000.0);
    field->escortCount = readIntInRange("Enter number of escort ships: ", 1, MAX_ESCORTS);
    do {
        printf("Enter Battleship type (U/M/R/S): ");
        scanf(" %c", &field->battleship.type);
    } while (field->battleship.type != 'U' && field->battleship.type != 'M' &&
             field->battleship.type != 'R' && field->battleship.type != 'S');
    field->battleship.x = readDoubleInRange("Enter Battleship X position: ", 0.0, field->size);
    field->battleship.y = readDoubleInRange("Enter Battleship Y position: ", 0.0, field->size);
    field->battleship.maxVelocity =
        readDoubleInRange("Enter Battleship maximum velocity: ", 1.0, 100000.0);
    field->battleship.alive = 1;

    /* A fixed seed makes generated battlefields reproducible for testing. */
    srand(10);
    for (i = 0; i < field->escortCount; i++) {
        EscortShip *escort = &field->escorts[i];
        escort->id = i + 1;
        escort->x = field->size * rand() / (double)RAND_MAX;
        escort->y = field->size * rand() / (double)RAND_MAX;
        setEscortType(escort, types[rand() % 5]);
        escort->minAngle = 1.0 + rand() % (int)(89.0 - escort->maxAngle);
        escort->maxAngle += escort->minAngle;
        escort->minVelocity = 20.0 + rand() % 21;
        escort->maxVelocity = escort->minVelocity + 20.0 + rand() % 31;
        escort->alive = 1;
    }
    printf("Battlefield setup completed.\n");
}

static void showBattlefield(const Battlefield *field) {
    int i;
    printf("\nCanvas: %.2f x %.2f\nBattleship %c at (%.2f, %.2f), max velocity %.2f\n",
           field->size, field->size, field->battleship.type, field->battleship.x,
           field->battleship.y, field->battleship.maxVelocity);
    for (i = 0; i < field->escortCount; i++) {
        const EscortShip *e = &field->escorts[i];
        printf("Escort %d (E%c): (%.2f, %.2f), velocity %.2f-%.2f, angle %.0f-%.0f, "
               "impact %.2f\n", e->id, e->type, e->x, e->y, e->minVelocity,
               e->maxVelocity, e->minAngle, e->maxAngle, e->impactPower);
    }
}

static int canEscortHitBattleship(const Battlefield *field, const EscortShip *escort,
                                  ShotSolution *solution) {
    double distance = calculateDistance(escort->x, escort->y, field->battleship.x,
                                        field->battleship.y);
    *solution = findShotSolution(distance, escort->minVelocity, escort->maxVelocity,
                                 escort->minAngle, escort->maxAngle);
    return solution->possible;
}

static int canBattleshipHitEscort(const Battlefield *field, const EscortShip *escort,
                                  ShotSolution *solution) {
    double distance = calculateDistance(field->battleship.x, field->battleship.y,
                                        escort->x, escort->y);
    *solution = findShotSolution(distance, 1.0, field->battleship.maxVelocity, 1.0, 89.0);
    return solution->possible;
}

static int saveInitialConditions(const Battlefield *field, const char *path) {
    FILE *file = fopen(path, "w");
    int i;
    if (file == NULL) {
        perror(path);
        return 0;
    }
    fprintf(file, "Part 1-A initial battlefield\nCanvas %.2f\n", field->size);
    fprintf(file, "Battleship %c position %.2f %.2f max_velocity %.2f\n",
            field->battleship.type, field->battleship.x, field->battleship.y,
            field->battleship.maxVelocity);
    for (i = 0; i < field->escortCount; i++) {
        const EscortShip *e = &field->escorts[i];
        fprintf(file, "Escort %d E%c position %.2f %.2f velocity %.2f %.2f "
                     "angle %.2f %.2f impact %.2f\n", e->id, e->type, e->x, e->y,
                e->minVelocity, e->maxVelocity, e->minAngle, e->maxAngle,
                e->impactPower);
    }
    fclose(file);
    return 1;
}

static int saveFinalConditions(const Battlefield *field, const char *path) {
    FILE *file = fopen(path, "w");
    int i;
    if (file == NULL) {
        perror(path);
        return 0;
    }
    fprintf(file, "Part 1-A final battlefield\nBattleship alive: %s\n",
            field->battleship.alive ? "yes" : "no");
    for (i = 0; i < field->escortCount; i++) {
        fprintf(file, "Escort %d alive: %s\n", field->escorts[i].id,
                field->escorts[i].alive ? "yes" : "no");
    }
    fclose(file);
    return 1;
}

static void runPart1A(Battlefield *field) {
    int i;
    int sinkingEscort = -1;
    double earliestTime = 0.0;
    ShotSolution solution;
    ShotSolution sinkingSolution = {0, 0.0, 0.0, 0.0};
    FILE *hits;

    system("mkdir -p results");
    saveInitialConditions(field, "results/part1a_initial.txt");
    hits = fopen("results/part1a_hits.txt", "w");
    if (hits == NULL) {
        perror("results/part1a_hits.txt");
        return;
    }
    for (i = 0; i < field->escortCount; i++) {
        if (canEscortHitBattleship(field, &field->escorts[i], &solution) &&
            (sinkingEscort < 0 || solution.flightTime < earliestTime)) {
            sinkingEscort = i;
            earliestTime = solution.flightTime;
            sinkingSolution = solution;
        }
    }
    if (sinkingEscort >= 0) {
        field->battleship.alive = 0;
        printf("Battleship was sunk by Escort %d after %.2f seconds.\n",
               field->escorts[sinkingEscort].id, earliestTime);
        fprintf(hits, "Battleship sunk by Escort %d, time %.2f, velocity %.2f, angle %.2f\n",
                field->escorts[sinkingEscort].id, earliestTime, sinkingSolution.velocity,
                sinkingSolution.angle);
    } else {
        int hitCount = 0;
        double duration = 0.0;
        printf("No Escort can hit the Battleship. Battleship attacks now.\n");
        for (i = 0; i < field->escortCount; i++) {
            if (canBattleshipHitEscort(field, &field->escorts[i], &solution)) {
                field->escorts[i].alive = 0;
                hitCount++;
                if (solution.flightTime > duration) {
                    duration = solution.flightTime;
                }
                fprintf(hits, "Escort %d hit, time %.2f, velocity %.2f, angle %.2f\n",
                        field->escorts[i].id, solution.flightTime, solution.velocity,
                        solution.angle);
            }
        }
        printf("Battleship hit %d Escort ship(s); battle duration %.2f seconds.\n",
               hitCount, duration);
    }
    fclose(hits);
    saveFinalConditions(field, "results/part1a_final.txt");
}

static void showInstructions(void) {
    printf("\nSetup a battlefield, view its generated ships, then run Part 1-A.\n");
    printf("Projectile range uses R = v^2 sin(2 theta) / g with g = 9.81.\n");
    printf("If several Escorts can hit first, the earliest calculated impact wins.\n");
}

int main(void) {
    Battlefield field = {0};
    int setupDone = 0;
    int choice;

    do {
        printf("\n=== NAVAL BATTLE SIMULATOR ===\n");
        printf("1. Setup Battlefield\n2. Show Battlefield\n3. Run Part 1-A\n");
        printf("4. View Instructions\n0. Exit\n");
        choice = readIntInRange("Enter your choice: ", 0, 4);
        if (choice == 1) {
            setupBattlefield(&field);
            setupDone = 1;
        } else if (choice == 2) {
            if (setupDone) showBattlefield(&field);
            else printf("Please setup the battlefield first.\n");
        } else if (choice == 3) {
            if (setupDone) runPart1A(&field);
            else printf("Please setup the battlefield first.\n");
        } else if (choice == 4) {
            showInstructions();
        }
    } while (choice != 0);
    return 0;
}
