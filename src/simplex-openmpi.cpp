// This code was adapted from simplex.java by Danny Sleator (which in turn was
// adapted from UBC CODERCHIVE 2014).
// It is in the public domain. --- Carl Kingsford Nov. 2017
// We got this code from 15-451.

#include "mpi.h"
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

const static int FEASIBLE = 1; // int vars are ok though
const static int INFEASIBLE = 0;
const static int UNBOUNDED = -1;
const double EPS = 1e-9;
const double INF = 1e100;

int main(int argc, char *argv[]) {
    int pid;
    int nproc;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    std::cout << pid << std::endl;
    if (pid == 0) {
    int numRules = 20000;
    int numVars = 20000;

    std::vector<std::vector<double>> A;
    std::vector<double> B;
    std::vector<double> C;
    std::vector<int> basic;    // size numRules.  indices of basic vars
    std::vector<int> nonbasic; // size numVars.  indices of non-basic vars

    std::mt19937 randGen(1);
    std::uniform_real_distribution<double> randReal(0, 100000.f);

    auto randFloat = [&]() { return randReal(randGen); };

    A.resize(numRules + 1);
    for (int i = 0; i < numRules + 1; i++) {
        A[i].resize(numVars + 1);
    }
    for (int i = 0; i < numRules; i++) {
        for (int j = 0; j < numVars; j++) {
            A[i][j] = randFloat();
        }
    }

    B.resize(numRules);
    for (int i = 0; i < numRules; i++) {
        B[i] = randFloat();
    }

    C.resize(numVars);
    for (int i = 0; i < numVars; i++) {
        C[i] = randFloat();
    }

    #pragma clang loop interleave(enable)
    for (int j = 0; j < numRules; j++)
        basic[j] = numVars + j;

    #pragma clang loop interleave(enable)
    for (int i = 0; i < numVars; i++)
        nonbasic[i] = i;
    
    #pragma clang loop unroll(enable)
    for (int i = 0; i < numRules; i++) 
        A[i][numVars] = B[i];

    // Add c vector to A
    #pragma clang loop interleave(enable)
    for (int j = 0; j < numVars; j++)
        A[numRules][j] = C[j];
    
    std::cout << "Loaded" << std::endl;
    }

    std::vector<std::vector<double>> myA;
    std::vector<double> myB;
    std::vector<double> myC;


    // Split these into equal parts by first index (rows)



    std::chrono::steady_clock::time_point begin =
        std::chrono::steady_clock::now();

    // Simplex lp(numRules, numVars, A, B, C);

    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();

    // if (lp.lp_type == lp.UNBOUNDED) {
    //     std::cout << "unbounded" << std::endl;
    // } else if (lp.lp_type == lp.INFEASIBLE) {
    //     std::cout << "infeasible" << std::endl;
    // } else if (lp.lp_type == lp.FEASIBLE) {
    //     std::cout << "The optimum is " << lp.z << std::endl;
    // } else {
    //     std::cout << "Should not have happened" << std::endl;
    // }

    std::cout << "Time difference = "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       begin)
                     .count()
              << "[ms]" << std::endl;
    MPI_Finalize();
}
