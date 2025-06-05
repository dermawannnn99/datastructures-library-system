#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <limits>
#include <iomanip>
#include <string>

using namespace std;

struct Mahasiswa {
    string nama;
    string nim;
    string jurusan;
    float ipk;
};

vector<Mahasiswa> daftarMahasiswa;
map<int, vector<Mahasiswa>> pembagianKelompok;
vector<map<int, vector<Mahasiswa>>> riwayatPembagian;

// Membersihkan input buffer
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Validasi input numerik
bool inputNumerik(int &value, const string &prompt) {
    cout << prompt;
    if (cin >> value) return true;
    cout << "Input harus angka.\n";
    clearInputBuffer();
    return false;
}

// Validasi input string tidak kosong
string inputString(const string &prompt) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        if (!input.empty()) return input;
        cout << "Input tidak boleh kosong.\n";
    }
}

// Validasi NIM (10 digit angka)
bool validasiNIM(const string &nim) {
    return nim.length() == 10 && all_of(nim.begin(), nim.end(), ::isdigit);
}

// Cek duplikasi NIM
bool cekDuplikasiNIM(const string &nim) {
    for (const auto &m : daftarMahasiswa) {
        if (m.nim == nim) return true;
    }
    return false;
}

void tambahMahasiswa() {
    Mahasiswa m;
    clearInputBuffer();
    m.nama = inputString("Nama: ");
    
    while (true) {
        m.nim = inputString("NIM: ");
        if (!validasiNIM(m.nim)) {
            cout << "NIM harus 10 digit angka.\n";
            continue;
        }
        if (cekDuplikasiNIM(m.nim)) {
            cout << "NIM sudah terdaftar. Gunakan NIM lain.\n";
            continue;
        }
        break;
    }
    
    m.jurusan = inputString("Jurusan: ");
    
    while (true) {
        cout << "IPK: ";
        if (cin >> m.ipk && m.ipk >= 0.0 && m.ipk <= 4.0) break;
        cout << "IPK tidak valid (0.0 - 4.0).\n";
        clearInputBuffer();
    }
    
    daftarMahasiswa.push_back(m);
    cout << "\033[1;32mMahasiswa berhasil ditambahkan.\033[0m\n";
}

void tampilkanSemua() {
    if (daftarMahasiswa.empty()) {
        cout << "Belum ada data mahasiswa.\n";
        return;
    }
    cout << left << setw(5) << "No" << setw(20) << "Nama" << setw(15) << "NIM" 
         << setw(20) << "Jurusan" << setw(5) << "IPK" << endl;
    cout << string(65, '-') << endl;
    for (size_t i = 0; i < daftarMahasiswa.size(); ++i) {
        cout << left << setw(5) << i + 1 << setw(20) << daftarMahasiswa[i].nama
             << setw(15) << daftarMahasiswa[i].nim << setw(20) << daftarMahasiswa[i].jurusan
             << setw(5) << fixed << setprecision(2) << daftarMahasiswa[i].ipk << endl;
    }
}

void tampilkanPot() {
    if (daftarMahasiswa.empty()) {
        cout << "Belum ada data mahasiswa.\n";
        return;
    }
    vector<Mahasiswa> sorted = daftarMahasiswa;
    sort(sorted.begin(), sorted.end(), [](const Mahasiswa& a, const Mahasiswa& b) {
        return a.ipk > b.ipk;
    });
    int jumlahPot = 3;
    int perPot = sorted.size() / jumlahPot + (sorted.size() % jumlahPot != 0);

    for (int i = 0; i < jumlahPot; ++i) {
        cout << "\nPot " << i + 1 << ":\n";
        int start = i * perPot;
        int end = min((i + 1) * perPot, (int)sorted.size());
        for (int j = start; j < end; ++j) {
            cout << "- " << sorted[j].nama << " (" << sorted[j].nim << ", IPK: " 
                 << fixed << setprecision(2) << sorted[j].ipk << ")\n";
        }
    }
}

void ubahMahasiswa() {
    tampilkanSemua();
    int idx;
    while (!inputNumerik(idx, "Pilih nomor mahasiswa yang ingin diubah: ") || 
           idx < 1 || idx > daftarMahasiswa.size()) {
        cout << "Nomor tidak valid.\n";
    }
    Mahasiswa& m = daftarMahasiswa[idx - 1];
    clearInputBuffer();
    m.nama = inputString("Nama baru: ");
    
    while (true) {
        m.nim = inputString("NIM baru: ");
        if (!validasiNIM(m.nim)) {
            cout << "NIM harus 10 digit angka.\n";
            continue;
        }
        if (cekDuplikasiNIM(m.nim) && m.nim != daftarMahasiswa[idx - 1].nim) {
            cout << "NIM sudah terdaftar. Gunakan NIM lain.\n";
            continue;
        }
        break;
    }
    
    m.jurusan = inputString("Jurusan baru: ");
    
    while (true) {
        cout << "IPK baru: ";
        if (cin >> m.ipk && m.ipk >= 0.0 && m.ipk <= 4.0) break;
        cout << "IPK tidak valid (0.0 - 4.0).\n";
        clearInputBuffer();
    }
    
    cout << "\033[1;32mData berhasil diubah.\033[0m\n";
}

void hapusMahasiswa() {
    tampilkanSemua();
    int idx;
    while (!inputNumerik(idx, "Pilih nomor mahasiswa yang ingin dihapus: ") || 
           idx < 1 || idx > daftarMahasiswa.size()) {
        cout << "Nomor tidak valid.\n";
    }
    daftarMahasiswa.erase(daftarMahasiswa.begin() + (idx - 1));
    cout << "\033[1;32mData berhasil dihapus.\033[0m\n";
}

void bagiKelompokAcak() {
    int jumlahKelompok;
    while (!inputNumerik(jumlahKelompok, "Masukkan jumlah kelompok: ") || 
           jumlahKelompok <= 0 || jumlahKelompok > daftarMahasiswa.size()) {
        cout << "Jumlah kelompok tidak valid atau melebihi jumlah mahasiswa.\n";
    }
    if (daftarMahasiswa.empty()) {
        cout << "Belum ada data mahasiswa.\n";
        return;
    }

    vector<Mahasiswa> acakMahasiswa = daftarMahasiswa;
    sort(acakMahasiswa.begin(), acakMahasiswa.end(), [](const Mahasiswa& a, const Mahasiswa& b) {
        return a.ipk > b.ipk; // Sort descending by IPK
    });

    riwayatPembagian.push_back(pembagianKelompok); // Simpan riwayat
    pembagianKelompok.clear();

    // Distribusi round-robin berdasarkan IPK untuk keseimbangan
    for (size_t i = 0; i < acakMahasiswa.size(); ++i) {
        int kelompokKe = i % jumlahKelompok + 1;
        pembagianKelompok[kelompokKe].push_back(acakMahasiswa[i]);
    }
    cout << "\033[1;32mPembagian berhasil dilakukan.\033[0m\n";
}

void lihatPembagian() {
    if (pembagianKelompok.empty()) {
        cout << "Belum ada pembagian kelompok.\n";
        return;
    }
    for (const auto& [kelompok, anggota] : pembagianKelompok) {
        cout << "\nKelompok " << kelompok << ":\n";
        for (const auto& m : anggota) {
            cout << "- " << m.nama << " (" << m.nim << ", IPK: " 
                 << fixed << setprecision(2) << m.ipk << ")\n";
        }
    }
}

void undoPembagian() {
    if (riwayatPembagian.empty()) {
        cout << "Tidak ada riwayat untuk undo.\n";
        return;
    }
    pembagianKelompok = riwayatPembagian.back();
    riwayatPembagian.pop_back();
    cout << "\033[1;32mUndo berhasil.\033[0m\n";
}

void lihatRiwayat() {
    if (riwayatPembagian.empty()) {
        cout << "Belum ada riwayat pembagian.\n";
        return;
    }
    int idx = 1;
    for (const auto& riwayat : riwayatPembagian) {
        cout << "\n-- Riwayat ke-" << idx++ << " --\n";
        for (const auto& [kelompok, anggota] : riwayat) {
            cout << "Kelompok " << kelompok << ": ";
            for (const auto& m : anggota) cout << m.nama << ", ";
            cout << "\n";
        }
    }
}

void cariMahasiswa() {
    clearInputBuffer();
    string query = inputString("Masukkan nama atau NIM: ");
    bool found = false;
    cout << left << setw(5) << "No" << setw(20) << "Nama" << setw(15) << "NIM" 
         << setw(20) << "Jurusan" << setw(5) << "IPK" << endl;
    cout << string(65, '-') << endl;
    for (size_t i = 0; i < daftarMahasiswa.size(); ++i) {
        if (daftarMahasiswa[i].nama.find(query) != string::npos || 
            daftarMahasiswa[i].nim == query) {
            cout << left << setw(5) << i + 1 << setw(20) << daftarMahasiswa[i].nama
                 << setw(15) << daftarMahasiswa[i].nim << setw(20) << daftarMahasiswa[i].jurusan
                 << setw(5) << fixed << setprecision(2) << daftarMahasiswa[i].ipk << endl;
            found = true;
        }
    }
    if (!found) cout << "Tidak ada mahasiswa dengan nama/NIM tersebut.\n";
}

void simpanMahasiswa() {
    ofstream file("data_mahasiswa.txt", ios::out | ios::trunc);
    if (!file.is_open()) {
        cout << "Gagal membuka file untuk penulisan.\n";
        return;
    }
    for (const auto& m : daftarMahasiswa) {
        file << m.nama << ";" << m.nim << ";" << m.jurusan << ";" << m.ipk << "\n";
    }
    file.close();
    cout << "\033[1;32mData mahasiswa disimpan ke data_mahasiswa.txt.\033[0m\n";
}

void imporMahasiswa() {
    ifstream file("data_mahasiswa.txt");
    if (!file.is_open()) {
        cout << "Gagal membaca file.\n";
        return;
    }
    daftarMahasiswa.clear();
    string line;
    while (getline(file, line)) {
        Mahasiswa m;
        size_t pos1 = line.find(';');
        size_t pos2 = line.find(';', pos1 + 1);
        size_t pos3 = line.find(';', pos2 + 1);
        if (pos1 == string::npos || pos2 == string::npos || pos3 == string::npos) continue;
        m.nama = line.substr(0, pos1);
        m.nim = line.substr(pos1 + 1, pos2 - pos1 - 1);
        m.jurusan = line.substr(pos2 + 1, pos3 - pos2 - 1);
        try {
            m.ipk = stof(line.substr(pos3 + 1));
            if (m.ipk < 0.0 || m.ipk > 4.0) continue;
            daftarMahasiswa.push_back(m);
        } catch (...) {
            continue;
        }
    }
    file.close();
    cout << "\033[1;32mData mahasiswa berhasil diimpor.\033[0m\n";
}

void exportKeFile() {
    if (ifstream("hasil_pembagian.txt")) {
        cout << "File sudah ada. Timpa? (y/n): ";
        char choice; cin >> choice;
        if (tolower(choice) != 'y') {
            cout << "Ekspor dibatalkan.\n";
            return;
        }
    }
    ofstream file("hasil_pembagian.txt", ios::out | ios::trunc);
    if (!file.is_open()) {
        cout << "Gagal membuka file untuk penulisan.\n";
        return;
    }
    for (const auto& [kelompok, anggota] : pembagianKelompok) {
        file << "Kelompok " << kelompok << ":\n";
        for (const auto& m : anggota) {
            file << "- " << m.nama << " (" << m.nim << ", IPK: " 
                 << fixed << setprecision(2) << m.ipk << ")\n";
        }
    }
    file.close();
    cout << "\033[1;32mData berhasil diekspor ke hasil_pembagian.txt.\033[0m\n";
}

void resetData() {
    daftarMahasiswa.clear();
    pembagianKelompok.clear();
    riwayatPembagian.clear();
    cout << "\033[1;32mSemua data telah direset.\033[0m\n";
}

int main() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    cout << "Selamat datang di Sistem Pengelolaan Mahasiswa!\n"
         << "Gunakan menu untuk mengelola data mahasiswa dan pembagian kelompok.\n\n";

    int pilihan;
    do {
        cout << "\n===== MENU UTAMA =====\n";
        cout << "1. Tambah Mahasiswa\n";
        cout << "2. Tampilkan Semua Mahasiswa\n";
        cout << "3. Ubah Data Mahasiswa\n";
        cout << "4. Hapus Mahasiswa\n";
        cout << "5. Bagi Kelompok Secara Acak\n";
        cout << "6. Lihat Hasil Pembagian\n";
        cout << "7. Undo Pembagian\n";
        cout << "8. Lihat Riwayat Pembagian\n";
        cout << "9. Tampilkan Pot Berdasarkan IPK\n";
        cout << "10. Cari Mahasiswa\n";
        cout << "11. Simpan Data Mahasiswa\n";
        cout << "12. Impor Data Mahasiswa\n";
        cout << "13. Export Pembagian ke File\n";
        cout << "14. Reset Semua Data\n";
        cout << "0. Keluar\n";
        while (!inputNumerik(pilihan, "Pilih menu: ")) {
            cout << "Pilih menu: ";
        }

        switch (pilihan) {
            case 1: tambahMahasiswa(); break;
            case 2: tampilkanSemua(); break;
            case 3: ubahMahasiswa(); break;
            case 4: hapusMahasiswa(); break;
            case 5: bagiKelompokAcak(); break;
            case 6: lihatPembagian(); break;
            case 7: undoPembagian(); break;
            case 8: lihatRiwayat(); break;
            case 9: tampilkanPot(); break;
            case 10: cariMahasiswa(); break;
            case 11: simpanMahasiswa(); break;
            case 12: imporMahasiswa(); break;
            case 13: exportKeFile(); break;
            case 14: resetData(); break;
            case 0: cout << "\033[1;32mTerima kasih telah menggunakan program ini!\033[0m\n"; break;
            default: cout << "Pilihan tidak valid.\n";
        }
    } while (pilihan != 0);
    return 0;
}