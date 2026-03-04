#include "PokerHand.h"
#include <algorithm>
#include <map>
#include <vector>

static HandResult makeResult(const std::string& name, double chips, double multi) {
    return HandResult{name, chips, multi};
}

// Flush untuk ukuran berapa pun: semua suit harus sama.
// (Untuk 1 kartu, kita anggap bukan flush)
static bool isFlushAny(const std::vector<Card>& cards) {
    if (cards.size() < 2) return false;
    Suit s = cards[0].suit;
    for (size_t i = 1; i < cards.size(); ++i) {
        if (cards[i].suit != s) return false;
    }
    return true;
}

// Straight untuk ukuran berapa pun (2..5):
// - Harus unique ranks = jumlah kartu (tidak boleh ada duplikat)
// - Harus berurutan
// - Mendukung A sebagai 1 (A-2-3-4-5) dan A sebagai tinggi (10-J-Q-K-A)
//   Karena rank As kamu = 1, kita coba 2 versi: As=1 dan As=14.
static bool isStraightAny(const std::vector<Card>& cards) {
    if (cards.size() < 2) return false;

    std::vector<int> ranks;
    ranks.reserve(cards.size());
    for (const auto& c : cards) ranks.push_back(c.rank);

    // Unique check
    std::sort(ranks.begin(), ranks.end());
    auto it = std::unique(ranks.begin(), ranks.end());
    if ((size_t)std::distance(ranks.begin(), it) != cards.size()) return false;
    ranks.erase(it, ranks.end());

    // Cek consecutive helper
    auto isConsecutive = [](const std::vector<int>& v) -> bool {
        for (size_t i = 1; i < v.size(); ++i) {
            if (v[i] != v[i - 1] + 1) return false;
        }
        return true;
    };

    // Coba straight normal (As tetap 1)
    if (isConsecutive(ranks)) return true;

    // Coba straight dengan As dianggap 14 (biar 10-J-Q-K-A bisa)
    // Kalau ada rank 1, ubah jadi 14 lalu cek lagi
    bool hasAce = (std::find(ranks.begin(), ranks.end(), 1) != ranks.end());
    if (hasAce) {
        std::vector<int> ranksAceHigh = ranks;
        for (int& r : ranksAceHigh) {
            if (r == 1) r = 14;
        }
        std::sort(ranksAceHigh.begin(), ranksAceHigh.end());
        if (isConsecutive(ranksAceHigh)) return true;
    }

    return false;
}

// === Fungsi dari header kamu (biar tetap kompatibel) ===
bool PokerHand::isFlush(const std::vector<Card>& cards) {
    // Versi 5 kartu (aslinya). Tetap disediakan sesuai header.
    if (cards.size() != 5) return false;
    return isFlushAny(cards);
}

bool PokerHand::isStraight(const std::vector<int>& ranksSortedUnique) {
    // Versi 5 kartu (sesuai header). Ini dipakai untuk evaluasi 5 kartu saja.
    if (ranksSortedUnique.size() != 5) return false;

    // Normal straight
    bool normal = true;
    for (int i = 1; i < 5; ++i) {
        if (ranksSortedUnique[i] != ranksSortedUnique[i - 1] + 1) {
            normal = false;
            break;
        }
    }
    if (normal) return true;

    // Wheel A-2-3-4-5
    if (ranksSortedUnique[0] == 1 &&
        ranksSortedUnique[1] == 2 &&
        ranksSortedUnique[2] == 3 &&
        ranksSortedUnique[3] == 4 &&
        ranksSortedUnique[4] == 5) {
        return true;
    }

    // Ace-high 10-J-Q-K-A (rank A = 1)
    // ranksSortedUnique akan jadi [1,10,11,12,13]
    if (ranksSortedUnique[0] == 1 &&
        ranksSortedUnique[1] == 10 &&
        ranksSortedUnique[2] == 11 &&
        ranksSortedUnique[3] == 12 &&
        ranksSortedUnique[4] == 13) {
        return true;
    }

    return false;
}

HandResult PokerHand::evaluate(const std::vector<Card>& cards) {
    const int n = (int)cards.size();
    if (n == 0) return makeResult("No Cards", 0.0, 1.0);

    // Hitung frekuensi rank
    std::map<int, int> freqRank;
    for (const Card& c : cards) freqRank[c.rank]++;

    std::vector<int> freq;
    freq.reserve(freqRank.size());
    for (auto& kv : freqRank) freq.push_back(kv.second);
    std::sort(freq.begin(), freq.end(), std::greater<int>());

    // ===== Reward partial straight/flush (2-4 kartu) =====
    // Kamu bisa ubah angka-angka ini buat balancing.
    auto partialFlushReward = [&](int k) -> HandResult {
        // k = jumlah kartu
        if (k == 2) return makeResult("Flush (2)", 6.0, 1.15);
        if (k == 3) return makeResult("Flush (3)", 12.0, 1.30);
        if (k == 4) return makeResult("Flush (4)", 20.0, 1.60);
        return makeResult("Flush", 50.0, 2.50); // fallback 5
    };

    auto partialStraightReward = [&](int k) -> HandResult {
        if (k == 2) return makeResult("Straight (2)", 5.0, 1.10);
        if (k == 3) return makeResult("Straight (3)", 12.0, 1.30);
        if (k == 4) return makeResult("Straight (4)", 22.0, 1.70);
        return makeResult("Straight", 45.0, 2.20); // fallback 5
    };

    auto partialStraightFlushReward = [&](int k) -> HandResult {
        if (k == 2) return makeResult("Straight Flush (2)", 8.0, 1.25);
        if (k == 3) return makeResult("Straight Flush (3)", 18.0, 1.60);
        if (k == 4) return makeResult("Straight Flush (4)", 35.0, 2.20);
        return makeResult("Straight Flush", 120.0, 6.0); // fallback 5
    };

    // ======================
    // 5 kartu: evaluasi lengkap
    // ======================
    if (n == 5) {
        // buat ranks unique sorted utk isStraight(5)
        std::vector<int> ranks;
        ranks.reserve(5);
        for (const auto& c : cards) ranks.push_back(c.rank);
        std::sort(ranks.begin(), ranks.end());
        ranks.erase(std::unique(ranks.begin(), ranks.end()), ranks.end());

        bool flush = isFlush(cards);
        bool straight = isStraight(ranks);

        if (flush && straight) return makeResult("Straight Flush", 120.0, 6.0);
        if (freq[0] == 4)       return makeResult("Four of a Kind", 80.0, 4.0);
        if (freq.size() >= 2 && freq[0] == 3 && freq[1] == 2) return makeResult("Full House", 60.0, 3.0);
        if (flush)             return makeResult("Flush", 50.0, 2.5);
        if (straight)          return makeResult("Straight", 45.0, 2.2);
        if (freq[0] == 3)      return makeResult("Three of a Kind", 35.0, 2.0);
        if (freq.size() >= 2 && freq[0] == 2 && freq[1] == 2) return makeResult("Two Pair", 25.0, 1.8);
        if (freq[0] == 2)      return makeResult("One Pair", 15.0, 1.5);

        return makeResult("High Card", 0.0, 1.0);
    }

    // ======================
    // < 5 kartu: evaluasi fleksibel
    // Urutan prioritas (kamu boleh ubah):
    // FourKind > StraightFlush > ThreeKind > Flush > Straight > TwoPair > Pair > HighCard
    // ======================
    const bool flushAny = isFlushAny(cards);
    const bool straightAny = isStraightAny(cards);

    if (n >= 4 && freq[0] == 4) {
        return makeResult("Four of a Kind", 50.0, 3.0);
    }

    // Straight Flush partial: (2 kartu juga boleh kalau kamu mau)
    if (flushAny && straightAny) {
        return partialStraightFlushReward(n);
    }

    if (n >= 3 && freq[0] == 3) {
        return makeResult("Three of a Kind", 25.0, 2.0);
    }

    if (flushAny) {
        return partialFlushReward(n);
    }

    if (straightAny) {
        return partialStraightReward(n);
    }

    if (n >= 4 && freq.size() >= 2 && freq[0] == 2 && freq[1] == 2) {
        return makeResult("Two Pair", 18.0, 1.7);
    }

    if (n >= 2 && freq[0] == 2) {
        return makeResult("One Pair", 10.0, 1.4);
    }

    return makeResult("High Card", 0.0, 1.0);
}