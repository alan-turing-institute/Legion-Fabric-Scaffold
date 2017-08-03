# MPI-Scaffold
Scaffold for a CMake Fabric project for Turing compute resoruces

## Running via fabric

```
pip install fabric # You need a working python install
git clone git@github.com:alan-turing-institute/Turing-Fabric-Scaffold.git
cd Turing-Fabric-Scaffold
edit deploy/cirrus.py with your userid instead of mine
fab cirrus.cold
fab cirrus.sub:processes=72
fab intel.wait
fab intel.fetch
```
