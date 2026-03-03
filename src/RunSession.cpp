#include "RunSession.h"
#include <iostream>

RunSession::RunSession() : currentRound(1), maxRounds(3), isRunActive(true), totalScore(0) {}

void RunSession::startRun() {
    std::cout << "--- Game Started ---" << std::endl;
    totalScore = 0;
}

void RunSession::playHand() {
    std::cout << "\nRound " << currentRound << ": Playing hand..." << std::endl;
    
    // Kita anggap skor dasar setiap main adalah 100
    double base = 100.0;
    double roundScore = scorer.calculateScore(base);
    
    totalScore += roundScore;
    
    std::cout << "Skor Ronde Ini: " << roundScore << std::endl;
    std::cout << "Total Skor Sekarang: " << totalScore << std::endl;
    
    currentRound++;
    if (currentRound > maxRounds) {
        std::cout << "Semua ronde telah selesai!" << std::endl;
        endRun();
    }
}

void RunSession::enterShop() {
    std::cout << "Entering Shop... (Belum ada item untuk dibeli)" << std::endl;
}

void RunSession::endRun() {
    std::cout << "\n--- Run Ended ---" << std::endl;
    std::cout << "Final Total Score: " << totalScore << std::endl;
    isRunActive = false;
}