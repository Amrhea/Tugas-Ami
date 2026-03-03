#ifndef POKERHAND_H
#define POKERHAND_H

#include <vector>
#include <string>
#include "Card.h"

// Enum untuk tipe Hand dari bawah ke atas
enum class HandType {
    HighCard,
    Pair,
    TwoPair,
    ThreeOfAKind,
    Straight
    // Bisa ditambah Flush, FullHouse, dll nanti
};

struct HandResult {
    HandType type;
    std::string name;
    double baseChips; // Tambahan skor tetap
    double multi;     // Tambahan multiplier
};

class PokerHand {
public:
    // Fungsi utama untuk mengecek 5 kartu dan mengembalikan hasilnya
    static HandResult evaluate(std::vector<Card>& cards);
};

#endif