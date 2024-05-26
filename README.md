# Top-K-Shortest-Paths
Top K Shortest Path Problem with MPI and OpenMP

# Top K Shortest Paths - Parallel Version
This project is a parallel implementation of the algorithm to find the top K shortest paths in a graph. It uses the OpenMP library for parallel computation and the MPI library for inter-process communication. The code is written in C++.

# Top K Shortest Paths - Serial Version
The serial version of the program is also included to compare the execution times.

## Features

- The code reads a graph from a text or CSV file where each line represents an edge. The first two columns are the source and target nodes, and the third column is the weight of the edge.
- The code maps the source and target nodes to integers for efficient computation.
- The code generates 10 random pairs of nodes and finds the top K shortest paths between each pair.
- The computation is divided equally among the available processes. If there are more pairs than processes, the remaining pairs are randomly assigned to the processes.
- The results are gathered at the root process and printed.
- The main function can handle both text and CSV datasets, making it versatile for different types of input data.

## Datasets

The code supports two types of datasets:

- Text file datasets: The graph is represented as a list of edges. Each line of the file contains two integers representing an edge and a third integer representing the weight of the edge.
- CSV file datasets: The graph is represented as a CSV file where each line represents an edge. The first two columns are the source and target nodes, and the third column is the weight of the edge.

## Usage

To compile the code, use the following command:

```bash
mpic++ -fopenmp TopKParallel.cpp -o TopKParallel
```

To run the code, use the following command:

```bash
mpirun -np <number_of_processes> ./TopKParallel
```

Replace `<number_of_processes>` with the number of processes you want to use.

## Requirements

- MPI library
- OpenMP library
- C++ compiler
