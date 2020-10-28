#include <stdlib.h>
#include <stdio.h>
#include "3rdparty/thread.h"
#include "3rdparty/CV.h"

using namespace std;

int arrivedO = 0;
int arrivedH = 0;
int departingO = 0;
int departingH = 0;
Lock lock;
CV allArrived;
CV allDeparting;

void HArrives();
void OArrives();
bool enoughAtomsForWater();
bool allAtomsDeparting();
void makeWater();
void supplyH(int atomCount);
void supplyO(int atomCount);

int main()
{
    thread_t suppliers[3];

    thread_create(&suppliers[0], supplyH, 50);
    thread_create(&suppliers[1], supplyH, 50);
    thread_create(&suppliers[2], supplyO, 50);

    thread_join(suppliers[0]);
    thread_join(suppliers[1]);
    thread_join(suppliers[2]);

    return 0;
}

void HArrives()
{
    lock.acquire();

    // First thing as thread H thread arrives, it increments H atom counter
    // we need it to count that enough H atoms arrived to make water molecule
    arrivedH++;
    printf("H atom arrived. Now we have %d H atoms and %d O atoms.\n", arrivedH, arrivedO);

    if (!enoughAtomsForWater()) {
        // If there's not enough atoms for water, we wait for more atoms in the loop
        while (!enoughAtomsForWater()) {
            allArrived.wait(&lock);
        }
    } else {
        // Otherwise we call procedure to make water from waiting threads of atoms
        makeWater();
    }

    // We need to keep count of departing threads also, so our atoms departing
    // simultaneously inside molecule and preventing the case when new atoms arrived
    // before all previous departed
    departingH++;

    if (!allAtomsDeparting()){
        // Until all atoms ready checked for departing, wait in a loop
        while (!allAtomsDeparting()) {
            allDeparting.wait(&lock);
        }
    } else {
        // Otherwise reset arrived counter and wake up departing threads waiting to leave
        arrivedH = 0;
        arrivedO = 0;
        allDeparting.broadcast();
    }

    printf("H atom departed inside water molecule.\n");
    lock.release();
}

void OArrives()
{
    // Same algorithm from HArrives() procedure used here but for O atoms
    lock.acquire();
    arrivedO++;
    printf("O atom arrived. Now we have %d H atoms and %d O atoms.\n", arrivedH, arrivedO);

    if (!enoughAtomsForWater()) {
        while (!enoughAtomsForWater()) {
            allArrived.wait(&lock);
        }
    } else {
        makeWater();
    }

    departingO++;

    if (!allAtomsDeparting()) {
        while (!allAtomsDeparting()) {
            allDeparting.wait(&lock);
        }
    } else {
        arrivedO = 0;
        arrivedH = 0;
        allDeparting.broadcast();
    }

    printf("O atom departed inside water molecule.\n");
    lock.release();
}

bool enoughAtomsForWater()
{
    return arrivedH == 2 && arrivedO == 1;
}

bool allAtomsDeparting()
{
    return departingH == 2 && departingO == 1;
}


void makeWater()
{
    departingH = 0;
    departingO = 0;
    allArrived.broadcast();
}

void supplyH(int atomCount)
{
    for (int i = 0; i < atomCount; i++) {
        HArrives();
    }
}

void supplyO(int atomCount)
{
    for (int i = 0; i < atomCount; i++) {
        OArrives();
    }
}
