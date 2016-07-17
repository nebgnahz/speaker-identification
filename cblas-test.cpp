#include <stdio.h>

#if __APPLE__
#include <Accelerate/Accelerate.h>
#elif __linux__
#include <cblas.h>
#endif

// Calculate a * b
float a[4][4] = {
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 1.0f},
};

float b[4][4] = {
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 1.0f},
};

float c[4][4] = {
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
};

int main() {
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 4, 4, 4,
                1.0f, (float*)a, 4, (float*)b, 4, 1.0f, (float*)c, 4);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%.2f ", c[i][j]);
        }
        printf("\n");
    }

    return 0;
}
