#include <iostream>
#include <string>
using namespace std;

const int POIN_MENANG = 3;

// ================= MLL-02: Parent (Tim) -> Child (Match) =================
// List tim juga dipakai sebagai SLL (pendaftaran tim = insert first)
struct Match {
    string idMatch, lawan, status;
    int skorSendiri, skorLawan;
    Match* nextChild;
};

struct Tim {
    string idTim, nama;
    int poin;
    Tim* next;
    Match* firstChild;
};
Tim* headTim = nullptr; // pointer ke elemen pertama dalam linked list tim
int counterTim = 0, counterMatch = 0;

// fungsi pembuat ID otomatis
string buatId(char prefix, int n) {
    string s = to_string(n);
    while (s.size() < 3) s = "0" + s;
    return string(1, prefix) + s;
}
// fungsi pencarian tim berdasarkan ID
Tim* cariTim(const string& id) {
    for (Tim* t = headTim; t; t = t->next)
        if (t->idTim == id) return t;
    return nullptr;
}

// SLL: Insert First
void daftarTim(const string& nama) {
    Tim* t = new Tim{buatId('T', ++counterTim), nama, 0, headTim, nullptr};
    headTim = t;
    cout << "Tim " << nama << " terdaftar dengan ID " << t->idTim << endl;
}

void tampilTim() {
    if (!headTim) { cout << "Belum ada tim.\n"; return; }
    for (Tim* t = headTim; t; t = t->next)
        cout << t->idTim << " | " << t->nama << " | " << t->poin << " poin\n";
}

// MLL-02: Insert Last pada child
void tambahMatch(Tim* t, const string& idM, const string& lawan, int sS, int sL) {
    Match* m = new Match{idM, lawan, (sS > sL ? "WIN" : "LOSS"), sS, sL, nullptr};
    if (!t->firstChild) { t->firstChild = m; return; }
    Match* p = t->firstChild;
    while (p->nextChild) p = p->nextChild;
    p->nextChild = m;
}

// ================= Queue (FIFO): antrean pertandingan =================
struct NodeAntrean { Tim* tim; NodeAntrean* next; };
NodeAntrean *front = nullptr, *rear = nullptr;

bool sudahAntre(Tim* t) {
    for (NodeAntrean* n = front; n; n = n->next)
        if (n->tim == t) return true;
    return false;
}
// Insert Last
void enqueue(Tim* t) {
    NodeAntrean* n = new NodeAntrean{t, nullptr};
    if (!rear) front = rear = n;
    else { rear->next = n; rear = n; }
}
Tim* dequeue() {
    if (!front) return nullptr;
    NodeAntrean* n = front;
    Tim* t = n->tim;
    front = front->next;
    if (!front) rear = nullptr;
    delete n;
    return t;
}
void tampilAntrean() {
    if (!front) { cout << "Antrean kosong.\n"; return; }
    int i = 1;
    for (NodeAntrean* n = front; n; n = n->next)
        cout << i++ << ". " << n->tim->nama << endl;
}

// ================= Alur pertandingan =================
int bacaInt(const string& prompt) {
    while (true) {
        cout << prompt;
        string s;
        getline(cin, s);
        try {
            size_t p;
            int v = stoi(s, &p);
            if (p == s.size() && v >= 0) return v;
        } catch (...) {}
        cout << "Input tidak valid.\n";
    }
}

void daftarBanyakTim() {
    int n = bacaInt("Mau daftar berapa tim? ");
    if (n == 0) { cout << "Jumlah harus lebih dari 0.\n"; return; }
    for (int i = 1; i <= n; i++) {
        string nama;
        cout << "Nama tim ke-" << i << ": ";
        getline(cin, nama);
        if (nama.empty()) {
            cout << "Nama tidak boleh kosong, ulangi.\n";
            i--;          // ulangi nomor yang sama
            continue;
        }
        daftarTim(nama);
    }
}

void mulaiPertandingan() {
    if (!front || !front->next) { cout << "Butuh minimal 2 tim di antrean.\n"; return; }
    Tim* a = dequeue();
    Tim* b = dequeue();
    cout << a->nama << " vs " << b->nama << endl;
    int sa = bacaInt("Skor " + a->nama + ": ");
    int sb = bacaInt("Skor " + b->nama + ": ");
    if (sa == sb) {
        cout << "Skor tidak boleh seri. Kedua tim dikembalikan ke antrean.\n";
        enqueue(a); enqueue(b);
        return;
    }
    string idM = buatId('M', ++counterMatch);
    tambahMatch(a, idM, b->nama, sa, sb);
    tambahMatch(b, idM, a->nama, sb, sa);
    int dA = (sa > sb) ? POIN_MENANG : 0;
    int dB = (sb > sa) ? POIN_MENANG : 0;
    a->poin += dA;
    b->poin += dB;
    cout << "Match " << idM << " tercatat.\n";
}

void menu() {
    cout << "\n=== MONFLOW ===\n"
         << "1. Daftar tim\n2. Lihat semua tim\n3. Masukkan tim ke antrean\n"
         << "4. Lihat antrean\n5. Mulai pertandingan\n" << "0. Keluar\n";
}

int main() {
    int pilih;
    do {
        menu();
        pilih = bacaInt("Pilih: ");
        string input;
        switch (pilih) {
        case 1:
            daftarBanyakTim();
            break;
        case 2: tampilTim(); break;
        case 3: {
            cout << "ID tim: ";
            getline(cin, input);
            Tim* t = cariTim(input);
            if (!t) cout << "Tim tidak ditemukan.\n";
            else if (sudahAntre(t)) cout << "Tim sudah ada di antrean.\n";
            else { enqueue(t); cout << t->nama << " masuk antrean.\n"; }
            break;
        }
        case 4: tampilAntrean(); break;
        case 5: mulaiPertandingan(); break;
        }
    } while (pilih != 0);
    return 0;
}