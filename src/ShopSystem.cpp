#include "ShopSystem.h"
#include "ModifierFactory.h"
#include "modifier/IModifier.h"

#include <iostream>
#include <random>

ShopSystem::ShopSystem(int maxModifierSlots)
    : maxSlots(maxModifierSlots)
{
    pool = {"Flat", "Double"};
}

ShopOffer ShopSystem::makeOffer(const std::string& id) const {
    // Balance harga berdasarkan power:
    // Flat (+50) kecil -> murah
    // Double (x2) besar -> mahal
    if (id == "Flat")   return {id, "Flat Boost (+50)", 8};
    if (id == "Double") return {id, "Double Upgrade (x2)", 22};
    return {id, id, 10};
}

void ShopSystem::rollOffers() {
    offers.clear();
    if (pool.empty()) return;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(0, (int)pool.size() - 1);

    for (int i = 0; i < 3; ++i) {
        std::string id = pool[dist(rng)];
        offers.push_back(makeOffer(id));
    }
}

void ShopSystem::print(int coins, const std::vector<IModifier*>& activeModifiers) const {
    std::cout << "\n========== SHOP ==========\n";
    std::cout << "Coins: " << coins << "\n";
    std::cout << "Active Modifiers (" << activeModifiers.size() << "/" << maxSlots << "):\n";
    for (size_t i = 0; i < activeModifiers.size(); ++i) {
        std::cout << "  - " << activeModifiers[i]->getName() << "\n";
    }

    std::cout << "\nOffers:\n";
    for (size_t i = 0; i < offers.size(); ++i) {
        const auto& o = offers[i];
        std::cout << (i + 1) << ") " << o.name
                  << " [type=" << o.id << "]"
                  << " price=" << o.price << "\n";
    }

    std::cout << "\nCommand:\n";
    std::cout << "  buy <1-3>\n";
    std::cout << "  reroll\n";
    std::cout << "  exit\n";
    std::cout << "==========================\n";
}

void ShopSystem::open(int& coins, std::vector<IModifier*>& activeModifiers) {
    rollOffers();

    while (true) {
        print(coins, activeModifiers);

        std::cout << "> ";
        std::string cmd;
        std::cin >> cmd;

        if (cmd == "exit") {
            std::cout << "Keluar dari shop.\n";
            return;
        }

        if (cmd == "reroll") {
            const int rerollCost = 4;
            if (coins < rerollCost) {
                std::cout << "Coins tidak cukup untuk reroll (" << rerollCost << ").\n";
                continue;
            }
            coins -= rerollCost;
            rollOffers();
            continue;
        }

        if (cmd == "buy") {
            int idx;
            std::cin >> idx;
            idx -= 1;

            if (idx < 0 || idx >= (int)offers.size()) {
                std::cout << "Pilihan tidak valid.\n";
                continue;
            }

            if ((int)activeModifiers.size() >= maxSlots) {
                std::cout << "Slot modifier penuh! (max " << maxSlots << ").\n";
                std::cout << "Tip: implement fitur remove/sell kalau mau.\n";
                continue;
            }

            const ShopOffer& item = offers[idx];
            if (coins < item.price) {
                std::cout << "Coins tidak cukup.\n";
                continue;
            }

            IModifier* mod = ModifierFactory::createModifier(item.id);
            if (!mod) {
                std::cout << "Gagal membuat modifier: " << item.id << "\n";
                continue;
            }

            coins -= item.price;
            activeModifiers.push_back(mod);

            std::cout << "Berhasil beli: " << item.name
                      << " -> Active: " << mod->getName() << "\n";

            // refresh offers setelah beli
            rollOffers();
            continue;
        }

        std::cout << "Command tidak dikenal.\n";
    }
}