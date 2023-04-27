#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <pthread.h>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

const int CARS = 8;
const int MAX_THREADS = 8;
const int SLEEP_TIME = 1;

enum{
    CROSSING,
    WAITING
} state[CARS];

int ids[CARS];

pthread_t tids[MAX_THREADS];
pthread_cond_t areCarsCrossing;
pthread_mutex_t mutex_lock_bridge;


void waiting(int id) {
    sleep(SLEEP_TIME);
}

void crossing(int id) {
    std::cout << "card: " << id << " is crossing." << std::endl;
    sleep(SLEEP_TIME);
}

void test(int i) {
}


int main(int argc, char* argv[]) {

    return 0;
}