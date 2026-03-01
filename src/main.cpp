#include "RunSession.h"

int main() {
    RunSession game;
    game.startRun();

    while (game.isActive()) {
        game.playHand();
        game.enterShop();
        
        // Contoh sederhana: hentikan game setelah 3 ronde
        // Sesuai syarat minimum 3 rounds 
        if (game.isActive() && (/* logic untuk cek ronde */ false)) {
             // nanti kita perbaiki di commit selanjutnya
        }
        
        // Untuk sementara, kita batasi manual di loop main
        static int loopCount = 0;
        if (++loopCount >= 3) game.endRun();
    }

    return 0;
}