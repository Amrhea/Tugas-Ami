#ifndef RUNSESSION_H
#define RUNSESSION_H

#include <iostream>

class RunSession {
private:
    int currentRound;
    int maxRounds;
    bool isRunActive;

public:
    RunSession();           // Hanya deklarasi
    void startRun();        // Hanya deklarasi
    void playHand();        // Hanya deklarasi
    void enterShop();       // Hanya deklarasi
    void endRun();          // Hanya deklarasi
    
    bool isActive() const { return isRunActive; }
};

#endif