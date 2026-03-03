#include "Card.h"

// Constructor
Card::Card(Suit s, int r) : suit(s), rank(r) {
    // Kita anggap kartu J, Q, K nilainya sama dengan 10 (seperti aturan Blackjack)
    // Jika As (1) nilainya bisa kita set 11 atau 1 (untuk sekarang kita pakai 10 agar mudah)
    if (rank >= 11) {
        value = 10.0;
    } else if (rank == 1) {
        value = 11.0; 
    } else {
        value = static_cast<double>(rank);
    }
}

// Fungsi pembantu untuk mencetak nama kartu
std::string Card::toString() const {
    std::string rankStr;
    switch (rank) {
        case 1:  rankStr = "As"; break;
        case 11: rankStr = "Jack"; break;
        case 12: rankStr = "Queen"; break;
        case 13: rankStr = "King"; break;
        default: rankStr = std::to_string(rank); break;
    }

    std::string suitStr;
    switch (suit) {
        case Suit::Hati:     suitStr = "Hati (♥)"; break;
        case Suit::Keriting: suitStr = "Keriting (♣)"; break;
        case Suit::Wajik:    suitStr = "Wajik (♦)"; break;
        case Suit::Sekop:    suitStr = "Sekop (♠)"; break;
    }

    return rankStr + " " + suitStr + " [Poin: " + std::to_string(static_cast<int>(value)) + "]";
}