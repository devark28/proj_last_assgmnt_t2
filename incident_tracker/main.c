#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "incident_list.h"

#define SESSION_FILE "session.log"
#define LIVE_INTERVAL 2

static void printHelp(void) {
    printf("Commands: f=forward  b=backward  l=live  s=stop  d=delete all  q=quit\n");
}

static void liveMonitor(IncidentList *l) {
    printf("Live monitoring active. Press 's' to stop.\n");

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    time_t lastTick = time(NULL);

    while (1) {
        time_t now = time(NULL);
        if (now - lastTick >= LIVE_INTERVAL) {
            generateSyntheticIncident(l);
            printf("  [LIVE] New incident added:\n");
            Incident *newest = l->tail;
            printf("         #%d %-10s %s\n", newest->id, newest->source, newest->description);
            lastTick = now;
        }

        int c = getchar();
        if (c == EOF && errno == EAGAIN) continue;
        if (c == 's') break;
        if (c == 'f') {
            if (!listMoveForward(l)) printf("  Already at newest.\n");
            else printCurrentIncident(l);
        } else if (c == 'b') {
            if (!listMoveBackward(l)) printf("  Already at oldest.\n");
            else printCurrentIncident(l);
        }
    }

    fcntl(STDIN_FILENO, F_SETFL, flags);
    printf("Live monitoring stopped.\n");
}

int main(void) {
    IncidentList list;
    listInit(&list);

    listAdd(&list, "POLICE",    "Traffic collision on Main St");
    listAdd(&list, "FIRE",      "Structure fire at 45 Oak Ave");
    listAdd(&list, "AMBULANCE", "Cardiac arrest reported downtown");
    listAdd(&list, "POLICE",    "Armed robbery at Central Bank");
    listAdd(&list, "FIRE",      "Gas leak at Industrial Park");

    printf("Incident Tracker — %d incidents loaded\n", list.count);
    printHelp();
    printCurrentIncident(&list);
    printf("\n");

    int running = 1;
    while (running) {
        printf("> ");
        fflush(stdout);

        int c = getchar();
        if (c == EOF) break;
        if (c == '\n') continue;

        while (getchar() != '\n');

        switch (c) {
            case 'f':
                if (!listMoveForward(&list)) printf("  Already at newest.\n");
                else printCurrentIncident(&list);
                break;
            case 'b':
                if (!listMoveBackward(&list)) printf("  Already at oldest.\n");
                else printCurrentIncident(&list);
                break;
            case 'l':
                liveMonitor(&list);
                break;
            case 's':
                printf("  Not in live mode.\n");
                break;
            case 'd':
                listDeleteAll(&list);
                printf("  All incidents deleted.\n");
                break;
            case 'q':
                listSaveToFile(&list, SESSION_FILE);
                printf("  Session saved to %s.\n", SESSION_FILE);
                running = 0;
                break;
            default:
                printf("  Unknown command.\n");
                printHelp();
        }
        printf("\n");
    }

    return 0;
}
