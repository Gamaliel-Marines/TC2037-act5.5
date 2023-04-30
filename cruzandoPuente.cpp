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
const int SLEEP_TIME = 1;

enum states {
    CROSSING,
    WAITING,
    FINISHED,
    END
};

enum directions {
    NORTH_TO_SOUTH,
    SOUTH_TO_NORTH,
    NONE
};

typedef struct {
    int id;
    directions direction;
    states state;
} Car;

// Bridge
int number_cars = 0;
directions bridge_direction = NONE;

pthread_cond_t can_cross_n_to_s;
pthread_cond_t can_cross_s_to_n;
pthread_mutex_t mutex_lock_bridge;

void waiting(Car *car) {
    // No está cruzando
    if (car->state != WAITING) return;

    if (car->direction == NORTH_TO_SOUTH) {
        cout << "Carro (" << car->id << "), está esperando a cruzar de norte a sur" << endl;
    } else if (car->direction == SOUTH_TO_NORTH) {
        cout << "Carro (" << car->id << "), está esperando a cruzar de sur a norte" << endl;
    }
    sleep(SLEEP_TIME);
}

void crossing(Car *car) {
    // No está cruzando
    if (car->state != CROSSING) return;

    // Cruzando
    if (car->direction == NORTH_TO_SOUTH) {
        cout << "Carro (" << car->id << "), está cruzando de norte a sur" << endl;
    } else if (car->direction == SOUTH_TO_NORTH) {
        cout << "Carro (" << car->id << "), está cruzando de sur a norte" << endl;
    }
    sleep(SLEEP_TIME);
}

void finishing(Car *car) {
    // No está cruzando
    if (car->state != FINISHED) return;

    // Cruzando
    if (car->direction == NORTH_TO_SOUTH) {
        cout << "Carro (" << car->id << "), terminó de cruzar de norte a sur" << endl;
    } else if (car->direction == SOUTH_TO_NORTH) {
        cout << "Carro (" << car->id << "), terminó de cruzar de sur a norte" << endl;
    }
    sleep(SLEEP_TIME);
}

void release(Car *car) {
    if (car->direction == NORTH_TO_SOUTH) {
        pthread_cond_signal(&can_cross_n_to_s);
    } else if (car->direction == SOUTH_TO_NORTH){
        pthread_cond_signal(&can_cross_s_to_n);
    };
};

void wait_to_cross(Car *car) {
    if (car->direction == NORTH_TO_SOUTH) {
        pthread_cond_wait(&can_cross_n_to_s, &mutex_lock_bridge);
    } else if (car->direction == SOUTH_TO_NORTH){
        pthread_cond_wait(&can_cross_s_to_n, &mutex_lock_bridge);
    };
};

void arrive_bridge(Car *car) {
    waiting(car); // Log

    pthread_mutex_lock(&mutex_lock_bridge);

    // No hay ningún carro en el puente
    if (bridge_direction == NONE) {
        // Ponerlo en esta dirección
        bridge_direction = car->direction;
    };


    if (
        bridge_direction != car->direction || // Sentido opuesto
        number_cars == 3  // Lleno
    ) {
        wait_to_cross(car);
    };


    number_cars += 1;
    car->state = CROSSING;

    pthread_mutex_unlock(&mutex_lock_bridge);
};

void cross_bridge(Car *car) {
    crossing(car);
    car->state = FINISHED;
};

void exit_bridge(Car *car) {
    finishing(car);

    pthread_mutex_lock(&mutex_lock_bridge);

    car->state = END;
    number_cars -= 1;

    if (number_cars == 0) {
        bridge_direction = NONE;
        pthread_cond_signal(&can_cross_n_to_s);
        pthread_cond_signal(&can_cross_s_to_n);
    } else {
        release(car);
    };

    pthread_mutex_unlock(&mutex_lock_bridge);
};

void* cross(void* param) {
    Car* car;
    car = (Car*) param;

    while (true) {
        // Aún no está cruzando
        if(car->state == WAITING) {
            arrive_bridge(car);
        };

        // Cruzando
        if(car->state == CROSSING) {
            cross_bridge(car);
        };

        // Saliendo
        if(car->state == FINISHED) {
            exit_bridge(car);
        };
    };

    pthread_exit(NULL);
};


int main(int argc, char* argv[]) {
    pthread_t cars_threads[CARS];
    Car cars[CARS];

    pthread_mutex_init(&mutex_lock_bridge, NULL);
    pthread_cond_init(&can_cross_n_to_s, NULL);
    pthread_cond_init(&can_cross_s_to_n, NULL);

    for(int i = 0; i < CARS; i++){ 
        cars[i].id = i + 1;
        cars[i].state = WAITING;

        if (i % 2 == 0) {
            cars[i].direction = NORTH_TO_SOUTH;
        } else {
            cars[i].direction = SOUTH_TO_NORTH;
        };

        pthread_create(&cars_threads[i], NULL, cross, &cars[i]);
    }

    for(int i = 0; i < CARS; i++){
        pthread_join(cars_threads[i], NULL);
    }

    return 0;
}