#include "ModifierFactory.h"
#include "modifier/FlatBoost.h"
#include "modifier/DoubleUpgrade.h"

IModifier* ModifierFactory::createModifier(const std::string& type) {
    if (type == "Flat") {
        return new FlatBoost();
    } 
    else if (type == "Double") {
        return new DoubleUpgrade();
    }
    
    // Jika tipe tidak dikenali
    return nullptr; 
}