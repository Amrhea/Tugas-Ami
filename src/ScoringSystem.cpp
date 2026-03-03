#include "ScoringSystem.h"
#include <iostream>

double ScoringSystem::calculateScore(double baseScore) {
    // Logika sederhana: skor dasar dikali 1.5 sebagai simulasi
    double finalScore = baseScore * 1.5;
    std::cout << "[ScoringSystem] Perhitungan: " << baseScore << " x 1.5 = " << finalScore << std::endl;
    return finalScore;
}