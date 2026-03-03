#ifndef RUNSESSION_H
#define RUNSESSION_H

#include "ScoringSystem.h" // Tambahkan ini

class RunSession {
private:
    int currentRound;
    int maxRounds;
    bool isRunActive;
    double totalScore;       // Tambahkan untuk menampung skor
    ScoringSystem scorer;    // Tambahkan objek scoring system

public:
    RunSession();
    void startRun();
    void playHand();
    void enterShop();
    void endRun();
    bool isActive() const { return isRunActive; }
};

#endif