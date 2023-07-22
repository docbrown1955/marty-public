#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define RESET "\033[0m"
#define ERRORFONT "\033[1m\033[31m"   /* Red */
#define SUCCESSFONT "\033[1m\033[32m" /* Green */


inline std::vector<double> readLine(std::ifstream &fin)
{
    std::vector<double> res;
    res.reserve(5);
    std::string line;
    std::getline(fin, line);
    std::istringstream sin(line);
    while (!sin.eof()) {
        double value;
        sin >> value;
        res.push_back(value);
    }

    return res;
}

inline double compare(double a, double b, double)
{
    const auto diff = std::abs(a - b) / ((a < 1e-10) ? 1 : a);
    // if (diff > eps) {
    //     std::cerr << ERRORFONT << "Error: value mismatch " << RESET
    //         << a << "  and  " << b << " ! \n";
    //     exit(2);
    // }
    return diff;
}

inline std::pair<double, double>
compare(std::vector<double> const &A, std::vector<double> const &B, double eps)
{
    if (A.size() != B.size()) {
        return {-1, -1};
    }
    double diff    = 0;
    auto   iterA   = A.begin();
    auto   iterB   = B.begin();
    double maxdiff = 0;
    while (iterA != A.end()) {
        auto diff_i = compare(*iterA, *iterB, eps);
        if (diff_i > maxdiff)
            maxdiff = diff_i;
        diff += diff_i;
        ++iterA;
        ++iterB;
    }

    return {(A.size() == 0) ? 0 : diff / A.size(), maxdiff};
}

inline std::pair<double, double>
compare(char const *file1, char const *file2, double eps)
{
    std::ifstream f1(file1);
    if (!f1) {
        std::cerr << ERRORFONT << "Error:" << RESET << " file \"" << file1
                  << "\" not found!\n";
        return {-1, -1};
    }
    std::ifstream f2(file2);
    if (!f2) {
        std::cerr << ERRORFONT << "Error:" << RESET << " file \"" << file2
                  << "\" not found!\n";
        return {-1, -1};
    }
    std::pair<double, double> diff = {0, 0};
    size_t                    n    = 0;
    while (!f1.eof()) {
        auto diff_i = compare(readLine(f1), readLine(f2), eps);
        if (diff_i.first == -1 || diff_i.second == -1) {
            std::cout << ERRORFONT << "Error: size mismatch in results at line " << n+1
                      << RESET << '\n';
            return {-1, -1};
        }
        diff.first += diff_i.first;
        if (diff_i.second > diff.second)
            diff.second = diff_i.second;
        ++n;
    }
    if (!f2.eof()) {
        std::cerr << ERRORFONT << "Error:" << RESET << " file mismatch for \""
                  << file1 << "\" and "
                  << "\"" << file2 << "\" !\n";
        return {-1, -1};
    }

    if (n != 0)
        return {diff.first / n, diff.second};
    return diff;
}

inline int
assert_equal(std::string const &fileA, std::string const &fileB, double eps)
{
    srand(time(0));
    std::cout.precision(15);

    auto res = compare(fileA.c_str(), fileB.c_str(), eps);
    if (res.first >= 0 && res.second >= 0 && res.second < eps) {
        std::cout << SUCCESSFONT << "Successful comparison of file \"" << fileA
                  << "\" and file \"" << fileB << "\": \n"
                  << RESET;
        std::cout << "Average relative error = " << res.first << std::endl;
        std::cout << "Maximum relative error = " << res.second << std::endl;
        return 0;
    }
    else {
        std::cout << ERRORFONT << "Discrepancy between file \"" << fileA
                  << "\" and file \"" << fileB << "\": \n"
                  << RESET;
        std::cout << "Average relative error = " << res.first << std::endl;
        std::cout << "Maximum relative error = " << res.second << std::endl;
        return 1;
    }
}
