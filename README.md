# Tugas-Ami — Game Run Simulator

Sebuah game card-based run system yang menggabungkan poker hand evaluation, modifier system (dengan Factory Pattern), dan scoring mekanik yang fleksibel. Proyek ini dirancang sebagai demonstrasi penggunaan design patterns (Creational, Structural, Behavioral).

---

## Architecture Overview

Sistem dibangun dari beberapa komponen utama yang bekerja bersama:

### Core Classes

#### 1. **Card** (`src/Card.h`, `src/Card.cpp`)
- Merepresentasikan satu kartu dengan suit dan rank.
- Memiliki `value` (nilai chip dari kartu) dan fungsi `toString()` untuk display.
- Bertanggung jawab atas: identifikasi kartu individual.

#### 2. **PokerHand** (`src/PokerHand.h`, `src/PokerHand.cpp`)
- Mengevaluasi kombinasi kartu yang dimainkan (1-5 kartu atau lebih).
- Mengembalikan `HandResult` yang berisi:
  - `name`: nama kombinasi (Pair, Flush, Straight, dll)
  - `baseChips`: bonus chip untuk kombinasi tersebut
  - `multi`: multiplier untuk skor kombinasi
- Mendukung partial hands (2-4 kartu) dan full hands (5 kartu), termasuk straight/flush khusus dengan Ace.
- Bertanggung jawab atas: evaluasi logika poker dan perhitungan reward.

#### 3. **IModifier** (`src/modifier/IModifier.h`)
- Interface abstrak untuk semua modifier.
- Mendefinisikan:
  - `apply(double currentScore)`: memodifikasi skor dengan aturan tertentu.
  - `getName()`: mengembalikan nama deskriptif untuk UI.
- Bertanggung jawab atas: kontrak untuk behavior semua modifier.

#### 4. **Concrete Modifiers** (`src/modifier/FlatBoost.h`, `src/modifier/DoubleUpgrade.h`)
- **FlatBoost**: menambah score dengan nilai tetap (+50).
- **DoubleUpgrade**: mengalikan score dengan 2x.
- Implementasi konkret dari `IModifier`.
- Bertanggung jawab atas: logika modifikasi skor spesifik.

#### 5. **ModifierFactory** (`src/ModifierFactory.h`, `src/ModifierFactory.cpp`)
- Factory untuk membuat instance modifier berdasarkan string type.
- Fungsi statis `createModifier(const std::string& type)`:
  - Menerima "Flat" → return `new FlatBoost()`
  - Menerima "Double" → return `new DoubleUpgrade()`
  - Menerima type lain → return `nullptr`
- Bertanggung jawab atas: sentralisasi pembuatan modifier objects.

#### 6. **ScoringSystem** (`src/ScoringSystem.h`, `src/ScoringSystem.cpp`)
- Menghitung skor final dari skor dasar.
- Saat ini mengimplementasikan formula: `baseScore * 1.5`.
- Bertanggung jawab atas: perhitungan skor dan balancing.

#### 7. **ShopSystem** (`src/ShopSystem.h`, `src/ShopSystem.cpp`)
- Mengelola toko tempat pemain membeli modifier.
- Fitur:
  - `rollOffers()`: random 3 modifier dari pool.
  - `makeOffer()`: membuat pricing untuk setiap modifier.
  - `open()`: loop interaktif untuk buy/reroll/exit.
- Menyimpan `activeModifiers` yang dimainkan (*note: ini reference dari RunSession*).
- Bertanggung jawab atas: UI shop dan logika transaksi.

#### 8. **RunSession** (`src/RunSession.h`, `src/RunSession.cpp`)
- Merupakan state utama permainan selama satu "run".
- Mengelola:
  - `currentRound`, `currentBlind`, `currentBlindTarget`: fase permainan.
  - `totalScore`: skor kumulatif sepanjang run.
  - `coins`: currency untuk membeli modifier.
  - `activeModifiers`: list modifier yang sedang aktif.
  - `deck`: dek kartu 52 kartu.
- Fungsi utama:
  - `startRun()`: inisialisasi run baru.
  - `playHand()`: menjalankan satu round (deal kartu, pilih kartu, evaluasi, apply modifier, check target).
  - `enterShop()`: memanggil `ShopSystem::open()`.
  - `endRun()`: mengakhiri run.
- Bertanggung jawab atas: orchestration logika game flow.

---

## Pattern Usage

### Creational Patterns

#### Factory Pattern
- **Lokasi**: `src/ModifierFactory.cpp`, metode statis `createModifier()`.
- **Penggunaan**: 
  - Dipakai di `RunSession::RunSession()` untuk membuat modifier awal.
  - Dipakai di `ShopSystem::open()` saat pemain membeli modifier.
- **Manfaat**: 
  - Sentralisasi pembuatan modifier → perubahan constructor mudah dikerjakan satu tempat.
  - Memudahkan penambahan modifier baru tanpa mengubah caller code.

### Structural Patterns

#### Decorator Pattern
- **Lokasi**: Sistem modifier di `src/modifier/` dan `src/RunSession.cpp` (loop di `playHand()`).
- **Penggunaan**: Setiap `IModifier::apply()` menerima skor saat ini dan mengembalikan skor termodifikasi.
- **Flow**: 
  ```
  baseScore 
    → FlatBoost::apply()   → score + 50
    → DoubleUpgrade::apply() → (score + 50) * 2
  ```
- **Manfaat**: Modifier bersifat composable; urutan aplikasi penting dan flexible.

#### Adapter Pattern (Konseptual)
- **Potensi lokasi**: `ScoringSystem` dapat di-adapt jika ada eksternal scoring engine.
- **Belum diimplementasikan**: Saat ini hanya ada satu `ScoringSystem`.

#### Facade Pattern (Konseptual)
- **Potensi**: `RunSession` sudah bertindak mirip facade, menggabungkan `PokerHand`, `ScoringSystem`, `ShopSystem`.
- **Belum dioptimalkan**: Bisa dibuat kelas `GameFacade` explicit untuk menyederhanakan main loop.

### Behavioral Patterns

#### Strategy Pattern (Konseptual)
- **Potensi**: `ScoringSystem::calculateScore()` bisa menjadi interface dengan beberapa strategi (SimpleScoring, AdvancedScoring).
- **Saat ini**: Hardcoded sebagai `baseScore * 1.5`.

#### State Pattern (Konseptual)
- **Potensi**: Setiap fase game (`MenuState`, `RunState`, `ShopState`, `GameOverState`) bisa menjadi state object.
- **Saat ini**: State ditangani dengan `enum BlindType` dan flag `isRunActive`.

---

## Modification Log

### Menambahkan `SquareScoreModifier`

Jika ingin menambahkan modifier baru bernama `SquareScoreModifier` (yang mengkuadratkan skor), berikut langkahnya:

#### 1. **Buat File Header** (`src/modifier/SquareScoreModifier.h`)
```cpp
#ifndef SQUARESCOREMODIFIER_H
#define SQUARESCOREMODIFIER_H

#include "IModifier.h"

class SquareScoreModifier : public IModifier {
public:
    double apply(double currentScore) override {
        return currentScore * currentScore;
    }

    std::string getName() override {
        return "Square Score (^2)";
    }
};

#endif
```

#### 2. **Update ModifierFactory** (`src/ModifierFactory.cpp`)
```cpp
#include "ModifierFactory.h"
#include "modifier/FlatBoost.h"
#include "modifier/DoubleUpgrade.h"
#include "modifier/SquareScoreModifier.h"  // ← Tambahkan include

IModifier* ModifierFactory::createModifier(const std::string& type) {
    if (type == "Flat") {
        return new FlatBoost();
    }
    else if (type == "Double") {
        return new DoubleUpgrade();
    }
    else if (type == "Square") {  // ← Tambahkan case baru
        return new SquareScoreModifier();
    }
    return nullptr;
}
```

#### 3. **Update ShopSystem** (Opsional, `src/ShopSystem.cpp`)
```cpp
ShopSystem::ShopSystem(int maxModifierSlots)
    : maxSlots(maxModifierSlots)
{
    pool = {"Flat", "Double", "Square"};  // ← Tambahkan "Square"
}

ShopOffer ShopSystem::makeOffer(const std::string& id) const {
    if (id == "Flat")   return {id, "Flat Boost (+50)", 8};
    if (id == "Double") return {id, "Double Upgrade (x2)", 22};
    if (id == "Square") return {id, "Square Score (^2)", 35};  // ← Tambahkan offer
    return {id, id, 10};
}
```

#### File yang berubah:
- ✅ `src/modifier/SquareScoreModifier.h` (baru)
- ✅ `src/ModifierFactory.cpp` (update include + case)
- ✅ `src/ShopSystem.cpp` (opsional: update pool + offer)
- ❌ `src/RunSession.cpp` (tidak perlu diubah)
- ❌ `src/ScoringSystem.cpp` (tidak perlu diubah)

**Kenapa desain ini extensible?**
- Interface `IModifier` stabil → tidak ada perubahan kontrak.
- Pembuatan object terpusat di `ModifierFactory` → hanya 1 file yang perlu update logic.
- Shop pool bersifat generic → cukup tambah string ke list.

---

## AI Usage Disclosure

### Apa yang AI bantu dalam proyek ini:

1. **Desain Architecture**
   - Menyarankan struktur folder (`src/modifier/`) dan pemisahan concern.
   - Membantu merancang interface `IModifier` dan factory pattern.
   - Debugging struktur data dan flow game loop.

2. **Implementasi ModifierFactory & Pattern**
   - Menulis pseudocode dan kode production untuk Factory pattern.
   - Menjelaskan mengapa centralize creation di factory berguna.
   - Membantu memahami scalability vs direct instantiation.

3. **PokerHand Evaluation Logic**
   - Membantu fix edge case untuk straight (Ace sebagai 1 dan 14).
   - Merancang reward system untuk partial hands (2-4 kartu).
   - Debugging logika flush/straight validation.

4. **RunSession Game Flow**
   - Menulis structure playHand() dengan urutan logis: deal → pilih → evaluasi → modifier → check target.
   - Membantu implementasi state machine sederhana (SmallBlind → HighBlind → BossBlind).
   - Debugging input validation dan limit numeric handling.

5. **ShopSystem UI & Logic**
   - Implement reroll mechanism dan coin economy.
   - Membuat offer pricing logic.
   - Debugging interactive loop (buy/reroll/exit).

6. **Analisis Document (analisis.md)**
   - Menjelaskan Creational/Structural/Behavioral patterns.
   - Memberikan contoh kode untuk pattern yang belum diimplementasikan (Adapter, Facade, State).
   - Menyarankan improvements (Composite, Clone prototype, Strategy interface).

### Bagian yang AI NOT touchnya:
- Bisnis logic per-item, pricing balance, game rule decisions adalah milik user.
- Testing dan debugging runtime sebagian besar di-handle user.
- Final polish & tuning game balance.

---

## Cara Bermain

1. **Start Run**: Jalankan program, pilih "1" untuk mulai run.
2. **Play Hand**: 
   - Sistem deal 8 kartu.
   - Pilih 1-5 kartu untuk dimainkan.
   - Skor dihitung dari: (sum card values + bonus dari hand type) × multiplier.
3. **Apply Modifiers**: Skor di-apply modifier secara berurutan.
4. **Shop**: Setelah hand berhasil, kunjungi shop untuk beli modifier atau reroll.
5. **Progression**: Naik blind tier (SmallBlind → HighBlind → BossBlind) untuk setiap 3 hand.
6. **Win**: Selesaikan semua 5 round tanpa kalah.

---

## File Structure

```
game/
├── README.md                          (dokumen ini)
├── analisis/
│   └── analisis.md                   (detailed analysis + patterns)
└── src/
    ├── main.cpp
    ├── Card.h, Card.cpp
    ├── PokerHand.h, PokerHand.cpp
    ├── ScoringSystem.h, ScoringSystem.cpp
    ├── RunSession.h, RunSession.cpp
    ├── ShopSystem.h, ShopSystem.cpp
    ├── ModifierFactory.h, ModifierFactory.cpp
    └── modifier/
        ├── IModifier.h
        ├── FlatBoost.h
        └── DoubleUpgrade.h
```

---

## Kesimpulan

Proyek ini mendemonstrasikan penggunaan praktis design patterns dalam game development:
- **Factory Pattern** untuk extensible modifier creation.
- **Decorator Pattern** untuk composable score modifications.
- **Konsep architektur** yang siap untuk state machine, strategy, dan adapter patterns.

Dengan struktur ini, penambahan fitur baru (modifier, shop types, scoring strategies) dapat dilakukan tanpa mengubah core game logic.