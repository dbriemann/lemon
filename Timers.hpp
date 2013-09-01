#ifndef TIMER_HPP
#define TIMER_HPP

#include <sys/time.h>
#include <ctime>

struct WallTimer {
    timeval startTime;

    void start(){
        gettimeofday(&startTime, NULL);
    }

    bool out_of_time(const double &max_duration_in_sec) {
        if(get_elapsed() >= max_duration_in_sec) {
            return true;
        }
        return false;
    }

    double get_elapsed(){
        timeval endTime;
        long seconds, useconds;
        double duration;

        gettimeofday(&endTime, NULL);

        seconds  = endTime.tv_sec  - startTime.tv_sec;
        useconds = endTime.tv_usec - startTime.tv_usec;

        duration = seconds + useconds/1000000.0;

        return duration;
    }

    static void print(double duration) {
        cerr << duration << " wsec." << endl;
    }
};

struct CPUTimer {
    clock_t start_time;
    double cpu_time;

    void start() {
        start_time = clock();
    }

    double get_elapsed() {
        cpu_time = (double)(clock() - start_time) / (double)(CLOCKS_PER_SEC);
        return cpu_time;
    }

    void save() {
        cpu_time = (double)(clock() - start_time) / (double)(CLOCKS_PER_SEC);
    }

    void print() {
        cerr << cpu_time << " sec." << endl;
    }

    bool out_of_time(const double &max_duration_in_sec) {
        if(((double)(clock() - start_time) / (double)(CLOCKS_PER_SEC)) >= max_duration_in_sec) {
            return true;
        }
        return false;
    }
};

#endif // TIMER_HPP
