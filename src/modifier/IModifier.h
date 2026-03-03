#ifndef IMODIFIER_H
#define IMODIFIER_H

#include <string>

class IModifier {
public:
    virtual ~IModifier() {}
    
    // Fungsi inti untuk mengubah skor
    virtual double apply(double currentScore) = 0;
    
    // Untuk menampilkan nama modifier di terminal
    virtual std::string getName() = 0;
};

#endif