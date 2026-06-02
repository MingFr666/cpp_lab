# cpp_memory_lab

A small C++17 lab project for learning C++ memory and OS virtual memory.

## Build

```bash
mkdir -p build
cd build
cmake ..
cmake --build . -j
```

## Programs

```bash
./memory_layout
./stack_heap_raii
./vector_growth
./mmap_file
./page_faults
```

Shared memory demo, use two terminals:

```bash
# terminal 1
./shared_memory_writer

# terminal 2
./shared_memory_reader
```

On Linux, inspect memory mapping:

```bash
cat /proc/<pid>/maps
pmap -x <pid>
```

Remove shared memory object:

```bash
rm /dev/shm/cpp_memory_lab_shm
```
