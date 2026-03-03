#ifndef SCORINGSYSTEM_H
#define SCORINGSYSTEM_H

class ScoringSystem {
public:
    // Fungsi ini menerima skor dasar dan mengembalikan hasil perhitungan
    // Kita gunakan 'double' agar bisa menghitung perkalian desimal nantinya
    double calculateScore(double baseScore);
};

#endif