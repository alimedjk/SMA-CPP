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

仅保存图片（推荐在无图形环境或不想弹窗时）：
./bin/ihm_demo --ids=1,2,3 --id=2
输出：

out/compare.png（多仿真对比条形图）

out/pie_2.png（id=2 的饼图）

弹窗显示（Windows 桌面要先启动 VcXsrv，WSL 设置好 DISPLAY；ihm.cpp 内已根据 --show 切到 TkAgg）：
./bin/ihm_demo --ids=1,2,3 --id=2 --show

指定输出目录：
./bin/ihm_demo --ids=1,4,3 --id=4 --outdir=figs

参数说明：

--ids=1,2,3 多个仿真 ID，用于生成条形图

--id=2 单个仿真 ID，用于生成饼图

--show 打开窗口显示（TkAgg），否则只保存（Agg）

--outdir=out 图片保存目录（默认 out）


