# Lab 8: Memory Management Simulation

A C simulator for contiguous memory allocation using:

- First Fit (FIFO free list)
- Best Fit (ascending block-size list)
- Worst Fit (descending block-size list)
- Process deallocation
- Coalescing of physically adjacent free blocks

## Build

```bash
make mmu
```

## Run

```bash
./mmu input0.txt -F
./mmu input0.txt -B
./mmu input0.txt -W
```

The program prints the free and allocated memory lists after every operation.
