# Finite Element Method Solver

### INTRODUCTION
This project is implementation of my master degree thesis task. The program is to solve flat truss using FEM (Finite Element Method).

Program was developed on Ubuntu Linux 18.04 LTS (WSL 2) but is easy to port into Windows.

### TOOLS
Following tools were used:
  - gcc (g++)
  - GNU Make (build-essential)
  - CMake
  - git
  - Visual Studio Code

### LIBRARIES

Following libraries need to be installed for correct compilation:
- LAPAK
```sh
$ sudo apt install liblapack-dev
```
- OpenBLAS
```sh
$ sudo apt install -y libopenblas-dev libopenblas-base
```
- ARPACK
```sh
$ sudo apt install -y libarpack++2-dev libarpack++2c2a libarpack2 libarpack2-dev
```
- SuperLU
```sh
$ sudo apt install -y libsuperlu libsuperlu-dev libsuperlu-dist-dev libsuperlu-dist5 libsuperlu5
```
- OpenMP
```sh
$ sudo apt-get install libomp-dev
```
- Armadillo
```sh
$ wget http://sourceforge.net/projects/arma/files/armadillo-10.1.2.tar.xz
$ tar -xvf armadillo-10.1.2.tar.xz
$ cd armadillo-10.1.2/
$ cmake .
$ sudo make install
```
- simple-svg - single header library
```https://github.com/adishavit/simple-svg```

### COMPILATION
Lazy way to run the project:
```sh
$ ./rebuild
```
Just build the solver app:
```sh
$ make build
```
or simply
```sh
$ make
```

Remove created files:
```sh
$ make clean
```

### USAGE
Display usage instructions:
```sh
$ ./solver
```

Solve example input truss using dense sfiffness matrix:
```sh
$ ./solver input/97.truss -dense
```

Solve example input truss using sparse sfiffness matrix:
```sh
$ ./solver input/97.truss -sparse
```

Generate new truss (about 5000 elements, 5m long):
```sh
$ cd input
$ ./generator 5000 5
```