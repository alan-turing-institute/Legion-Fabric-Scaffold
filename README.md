# MPI-Scaffold
Scaffold for a CMake Fabric project for Legion

## Running with ssh

```
ssh <intel cluster>
mkdir -p ~/devel/mpi-scaffold
mkdir -p ~/Scratch/Scaffold/output
cd ~/devel/mpi-scaffold
git clone https://github.com/UCL-RITS/Legion-Fabric-Scaffold.git
cd Turing-Fabric-Scaffold
edit subme.sh with your userid instead of mine
./buildme.sh
qsub subme.sh
```

## Running via fabric

```
pip install fabric # You need a working python install
git clone https://github.com/UCL-RITS/Legion-Fabric-Scaffold.git
cd Legion-Fabric-Scaffold
edit intel.py with your userid instead of mine
fab intel.cold
fab intel.sub
fab intel.wait
fab intel.fetch
```
