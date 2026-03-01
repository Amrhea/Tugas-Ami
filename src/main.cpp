#include "RunSession.h"
#include <iostream>

int main() {
    RunSession game;
    game.startRun();

    int choice = 0;
    // Loop ini akan terus berjalan selama game.isActive() bernilai true
    while (game.isActive()) {
        std::cout << "\n========== MENU RUN ==========" << std::endl;
        std::cout << "1. Play Hand" << std::endl;
        std::cout << "2. Enter Shop" << std::endl;
        std::cout << "3. End Run" << std::endl;
        std::cout << "Pilih aksi: ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            continue;
        }

        if (choice == 1) {
            game.playHand();
        } else if (choice == 2) {
            game.enterShop();
        } else if (choice == 3) {
            game.endRun();
        } else {
            std::cout << "Pilihan salah!" << std::endl;
        }
    }

    return 0;
}