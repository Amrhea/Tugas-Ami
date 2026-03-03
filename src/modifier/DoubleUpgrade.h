#ifndef DOUBLEUPGRADE_H
#define DOUBLEUPGRADE_H


class DoubleUpgrade : public IModifier {
public:
    double apply(double currentScore) override {
        return currentScore * 2.0; 
    }
    std::string getName() override {
        return "Double Upgrade (x2)";
    }
};
#endif