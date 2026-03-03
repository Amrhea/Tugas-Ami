#include "RunSession.h"
#include "ModifierFactory.h"
#include "PokerHand.h" // Tambahkan ini!
#include <iostream>
#include <limits> // Untuk mencegah error saat cin << salah ketik

RunSession::RunSession() : 
    currentRound(1), maxRounds(5), isRunActive(true), totalScore(0), 
    currentBlind(BlindType::SmallBlind), currentBlindTarget(20.0) 
{
    IModifier* mod1 = ModifierFactory::createModifier("Flat");
    IModifier* mod2 = ModifierFactory::createModifier("Double");
    if (mod1) activeModifiers.push_back(mod1);
    if (mod2) activeModifiers.push_back(mod2);
    
    generateDeck();
}

RunSession::~RunSession() {
    for (IModifier* mod : activeModifiers) {
        delete mod;
    }
    activeModifiers.clear();
}

void RunSession::generateDeck() {
    deck.clear();
    for (int s = 0; s < 4; ++s) {
        for (int r = 1; r <= 13; ++r) {
            deck.push_back(Card(static_cast<Suit>(s), r));
        }
    }
}

// FUNGSI BARU UTILITY
std::string RunSession::getBlindName(BlindType type) {
    switch (type) {
        case BlindType::SmallBlind: return "Small Blind";
        case BlindType::HighBlind:  return "High Blind";
        case BlindType::BossBlind:  return "Boss Blind";
        default: return "Unknown";
    }
}

// FUNGSI BARU UTILITY
void RunSession::setBlindTarget() {
    double baseTarget = 0;
    if (currentBlind == BlindType::SmallBlind) baseTarget = 110.0;
    else if (currentBlind == BlindType::HighBlind) baseTarget = 300.0;
    else if (currentBlind == BlindType::BossBlind) baseTarget = 500.0;

    // Tambah 5 poin setiap peningkatan ronde (mulai ronde 2 dst)
    currentBlindTarget = baseTarget + ((currentRound - 1) * 5.0);
}

void RunSession::startRun() {
    std::cout << "\n=========================================" << std::endl;
    std::cout << "--- MEMULAI RUN BARU ---" << std::endl;
    std::cout << "=========================================" << std::endl;
    totalScore = 0;
    currentRound = 1;
    currentBlind = BlindType::SmallBlind;
    isRunActive = true;
    setBlindTarget();
}

void RunSession::playHand() {
    std::cout << "\n-----------------------------------------" << std::endl;
    std::cout << "RONDE " << currentRound << " | FASE: " << getBlindName(currentBlind) << std::endl;
    std::cout << "TARGET SKOR UNTUK MENANG: " << currentBlindTarget << std::endl;
    std::cout << "-----------------------------------------" << std::endl;

    // --- LANGKAH 1: KOCOK KARTU & BAGIKAN 8 KARTU ---
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin(), deck.end(), g);

    std::vector<Card> hand;
    for (int i = 0; i < 8; ++i) {
        hand.push_back(deck[i]);
    }

    std::cout << "Kartu di tanganmu:" << std::endl;
    for (size_t i = 0; i < hand.size(); ++i) {
        std::cout << (i + 1) << ". " << hand[i].toString() << std::endl;
    }

    // --- LANGKAH 2: PENGGUNA MEMILIH 5 KARTU ---
    std::vector<Card> playedCards;
    std::cout << "\nKetik 5 nomor kartu yang ingin dimainkan (pisahkan spasi): ";
    
    for (int i = 0; i < 5; ++i) {
        int choice;
        std::cin >> choice;
        
        // Pencegahan bug jika player mengetik huruf (bukan angka)
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = i + 1; // Auto pick urutan jika error
        }
        else if (choice < 1 || choice > 8) {
            choice = i + 1;
        }
        
        playedCards.push_back(hand[choice - 1]);
    }

    // --- LANGKAH 3: EVALUASI KARTU YANG DIPILIH (POKER HAND & CHIPS) ---
    double cardChips = 0.0;
    std::cout << "\nKartu yang kamu mainkan:" << std::endl;
    for (const Card& c : playedCards) {
        std::cout << "- " << c.toString() << std::endl;
        cardChips += c.value;
    }
    
    // Evaluasi Poker Hand (Sekarang variabel playedCards sudah terdefinisi dan ada isinya!)
    HandResult handResult = PokerHand::evaluate(playedCards);
    std::cout << "\n[!] Kombinasi Kartu: " << handResult.name << "!" << std::endl;
    std::cout << "[!] Bonus Hand -> Chips: +" << handResult.baseChips << " | Multiplier: x" << handResult.multi << std::endl;

    // Hitung currentScore awal (Total Poin Kartu + Bonus Chips Kombinasi) x Multi Kombinasi
    double currentScore = (cardChips + handResult.baseChips) * handResult.multi;
    std::cout << "\nBase Score Kartu: " << currentScore << std::endl;

    // --- LANGKAH 4: PROSES MODIFIER DARI TOKO ---
    for (IModifier* mod : activeModifiers) {
        currentScore = mod->apply(currentScore);
        std::cout << "Applied " << mod->getName() << " -> New Score: " << currentScore << std::endl;
    }

    // --- LANGKAH 5: HITUNG SKOR FINAL DAN TENTUKAN MENANG/KALAH ---
    double finalRoundScore = scorer.calculateScore(currentScore);
    totalScore += finalRoundScore;
    
    std::cout << "\nSKOR AKHIR FASE INI: " << finalRoundScore << std::endl;

    if (finalRoundScore < currentBlindTarget) {
        std::cout << "\n[GAME OVER] Kamu gagal mencapai target " << currentBlindTarget << "!" << std::endl;
        endRun();
        return; // Hentikan fungsi playHand seketika
    } else {
        std::cout << "[BERHASIL] Kamu melampaui target " << currentBlindTarget << "!" << std::endl;
    }

    // --- LANGKAH 6: LOGIKA PERGANTIAN STATE (BLIND & ROUND) ---
    if (currentBlind == BlindType::SmallBlind) {
        currentBlind = BlindType::HighBlind;
    } 
    else if (currentBlind == BlindType::HighBlind) {
        currentBlind = BlindType::BossBlind;
    } 
    else if (currentBlind == BlindType::BossBlind) {
        currentRound++;
        if (currentRound > maxRounds) {
            std::cout << "\nSelamat! Kamu telah mengalahkan seluruh " << maxRounds << " Ronde!" << std::endl;
            endRun();
            return;
        }
        currentBlind = BlindType::SmallBlind;
    }

    // Update target untuk pertarungan berikutnya
    setBlindTarget();
}

void RunSession::enterShop() {
    std::cout << "Entering Shop... (Belum ada item untuk dibeli)" << std::endl;
}

void RunSession::endRun() {
    std::cout << "\n--- RUN SELESAI / GAME OVER ---" << std::endl;
    std::cout << "Total Kumulatif Skor: " << totalScore << std::endl;
    std::cout << "Tekan ENTER untuk kembali ke Main Menu...";
    
    // Memberikan jeda sebelum kembali ke Main Menu
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // bersihkan sisa enter sebelumnya
    std::cin.get(); // tunggu user tekan enter lagi
    
    isRunActive = false;
}