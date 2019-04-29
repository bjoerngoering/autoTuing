#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

struct Result {
    std::vector< std::vector<int> > A;
    std::vector< std::vector<int> > B;
};

Result read(const std::string& filename) {
    std::vector< std::vector<int> > A, B;
    Result ab;
    std::string line;
    std::ifstream infile;
    infile.open (filename.c_str());
    if (!infile) { std::cout << "Could not open file " << filename << std::endl;}
    int i = 0;
    while (getline(infile, line) && !line.empty()) {
        std::istringstream iss(line);
        A.resize(A.size() + 1);
        int a, j = 0;
        while (iss >> a) {
            A[i].push_back(a);
            j++;
        }
        i++;
    }

    i = 0;
    while (getline(infile, line)) {
        std::istringstream iss(line);
        B.resize(B.size() + 1);
        int a;
        int j = 0;
        while (iss >> a) {
            B[i].push_back(a);
            j++;
        }
        i++;
    }

    infile.close();
    ab.A = A;
    ab.B = B;
    return ab;
}

std::vector< std::vector<int> > ijkalgorithm(std::vector< std::vector<int> > A, 
                                    std::vector< std::vector<int> > B) {
    int n = A.size();
    // initialise C with 0s
    std::vector<int> tmp(n, 0);
    std::vector< std::vector<int> > C(n, tmp);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

void printMatrix(std::vector< std::vector<int> > matrix) {
    std::vector< std::vector<int> >::iterator it;
    std::vector<int>::iterator inner;
    for (it=matrix.begin(); it != matrix.end(); it++) {
        for (inner = it->begin(); inner != it->end(); inner++) {
            std::cout << *inner;
            if(inner+1 != it->end()) {
                std::cout << "\t";
            }
        }
        std::cout << std::endl;
    }
}

int main (int argc, char* argv[]) {
    std::string filename;
    if (argc < 3) {
        filename = "resources/500.in";
    } else {
        filename = argv[2];
    }
    Result result = read (filename);
    std::vector< std::vector<int> > C = ijkalgorithm(result.A, result.B);
    //printMatrix(C);
    return 0;
}