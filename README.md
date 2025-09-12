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
NUMPY_INC=$(python3 -c "import numpy; print(numpy.get_include())")
```

build  
```bash
g++ -std=c++17 -O2 \
  -I include \
  -I include/third_party \
  -I /usr/include/postgresql \
  -I "$NUMPY_INC" \
  $(python3-config --includes) \
  src/bank/*.cpp \
  src/client/*.cpp \
  src/simulation/Simulation.cpp \
  src/simulation/SimulationEntry.cpp \
  src/simulation/SimulationUtility.cpp \
  src/simulation/StatisticManager.cpp \
  src/bdd.cpp \
  src/ihm.cpp \
  -lpq $(python3-config --embed --ldflags 2>/dev/null || python3-config --ldflags) \
  -o bin/ihm_demo

```

RUN the execution file

```bash
./bin/ihm_demo
```

### Generate plots  
Windows 10 + WSL2

```bash
export DISPLAY=$(grep -m1 nameserver /etc/resolv.conf | awk '{print $2}'):0
IHM_SHOW=1 ./bin/ihm_demo

```


