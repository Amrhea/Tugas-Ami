#pragma once
#include <vector>
#include <string>
#include "Card.h"

struct HandResult {
    std::string name;
    double baseChips;
    double multi;
};

class PokerHand {
public:
    static HandResult evaluate(const std::vector<Card>& cards);

private:
    static bool isFlush(const std::vector<Card>& cards);
    static bool isStraight(const std::vector<int>& ranksSortedUnique); // hanya valid untuk 5 kartu
};