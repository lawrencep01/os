#include "life.h"
#include <pthread.h>
#include <cstdio>
#include <vector>
#include <iostream>

pthread_barrier_t barrier;
pthread_barrier_t barrier2;

LifeBoard next_state;

struct args {
    int start;
    int end;
    int steps;
    int id;
    LifeBoard *state;
    LifeBoard *next_state;
};

void *compute_simulation(void *arguments){

    struct args *data;
    data = (struct args*) arguments;

    for (int step = 0; step < data->steps; ++step) {
        for (int y = data->start; y < data->end; ++y) {
            for (int x = 1; x < (data->state->width()) - 1; ++x) {
                int live_in_window = 0;
                for (int y_offset = -1; y_offset <= 1; ++y_offset) {
                    for (int x_offset = -1; x_offset <= 1; ++x_offset) {
                        if (data->state->at(x + x_offset, y + y_offset)) {
                            ++live_in_window;
                        }
                    }
                }
                data->next_state->at(x, y) = (
                    live_in_window == 3 ||
                    (live_in_window == 4 && data->state->at(x, y))
                );
            } 
        }

        pthread_barrier_wait(&barrier);
        if (data->id == 0){
            swap(*(data->state), *(data->next_state));
            LifeBoard next_next_state{data->state->width(), data->state->height()};
            swap(*(data->next_state), next_next_state);
        }
        pthread_barrier_wait(&barrier);
        
    }
    pthread_barrier_wait(&barrier2);
    pthread_exit(NULL);
    return nullptr;

}

void simulate_life_parallel(int threads, LifeBoard &state, int steps) {

    // Partioning code (by rows)
    std::vector<int> rows;
    int step = (state.height() - 2 + (threads - 1)) / threads;
    int row = 1;
    rows.push_back(row);
    int threads_used = 0;
    while (row < state.height() - 1){
        int end = (row + step < state.height() - 1) ? row + step : state.height() - 1;
        rows.push_back(end);
        row += step;
        threads_used += 1;
    }

    // Buffer Initiation
    pthread_barrier_init(&barrier, NULL, threads_used);
    pthread_barrier_init(&barrier2, NULL, threads_used + 1);

    // Thread Creation
    struct args *arguments = new args[threads_used];
    pthread_t *tds = new pthread_t[threads_used];
    next_state = {state.width(), state.height()};
    for (int i = 0; i < threads_used; i++){
        arguments[i].start = rows[i];
        arguments[i].end = rows[i + 1];
        arguments[i].steps = steps;
        arguments[i].id = i;
        arguments[i].state = &state;
        arguments[i].next_state = &next_state;
        pthread_create(&tds[i], NULL, compute_simulation, (void *) &arguments[i]);
    }
    pthread_barrier_wait(&barrier2);

    for (int i = 0; i < threads_used; i++){
        pthread_join(tds[i], NULL);
    }
    delete[] arguments;
    delete[] tds;
}