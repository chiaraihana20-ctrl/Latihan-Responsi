#include <iostream>

using namespace std;

string toLowerStr(string str) {
    for (int i = 0; i < str.length(); i++) {
        str[i] = tolower(str[i]);
    }
    return str;
}

struct QueueNode {
    string namaPelanggan;
    QueueNode* next;
};

struct MenuItem {
    string nama;         
    string namaLower;    
    string kategori;
    int harga;
    bool tersedia; 
    
    QueueNode* front;
    QueueNode* rear;

    MenuItem* left;
    MenuItem* right;
};

struct HistoryNode {
    string jenisAksi; 
    string namaMenu;
    string namaPelanggan;
    HistoryNode* next;
};

MenuItem* rootMenu = nullptr;
HistoryNode* topHistory = nullptr;

void initStack(HistoryNode*& top) {
    top = nullptr;
}

bool isEmptyStack(const HistoryNode* top) {
    return top == nullptr;
}

void push(HistoryNode*& top, string jenis, string menu, string pelanggan = "") {
    HistoryNode* newNode = new HistoryNode{jenis, menu, pelanggan, top};
    top = newNode;
}

bool pop(HistoryNode*& top, HistoryNode& dataKeluar) {
    if (isEmptyStack(top)) return false;
    HistoryNode* hapus = top;
    dataKeluar = *hapus;
    top = top->next;
    delete hapus;
    return true;
}

void initQueue(QueueNode*& front, QueueNode*& rear) {
    front = nullptr;
    rear = nullptr;
}

bool isEmptyQueue(const QueueNode* front) {
    return front == nullptr;
}

void enqueue(QueueNode*& front, QueueNode*& rear, string namaPelanggan) {
    QueueNode* baru = new QueueNode{namaPelanggan, nullptr};
    if (rear == nullptr) {
        front = rear = baru;
    } else {
        rear->next = baru;
        rear = baru;
    }
}

bool dequeue(QueueNode*& front, QueueNode*& rear, string& namaKeluar) {
    if (isEmptyQueue(front)) return false;
    QueueNode* hapus = front;
    namaKeluar = hapus->namaPelanggan;
    front = front->next;
    if (front == nullptr) {
        rear = nullptr;
    }
    delete hapus;
    return true;
}

MenuItem* insertMenu(MenuItem* node, string nama, string kategori, int harga) {
    string namaLower = toLowerStr(nama);
    if (node == nullptr) {
        MenuItem* baru = new MenuItem;
        baru->nama = nama;
        baru->namaLower = namaLower;
        baru->kategori = kategori;
        baru->harga = harga;
        baru->tersedia = true; 
        baru->front = nullptr;
        baru->rear = nullptr;
        baru->left = nullptr;
        baru->right = nullptr;
        return baru;
    }
    if (namaLower < node->namaLower) {
        node->left = insertMenu(node->left, nama, kategori, harga);
    } else if (namaLower > node->namaLower) {
        node->right = insertMenu(node->right, nama, kategori, harga);
    }
    return node;
}

void displayMenuInOrder(MenuItem* node) {
    if (node == nullptr) return;
    displayMenuInOrder(node->left);
    cout << "- " << node->nama << " | Kategori: " << node->kategori 
         << " | Harga: " << node->harga << " | Status: " 
         << (node->tersedia ? "Tersedia" : "Habis") << endl;
    displayMenuInOrder(node->right);
}

MenuItem* searchMenu(MenuItem* node, string namaCari) {
    string cariLower = toLowerStr(namaCari);
    while (node != nullptr) {
        if (cariLower == node->namaLower) {
            return node;
        } else if (cariLower < node->namaLower) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return nullptr;
}

MenuItem* findMin(MenuItem* node) {
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

MenuItem* deleteMenuNode(MenuItem* node, string namaCari, bool &success) {
    string cariLower = toLowerStr(namaCari);
    if (node == nullptr) {
        success = false;
        return nullptr;
    }

    if (cariLower < node->namaLower) {
        node->left = deleteMenuNode(node->left, namaCari, success);
    } else if (cariLower > node->namaLower) {
        node->right = deleteMenuNode(node->right, namaCari, success);
    } else {
        success = true;
        if (node->left == nullptr) {
            MenuItem* temp = node->right;
            delete node;
            return temp;
        } else if (node->right == nullptr) {
            MenuItem* temp = node->left;
            delete node;
            return temp;
        }

        MenuItem* temp = findMin(node->right);
        node->nama = temp->nama;
        node->namaLower = temp->namaLower;
        node->kategori = temp->kategori;
        node->harga = temp->harga;
        node->tersedia = temp->tersedia;
        node->front = temp->front;
        node->rear = temp->rear;
        node->right = deleteMenuNode(node->right, temp->nama, success);
    }
    return node;
}

void tambahMenu(string nama, string kategori, int harga) {
    rootMenu = insertMenu(rootMenu, nama, kategori, harga);
    cout << "Menu berhasil ditambahkan\n";
}

void lihatDaftarMenu() {
    if (rootMenu == nullptr) {
        cout << "Belum ada menu yang terdaftar.\n";
        return;
    }
    cout << "=== DAFTAR MENU RESTORAN ===\n";
    displayMenuInOrder(rootMenu);
}

void ubahStatusMenu(string namaMenu) {
    MenuItem* menu = searchMenu(rootMenu, namaMenu);
    if (menu == nullptr) {
        cout << "Menu tidak ditemukan.\n";
        return;
    }

    if (menu->tersedia) {
        menu->tersedia = false;
        cout << "Menu " << menu->nama << " sekarang berstatus: Habis\n";
        push(topHistory, "STATUS", menu->nama);
    } else {
        menu->tersedia = true;
        cout << "Menu " << menu->nama << " sekarang berstatus: Tersedia kembali\n";
        push(topHistory, "STATUS", menu->nama);

        if (!isEmptyQueue(menu->front)) {
            string pelangganPertama;
            dequeue(menu->front, menu->rear, pelangganPertama);
            cout << "Pesanan langsung diberikan kepada " << pelangganPertama << " dari waiting list\n";
        }
    }
}

void catatPesanan(string namaPelanggan, string namaMenu) {
    MenuItem* menu = searchMenu(rootMenu, namaMenu);
    if (menu == nullptr) {
        cout << "Menu tidak ditemukan.\n";
        return;
    }

    if (menu->tersedia) {
        cout << "Pesanan " << namaPelanggan << " berhasil dibuat\n";
        push(topHistory, "PESAN", menu->nama, namaPelanggan);
    } else {
        cout << "Menu sedang habis, " << namaPelanggan << " masuk ke dalam waiting list\n";
        enqueue(menu->front, menu->rear, namaPelanggan);
        push(topHistory, "WAITING", menu->nama, namaPelanggan);
    }
}

void hapusMenu(string namaMenu) {
    bool success = false;
    rootMenu = deleteMenuNode(rootMenu, namaMenu, success);
    if (success) {
        cout << "Menu berhasil dihapus\n";
    } else {
        cout << "Menu tidak ditemukan untuk dihapus.\n";
    }
}

void undoAksi() {
    if (isEmptyStack(topHistory)) {
        cout << "Tidak ada aksi untuk di-undo\n";
        return;
    }

    HistoryNode lastAction;
    pop(topHistory, lastAction);

    if (lastAction.jenisAksi == "PESAN" || lastAction.jenisAksi == "WAITING") {
        cout << "Undo: Pesanan menu " << lastAction.namaMenu << " dibatalkan\n";
    } else if (lastAction.jenisAksi == "STATUS") {
        MenuItem* menu = searchMenu(rootMenu, lastAction.namaMenu);
        if (menu != nullptr) {
            menu->tersedia = !menu->tersedia;
        }
        cout << "Undo: Pembaruan status menu " << lastAction.namaMenu << " dibatalkan\n";
    }
}

void tampilkanAntrian(string namaMenu) {
    MenuItem* menu = searchMenu(rootMenu, namaMenu);
    if (menu == nullptr) {
        cout << "Menu tidak ditemukan.\n";
        return;
    }

    if (isEmptyQueue(menu->front)) {
        cout << "Antrian kosong\n";
    } else {
        QueueNode* curr = menu->front;
        while (curr != nullptr) {
            cout << curr->namaPelanggan << " menunggu menu " << menu->nama << endl;
            curr = curr->next;
        }
    }
}

int main() {
    initStack(topHistory);
    int pilihan;
    string nama, kategori, pelanggan;
    int harga;

    do {
        cout << "\n=========================================\n";
        cout << "   SISTEM MANAJEMEN PESANAN RESTORAN     \n";
        cout << "=========================================\n";
        cout << "1. Tambah Menu Makanan\n";
        cout << "2. Lihat Daftar Menu\n";
        cout << "3. Catat Pesanan Pelanggan\n";
        cout << "4. Ubah Status Menu (Tersedia / Habis)\n";
        cout << "5. Hapus Menu\n";
        cout << "6. Undo Aksi Terakhir\n";
        cout << "7. Lihat Antrian Pelanggan\n";
        cout << "0. Keluar\n";
        cout << "Pilih opsi: ";
        cin >> pilihan;
        cin.ignore();

        switch (pilihan) {
            case 1:
                cout << "Masukkan Nama Menu: ";
                getline(cin, nama);
                cout << "Masukkan Kategori Makanan: ";
                getline(cin, kategori);
                cout << "Masukkan Harga: ";
                cin >> harga;
                tambahMenu(nama, kategori, harga);
                break;
            case 2:
                lihatDaftarMenu();
                break;
            case 3:
                cout << "Masukkan Nama Pelanggan: ";
                getline(cin, pelanggan);
                cout << "Masukkan Nama Menu yang Dipesan: ";
                getline(cin, nama);
                catatPesanan(pelanggan, nama);
                break;
            case 4:
                cout << "Masukkan Nama Menu yang Ingin Diubah Statusnya: ";
                getline(cin, nama);
                ubahStatusMenu(nama);
                break;
            case 5:
                cout << "Masukkan Nama Menu yang Ingin Dihapus: ";
                getline(cin, nama);
                hapusMenu(nama);
                break;
            case 6:
                undoAksi();
                break;
            case 7:
                cout << "Masukkan Nama Menu untuk Cek Antrian: ";
                getline(cin, nama);
                tampilkanAntrian(nama);
                break;
            case 0:
                cout << "Terima kasih telah menggunakan program ini. Semangat!\n";
                break;
            default:
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }
    } while (pilihan != 0);

    return 0;
}
