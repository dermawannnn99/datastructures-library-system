#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <stack>
#include <queue>
#include <cstdlib>

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

using namespace std;

// struct untuk buku
struct buku {
    int id_buku;
    string judul;
    string penulis;
    int nomor_rak;
    bool dipinjam;
};

// struct untuk anggota
struct anggota {
    int id_anggota;
    string nama;
    vector<int> buku_dipinjam; // menyimpan id buku
};

// struct untuk transaksi (untuk stack)
struct transaksi {
    string jenis; // "pinjam" atau "kembali"
    int id_buku;
    int id_anggota;
};

// node untuk pohon pencarian biner
struct node_pohon {
    buku* ptr_buku; // pointer ke buku
    node_pohon* kiri;
    node_pohon* kanan;
    node_pohon(buku* b) : ptr_buku(b), kiri(nullptr), kanan(nullptr) {}
};

// kelas untuk manajemen perpustakaan
class perpustakaan {
private:
    static const int maks_buku = 100;
    static const int maks_anggota = 50;
    static const int maks_rak = 5;
    buku daftar_buku[maks_buku];
    anggota daftar_anggota[maks_anggota];
    int jumlah_buku;
    int jumlah_anggota;
    stack<transaksi> tumpukan_transaksi; // stack untuk undo
    queue<transaksi> antrian_pinjam; // queue untuk permintaan pinjam
    node_pohon* akar_pohon; // pohon untuk pencarian buku
    int graf_rak[maks_rak][maks_rak]; // matriks ketetanggaan untuk rak

public:
    perpustakaan() : jumlah_buku(0), jumlah_anggota(0), akar_pohon(nullptr) {
        // inisialisasi matriks ketetanggaan (1 jika rak berdekatan, 0 jika tidak)
        for (int i = 0; i < maks_rak; i++)
            for (int j = 0; j < maks_rak; j++)
                graf_rak[i][j] = (abs(i - j) == 1) ? 1 : 0;
    }

    // sisip ke pohon pencarian biner
    void sisip_pohon(buku* buku_baru) {
        node_pohon** saat_ini = &akar_pohon;
        while (*saat_ini) {
            if (buku_baru->id_buku < (*saat_ini)->ptr_buku->id_buku)
                saat_ini = &(*saat_ini)->kiri;
            else
                saat_ini = &(*saat_ini)->kanan;
        }
        *saat_ini = new node_pohon(buku_baru);
    }

    // cari buku di pohon
    buku* cari_pohon(int id_buku) {
        node_pohon* saat_ini = akar_pohon;
        while (saat_ini) {
            if (id_buku == saat_ini->ptr_buku->id_buku)
                return saat_ini->ptr_buku;
            else if (id_buku < saat_ini->ptr_buku->id_buku)
                saat_ini = saat_ini->kiri;
            else
                saat_ini = saat_ini->kanan;
        }
        return nullptr;
    }

    // hapus node dari pohon pencarian biner
    node_pohon* hapus_node_pohon(node_pohon* node, int id_buku) {
        if (!node) return nullptr;
        if (id_buku < node->ptr_buku->id_buku) {
            node->kiri = hapus_node_pohon(node->kiri, id_buku);
        } else if (id_buku > node->ptr_buku->id_buku) {
            node->kanan = hapus_node_pohon(node->kanan, id_buku);
        } else {
            // kasus 1: node tanpa anak
            if (!node->kiri && !node->kanan) {
                delete node;
                return nullptr;
            }
            // kasus 2: node dengan satu anak
            if (!node->kiri) {
                node_pohon* temp = node->kanan;
                delete node;
                return temp;
            }
            if (!node->kanan) {
                node_pohon* temp = node->kiri;
                delete node;
                return temp;
            }
            // kasus 3: node dengan dua anak
            node_pohon* pengganti = node->kanan;
            while (pengganti->kiri) pengganti = pengganti->kiri;
            node->ptr_buku = pengganti->ptr_buku;
            node->kanan = hapus_node_pohon(node->kanan, pengganti->ptr_buku->id_buku);
        }
        return node;
    }

    // tambah buku
    void tambah_buku(int id_buku, string judul, string penulis, int nomor_rak) {
        if (jumlah_buku >= maks_buku) {
            cout << "===========================" << endl;
            cout << "|   perpustakaan penuh!   |" << endl;
            cout << "===========================" << endl;
            return;
        }
        for (int i = 0; i < jumlah_buku; i++) {
            if (daftar_buku[i].id_buku == id_buku) {
                cout << "=============================" << endl;
                cout << "|  id buku sudah digunakan! |" << endl;
                cout << "=============================" << endl;
                return;
            }
        }
        daftar_buku[jumlah_buku] = {id_buku, judul, penulis, nomor_rak, false};
        sisip_pohon(&daftar_buku[jumlah_buku]);
        jumlah_buku++;
        cout << "==================================" << endl;
        cout << "|   buku berhasil ditambahkan!   |" << endl;
        cout << "==================================" << endl;
    }

    // tambah anggota
    void tambah_anggota(int id_anggota, string nama) {
        if (jumlah_anggota >= maks_anggota) {
            cout << "=============================" << endl;
            cout << "|  batas anggota tercapai!  |" << endl;
            cout << "=============================" << endl;
            return;
        }
        for (int i = 0; i < jumlah_anggota; i++) {
            if (daftar_anggota[i].id_anggota == id_anggota) {
                cout << "=================================" << endl;
                cout << "|  id anggota sudah digunakan!  |" << endl;
                cout << "=================================" << endl;
                return;
            }
        }
        daftar_anggota[jumlah_anggota] = {id_anggota, nama, {}};
        jumlah_anggota++;
        cout << "=================================" << endl;
        cout << "| anggota berhasil ditambahkan! |" << endl;
        cout << "=================================" << endl;
    }

    // hapus buku
    void hapus_buku(int id_buku) {
        buku* buku_ditemukan = cari_pohon(id_buku);
        if (!buku_ditemukan) {
            cout << "==============================" << endl;
            cout << "|    buku tidak ditemukan!   |" << endl;
            cout << "==============================" << endl;
            return;
        }
        if (buku_ditemukan->dipinjam) {
            cout << "====================================" << endl;
            cout << "|   buku sedang dipinjam, tidak    |" << endl;
            cout << "|         bisa dihapus!            |" << endl;
            cout << "====================================" << endl;
            return;
        }
        // hapus dari pohon
        akar_pohon = hapus_node_pohon(akar_pohon, id_buku);
        // hapus dari array
        for (int i = 0; i < jumlah_buku; i++) {
            if (daftar_buku[i].id_buku == id_buku) {
                for (int j = i; j < jumlah_buku - 1; j++) {
                    daftar_buku[j] = daftar_buku[j + 1];
                }
                jumlah_buku--;
                break;
            }
        }
        cout << "=================================" << endl;
        cout << "|   buku berhasil dihapus!      |" << endl;
        cout << "=================================" << endl;
    }

    // hapus anggota
    void hapus_anggota(int id_anggota) {
        for (int i = 0; i < jumlah_anggota; i++) {
            if (daftar_anggota[i].id_anggota == id_anggota) {
                if (!daftar_anggota[i].buku_dipinjam.empty()) {
                    cout << "====================================" << endl;
                    cout << "| anggota sedang meminjam buku,    |" << endl;
                    cout << "|         tidak bisa dihapus!      |" << endl;
                    cout << "====================================" << endl;
                    return;
                }
                for (int j = i; j < jumlah_anggota - 1; j++) {
                    daftar_anggota[j] = daftar_anggota[j + 1];
                }
                jumlah_anggota--;
                cout << "=================================" << endl;
                cout << "| anggota berhasil dihapus!     |" << endl;
                cout << "=================================" << endl;
                return;
            }
        }
        cout << "====================================" << endl;
        cout << "|     anggota tidak ditemukan!     |" << endl;
        cout << "====================================" << endl;
    }

    // pinjam buku
    void pinjam_buku(int id_buku, int id_anggota) {
        buku* buku_ditemukan = cari_pohon(id_buku);
        if (!buku_ditemukan) {
            cout << "==============================" << endl;
            cout << "|    buku tidak ditemukan!   |" << endl;
            cout << "==============================" << endl;
            return;
        }
        if (buku_ditemukan->dipinjam) {
            cout << "====================================" << endl;
            cout << "|       buku sedang dipinjam!      |" << endl;
            cout << "| ditambahkan ke antrian pinjam.   |" << endl;
            cout << "====================================" << endl;
            antrian_pinjam.push({"pinjam", id_buku, id_anggota});
            return;
        }
        for (int i = 0; i < jumlah_anggota; i++) {
            if (daftar_anggota[i].id_anggota == id_anggota) {
                buku_ditemukan->dipinjam = true;
                daftar_anggota[i].buku_dipinjam.push_back(id_buku);
                tumpukan_transaksi.push({"pinjam", id_buku, id_anggota});
                cout << "==================================" << endl;
                cout << "|    buku berhasil dipinjam!     |" << endl;
                cout << "==================================" << endl;
                return;
            }
        }
        cout << "====================================" << endl;
        cout << "|     anggota tidak ditemukan!     |" << endl;
        cout << "====================================" << endl;
    }

    // kembalikan buku
    void kembalikan_buku(int id_buku, int id_anggota) {
        buku* buku_ditemukan = cari_pohon(id_buku);
        if (!buku_ditemukan) {
            cout << "==============================" << endl;
            cout << "|    buku tidak ditemukan!   |" << endl;
            cout << "==============================" << endl;
            return;
        }
        if (!buku_ditemukan->dipinjam) {
            cout << "====================================" << endl;
            cout << "|    buku sedang tidak dipinjam!   |" << endl;
            cout << "====================================" << endl;
            return;
        }
        for (int i = 0; i < jumlah_anggota; i++) {
            if (daftar_anggota[i].id_anggota == id_anggota) {
                for (auto it = daftar_anggota[i].buku_dipinjam.begin(); it != daftar_anggota[i].buku_dipinjam.end(); ++it) {
                    if (*it == id_buku) {
                        daftar_anggota[i].buku_dipinjam.erase(it);
                        buku_ditemukan->dipinjam = false;
                        tumpukan_transaksi.push({"kembali", id_buku, id_anggota});
                        cout << "====================================" << endl;
                        cout << "|    buku berhasil dikembalikan!   |" << endl;
                        cout << "====================================" << endl;
                        proses_antrian_pinjam();
                        return;
                    }
                }
                cout << "====================================" << endl;
                cout << "| anggota tidak meminjam buku ini! |" << endl;
                cout << "====================================" << endl;
                return;
            }
        }
        cout << "====================================" << endl;
        cout << "|     anggota tidak ditemukan!     |" << endl;
        cout << "====================================" << endl;
    }

    // proses antrian peminjaman
    void proses_antrian_pinjam() {
        if (antrian_pinjam.empty()) return;
        transaksi t = antrian_pinjam.front();
        buku* buku_ditemukan = cari_pohon(t.id_buku);
        if (buku_ditemukan && !buku_ditemukan->dipinjam) {
            antrian_pinjam.pop();
            pinjam_buku(t.id_buku, t.id_anggota);
        }
    }

    // batalkan transaksi terakhir
    void batalkan_transaksi() {
        if (tumpukan_transaksi.empty()) {
            cout << "====================================" << endl;
            cout << "|    tidak ada transaksi untuk     |" << endl;
            cout << "|            dibatalkan!           |" << endl;
            cout << "====================================" << endl;
            return;
        }
        transaksi t = tumpukan_transaksi.top();
        tumpukan_transaksi.pop();
        if (t.jenis == "pinjam") {
            kembalikan_buku(t.id_buku, t.id_anggota);
        } else {
            pinjam_buku(t.id_buku, t.id_anggota);
        }
        cout << "====================================" << endl;
        cout << "|  transaksi terakhir dibatalkan!  |" << endl;
        cout << "====================================" << endl;
    }

    // tampilkan daftar buku dalam tabel
    void tampilkan_buku() {
        system(CLEAR);
        cout << "> SECTION DAFTAR BUKU" << endl << endl;
        if (jumlah_buku == 0) {
            cout << "====================================" << endl;
            cout << "|       daftar buku kosong!        |" << endl;
            cout << "====================================" << endl;
            return;
        }
        cout << "+-----+--------------------------------+--------------------------------+-------+------------+" << endl;
        cout << "| id  | judul                          | penulis                        | rak   | status     |" << endl;
        cout << "+-----+--------------------------------+--------------------------------+-------+------------+" << endl;
        for (int i = 0; i < jumlah_buku; i++) {
            cout << "| " << setw(3) << daftar_buku[i].id_buku << " | "
                 << setw(30) << left << daftar_buku[i].judul.substr(0, 30) << " | "
                 << setw(30) << daftar_buku[i].penulis.substr(0, 30) << " | "
                 << setw(5) << daftar_buku[i].nomor_rak << " | "
                 << setw(10) << (daftar_buku[i].dipinjam ? "dipinjam" : "tersedia") << " |" << endl;
        }
        cout << "+-----+--------------------------------+--------------------------------+-------+------------+" << endl;
    }

    // tampilkan daftar anggota dalam tabel
    void tampilkan_anggota() {
        system(CLEAR);
        cout << "> SECTION DAFTAR ANGGOTA" << endl << endl;
        if (jumlah_anggota == 0) {
            cout << "====================================" << endl;
            cout << "|     daftar anggota kosong!       |" << endl;
            cout << "====================================" << endl;
            return;
        }
        cout << "+-----+----------------------+--------------------------------+" << endl;
        cout << "| id  | nama                 | buku dipinjam                  |" << endl;
        cout << "+-----+----------------------+--------------------------------+" << endl;
        for (int i = 0; i < jumlah_anggota; i++) {
            cout << "| " << setw(3) << daftar_anggota[i].id_anggota << " | "
                 << setw(20) << left << daftar_anggota[i].nama.substr(0, 20) << " | ";
            for (int id_buku : daftar_anggota[i].buku_dipinjam) {
                cout << id_buku << " ";
            }
            cout << setw(30 - daftar_anggota[i].buku_dipinjam.size() * 2) << "" << "|" << endl;
        }
        cout << "+-----+----------------------+--------------------------------+" << endl;
    }

    // cari buku
    void cari_buku(int id_buku) {
        system(CLEAR);
        buku* buku_ditemukan = cari_pohon(id_buku);
        if (buku_ditemukan) {
            cout << "> SECTION CARI BUKU " << endl << endl;
            cout << "+-----+--------------------------------+--------------------------------+-------+------------+" << endl;
            cout << "| id  | judul                          | penulis                        | rak   | status     |" << endl;
            cout << "+-----+--------------------------------+--------------------------------+-------+------------+" << endl;
            cout << "| " << setw(3) << buku_ditemukan->id_buku << " | "
                 << setw(30) << left << buku_ditemukan->judul.substr(0, 30) << " | "
                 << setw(30) << buku_ditemukan->penulis.substr(0, 30) << " | "
                 << setw(5) << buku_ditemukan->nomor_rak << " | "
                 << setw(10) << (buku_ditemukan->dipinjam ? "dipinjam" : "tersedia") << " |" << endl;
            cout << "+-----+--------------------------------+--------------------------------+-------+------------+" << endl;
        } else {
            cout << "==============================" << endl;
            cout << "|    buku tidak ditemukan!   |" << endl;
            cout << "==============================" << endl;
        }
    }

    // petakan lokasi buku berdasarkan rak
    void petakan_lokasi_buku(int nomor_rak) {
        system(CLEAR);
        if (nomor_rak < 0 || nomor_rak >= maks_rak) {
            cout << "====================================" << endl;
            cout << "|      nomor rak tidak valid!      |" << endl;
            cout << "====================================" << endl;
            return;
        }
        cout << "=== buku di rak " << nomor_rak << " ===" << endl;
        bool ada_buku = false;
        for (int i = 0; i < jumlah_buku; i++) {
            if (daftar_buku[i].nomor_rak == nomor_rak) {
                if (!ada_buku) {
                    cout << "+-----+--------------------------------+--------------------------------+" << endl;
                    cout << "| id  | judul                          | penulis                        |" << endl;
                    cout << "+-----+--------------------------------+--------------------------------+" << endl;
                    ada_buku = true;
                }
                cout << "| " << setw(3) << daftar_buku[i].id_buku << " | "
                     << setw(30) << left << daftar_buku[i].judul.substr(0, 30) << " | "
                     << setw(30) << daftar_buku[i].penulis.substr(0, 30) << " |" << endl;
            }
        }
        if (ada_buku) {
            cout << "+-----+--------------------------------+--------------------------------+" << endl;
            cout << "rak terdekat: ";
            for (int i = 0; i < maks_rak; i++) {
                if (graf_rak[nomor_rak][i] == 1) {
                    cout << i << " ";
                }
            }
            cout << endl;
        } else {
            cout << "====================================" << endl;
            cout << "|  tidak ada buku di rak ini!      |" << endl;
            cout << "====================================" << endl;
        }
    }
};

// tampilkan layar selamat datang
void tampilkan_selamat_datang() {
    system(CLEAR);
    cout << "==========================================" << endl;
    cout << "|                                        |" << endl;
    cout << "|   SELAMAT DATANG DI SISPERPUS UNSIKA   |" << endl;
    cout << "|                                        |" << endl;
    cout << "==========================================" << endl;
    cout << "tekan enter untuk masuk..." << endl;
    cin.get();
}

// tampilkan menu utama
void tampilkan_menu() {
    system(CLEAR);
    cout << "==========================================" << endl;
    cout << "|                                        |" << endl;
    cout << "|      MENU UTAMA SISPERPUS UNSIKA       |" << endl;
    cout << "|                                        |" << endl;
    cout << "==========================================" << endl;
    cout << "| 1. tambah buku" << endl;
    cout << "| 2. tambah anggota" << endl;
    cout << "| 3. pinjam buku" << endl;
    cout << "| 4. kembalikan buku" << endl;
    cout << "| 5. batalkan transaksi terakhir" << endl;
    cout << "| 6. tampilkan buku" << endl;
    cout << "| 7. tampilkan anggota" << endl;
    cout << "| 8. cari buku" << endl;
    cout << "| 9. petakan lokasi buku" << endl;
    cout << "| 10. hapus buku" << endl;
    cout << "| 11. hapus anggota" << endl;
    cout << "| 12. keluar" << endl;
    cout << "==========================================" << endl;
    cout << "masukkan pilihan \n: ";
}

int main() {
    perpustakaan lib;
    int pilihan, id_buku, nomor_rak, id_anggota;
    string judul, penulis, nama;

    // tampilkan layar selamat datang
    tampilkan_selamat_datang();

    while (true) {
        tampilkan_menu();
        cin >> pilihan;
        cin.ignore();

        switch (pilihan) {
            case 1: // tambah buku
                system(CLEAR);
                cout << "> SECTION TAMBAH BUKU" << endl << endl;
                cout << "masukkan id buku: ";
                cin >> id_buku;
                cin.ignore();
                cout << "masukkan judul: ";
                getline(cin, judul);
                cout << "masukkan penulis: ";
                getline(cin, penulis);
                cout << "masukkan nomor rak (0-4): ";
                cin >> nomor_rak;
                lib.tambah_buku(id_buku, judul, penulis, nomor_rak);
                cout << "tekan enter untuk lanjut...";
                cin.ignore();
                cin.get();
                break;

            case 2: // tambah anggota
                system(CLEAR);
                cout << "> SECTION TAMBAH ANGGOTA" << endl << endl;
                cout << "masukkan id anggota: ";
                cin >> id_anggota;
                cin.ignore();
                cout << "masukkan nama: ";
                getline(cin, nama);
                lib.tambah_anggota(id_anggota, nama);
                cout << "tekan enter untuk lanjut...";
                cin.get();
                break;

            case 3: // pinjam buku
                system(CLEAR);
                cout << "> SECTION PINJAM BUKU" << endl << endl;
                cout << "masukkan id buku: ";
                cin >> id_buku;
                cout << "masukkan id anggota: ";
                cin >> id_anggota;
                lib.pinjam_buku(id_buku, id_anggota);
                cout << "tekan enter untuk lanjut...";
                cin.ignore();
                cin.get();
                break;

            case 4: // kembalikan buku
                system(CLEAR);
                cout << "> SECTION KEMBALIKAN BUKU" << endl << endl;
                cout << "masukkan id buku: ";
                cin >> id_buku;
                cout << "masukkan id anggota: ";
                cin >> id_anggota;
                lib.kembalikan_buku(id_buku, id_anggota);
                cout << "tekan enter untuk lanjut...";
                cin.ignore();
                cin.get();
                break;

            case 5: // batalkan transaksi
                system(CLEAR);
                cout << "> SECTION BATALKAN TRANSAKSI" << endl << endl;
                lib.batalkan_transaksi();
                cout << "tekan enter untuk lanjut...";
                cin.ignore();
                cin.get();
                break;

            case 6: // tampilkan buku
                cout << "> SECTION TAMPILKAN BUKU" << endl << endl;
                lib.tampilkan_buku();
                cout << "tekan enter untuk lanjut...";
                cin.get();
                break;

            case 7: // tampilkan anggota
                cout << "> SECTION TAMPILKAN ANGGOTA" << endl << endl;
                lib.tampilkan_anggota();
                cout << "tekan enter untuk lanjut...";
                cin.get();
                break;

            case 8: // cari buku
                system(CLEAR);
                cout << "> SECTION CARI BUKU" << endl << endl;
                cout << "masukkan id buku untuk dicari: ";
                cin >> id_buku;
                lib.cari_buku(id_buku);
                cout << "tekan enter untuk lanjut...";
                cin.ignore();
                cin.get();
                break;

            case 9: // petakan lokasi buku
                system(CLEAR);
                cout << "> SECTION PETAKAN LOKASI BUKU" << endl << endl;
                cout << "masukkan nomor rak (0-4): ";
                cin >> nomor_rak;
                lib.petakan_lokasi_buku(nomor_rak);
                cout << "tekan enter untuk lanjut...";
                cin.ignore();
                cin.get();
                break;

            case 10: // hapus buku
                system(CLEAR);
                cout << "> SECTION HAPUS BUKU" << endl << endl;
                cout << "masukkan id buku untuk dihapus: ";
                cin >> id_buku;
                lib.hapus_buku(id_buku);
                cout << "tekan enter untuk lanjut...";
                cin.ignore();
                cin.get();
                break;

            case 11: // hapus anggota
                system(CLEAR);
                cout << "> SECTION HAPUS ANGGOTA" << endl << endl;
                cout << "masukkan id anggota untuk dihapus: ";
                cin >> id_anggota;
                lib.hapus_anggota(id_anggota);
                cout << "tekan enter untuk lanjut...";
                cin.ignore();
                cin.get();
                break;

            case 12: // keluar
                system(CLEAR);
                cout << "====================================" << endl;
                cout << "|    ANDA KELUAR DARI PROGRAM.     |" << endl;
                cout << "====================================" << endl;
                return 0;

            default:
                system(CLEAR);
                cout << "====================================" << endl;
                cout << "|   pilihan tidak valid!           |" << endl;
                cout << "|   tekan enter untuk kembali...   |" << endl;
                cout << "====================================" << endl;
                cin.get();
        }
    }
    return 0;
}