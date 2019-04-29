#include <iostream>
#include <array>

void matrix_multiplication(){
    int i, j, k;
    int n=500;
    int N=200;
    int a[n][N];
    int b[N][n];
    int c[n][n];
    for(i=0;i<n;++i) {
        for(j=0;j<n;++j){
            c[i][j]=0;
            for(k=0;k<N;++k){
                a[i][k] = b[k][j];
                c[i][j] = a[i][k] + 1;
                c[i][j]+=a[i][k]*b[k][j];
            }
        }
    }
}

int main(int argc, char** argv){
    
    

    matrix_multiplication();
    
    return 0;
}

