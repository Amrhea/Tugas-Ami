#ifndef FLATBOOST_H
#define FLATBOOST_H

#include "IModifier.h"

class FlatBoost : public IModifier {
public:
    double apply(double currentScore) override {
        return currentScore + 50.0; // Menambah 50 ke skor
    }
    
    std::string getName() override {
        return "Flat Boost (+50)";
    }
};

#endif