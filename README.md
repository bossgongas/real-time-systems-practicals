# ⏱️ Real-Time Systems – Practical Assignments

This repository contains the two main practical assignments developed for the course **Sistemas de Tempo Real (Real-Time Systems)**, part of the **Bachelor in Electrical and Computer Engineering at the University of Coimbra**, academic year **2023/2024**.

> 👨‍🎓 **Authors**: Gonçalo Bastos eusoudebastos@gamil.com, Leonardo Gonçalves leoleocordeiro@gmail.com
> 🏫 **University**: Universidade de Coimbra – DEEC  
> 💼 **Course**: Real-Time Systems (STR)

---

## 📘 Contents

### 📄 Practical Assignment 1 – Task Scheduling and Timing

In this assignment, we:
- Measured computation times of three periodic tasks (f1, f2, f3)
- Analyzed schedulability using Rate Monotonic Scheduling (RMPO) and its inverse
- Implemented CPU affinity, memory locking, and thread configuration (POSIX)
- Evaluated deadline misses, jitter, and response times under various scheduling policies
- Switched priorities dynamically at runtime
- Compared FIFO and Round-Robin scheduling

📎 [TP1 Report](./t1g5r1.pdf)

---

### 📄 Practical Assignment 2 – Point Cloud Processing with Thread Synchronization

In this second assignment, we:
- Processed LiDAR point cloud data from multiple files
- Computed statistics and filtered drivable areas
- Visualized the processed data using MATLAB
- Split the processing pipeline across three synchronized threads
- Used semaphores and `clock_nanosleep` to ensure timing constraints
- Verified end-to-end computation stayed under 95 ms

📎 [TP2 Report](./STR_TP_02.pdf)

---

## 🛠️ Technologies Used

- **C / C++**
- **POSIX Threads (pthreads)**
- **Semaphores & Scheduling policies (SCHED_FIFO, SCHED_RR)**
- **MATLAB** (for visualization)
- **Linux-based systems**


---

## 🔍 Notes

These assignments simulate real-time embedded systems using Linux and provide insight into the challenges of thread synchronization, CPU scheduling, and soft real-time constraints.



