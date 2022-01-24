#include <pthread.h>
#include <assert.h>

#define NUMBER 3

void* run() {
    

    return NULL;
}

int main() {
    pthread_t threads[NUMBER];
    int threadsId[NUMBER], size = sizeof(threadsId);
    assert(tfs_init() != -1);
    for (int i = 0; i < size; i++) {
        threadsId[i] = pthread_create(&threads[i], NULL, &run, NULL);
    }
    for (int i = 0; i < size; i++) {
        pthread_join(threads[i], NULL);
    }
    tfs_destroy();
    return 0;
}