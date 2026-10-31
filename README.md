# QNX-Based ADAS Sensor Fusion System

## Overview

This project implements a real-time Advanced Driver Assistance System (ADAS) Sensor Fusion Simulation using QNX Neutrino RTOS and POSIX Multithreading in C.

The system simulates multiple sensor operations, monitors sensor health, processes sensor information, calculates Time to Collision (TTC), and generates collision warnings.

The application uses concurrent threads to demonstrate real-time task execution, synchronization, sensor monitoring, and watchdog functionality.

## Technologies Used

- **Operating System:** QNX Neutrino RTOS 8.0
- **Hardware Platform:** Raspberry Pi 4 Model B
- **Programming Language:** C
- **Development Environment:** QNX Momentics IDE 8.0.2
- **Multithreading:** POSIX Pthreads
- **Synchronization:** POSIX Mutex
- **IPC / Synchronization Concepts:** Shared memory within a process and mutex-based protection

## System Architecture

The application consists of five concurrent threads:

| Thread | Responsibility |
|---|---|
| Ultrasonic Sensor | Simulates obstacle distance measurements |
| IMU Sensor | Simulates accelerometer and gyroscope readings |
| Sensor Health Monitor | Checks the health of both sensors |
| Sensor Fusion | Processes sensor data and calculates TTC |
| Watchdog | Monitors thread heartbeats and detects timeouts |

The main thread initializes the system, creates the five threads, waits for the ultrasonic simulation to complete, and coordinates the shutdown of all threads.

## Key Features

- Concurrent execution using POSIX threads.
- Shared sensor data protected using a mutex.
- Simulated ultrasonic obstacle detection.
- Simulated IMU accelerometer and gyroscope readings.
- Vehicle speed estimation through a simplified calculation.
- Sensor health monitoring.
- Time-to-Collision (TTC) calculation.
- Collision risk classification.
- Watchdog-based thread monitoring.
- Controlled thread termination and cleanup.

## Collision Detection Logic

Time to Collision is calculated using:

**TTC = Distance / Vehicle Speed**

The system classifies collision risk based on the calculated TTC:

| TTC | System Response |
|---|---|
| 0.5 seconds or less | DANGER |
| 0.5–1.0 seconds | High collision risk |
| 1.0–2.5 seconds | Slow down |
| 2.5–4.0 seconds | Caution |
| Above 4.0 seconds | Driving is safe according to the simulation threshold |

## Thread Synchronization

A POSIX mutex is used to protect shared variables such as:

- Obstacle distance
- Accelerometer readings
- Gyroscope readings
- Vehicle speed
- Sensor health status
- Thread heartbeat timestamps

This prevents concurrent threads from accessing shared data without synchronization.

## Watchdog Monitoring

The watchdog thread periodically checks the heartbeat timestamps of:

- IMU thread
- Ultrasonic thread
- Health monitoring thread
- Sensor fusion thread

If a thread does not update its heartbeat within the expected time, the watchdog reports a timeout.

## Project Execution

### Prerequisites

- QNX Neutrino RTOS 8.0
- QNX Momentics IDE
- Raspberry Pi 4 Model B (for QNX target execution)
- POSIX Threads support

### Build and Run

Create a C project in QNX Momentics IDE and add `main.c` to the source folder.

Build the project using the QNX compiler.

Alternatively, on a POSIX-compatible Linux system with GCC:

```bash
gcc main.c -o adas -pthread
./adas
```

## Expected Output

The application displays:

- Sensor initialization messages.
- Ultrasonic obstacle distance readings.
- IMU accelerometer and gyroscope readings.
- Sensor health status.
- Vehicle speed and obstacle distance.
- Calculated Time to Collision.
- Collision warnings.
- Watchdog thread status.
- System shutdown messages.

## Project Limitations

- Sensor readings are simulated using predefined values.
- Physical ultrasonic and IMU sensor interfacing is not implemented in this version.
- Speed estimation is simplified and is not intended for actual vehicle control.
- TTC calculation assumes the reported vehicle speed represents the closing speed toward the obstacle.
- The project is a simulation and is not validated for safety-critical automotive applications.

## Future Enhancements

- Integrate a physical ultrasonic sensor.
- Interface an MPU6050 IMU sensor.
- Implement inter-process communication using QNX message passing.
- Introduce separate sensor and processing processes.
- Improve sensor fusion using filtering algorithms.
- Implement more robust fault detection and recovery.
- Add real-time scheduling and priority-based execution.

## Learning Outcomes

This project demonstrates practical implementation of:

- POSIX multithreading in C.
- Thread creation and termination.
- Mutex-based synchronization.
- Shared data management.
- Real-time sensor monitoring.
- Watchdog mechanisms.
- Basic ADAS sensor fusion concepts.
- Embedded software development using QNX Neutrino RTOS.

---

**Project:** QNX-Based ADAS Sensor Fusion System  
**Platform:** Raspberry Pi 4 Model B  
**RTOS:** QNX Neutrino 8.0  
**Language:** C