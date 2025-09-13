# SMA-CPP — Bank Simulation (C++17)  

This project is an object-oriented **bank queue simulation** written in C++17.  
It supports:
- Multi-cashier service simulation with client arrivals, patience, and priority
- Statistical results (served/unserved clients, satisfaction, average waiting/service time, cashier utilization)
- Results insertion into a **PostgreSQL** database (`libpq`)
- Visualization of results with Python/matplotlib (fallback script) or C++ header-only `matplotlibcpp`

### Generate plots  
Windows 10 + WSL2


## how to use ???  
Windows 10 + WSL2 (Ubuntu 24.04).

Windows side (only needed for GUI “--show”):

VcXsrv (X server) installed and running.

WSL side (Ubuntu):

build-essential, g++, pkg-config

libpq-dev, postgresql-client

libcairo2-dev

python3, python3-dev, python3-matplotlib, python3-tk

(optional, for quick X test) x11-apps

Commands (run inside WSL):
```bash
sudo apt update
sudo apt install -y build-essential g++ pkg-config
sudo apt install -y libpq-dev postgresql-client
sudo apt install -y libcairo2-dev
sudo apt install -y python3 python3-dev python3-matplotlib python3-tk
sudo apt install -y x11-apps
```


Note: Win11 + WSLg users don’t need VcXsrv. For Win10 (your setup), VcXsrv is required to show windows.



## Windows X server (VcXsrv / XLaunch) — download and usage

Get VcXsrv from its official page or a trusted mirror (search “VcXsrv Windows X Server”).

Install with defaults.

Start via XLaunch (GUI):

Open “XLaunch” from the Start menu.

Step 1: choose “Multiple windows”.

Step 2: choose “Start no client”.

Step 3: check “Disable access control” (or you may add “-ac” if using the command line).

Step 4: finish. When Windows Firewall prompts, allow VcXsrv on your networks.

Keep VcXsrv running in the background (you’ll see an “X” icon in the system tray). As long as you need “--show”, VcXsrv must stay open.

Start via command line (optional alternative):

Create a desktop shortcut to:
"C:\Program Files\VcXsrv\vcxsrv.exe" :0 -multiwindow -ac -clipboard -wgl
or run it from “Run…” (Win+R) with the same arguments.

Keep the window minimized; just make sure it stays running.

Verify the server is running:

You should see the VcXsrv icon in the system tray.

If you close it, “--show” will not be able to open windows from WSL.


Add the DISPLAY and OpenGL variables to your shell profile so GUI apps know where to render. In WSL:

echo 'export DISPLAY=$(ip route | awk "/^default/ {print $3}"):0.0' >> ~/.bashrc
echo 'export LIBGL_ALWAYS_INDIRECT=1' >> ~/.bashrc
source ~/.bashrc

Test that X forwarding works (with VcXsrv already running on Windows):
xclock
You should see a small clock window pop up on Windows. If it fails, recheck VcXsrv and your DISPLAY value.

Notes:

echo $DISPLAY may show something like “172.xx.yy.zz:0.0”. Seeing “:0.0” alone can also work depending on your setup.

If the window does not appear, ensure VcXsrv is running and that Windows Firewall didn’t block it.

## Build  
```bash
g++ -std=c++17 -O2 \
  -I include -I include/third_party -I /usr/include/postgresql \
  src/bank/Bank.cpp src/bank/Cashier.cpp src/bank/Queue.cpp \
  src/client/*.cpp \
  src/simulation/Simulation.cpp src/simulation/SimulationEntry.cpp src/simulation/SimulationUtility.cpp src/simulation/StatisticManager.cpp \
  src/bdd.cpp src/ihm.cpp \
  $(pkg-config --cflags --libs cairo) -lpq \
  $(python3-config --includes) \
  $(python3-config --embed --ldflags) \
  -DWITHOUT_NUMPY -Wno-deprecated-declarations \
  -o bin/ihm_demo

```

RUN the execution file

```bash
./bin/ihm_demo --ids=1,2,3,4 --id=2 --show

./bin/ihm_demo --ids=1,2,3 --id=4
```

Symptom: “Matplotlib is currently using agg… so cannot show the figure.”

You either didn’t pass “--show”, or the backend is still Agg.

Ensure:
a) You ran with --show.
b) The code sets plt::backend("TkAgg") before any plotting calls (current code does this).
c) python3-tk is installed.
d) VcXsrv is running, and DISPLAY is set correctly in WSL.

Symptom: No window appears when running with --show.

Confirm VcXsrv is running and not blocked by the firewall.

Recheck DISPLAY (echo $DISPLAY).

Test with xclock (requires x11-apps installed).

Symptom: “undefined reference to Py…” at link time.

Ensure your link line includes:
$(python3-config --embed --ldflags)
and that it comes after the source files.

Symptom: Database connection failure.

Check host/port, credentials, and network egress for port 5432.

Symptom: Images not saved when running without --show.

Check that the output directory is writable. The program uses std::filesystem::create_directories; the console will log paths for saved images.

