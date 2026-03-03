#include "PokerHand.h"
#include <algorithm>
#include <map>

HandResult PokerHand::evaluate(std::vector<Card>& cards) {
    // 1. Urutkan kartu berdasarkan rank (angka) dari terkecil ke terbesar
    std::sort(cards.begin(), cards.end(), [](const Card& a, const Card& b) {
        return a.rank < b.rank;
    });

    // 2. Hitung frekuensi setiap Rank (untuk cek Pair dll)
    std::map<int, int> rankCounts;
    for (const Card& c : cards) {
        rankCounts[c.rank]++;
    }

    // 3. Cari jumlah Pair dan Three of a Kind
    int pairCount = 0;
    bool hasThree = false;
    
    for (auto const& [rank, count] : rankCounts) {
        if (count == 2) pairCount++;
        if (count == 3) hasThree = true;
    }

    // 4. Cek Straight (Angka Berurutan)
    bool isStraight = true;
    if (cards.size() == 5) {
        for (int i = 0; i < 4; i++) {
            // Jika angka saat ini + 1 TIDAK SAMA dengan angka berikutnya, berarti bukan Straight
            if (cards[i].rank + 1 != cards[i+1].rank) {
                isStraight = false;
                break;
            }
        }
    } else {
        isStraight = false; // Harus 5 kartu untuk straight
    }

    // 5. Tentukan Hand Terbaik (Prioritas dari atas ke bawah)
    if (isStraight) {
        return {HandType::Straight, "Straight", 30.0, 4.0};
    } 
    else if (hasThree) {
        return {HandType::ThreeOfAKind, "Three of a Kind", 30.0, 3.0};
    } 
    else if (pairCount == 2) {
        return {HandType::TwoPair, "Two Pair", 20.0, 2.0};
    } 
    else if (pairCount == 1) {
        return {HandType::Pair, "Pair", 10.0, 2.0};
    } 
    else {
        return {HandType::HighCard, "High Card", 5.0, 1.0};
    }
}