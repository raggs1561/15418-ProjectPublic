#ifndef SIMPLEX
#define SIMPLEX

#include <math.h>
#include <memory>
#include <vector>
#include <limits>

struct Vec2 {
  double x, y;
  Vec2(double vx = 0.0f, double vy = 0.0f) : x(vx), y(vy) {}
  static double dot(Vec2 v0, Vec2 v1) { return v0.x * v1.x + v0.y * v1.y; }
  double &operator[](int i) { return ((double *)this)[i]; }
  Vec2 operator*(double s) const { return Vec2(*this) *= s; }
  Vec2 operator*(Vec2 vin) const { return Vec2(*this) *= vin; }
  Vec2 operator+(Vec2 vin) const { return Vec2(*this) += vin; }
  Vec2 operator-(Vec2 vin) const { return Vec2(*this) -= vin; }
  Vec2 operator-() const { return Vec2(-x, -y); }
  Vec2 &operator+=(Vec2 vin) {
    x += vin.x;
    y += vin.y;
    return *this;
  }
  Vec2 &operator-=(Vec2 vin) {
    x -= vin.x;
    y -= vin.y;
    return *this;
  }
  Vec2 &operator=(double v) {
    x = y = v;
    return *this;
  }
  Vec2 &operator*=(double s) {
    x *= s;
    y *= s;
    return *this;
  }
  Vec2 &operator*=(Vec2 vin) {
    x *= vin.x;
    y *= vin.y;
    return *this;
  }
  double length2() const { return x * x + y * y; }
  double length() const { return sqrt(length2()); }
};


bool canBeImproved(std::vector<std::vector<double>> tableau) {
    int n = tableau.size();
    std::vector<double> z = tableau[n - 1];

    // if we can increase the value of our objective function via visiting
    // a neighboring point, return true
    for (int i = 0; i < z.size(); i++) {
        if (z[i] > 0) {
            return true;
        }
    }
    return false;
}

Vec2 getPivotPosition(std::vector<std::vector<double>> tableau) {
    int n = tableau.size();
    std::vector<double> z = tableau[n - 1];

    // find the next position in the tableau we can move to
    int i = 0;
    while (i < z.size()) {
        if (z[i] > 0) {
            break;
        }
        i += 1;
    }
    int column = i;

    // make a list of restrictions we have
    std::vector<double> restrictions;

    // iterate through other parts of our tableau
    int minRow = 0;
    double currentMin = std::numeric_limits<double>::max();
    for (i = 0; i < n - 1; i++) {
        int el = tableau[i][column];

        if (el > 0) {
            if (tableau[i][z.size()] / el < currentMin) {
                minRow = i;
                currentMin = tableau[i][z.size()] / el;
            }
        }
    }

    return Vec2(minRow, column);
}

bool isBasic(std::vector<double> column) {
    double sum = 0;
    int numZeroes = 0;
    for (auto elt : column) {
        sum += elt;
        if (elt == 0) {
            numZeroes++;
        }
    }

    return (numZeroes == column.size() - 1) && sum == 1;
}

std::vector<double> getSolution(std::vector<std::vector<double>> tableau) {

}

def get_solution(tableau):
    columns = np.array(tableau).T
    solutions = []
    for column in columns[:-1]:
        solution = 0
        if is_basic(column):
            one_index = column.tolist().index(1)
            solution = columns[-1][one_index]
        solutions.append(solution)

    return solutions

def to_tableau(c, A, b):
    xb = [eq + [x] for eq, x in zip(A, b)]
    z = c + [0]
    return xb + [z]

std::vector<double> simplex(std::vector<double> c, std::vector<std::vector<double>> A, std::vector<double> b) {
    std::vector<std::vector<double>> tableau = getTableau(c, A, b);

    while(canBeImproved(tableau)) {
        int pivot = getPivotPosition(tableau);
        tableau = pivotStep(tableau, pivot);
    }

    return getSolution(tableau);
}