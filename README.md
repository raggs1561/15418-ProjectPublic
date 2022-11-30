# Parallelizing A Linear Programming Solver using OpenMP

## Summary

We plan on implementing a parallelized version of the Simplex Linear Programming solver using OpenMP on the GHC Clusters and PSC supercomputers. We want to show if parallelizing this algorithm will speed up computation compared to a sequential version. If any speedup exists, we want to determine how it varies depending on the number of cores/threads and how running it on PSC changes performance over the running it on the GHC clusters. We plan on using graphs to visually convey our findings.

## Background

Linear Programs are a way to represent multiple algorithms, such as maximum flow or scheduling. Linear programs can be used to represent optimization problems and are an important topic in the field of operations research and logistics. As a result, an efficient linear programming solver is highly useful for this field. There exist multiple open-source solvers (e.g. GNU Linear Programming Kit, HiGHS) and closed-source solvers from companies such as Gurobi.

A linear program is specified with inputs:
- Real-valued variables *x1, x2, ..., xN*
- A linear objective function using these variables as inputs
- *m* linear inequalities (not strict) in these variables

With the goal of maximizing the objective function while satisfying the constraints. We can represent the feasible region of valid variables *x1, x2, ..., xN* that satisfy our constraints as a *n*-dimensional polytope.

Algorithms for solving Linear Programs include the Simplex algorithm (Hill Climbing), Khachiyan's Ellipsoid algorithm, and Karmarkar's Algorithm (a kind of interior point/barrier method). While the latter two algorithms run in polynomial time, the Simplex algorithm is not necessarily slower than the others depending on the problem.

The simplex algorithm starts at a vertex in the polytope and iteratively moves to the best neighboring vertex (the neighboring vertex with the best objective value) until it reaches the maximum. This works since there are no local maxima since the feasible region is convex. 

There are exponentially many possible vertices the simplex algorithm can explore. There are multiple axes of parallelism we can explore to speed up this problem.

We seek to exploit axes of parallelism such as splitting the potential search space among different parallel execution elements. Additionally, linear programs can be converted into their dual form, which may be easier to solve. We can have threads for the primal problem and the dual problem, competing to reach the answer first. We could also have different parallel execution units start at different vertices, as starting position is a significant predictor of overall runtime. 

 ## Challenges Involved

The biggest challenge involves the large amount of ways to potentially parallelize and optimize an implementation of the simplex algorithm. Effective performance debugging will be a large challenge here.  

The simplex algorithm involves different potential axes of parallelism. With multiple different ways to parallelize the simplex algorithm, we will be implementing and testing multiple approaches to determine whether we can speed up this algorithm with OpenMP and if so, how much we can speed it up.

Communicating the representation of a problem uses a significant amount of data, so communicating and splitting the work between workers efficiently is a challenge. Since we are using OpenMP, we want to minimize the amount of data moved between each core's own L2 cache to reduce communication.

Finding which axes of parallelism work best for simplex algorithm is hard. Simplex can have an exponnential runtime since there can be an exponentially large number of vertices explored. Deciding whether to synchronize access to a shared data structure or to split the problems and combine results is another challenge.

Memory accesses can have high locality, since each execution unit will repeatedly use the same inputs to our problem, such as constraints. 

Properties of the system that make mapping the workload to a GHC cluster machine or PSC Blacklight machine challenging include dealing with limited memory, namely the size of a cache. Our working set of information (especially constraints) can be large. For instance, if we our number of constraints is in the thousands, representing our constraints as a matrix means potentially using megabytes of storage. The constraints have large spatial locality but not much temporal locality within a run. 

The reference implementations for the Simplex algorithm involve matrix multiplication. Since constraints usually take the form of a sparse matrix, we can either find a way to represent sparse matrices and perform matrix operations on them or we will need to use an parallelism model that has sufficiently large memory. For example, if we aren't using an efficient sparse matrix representation, then we can't store all our data in a block's memory in a GPU.

Additionally, we may want to make use of SIMD parallelism when finding argmax, or matrix mult, etc. This will be a target to explore once we have completed an OpenMP implementation.

# Resources To Use

We choose to use the C++ programming language, OpenMP, the GHC machines, and the PSC. 

Our codebase starts from sample sequential Python code for the Simplex algorithm we found online.

Sample Python Implementations of Simplex in blog posts we have found:
- https://medium.com/@jacob.d.moore1/coding-the-simplex-algorithm-from-scratch-using-python-and-numpy-93e3813e6e70
- https://github.com/kshitijl/linear-programming-interior-point
- https://radzion.com/blog/operations/simplex

References we used to learn more about this algorithm include:
- A 15-451 lecture that introduced linear programming. Both members of this group are currently taking 15-451. https://www.cs.cmu.edu/~15451-f22/lectures/lec15-lp1.pdf
- A paper by one of the creators of HiGHS, a high performance linear programming solver library titled "Towards a practical parallelisation of the simplex method": 
https://link.springer.com/content/pdf/10.1007/s10287-008-0080-5.pdf on optimizing simplex
- An online seminar by Gurobi: https://www.gurobi.com/events/how-to-exploit-parallelism-in-linear-and-mixed-integer-programming/
\end{itemize}

We have found a set of linear programming test cases at https://netlib.org/lp/data/index.html. We plan on using this if possible to avoid having to write our own test case generator.

We don't believe that we need to use special machines, but would benefit from using a shared-memory model machine similar to the one used in the aforementioned paper. This could let us use the paper's work as a benchmark to compare our own code to.

## Goals and Deliverables

Goals we plan on achieving:
- Sequential Simplex algorithm implementation 
- Parallel OpenMP Simplex algorithm implementation
- Speedup graphs for OpenMP with Simplex

Goals we hope to achieve:
- Revised Simplex method, sequential and parallel, utilizing sparse matrix multiplication methods. 
- Karmarkar's method, sequential and parallel, suitable for different problem types. 
- Simplex method with other parallelism models (e.g. OpenMPI)

## Platform Choice

We choose to use the C++ programming languages, OpenMP, the GHC machines, and the PSC. We are targeting the shared-memory parallelism model. We plan on applying the skills gained from project 3 to our final project. We feel that we are most familiar with this setup.

C++ and OpenMP have well-constructed parallelism infrastructure and excellent documentation. Additionally, we don't have to 
manually split and marshall data between workers, as we can use arrays in shared memory. This is an advantage over CUDA and OpenMPI. 

OpenMP will let us quickly parallelize the Simplex algorithm and explore multiple optimizations to a parallel implementation. 

## Schedule

Week 1 (November 7 to November 11): Finalize Project Idea, Write Project Proposal,  Set up Github Repository,  Implement Serial Version of Simplex Algorithm,  Establish Benchmark for Testing Serial Version,  Get Baseline Visuals

Week 2 (November 14 to November 18): Parallelize Simplex Algorithm

Week 3 (November 21 to November 25): Finish Parallelizing Simplex Algorithm, Implement Serial Karmarkar's Algorithm

Week 4 (November 28 to December 2): Optimize Parallel Simplex Code, Get Simplex Visuals, Implement Parallel Karmarkar's Algorithm

Week 5 (December 5 to December 9): Finish Final Report + Poster

# Milestone 1

## Summary of Progress Made So Far

We used code from 15-451's simplex algorithm implementation to implement our sequential version. This code in the public domain so we decided to use his code as a sequential baseline. Additionally, since it was used for 15-451's homeworks, it's been tested already. We did not know of this sequential implementation when writing our project proposal. We set up a Git repository, Makefile, and a checker to run both implementations and compare runtime. Since many linear programs are not in standard form and our baseline code only solves linear programs in standard form, we wrote a parser that converts arbitrary inputs into standard form. This involved a few operations on the inputs to ensure all variables are $\geq 0$, and that all constraints are of the less than or equal to form. Additionally, our goal function has been changed to a maximum goal. Our parser for input is written in Python and saves the standard form to a text file that is piped to our C++ solver as input.We wrote a basic parser and converter in Python. However, due to the large size of some inputs (with uncompressed versions reaching the hundreds of megabytes), we have found our current setup to be inefficient.

We then began work on basic parallelization of our code using OpenMP to test the feasibility of our approach. We initially found that the overhead of OpenMP overpowered the gains made from the parallel processing. This was due to synchronization issues with certain parts of the algorithm we attempted to parallelize. We were able to overcome this overhead to achieve a speedup slightly faster than our sequential algorithm. 

## Our Schedule:

We are currently on track with our original schedule. In our original schedule, this week (Nov. 28 to Dec. 2nd) was dedicated to optimizing the OpenMP implementation of the Simplex algorithm and getting visualizations of our performance boosts. We are still working on this right now. Our plans are to use synchronization better and profile our existing implementation. Since we are on track with our schedule, we believe we can produce all our deliverables. As far as nice to have goals (e.g. CUDA implementation, interior point methods), we believe that we may be able to work on CUDA if we finish the work for our required deliverables early.

Revised Schedule:

Week 4 (Nov. 28 to Dec. 2):
- Optimize Parallel Simplex Code (Raghav, Charlie, 11/30 to 12/2)
- Attempt better synchronization scheme (Raghav, Charlie, 11/30 to 12/2)
- Fix checker.py script so we can test new parallel code faster (Raghav, 11/30)
- Start work on CUDA Simplex Implementation (Raghav, Charlie, 12/3 to 12/5)
Week 5 (Dec. 5 to Dec. 9):
- Finish Implementing CUDA implementation (Raghav, Charlie, 12/5 to 12/7)
- Evaluate using the Thrust library with CUDA (Charlie, 12/5 to 12/7)
- Benchmark CUDA Implementation (Raghav, 12/7)
- Create Final Report (Raghav, 12/5 to 12/8)
- Create Final Poster (Raghav, Charlie, 12/5 to 12/7)
- Prepare for presenting the final poster (Raghav, Charlie, 12/7 to 12/8)

## Poster Session Data:

Our poster will include an overview of our project and how we parallelized the Simplex algorithm with OpenMP. In order to demonstrate our findings with our parallelization attempts, we plan to show graphs on how our sequential and OpenMP algorithms perform on different linear programs. We plan to explain our speedups as well by using performance tools mentioned in this class, such as perf.

## Preliminary Results::

Our initial results are as follows:
'''
Sequential Version:
Time difference = 1612 microseconds

OpenMP version:
Time difference = 1592 microseconds
'''

This is a speedup from running our OpenMP algorithm on a sample linear programming input.

## Our Concerns:

We are concerned with finishing everything in time. We believe that we can get a 100 percent implementation of our code working by the early deadline. Finishing the project and presentation during the last week while attempting our 125 percent deliverables may be difficult. Finding areas to improve parallelism with OpenMP will be hard, as we have been stuck on optimizing our current OpenMP implementation further. If we can't optimize our OpenMP implementation further, we will have to find out why we can't optimize further.
