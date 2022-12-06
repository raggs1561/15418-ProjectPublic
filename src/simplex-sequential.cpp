// This code was adapted from simplex.java by Danny Sleator (which in turn was
// adapted from UBC CODERCHIVE 2014).
// It is in the public domain. --- Carl Kingsford Nov. 2017

#include <iostream>
#include <vector>
#include <chrono>
#include <random>

using namespace std;

class Simplex {

  private:
    int m, n;
    std::vector<std::vector<float>> A;
    std::vector<int> basic;    // size m.  indices of basic vars
    std::vector<int> nonbasic; // size n.  indices of non-basic vars

  public:
    std::vector<float> soln;
    float z;    // return value of the objective function.
    int lp_type; // for return.  1 if feasible, 0 if not feasible, -1 if
                 // unbounded

    const float INF; // unbelivably, C++ doesn't support static floats
                      // initialized in a class
    const float EPS;
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
    Simplex(int m0, int n0, std::vector<std::vector<float>> &A0,
            std::vector<float> &B, std::vector<float> &C)
        : m(m0), n(n0), A(m0 + 1), basic(m0), nonbasic(n0), soln(n), INF(1e100),
          EPS(1e-9)

    {
        for (unsigned int i = 0; i < A.size(); i++) {
            A[i].resize(n + 1);
        }

        for (int j = 0; j < m; j++)
            basic[j] = n + j;
        for (int i = 0; i < n; i++)
            nonbasic[i] = i;

        for (int i = 0; i < m; i++) {
            A[i][n] = B[i];
            for (int j = 0; j < n; j++)
                A[i][j] = A0[i][j];
        }

        for (int j = 0; j < n; j++)
            A[m][j] = C[j];

        if (!Feasible()) {
            lp_type = INFEASIBLE;
            return;
        }

        while (true) {
            int r = 0, c = 0;
            float p = 0.0;
            // p is our objective value

            for (int i = 0; i < n; i++) {
                if (A[m][i] > p)
                    p = A[m][c = i];
            }

            if (p < EPS) {
                for (int j = 0; j < n; j++)
                    if (nonbasic[j] < n)
                        soln[nonbasic[j]] = 0;
                for (int i = 0; i < m; i++)
                    if (basic[i] < n)
                        soln[basic[i]] = A[i][n];
                z = -A[m][n];
                lp_type = FEASIBLE;
                break;
            }

            p = INF;
            for (int i = 0; i < m; i++) {
                if (A[i][c] > EPS) {
                    float v = A[i][n] / A[i][c];
                    if (v < p) {
                        p = v;
                        r = i;
                    }
                }
            }

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
        for (int j = 0; j <= n; j++) {
            if (j != c)
                A[r][j] *= A[r][c];
        }
        for (int i = 0; i <= m; i++) {
            if (i != r) {
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
            float p = INF;
            for (int i = 0; i < m; i++)
                if (A[i][n] < p)
                    p = A[r = i][n];
            if (p > -EPS)
                return true;
            p = 0.0;
            
            for (int i = 0; i < n; i++)
                if (A[r][i] < p)
                    p = A[r][c = i];
            if (p > -EPS)
                return false;
            
            p = A[r][n] / A[r][c];
            
            for (int i = r + 1; i < m; i++) {
                if (A[i][c] > EPS) {
                    float v = A[i][n] / A[i][c];
                    if (v < p) {
                        p = v;
                        r = i;
                    }
                }
            }
            Pivot(r, c);
        }
    }
};

int main(int argc, char *argv[]) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    std::vector<std::vector<float>> A;

    int numRules = 20000;
    int numVars = 20000;
    std::mt19937 randGen(1);
    std::uniform_real_distribution<float>randReal(0, std::numeric_limits<float>::max());

    auto randFloat = [&](){return randReal(randGen) ;};

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

    std::vector<float> B;
    B.resize(numRules);
    for (int i = 0; i < numRules; i++) {
        // std::cin >> B[i];
        B[i] = randFloat();
    }

    std::vector<float> C;
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
        // for (int i = 0; i < numVars; i++) {
        //     std::cout << "x" << i << " = " << lp.soln[i] << std::endl;
        // }
    } else {
        std::cout << "Should not have happened" << std::endl;
    }

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;


}