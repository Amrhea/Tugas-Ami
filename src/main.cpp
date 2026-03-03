#include "RunSession.h"
#include <iostream>
#include <limits> // Untuk stream clearance

int main() {
    RunSession game;
    game.startRun();

    int choice;
    // Loop akan terus menagih input selama status isRunActive true
    while (game.isActive()) {
        std::cout << "\n========== MENU UTAMA ==========" << std::endl;
        std::cout << "1. Play Hand (Lanjutkan Ronde)" << std::endl;
        std::cout << "2. Enter Shop (Masuk Toko)" << std::endl;
        std::cout << "3. End Run (Menyerah)" << std::endl;
        std::cout << "Pilih aksi (1-3): ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1:
                game.playHand();
                break;
            case 2:
                game.enterShop();
                break;
            case 3:
                game.endRun();
                break;
            default:
                std::cout << "Pilihan tidak valid!" << std::endl;
        }
    }

    return 0;
}