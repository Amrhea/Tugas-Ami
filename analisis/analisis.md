# Analisis Desain (Tugas Ami)

## PART A — Creational Pattern Analysis

### A1 — Factory
**Where are modifier objects created?**
- Modifier objects dibuat di `ModifierFactory::createModifier()` (file: `src/ModifierFactory.cpp`).
- Fungsi ini dipanggil dari:
  - `RunSession` constructor (`src/RunSession.cpp`) untuk membuat modifier awal (Flat & Double).
  - `ShopSystem::open()` (`src/ShopSystem.cpp`) ketika pemain membeli modifier.

**Mengapa centralizing creation berguna?**
- Menjaga tempat pembuatan objek terpusat sehingga perubahan tipe modifier hanya perlu dilakukan di satu tempat.
- Menghindari duplikasi `new FlatBoost()` / `new DoubleUpgrade()` di banyak file.
- Memudahkan penambahan modifier baru (hanya perlu tambahkan case baru di factory).

### A2 — Extensibility (Menambahkan `SquareScoreModifier`)
**Bagaimana menambahkan `SquareScoreModifier`?**
1. Buat kelas baru `SquareScoreModifier` di folder `src/modifier/` yang mengimplementasikan `IModifier`.
2. Tambahkan `#include "modifier/SquareScoreModifier.h"` dan case baru di `ModifierFactory::createModifier()`.
3. Jika diperlukan, tambahkan opsi baru di shop (`ShopSystem::pool`) agar bisa dibeli.

**File yang diubah:**
- `src/modifier/IModifier.h` (tetap sama, interface sudah mendukung extensi)
- `src/modifier/SquareScoreModifier.h` (kelas baru)
- `src/ModifierFactory.cpp` (tambah branch `if (type == "Square") ...`)
- `src/ShopSystem.cpp` (opsional: tambah pool/offer untuk modifier baru)

**Apakah desain mudah diperluas?**
Ya. Karena semua modifier mengimplementasikan `IModifier` dan dibuat melalui `ModifierFactory`, kita hanya perlu menambahkan kelas baru dan menambahkan satu baris di factory. Tidak ada perubahan di `RunSession` atau logika core permainan.

### A3 — Factory Method (Konseptual)
Jika ada beberapa type shop (misalnya `BasicShop`, `PremiumShop`, `EventShop`), `Factory Method` bisa digunakan untuk membuat modifier/offer sesuai jenis shop.

Contoh interface:
```cpp
class Shop {
public:
    virtual ~Shop() {}
    virtual ShopOffer makeOffer(const std::string& id) const = 0;
    virtual void open(int& coins, std::vector<IModifier*>& activeModifiers) = 0;
};

class BasicShop : public Shop {
public:
    ShopOffer makeOffer(const std::string& id) const override {
        // implementasi untuk shop dasar
    }
};
```
Dengan Factory Method, kode yang memanggil shop tidak perlu tahu detail tipe shop (hanya memanggil `shop->open(...)`).

### A4 — Prototype (Konseptual)
`Clone()` bisa membantu jika kita ingin menduplikasi modifier yang sudah ada (misalnya ketika pemain meng-copy modifier atau menyimpan loadout).

- `IModifier` bisa ditambah metode virtual `virtual IModifier* clone() const = 0;`.
- Setiap modifier (misalnya `FlatBoost`, `DoubleUpgrade`) mengimplementasikan `clone()` dengan `return new FlatBoost(*this);`.
- Dengan `Clone()`, kita bisa membuat salinan modifier tanpa bergantung ke concrete class di client code.

### A5 — Singleton (Konseptual)
**Apakah ada objek global?**
- Dalam kode saat ini, tidak ada singleton; semua objek dibuat normal (misal `RunSession` memegang `ScoringSystem scorer;`).

**Keuntungan/risiko jika menggunakan singleton:**
- + Memudahkan akses global (misal `GameConfig::instance()`), menghindari passing parameter ke banyak fungsi.
- - Risiko state global (sulit diuji, susah reset state, menimbulkan ketergantungan tersembunyi).
- - Sulit dijalankan beberapa instance (misal multi-run) jika singleton menyimpan state.

---
## PART B — Structural Pattern Analysis

### B1 — Decorator Concept
Sistem modifier bertindak mirip Decorator karena skor dasar "didekorasi" satu per satu oleh setiap modifier.

- Objek yang didekorasi: `double currentScore` (nilai skor dari `PokerHand` + `ScoringSystem`).
- Modifier (`IModifier`) bertindak seperti dekorator: setiap `apply()` menerima skor saat ini dan mengembalikan skor baru.
- Referensi kode: loop modifier di `RunSession::playHand()` (`src/RunSession.cpp`):
  ```cpp
  for (IModifier* mod : activeModifiers) {
      currentScore = mod->apply(currentScore);
  }
  ```

### B2 — Modifier Order
**Apakah urutan modifier memengaruhi skor?**
Ya. Karena `apply()` mengubah nilai `currentScore` dan hasilnya dipakai sebagai input modifier berikutnya.

Contoh (dari kode):
- Modifier A = `FlatBoost` (tambah +50)
- Modifier B = `DoubleUpgrade` (kali 2)

Jika urutan A lalu B: `(base + 50) * 2`
Jika urutan B lalu A: `(base * 2) + 50`

Kedua hasil berbeda, jadi urutan di `activeModifiers` penting.

### B3 — Composite (Konseptual)
Sebuah `ModifierGroup` bisa diimplementasikan sebagai composite:
- Buat kelas `CompositeModifier : public IModifier` yang memiliki `std::vector<IModifier*> children;`.
- `apply()` memanggil `apply()` pada setiap child berurutan.
- `getName()` bisa menggabungkan nama child atau menunjukkan grup.

Dengan cara ini, satu entitas modifier dapat mewakili beberapa modifier, membuat struktur tree/komposisi.

### B4 — Adapter (Konseptual)
Jika ada scoring eksternal dengan interface berbeda (misal `ExternalScoreEngine::compute(int)`), kita bisa buat adapter:

```cpp
class ExternalScoringAdapter : public ScoringSystem {
    ExternalScoreEngine engine;
public:
    double calculateScore(double baseScore) override {
        return engine.compute((int)baseScore);
    }
};
```

Adapter membuat kode dalam `RunSession` tetap menggunakan `ScoringSystem::calculateScore()` meski implementasinya berbeda.

### B5 — Facade (Konseptual)
Desain `GameFacade` bisa menyederhanakan penggunaan subsistem (RunSession, ShopSystem, ScoringSystem, PokerHand):

Fungsi yang diekspos:
- `void startNewRun();`
- `void playRound();` / `void playHand();`
- `void openShop();`
- `double getTotalScore() const;`
- `bool isRunActive() const;`

`GameFacade` akan memanggil `RunSession` dan `ShopSystem` di dalamnya, sehingga kode main hanya berinteraksi dengan facade.

---
## PART C — Behavioral Pattern Analysis

### C1 — Strategy
**Di mana logika scoring berada?**
- Logika scoring berada di `ScoringSystem::calculateScore()` (`src/ScoringSystem.cpp`).

**Bagaimana mendukung beberapa aturan scoring?**
- Ubah `ScoringSystem` menjadi interface (`IScoringStrategy`) dan buat beberapa implementasi (mis. `SimpleScoring`, `AdvancedScoring`).
- `RunSession` menerima pointer/ reference ke strategi scoring yang dipilih.

### C2 — Observer (Konseptual)
**Event yang ada di sistem:**
- Modifier dibeli/ditambahkan (`ShopSystem::open()` ketika `activeModifiers.push_back(mod)`)
- Skor diperbarui di akhir setiap hand (`RunSession::playHand()`)
- Pergantian state ronde/blind (`RunSession::playHand()`)

Observer bisa membantu agar UI/console otomatis merespons event (mis. menampilkan notifikasi, menyimpan riwayat) tanpa menambahkan logika print di banyak tempat.

### C3 — Command (Konseptual)
**Aksi yang bisa dijadikan Command:**
- `BuyModifierCommand` (beli modifier di shop)
- `PlayHandCommand` (jalankan satu hand)
- `RerollOfferCommand` (reroll shop)
- `EndRunCommand` / `StartRunCommand`

Setiap command mengimplementasikan `execute()` (dan mungkin `undo()` untuk fitur undo).

### C4 — State (Konseptual)
**State yang dapat diorganisir:**
- `MenuState` (menunggu input main/exit)
- `RunState` (sedang bermain, memanggil `playHand()`)
- `ShopState` (di dalam shop, memproses command `buy/exit/reroll`)
- `GameOverState` (run selesai)

State pattern membantu memisahkan logika input & transisi fase sehingga struktur `RunSession` lebih modular dan lebih mudah dipelihara.
