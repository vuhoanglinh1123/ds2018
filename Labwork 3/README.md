# Bash Commands
## Install OpenMPI
```
sudo apt install libopenmpi-dev 
```
## Compile the MPI file
```
mpicc <c/c++_file> -o <file_name>
```
Example:
```
mpicc hello.c -o hello
```
## Run the MPI file
```
mpirun -np <number_of_processes> <file_name>
```
Example:
```
mpirun -np 4 hello
```
