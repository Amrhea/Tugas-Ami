#ifndef RUNSESSION_H
#define RUNSESSION_H

#include "ScoringSystem.h"
#include <vector>
#include <random>
#include <algorithm>
#include "Card.h"
#include "modifier/IModifier.h"

// Baru: Enum untuk memisahkan 3 tipe blind
enum class BlindType {
    SmallBlind,
    HighBlind,
    BossBlind
};

class RunSession {
private:
    int currentRound;
    int maxRounds;
    bool isRunActive;
    
    // Total skor kumulatif sepanjang run (bersifat opsi statistik, tidak dipakai untuk cek menang kalah blind)
    double totalScore; 
    
    ScoringSystem scorer;
    std::vector<IModifier*> activeModifiers;
    std::vector<Card> deck;
    std::vector<std::string> playerCards; 

    // --- BARU: Mengatur Blind ---
    BlindType currentBlind;
    double currentBlindTarget;
    
    void generateDeck();
    void setBlindTarget(); // Fungsi utilitas mengatur target saat ronde berubah
    std::string getBlindName(BlindType type); // Mengambil nama String blind

public:
    RunSession();
    ~RunSession();
    void startRun();
    void playHand();
    void enterShop();
    void endRun();
    bool isActive() const { return isRunActive; }
};

#endif