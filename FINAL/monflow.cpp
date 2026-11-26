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
Tim* headTim = nullptr;
int counterTim = 0, counterMatch = 0;

string buatId(char prefix, int n) {
    string s = to_string(n);
    while (s.size() < 3) s = "0" + s;
    return string(1, prefix) + s;
}

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

// dipakai Undo: hapus child paling akhir
void hapusMatchTerakhir(Tim* t) {
    if (!t->firstChild) return;
    if (!t->firstChild->nextChild) { delete t->firstChild; t->firstChild = nullptr; return; }
    Match* p = t->firstChild;
    while (p->nextChild->nextChild) p = p->nextChild;
    delete p->nextChild;
    p->nextChild = nullptr;
}

void tampilMatchTim(Tim* t) {
    cout << "Riwayat match " << t->nama << ":\n";
    if (!t->firstChild) { cout << "  (belum ada match)\n"; return; }
    for (Match* m = t->firstChild; m; m = m->nextChild)
        cout << "  " << m->idMatch << " vs " << m->lawan << " | skor "
             << m->skorSendiri << "-" << m->skorLawan << " | " << m->status << endl;
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

// ================= BST: leaderboard berdasarkan poin =================
struct NodeBST { string nama; int poin; NodeBST *left, *right; };
NodeBST* root = nullptr;

NodeBST* insertBST(NodeBST* n, const string& nama, int poin) {
    if (!n) return new NodeBST{nama, poin, nullptr, nullptr};
    if (poin < n->poin) n->left = insertBST(n->left, nama, poin);
    else n->right = insertBST(n->right, nama, poin);
    return n;
}
void hapusBST(NodeBST* n) {
    if (!n) return;
    hapusBST(n->left);
    hapusBST(n->right);
    delete n;
}
// kanan -> node -> kiri supaya urutan dari poin terbesar
void tampilBST(NodeBST* n, int& rank) {
    if (!n) return;
    tampilBST(n->right, rank);
    cout << rank++ << ". " << n->nama << " - " << n->poin << " poin\n";
    tampilBST(n->left, rank);
}
// bangun ulang BST dari list tim setiap ada perubahan poin
void updateLeaderboard() {
    hapusBST(root);
    root = nullptr;
    for (Tim* t = headTim; t; t = t->next)
        root = insertBST(root, t->nama, t->poin);
}
void tampilLeaderboard() {
    if (!root) { cout << "Leaderboard kosong.\n"; return; }
    int rank = 1;
    tampilBST(root, rank);
}

// ================= DLL: log riwayat untuk Undo =================
struct Log {
    string idMatch;
    Tim *a, *b;
    int deltaA, deltaB;
    Log *prev, *next;
};
Log *logHead = nullptr, *logTail = nullptr;

// Insert Last
void tambahLog(const string& idM, Tim* a, Tim* b, int dA, int dB) {
    Log* l = new Log{idM, a, b, dA, dB, logTail, nullptr};
    if (!logHead) logHead = l;
    else logTail->next = l;
    logTail = l;
}
void tampilLogMaju() {
    cout << "Log (lama -> baru):\n";
    for (Log* l = logHead; l; l = l->next)
        cout << "  " << l->idMatch << ": " << l->a->nama << " vs " << l->b->nama << endl;
}
void tampilLogMundur() {
    cout << "Log (baru -> lama):\n";
    for (Log* l = logTail; l; l = l->prev)
        cout << "  " << l->idMatch << ": " << l->a->nama << " vs " << l->b->nama << endl;
}
void undo() {
    if (!logTail) { cout << "Tidak ada riwayat untuk di-undo.\n"; return; }
    Log* l = logTail;
    l->a->poin -= l->deltaA;
    l->b->poin -= l->deltaB;
    hapusMatchTerakhir(l->a);
    hapusMatchTerakhir(l->b);
    logTail = l->prev;
    if (logTail) logTail->next = nullptr;
    else logHead = nullptr;
    cout << "Match " << l->idMatch << " dibatalkan.\n";
    delete l;
    updateLeaderboard();
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
    tambahLog(idM, a, b, dA, dB);
    updateLeaderboard();
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