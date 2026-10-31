#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
float distance = 30.0;
float ax = 0, ay = 0, az = 9.8;
float gx = 0, gy = 0, gz = 0;
float speed = 8.0;
int running = 1;
int ultra_valid = 0;
int imu_healthy = 0;

time_t imu_heartbeat;
time_t ultra_heartbeat;
time_t health_heartbeat;
time_t fusion_heartbeat;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/* 1. ULTRASONIC SENSOR THREAD */

void *ultrasonic_thread(void *arg)
{
    float scenario[] = {30, 20, 12, 8, 5, 3, 1};
    int i;

    printf("\n Sensor started\n");
    printf("Checking for obstacles...\n");

    for (i = 0; i < 7; i++)
    {
        pthread_mutex_lock(&lock);

        if (!running)
        {
            pthread_mutex_unlock(&lock);
            break;
        }

        distance = scenario[i];
        ultra_valid = 1;
        ultra_heartbeat = time(NULL);

        printf("\n Obstacle detected!\n");
        printf(" Distance: %.1f m\n", distance);
        pthread_mutex_unlock(&lock);

        sleep(2);
    }

    printf("Sensor thread stopped\n");

    return NULL;
}

/* 2. IMU SENSOR THREAD */

void *imu_thread(void *arg)
{
    int count = 0;

    printf("\n Sensor started\n");
    printf(" Reading accelerometer and gyroscope data...\n");

    while (1)
    {
        pthread_mutex_lock(&lock);

        if (!running)
        {
            pthread_mutex_unlock(&lock);
            break;
        }

        ax = ((count % 5) - 2) * 0.1;
        ay = ((count % 3) - 1) * 0.1;
        az = 9.8;

        gx = (count % 4) * 0.5;
        gy = (count % 3) * 0.3;
        gz = (count % 5) * 0.2;

        /* Simple speed calculation */

        speed += ax * 0.1;

        if (speed < 0)
            speed = 0;

        imu_healthy = 1;
        imu_heartbeat = time(NULL);

        if (count % 10 == 0)
        {
            printf("\n Sensor readings updated\n");
            printf(" Acceleration: %.2f, %.2f, %.2f\n",
                   ax, ay, az);
            printf(" Gyroscope: %.2f, %.2f, %.2f\n",
                   gx, gy, gz);
        }

        pthread_mutex_unlock(&lock);

        count++;

        usleep(100000);
    }

    printf("Sensor thread stopped\n");

    return NULL;
}

/* 3. SENSOR HEALTH MONITOR THREAD */

void *health_thread(void *arg)
{
    int ultra_status;
    int imu_status;

    printf("\n Monitoring started\n");

    while (1)
    {
        pthread_mutex_lock(&lock);

        if (!running)
        {
            pthread_mutex_unlock(&lock);
            break;
        }

        ultra_status = ultra_valid && distance > 0;
        imu_status = imu_healthy && az > 8.0 && az < 11.5;

        printf("\n Checking sensor status...\n");

        if (ultra_status)
            printf(" Ultrasonic sensor: HEALTHY\n");
        else
            printf(" Ultrasonic sensor: FAULT\n");

        if (imu_status)
            printf(" IMU sensor: HEALTHY\n");
        else
            printf(" IMU sensor: FAULT\n");

        health_heartbeat = time(NULL);

        pthread_mutex_unlock(&lock);

        sleep(1);
    }

    printf("[HEALTH] Monitoring stopped\n");

    return NULL;
}


/* 4. SENSOR FUSION THREAD */

void *fusion_thread(void *arg)
{
    float d, v, ttc;
    int valid;
    printf("\n Sensor fusion started\n");
    printf(" Processing sensor data...\n");
    while (1)
    {
        pthread_mutex_lock(&lock);
        if (!running)
        {
            pthread_mutex_unlock(&lock);
            break;
        }

        d = distance;
        v = speed;
        valid = ultra_valid && imu_healthy;
        fusion_heartbeat = time(NULL);
        pthread_mutex_unlock(&lock);

        printf("\n========== ADAS SENSOR FUSION ==========\n");

        if (!valid)
        {
            printf("Waiting for sensor readings...\n");
        }
        else
        {
            ttc = d / (v > 0 ? v : 0.1);

            printf("Obstacle distance : %.2f m\n", d);
            printf("Vehicle speed     : %.2f m/s\n", v);
            printf("Time to collision : %.2f sec\n", ttc);

            if (ttc <= 0.5)
                printf("DANGER! Collision is very close!\n");

            else if (ttc <= 1.0)
                printf("High collision risk! Be careful!\n");

            else if (ttc <= 2.5)
                printf("Obstacle nearby. Slow down!\n");

            else if (ttc <= 4.0)
                printf("Caution! Obstacle detected ahead.\n");

            else
                printf("Road is clear. Driving is safe.\n");
        }

        printf("========================================\n");

        usleep(500000);
    }

    printf(" Sensor fusion stopped\n");

    return NULL;
}


/* 5. WATCHDOG THREAD */

void *watchdog_thread(void *arg)
{
    time_t now;

    printf("\n Monitoring started\n");

    while (1)
    {
        sleep(1);

        pthread_mutex_lock(&lock);

        if (!running)
        {
            pthread_mutex_unlock(&lock);
            break;
        }

        now = time(NULL);

        printf("\n Checking running threads...\n");

        if (now - imu_heartbeat > 2)
            printf("IMU thread: TIMEOUT\n");
        else
            printf("IMU thread: OK\n");

        if (now - ultra_heartbeat > 3)
            printf("Ultrasonic thread: TIMEOUT\n");
        else
            printf("Ultrasonic thread: OK\n");

        if (now - health_heartbeat > 2)
            printf("Health thread: TIMEOUT\n");
        else
            printf("Health thread: OK\n");

        if (now - fusion_heartbeat > 2)
            printf("Fusion thread: TIMEOUT\n");
        else
            printf("Fusion thread: OK\n");

        pthread_mutex_unlock(&lock);
    }

    printf("[WATCHDOG] Monitoring stopped\n");

    return NULL;
}

/* MAIN PROGRAM */

int main()
{
    pthread_t ultra, imu, health, fusion, watchdog;

    printf("\n");
    printf("============================================\n");
    printf("       QNX ADAS SENSOR FUSION SYSTEM        \n");
    printf("============================================\n");

    printf("Starting ADAS system...\n");
    printf("Initializing sensors...\n");
    printf("Creating sensor threads...\n");

    imu_heartbeat = time(NULL);
    ultra_heartbeat = time(NULL);
    health_heartbeat = time(NULL);
    fusion_heartbeat = time(NULL);

    pthread_create(&ultra, NULL, ultrasonic_thread, NULL);
    pthread_create(&imu, NULL, imu_thread, NULL);
    pthread_create(&health, NULL, health_thread, NULL);
    pthread_create(&fusion, NULL, fusion_thread, NULL);
    pthread_create(&watchdog, NULL, watchdog_thread, NULL);

    printf("\nAll threads started successfully!\n");

    /* Wait for ultrasonic scenario to finish */

    pthread_join(ultra, NULL);
    printf("\n[MAIN] Obstacle simulation completed.\n");
    printf("[MAIN] Stopping all threads...\n");
    pthread_mutex_lock(&lock);
    running = 0;
    pthread_mutex_unlock(&lock);

    pthread_join(imu, NULL);
    pthread_join(health, NULL);
    pthread_join(fusion, NULL);
    pthread_join(watchdog, NULL);

    printf("\n[MAIN] All threads stopped successfully.\n");

    pthread_mutex_destroy(&lock);

    printf("\n============================================\n");
    printf("       ADAS SIMULATION COMPLETED            \n");
    printf("============================================\n");

    return 0;
}