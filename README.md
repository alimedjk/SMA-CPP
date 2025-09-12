# SMA-CPP — Bank Simulation (C++17)  

This project is an object-oriented **bank queue simulation** written in C++17.  
It supports:
- Multi-cashier service simulation with client arrivals, patience, and priority
- Statistical results (served/unserved clients, satisfaction, average waiting/service time, cashier utilization)
- Results insertion into a **PostgreSQL** database (`libpq`)
- Visualization of results with Python/matplotlib (fallback script) or C++ header-only `matplotlibcpp`

## how to use ???  
```bash 
sudo apt update
sudo apt install -y build-essential g++ pkg-config
sudo apt install -y libpq-dev postgresql-client
sudo apt install -y python3-matplotlib python3-numpy python3-psycopg2
```

build  
```bash
g++ -std=c++17 -O2 -I include -I /usr/include/postgresql \
  src/bank/*.cpp src/client/*.cpp \
  src/simulation/Simulation.cpp src/simulation/SimulationEntry.cpp \
  src/simulation/SimulationUtility.cpp src/simulation/StatisticManager.cpp \
  src/bdd.cpp src/simulation/TestSimulation.cpp \
  -lpq -o bin/run_sim

```

RUN the execution file

```bash
./bin/run_sim
```

Generate plots (Python fallback)

```bash
python3 src/plot_fallback.py 5
```
the production will show in  
```bash
bin/satisfaction_pie.png
bin/clients_bars.png
bin/clients_bars_single.png
```


