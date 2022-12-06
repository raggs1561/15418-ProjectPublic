// This code was adapted from simplex.java by Danny Sleator (which in turn was
// adapted from UBC CODERCHIVE 2014).
// It is in the public domain. --- Carl Kingsford Nov. 2017
// We got this code from 15-451.

#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;

struct Compare { double val; int index; };    
#pragma omp declare reduction(minimum : struct Compare : omp_out = omp_in.val < omp_out.val ? omp_in : omp_out)
#pragma omp declare reduction(maximum : struct Compare : omp_out = omp_in.val > omp_out.val ? omp_in : omp_out)

class Simplex {

  private:
    int m, n;
    std::vector<std::vector<double>> A;
    std::vector<int> basic;    // size m.  indices of basic vars
    std::vector<int> nonbasic; // size n.  indices of non-basic vars

  public:
    std::vector<double> soln;
    double z;    // return value of the objective function.
    int lp_type; // for return.  1 if feasible, 0 if not feasible, -1 if
                 // unbounded

    const double INF; // unbelivably, C++ doesn't support static doubles
                      // initialized in a class
    const double EPS;
    const static int FEASIBLE = 1; // int vars are ok though
    const static int INFEASIBLE = 0;
    const static int UNBOUNDED = -1;

    /*
      input:
        m = #constraints, n =#variables
        max c dot x s.t. a x <= b  x >= 0
        where a = mxn, b = m vector, c = n vector
      output:
        Infeasible, or Unbounded, or a pair Feasible (z,soln) where z is
        the maximum objective function value, and soln is an n-vector of
        variable values.
      caveats:
        Cycling is possible.  Nothing is done to mitigate loss of
        precision when the number of iterations is large.
    */
    Simplex(int m0, int n0, std::vector<std::vector<double>> &A0,
            std::vector<double> &B, std::vector<double> &C)
        : m(m0), n(n0), A(m0 + 1), basic(m0), nonbasic(n0), soln(n), INF(1e100),
          EPS(1e-9)

    {
        // m constraints, n variables here

        // Create constraint matrix, resize to add the B matrix as the last column
        #pragma omp parallel for
        for (unsigned int i = 0; i < A.size(); i++) {
            A[i].resize(n + 1);
        }
        
        #pragma omp parallel for
        for (int j = 0; j < m; j++)
            basic[j] = n + j;

        #pragma omp parallel for
        for (int i = 0; i < n; i++)
            nonbasic[i] = i;
        
        # pragma omp parallel for
        for (int i = 0; i < m; i++) {
            A[i][n] = B[i];
            # pragma omp parallel for
            for (int j = 0; j < n; j++)
                A[i][j] = A0[i][j];
        }

        // Add c vector to A
        #pragma omp parallel for
        for (int j = 0; j < n; j++)
            A[m][j] = C[j];

        // Don't run simplex on an infeasible LP
        if (!Feasible()) {
            lp_type = INFEASIBLE;
            return;
        }

        while (true) {
            int r = 0, c = 0;
            double p = 0.0;
            
            struct Compare max;
            max.val = p;
            max.index = c;
            #pragma omp parallel reduction(maximum:max)
            for (int i = 0; i < n; i++) {
                if (A[m][i] > max.val) {
                    max.val = A[m][i];
                    max.index = i;
                }
            }
            p = max.val; 
            c = max.index;
            /*
            #pragma omp parallel
            {
                int p_local = p;
                int c_local = c;

                #pragma omp for nowait
                for (int i = 0; i < n; i++) {
                    if(A[m][i] > p_local) {
                        p_local = A[m][i];
                        c_local = i;
                    }
                }

                #pragma omp critical
                {
                    if (p_local > p) {
                        p = p_local;
                        c = c_local;
                    }
                }
            }
            */

            # pragma omp parallel
            {
            if (p < EPS) {
                #pragma omp for
                for (int j = 0; j < n; j++)
                    if (nonbasic[j] < n)
                        soln[nonbasic[j]] = 0;

                # pragma omp for
                for (int i = 0; i < m; i++)
                    if (basic[i] < n)
                        soln[basic[i]] = A[i][n];
                
                z = -A[m][n];
                lp_type = FEASIBLE;
            }
            }
            if (lp_type == FEASIBLE) break;

            p = INF;
            
            #pragma omp parallel
            {
                double p_local = p;
                int r_local = r;

                #pragma omp for nowait
                for (int i = 0; i < m; i++) {
                    if (A[i][c] > EPS) {
                        double val = A[i][n] / A[i][c];
                        if(val < p_local) {
                            p_local = val;
                            r_local = i;
                        }
                    }
                }

                #pragma omp critical
                {
                    if (p_local < p) {
                        p = p_local;
                        r = r_local;
                    }
                }
            }

            // Is this needed? 
            // #pragma omp barrier

            if (p == INF) {
                lp_type = UNBOUNDED;
                break;
            }
            Pivot(r, c);
        }
    }

  private:
    void printa() {
        int i, j;
        for (i = 0; i <= m; i++) {
            for (j = 0; j <= n; j++) {
                printf("A[%d][%d] = %f\n", i, j, A[i][j]);
            };
        }
    }

    void Pivot(int r, int c) {

        // printf("pivot %d %d\n", r, c);
        // printa();

        swap(basic[r], nonbasic[c]);

        A[r][c] = 1 / A[r][c];

        # pragma omp parallel for
        for (int j = 0; j <= n; j++) {
            if (j != c)
                A[r][j] *= A[r][c];
        }
        
        # pragma omp parallel for
        for (int i = 0; i <= m; i++) {
            if (i != r) {
                # pragma omp parallel for
                for (int j = 0; j <= n; j++) {
                    if (j != c)
                        A[i][j] -= A[i][c] * A[r][j];
                }
                A[i][c] = -A[i][c] * A[r][c];
            }
        }
    }

    bool Feasible() {
        int r = 0, c = 0;
        while (true) {
            double p = INF;
            
            struct Compare min;
            min.val = p;
            min.index = r;
            #pragma omp parallel for reduction(minimum:min)
            for (int i = 0; i < m; i++) {
                if (A[i][n] < min.val) {
                    min.val = A[i][n];
                    min.index = i;
                }
            }
            p = min.val; 
            r = min.index;

            if (p > -EPS)
                return true;
            
            p = 0.0;
            min.val = p;
            min.index = c;
            #pragma omp parallel for reduction(minimum:min)
            for (int i = 0; i < n; i++) {
                if (A[r][i] < min.val) {
                    min.val = A[r][i];
                    min.index = i;
                }
            }
            p = min.val; 
            c = min.index;

            if (p > -EPS)
                return false;
            
            p = A[r][n] / A[r][c];
            
            min.val = p;
            min.index = r;

            #pragma omp parallel for reduction(minimum:min)
            for (int i = r + 1; i < m; i++) {
                if (A[i][c] > EPS) {
                    double val = A[i][n] / A[i][c];
                    if (val < min.val) {
                        min.val = val;
                        min.index = i;
                    }
                }
            }
            p = min.val; 
            r = min.index;

            Pivot(r, c);
        }
    }
};

int main(int argc, char *argv[]) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    std::vector<std::vector<double>> A;
    int numRules = 20000;
    int numVars = 20000;
    srand(1);

    // std::cin >> numRules >> numVars;
    auto randFloat = [](){return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100000.f));};

    A.resize(numRules);
    for (int i = 0; i < numRules; i++) {
        A[i].resize(numVars);
    }
    for (int i = 0; i < numRules; i++) {
        for (int j = 0; j < numVars; j++) {
            // std::cin >> A[i][j];
            A[i][j] = randFloat();
        }
    }

    std::vector<double> B;
    B.resize(numRules);
    for (int i = 0; i < numRules; i++) {
        // std::cin >> B[i];
        B[i] = randFloat();
    }

    std::vector<double> C;
    C.resize(numVars);
    for (int i = 0; i < numVars; i++) {
        // std::cin >> C[i];
        C[i] = randFloat();
    }



    std::cout << "Loaded"  << std::endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    Simplex lp(numRules, numVars, A, B, C);
    
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    if (lp.lp_type == lp.UNBOUNDED) {
        std::cout << "unbounded" << std::endl;
    } else if (lp.lp_type == lp.INFEASIBLE) {
        std::cout << "infeasible" << std::endl;
    } else if (lp.lp_type == lp.FEASIBLE) {
        std::cout << "The optimum is " << lp.z << std::endl;
        /*
        for (int i = 0; i < numVars; i++) {
            std::cout << "x" << i << " = " << lp.soln[i] << std::endl;
        }
        */
    } else {
        std::cout << "Should not have happened" << std::endl;
    }

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;

}