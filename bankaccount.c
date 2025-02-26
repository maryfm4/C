#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

int stankonta = 0; 
sem_t semAddress;

struct KlientArguments{
    int liczba_operacji;
    int wartosc_operacji;
    int operacja; //0 wplata 1 wyplata
};


void* klient(void* arg) {
    int i = 0;
    int liczba_operacji = ((struct KlientArguments *)arg)->liczba_operacji;
    int wartosc_operacji = ((struct KlientArguments *)arg)->wartosc_operacji;
    int operacja = ((struct KlientArguments *)arg)->operacja;
    

    while (i < liczba_operacji) {
        usleep(rand()%(500000+1-1000)+1000);  // Opoznienie
        if (operacja == 0) {
            printf("Id wplaty %d. Wplacam %d zł\n", i, wartosc_operacji);
            sem_wait(&semAddress);
            stankonta += wartosc_operacji;
            printf("Wplata %d zakonczona. Nowe saldo: %d zł\n", i, stankonta);
            sem_post(&semAddress);
        } else {
            printf("Id wyplaty %d. Wyplacam %d zł\n", i, wartosc_operacji);
            sem_wait(&semAddress);
            if (wartosc_operacji > stankonta){
                printf("Id wyplaty %d .Brak srodkow na koncie\n", i);
                sem_post(&semAddress);
                continue;
            }
            stankonta -= wartosc_operacji;
            printf("Wyplata %d zakonczona. Nowe saldo: %d zł\n",i , stankonta);
            sem_post(&semAddress);
        }
        i++;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (sem_init(&semAddress, 0, 1) != 0) {
        perror("Semafor nie został zainicjalizowany");
        return 1;
    }

    struct KlientArguments wplata;
    wplata.liczba_operacji = 10;
    wplata.wartosc_operacji = 100;
    wplata.operacja = 0;


    struct KlientArguments wyplata;
    wyplata.liczba_operacji = 20;
    wyplata.wartosc_operacji = 50;
    wyplata.operacja = 1;

    pthread_t thread_wplata;
    pthread_t thread_wyplata;

    pthread_create(&thread_wplata, NULL, klient,(void *)&wplata);
    pthread_create(&thread_wyplata, NULL, klient,(void *)&wyplata);


    // Czekamy na wplate
    pthread_join(thread_wplata, NULL);

    // Czekamy na wyplate
    pthread_join(thread_wyplata, NULL);

    // Zniszczenie semafora
    sem_destroy(&semAddress);

    return 0;
}