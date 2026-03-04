#pragma once
#include <vector>
#include <string>

class IModifier;

struct ShopOffer {
    std::string id;        // "Flat" / "Double"
    std::string name;      // tampilan
    int price;
};

class ShopSystem {
public:
    ShopSystem(int maxModifierSlots = 5);

    void open(int& coins, std::vector<IModifier*>& activeModifiers);

private:
    int maxSlots;
    std::vector<std::string> pool;
    std::vector<ShopOffer> offers;

    void rollOffers();
    ShopOffer makeOffer(const std::string& id) const;

    void print(int coins, const std::vector<IModifier*>& activeModifiers) const;
};