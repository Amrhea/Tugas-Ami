#include "RunSession.h"
#include <iostream>

// Implementasi Constructor
RunSession::RunSession() : currentRound(1), maxRounds(3), isRunActive(true) {}

void RunSession::startRun() {
    std::cout << "--- Game Started ---" << std::endl;
}

void RunSession::playHand() {
    std::cout << "\nRound " << currentRound << ": Playing hand..." << std::endl;
    std::cout << "Hand played! Calculating score..." << std::endl;
    currentRound++; // Tambahkan ini agar ronde bertambah
}

void RunSession::enterShop() {
    std::cout << "Entering Shop... (No items available yet)" << std::endl;
}

void RunSession::endRun() {
    std::cout << "\n--- Run Ended. Final Score: 0 ---" << std::endl;
    isRunActive = false;
}