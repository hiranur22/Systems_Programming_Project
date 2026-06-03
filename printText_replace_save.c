
#include <stdio.h>
#include <string.h>
#include <ncurses.h>

/* ── Projenin ortak tanımları (editor.h'dan gelecek) ── */
#define MAX_LINES   100
#define MAX_CHARS    40
#define MAX_DISPLAY  30

struct node {
    char statement[MAX_CHARS];
    int  next;
    int  prev;
};

/* Bu global değişkenler buffer.c içinde tanımlanmış,
   biz sadece "extern" ile kullanıyoruz */
extern struct node textbuffer[MAX_LINES];
extern int head;
extern int tail;
extern int freeIdx;
extern int opCount;
extern char currentFile[256];


/* ══════════════════════════════════════════════════════════════
 * printText()
 *
 * Parametreler:
 *   highlightLine : şu an seçili satırın ekran sırası (0,1,2...)
 *   inCharMode    : 0 = satır seçme modu, 1 = karakter seçme modu
 *   selectedCol   : karakter modunda hangi sütun seçili
 *
 * Yapılan iş:
 *   head'den başla → linked list'i takip et → her satırı yaz
 *   Seçili satırı renkli/vurgulu göster
 *   En alta ">" prompt'u koy
 * ══════════════════════════════════════════════════════════════ */
void printText(int highlightLine, int inCharMode, int selectedCol) {
    int cur = head;   /* linked list'te gezinmek için mevcut node index'i */
    int row = 0;      /* ekranda kaçıncı satıra yazıyoruz                 */
    int col;          /* karakter modunda sütun sayacı                    */
    int len;          /* o satırdaki karakter sayısı                      */

    clear(); /* ekranı temizle */

    while (cur != -1 && row < MAX_DISPLAY) {

        if (row == highlightLine) {
            /* --- Bu satır seçili --- */
            if (inCharMode) {
                /*
                 * R tuşuna basıldıktan sonra buraya girilir.
                 * Satırdaki her karakteri tek tek yaz,
                 * selectedCol pozisyonundaki karakteri vurgula
                 * (kullanıcı tam olarak hangi karakteri değiştireceğini görsün)
                 */
                len = strlen(textbuffer[cur].statement);
                for (col = 0; col < len; col++) {
                    if (col == selectedCol) {
                        attron(A_BOLD | A_REVERSE);          /* vurgulu karakter */
                        mvaddch(row, col, textbuffer[cur].statement[col]);
                        attroff(A_BOLD | A_REVERSE);
                    } else {
                        attron(A_BOLD);                      /* diğer karakterler */
                        mvaddch(row, col, textbuffer[cur].statement[col]);
                        attroff(A_BOLD);
                    }
                }
            } else {
                /*
                 * Satır seçme modunda:
                 * Tüm satırı ters renkle (beyaz üstüne siyah) göster.
                 * %-39s → 39 karaktere kadar boşlukla doldur,
                 *          böylece highlight tüm satır boyunca görünür
                 */
                attron(A_REVERSE);
                mvprintw(row, 0, "%-39s", textbuffer[cur].statement);
                attroff(A_REVERSE);
            }
        } else {
            /* --- Seçili olmayan satır: düz yaz --- */
            mvprintw(row, 0, "%s", textbuffer[cur].statement);
        }

        cur = textbuffer[cur].next; /* linked list'te bir sonraki node'a geç */
        row++;
    }

    /* Komut satırı */
    mvprintw(row, 0, ">");
    move(row, 2);  /* imleci ">" den sonraya taşı */

    refresh(); /* ncurses'te her zaman refresh() ile ekranı güncelle */
}


/* ══════════════════════════════════════════════════════════════
 * replaceChar()
 *
 * Parametreler:
 *   lineIdx : değişiklik yapılacak node'un textbuffer[] index'i
 *             (cursorLine() tarafından döndürülür)
 *   charPos : o satırda değiştirilecek karakterin konumu
 *             (cursorChar() tarafından döndürülür)
 *   newChar : yazılacak yeni karakter
 *
 * Yapılan iş:
 *   Sadece tek bir karakter değişir.
 *   Linked list yapısına (next/prev) hiç dokunulmaz.
 *   opCount artırılmaz (replace insert/delete sayılmaz).
 * ══════════════════════════════════════════════════════════════ */
void replaceChar(int lineIdx, int charPos, char newChar) {
    int len;

    /* Geçersiz index kontrolü */
    if (lineIdx < 0 || lineIdx >= MAX_LINES) return;
    if (charPos < 0) return;

    len = strlen(textbuffer[lineIdx].statement);

    /* Satır sınırı dışına çıkma kontrolü */
    if (charPos >= len) return;

    /* Tek satır: karakteri güncelle */
    textbuffer[lineIdx].statement[charPos] = newChar;
}


/* ══════════════════════════════════════════════════════════════
 * save()
 *
 * Yapılan iş:
 *   currentFile isimli dosyayı yaz modunda aç.
 *   head'den başlayarak linked list'i takip et.
 *   Her node'un statement'ını dosyaya yaz (\n ekle).
 *   Dosyayı kapat.
 *
 * ÖNEMLİ:
 *   Silinmiş node'lar linked list'te görünmez (next/prev koparılmış),
 *   dolayısıyla dosyaya otomatik olarak yazılmazlar.
 *   Yani garbage collection yapılmasa bile save() doğru çalışır.
 * ══════════════════════════════════════════════════════════════ */
void save() {
    FILE *fp;
    int   cur;

    if (currentFile[0] == '\0') {
        /* Dosya adı henüz belirlenmemişse hata göster */
        mvprintw(MAX_DISPLAY + 1, 0, "HATA: Dosya adi yok!");
        refresh();
        return;
    }

    fp = fopen(currentFile, "w"); /* "w" → dosyayı sıfırdan yaz */
    if (fp == NULL) {
        mvprintw(MAX_DISPLAY + 1, 0, "HATA: %s acilamadi!", currentFile);
        refresh();
        return;
    }

    /* Linked list'i baştan sona dolaş, her satırı dosyaya yaz */
    cur = head;
    while (cur != -1) {
        fprintf(fp, "%s\n", textbuffer[cur].statement);
        cur = textbuffer[cur].next; /* bir sonraki node'a geç */
    }

    fclose(fp);

    /* Kullanıcıya bilgi mesajı */
    mvprintw(MAX_DISPLAY + 1, 0, "Kaydedildi: %-30s", currentFile);
    refresh();
}
