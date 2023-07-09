#include <pthread.h>
#include <stdio.h>

pthread_mutex_t m;
// CV stands for condition variable.
pthread_cond_t cv_host;
pthread_cond_t cv_guest;
int guest_count = 0;
int N = 5;

void openDoor()
{
    printf(
        "\n-----------------------------------------------------------\nTHE DOOR "
        "IS NOW OPEN. GUESTS CAN "
        "ENTER.\n-----------------------------------------------------------\n");
}

void enterHouse()
{
    printf(
        "\n-----------------------------------------------------------\nGUEST "
        "%lu HAS ENTERED THE "
        "HOUSE.\n-----------------------------------------------------------\n",
        pthread_self());
}

void *hostThread(void *args)
{
    printf("Host thread started.\n");

    pthread_mutex_lock(&m);
    printf("Host thread locked the mutex.\n");

    if (guest_count < N)
    {
        pthread_cond_wait(&cv_host, &m);
    }

    printf("Host thread received signal from all guests.\n");

    openDoor();
    printf("Host thread called openDoor()\n");

    while (guest_count > 0)
    {
        pthread_cond_signal(&cv_guest);
        guest_count--;
    }

    printf("Host thread signaled guests.\n");

    pthread_mutex_unlock(&m);
    printf("Host thread unlocked the mutex.\n");

    return NULL;
}

void *guestThread(void *args)
{
    printf("Guest thread started.\n");

    pthread_mutex_lock(&m);
    printf("Guest thread locked the mutex.\n");

    guest_count++;

    if (guest_count == N)
    {
        pthread_cond_signal(&cv_host);
        printf("Signaled host that all guests have arrived.\n");
    }

    pthread_cond_wait(&cv_guest, &m);
    printf("Guest thread received signal from host.\n");

    enterHouse();
    printf("Guest thread called enterHouse().\n");

    pthread_mutex_unlock(&m);
    printf("Guest thread unlocked the mutex.\n");

    return NULL;
}

int main()
{
    pthread_t host;
    pthread_t guests[N];

    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&cv_host, NULL);
    pthread_cond_init(&cv_guest, NULL);

    pthread_create(&host, NULL, hostThread, NULL);

    for (int i = 0; i < N; i++)
    {
        pthread_create(&guests[i], NULL, guestThread, NULL);
    }

    pthread_join(host, NULL);

    for (int i = 0; i < N; i++)
    {
        pthread_join(guests[i], NULL);
    }

    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&cv_host);
    pthread_cond_destroy(&cv_guest);

    return 0;
}