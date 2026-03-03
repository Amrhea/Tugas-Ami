#ifndef MODIFIERFACTORY_H
#define MODIFIERFACTORY_H

#include "modifier/IModifier.h"
#include <string>

class ModifierFactory {
public:
    // Fungsi statis agar bisa dipanggil tanpa membuat objek Factory terlebih dahulu
    static IModifier* createModifier(const std::string& type);
};

#endif