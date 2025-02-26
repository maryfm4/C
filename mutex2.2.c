#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int stankonta1 = 0;
int stankonta2 = 0;

// Semafor jako połączenie mutexa i zmiennej warunkowej
pthread_mutex_t mutex;
pthread_cond_t cond1;
pthread_cond_t cond2;
int semafor1 = 1; // 1 oznacza wolny, 0 zajęty
int semafor2 = 1;

struct KlientArguments {
    int konto; // 1 lub 2
    int liczba_operacji;
    int wartosc_operacji;
    int operacja; // 0 wplata, 1 wyplata
};

void zajmij_semafor(int konto) {
    pthread_mutex_lock(&mutex);
    if (konto == 1){
        while (semafor1== 0) {
            pthread_cond_wait(&cond1, &mutex);
        }
        semafor1 = 0;
    }else if (konto == 2){
        while (semafor2 == 0) {
            pthread_cond_wait(&cond2, &mutex);
        }
        semafor2 = 0;
    }
    pthread_mutex_unlock(&mutex);
}

void zwolnij_semafor(int konto) {
    pthread_mutex_lock(&mutex);
    if (konto == 1){
        semafor1 = 1;
        pthread_cond_signal(&cond1);
    }else if ( konto == 2 ){
        semafor2 = 1;
        pthread_cond_signal(&cond2);
    }
    pthread_mutex_unlock(&mutex);
}

void* klient(void* arg) {
    int i = 0;
    int liczba_operacji = ((struct KlientArguments *)arg)->liczba_operacji;
    int wartosc_operacji = ((struct KlientArguments *)arg)->wartosc_operacji;
    int operacja = ((struct KlientArguments *)arg)->operacja;
    int numer_konta =((struct KlientArguments *)arg)->konto;

    while (i < liczba_operacji) {
        usleep(rand() % (500000 + 1 - 1000) + 1000);
        if (operacja == 0) { 
            if (numer_konta == 1){
                printf("Id wpłaty %d. Wpłacam %d zł\n", i, wartosc_operacji);
                zajmij_semafor(numer_konta);
                stankonta1 += wartosc_operacji;
                printf("Wpłata %d zakończona. Nowe saldo konta 1: %d zł\n", i, stankonta1);
                zwolnij_semafor(numer_konta);
            } else if (numer_konta == 2){
                printf("Id wpłaty %d. Wpłacam %d zł\n", i, wartosc_operacji);
                zajmij_semafor(numer_konta);
                stankonta2 += wartosc_operacji;
                printf("Wpłata %d zakończona. Nowe saldo konta 2: %d zł\n", i, stankonta2);
                zwolnij_semafor(numer_konta);
            }
        } else {
            if (numer_konta == 1){
                printf("Id wypłaty %d. Wypłacam %d zł\n", i, wartosc_operacji);
                zajmij_semafor(numer_konta);
                if (wartosc_operacji > stankonta1) {
                    printf("Id wypłaty %d. Brak środków na koncie 1\n", i);
                    zwolnij_semafor(numer_konta);
                    continue;
                }
                stankonta1 -= wartosc_operacji;
                printf("Wypłata %d zakończona. Nowe saldo konta 1: %d zł\n", i, stankonta1);
                zwolnij_semafor(numer_konta);
            }else if ( numer_konta == 2 ){
                printf("Id wypłaty %d. Wypłacam %d zł\n", i, wartosc_operacji);
                zajmij_semafor(numer_konta);
                if (wartosc_operacji > stankonta2) {
                    printf("Id wypłaty %d. Brak środków na koncie 2\n", i);
                    zwolnij_semafor(numer_konta);
                    continue;
                }
                stankonta2 -= wartosc_operacji;
                printf("Wypłata %d zakończona. Nowe saldo konta 2: %d zł\n", i, stankonta2);
                zwolnij_semafor(numer_konta);
            }
        }
        i++;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Mutex nie został zainicjalizowany");
        return 1;
    }
    if (pthread_cond_init(&cond1, NULL) != 0) {
        perror("Zmienna warunkowa 1 nie została zainicjalizowana");
        return 1;
    }
    if (pthread_cond_init(&cond2, NULL) != 0) {
        perror("Zmienna warunkowa 2 nie została zainicjalizowana");
        return 1;
    }

    struct KlientArguments wplata1;
    wplata1.konto = 1;
    wplata1.liczba_operacji = 10;
    wplata1.wartosc_operacji = 100;
    wplata1.operacja = 0;

    struct KlientArguments wyplata1;
    wyplata1.konto =1;
    wyplata1.liczba_operacji = 20;
    wyplata1.wartosc_operacji = 50;
    wyplata1.operacja = 1;

    struct KlientArguments wplata2;
    wplata2.konto =  2;
    wplata2.liczba_operacji = 10;
    wplata2.wartosc_operacji = 100;
    wplata2.operacja = 0;

    struct KlientArguments wyplata2;
    wyplata2.konto = 2;
    wyplata2.liczba_operacji = 20;
    wyplata2.wartosc_operacji = 50;
    wyplata2.operacja = 1;

    pthread_t thread_wplata1;
    pthread_t thread_wyplata1;
    pthread_t thread_wplata2;
    pthread_t thread_wyplata2;

    pthread_create(&thread_wplata1, NULL, klient, (void *)&wplata1);
    pthread_create(&thread_wyplata1, NULL, klient, (void *)&wyplata1);

    pthread_create(&thread_wplata2, NULL, klient, (void *)&wplata2);
    pthread_create(&thread_wyplata2, NULL, klient, (void *)&wyplata2);

    pthread_join(thread_wplata1, NULL);
    pthread_join(thread_wyplata1, NULL);
    pthread_join(thread_wplata2, NULL);
    pthread_join(thread_wyplata2, NULL);
    

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);
    return 0;
}