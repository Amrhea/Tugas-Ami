#ifndef CARD_H
#define CARD_H

#include <string>

// Enum untuk tipe/simbol kartu agar lebih terstruktur (terhindar dari typo "Hatti")
enum class Suit {
    Hati,
    Keriting,
    Wajik,
    Sekop
};

class Card {
public:
    Suit suit;
    int rank;      // 1-13 (1=As, 11=Jack, 12=Queen, 13=King)
    double value;  // Nilai poin yang sebenarnya dipakai (misal King nilainya 10)

    Card(Suit s, int r);
    std::string toString() const; // Untuk mengubah objek Card menjadi teks (misal: "As Hati")
};

#endif