#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern void saxpy_asm(int n, float A, float* X, float* Y, float* Z);

int manual();
int test();

void saxpy_c(int n, float A, float* X, float* Y, float* Z) {
    for (int i = 0; i < n; i++) {
        Z[i] = A * X[i] + Y[i];
    }
}

double get_time() {
    return (double)clock() / CLOCKS_PER_SEC;
}

int main() {
    int choice = -1;

    while (choice == -1) {
        printf("0=Manual input, 1=Run tests: ");
        scanf("%d", &choice);

        switch (choice) {
        case -1:
            continue;
        case 0:
            manual();
            break;
        case 1:
            test();
            break;
        default:
            printf("Invalid choice! Please enter 0 or 1.\n");
            choice = -1;
            break;
        }
    }
    return 0;
}

int manual() {
    int n;
    float A;
    
    printf("\n=== Manual Input ===\n");
    
    printf("Enter the size of vectors (n): ");
    scanf("%d", &n);
    
    if (n <= 0) {
        printf("Error: n must be positive!\n");
        return 1;
    }
    
    printf("Enter scalar value A: ");
    scanf("%f", &A);
    
    float* X = (float*)malloc(n * sizeof(float));
    float* Y = (float*)malloc(n * sizeof(float));
    float* Z_c = (float*)malloc(n * sizeof(float));
    float* Z_asm = (float*)malloc(n * sizeof(float));
    
    if (!X || !Y || !Z_c || !Z_asm) {
        printf("Memory allocation failed for n = %d\n", n);
        free(X);
        free(Y);
        free(Z_c);
        free(Z_asm);
        return 1;
    }
    
    int input_choice;
    printf("\nInput method: 1=Manual, 2=Random: ");
    scanf("%d", &input_choice);
    
    if (input_choice == 1) {
        printf("\nEnter values for vector X:\n");
        for (int i = 0; i < n; i++) {
            printf("X[%d] = ", i);
            scanf("%f", &X[i]);
        }
        
        printf("\nEnter values for vector Y:\n");
        for (int i = 0; i < n; i++) {
            printf("Y[%d] = ", i);
            scanf("%f", &Y[i]);
        }
    } else {
        srand(67);
        for (int i = 0; i < n; i++) {
            X[i] = (float)rand() / (float)RAND_MAX;
            Y[i] = (float)rand() / (float)RAND_MAX;
        }
        printf("Vectors initialized with random values.\n");
    }
    
    double c_total_time = 0.0;
    for (int run = 0; run < 30; run++) {
        double start = get_time();
        saxpy_c(n, A, X, Y, Z_c);
        double end = get_time();
        c_total_time += (end - start);
    }
    double c_avg_time = c_total_time / 30.0;
    
    printf("\nC Version: %.6f seconds\n", c_avg_time);
    printf("First 10 elements:\n");
    for (int i = 0; i < 10 && i < n; i++) {
        printf("  Z[%d] = %.6f\n", i, Z_c[i]);
    }
    
    double asm_total_time = 0.0;
    for (int run = 0; run < 30; run++) {
        double start = get_time();
        saxpy_asm(n, A, X, Y, Z_asm);
        double end = get_time();
        asm_total_time += (end - start);
    }
    double asm_avg_time = asm_total_time / 30.0;
    
    printf("\nAssembly Version: %.6f seconds\n", asm_avg_time);
    printf("First 10 elements:\n");
    for (int i = 0; i < 10 && i < n; i++) {
        printf("  Z[%d] = %.6f\n", i, Z_asm[i]);
    }
    
    int errors = 0;
    float epsilon = 1e-5f;
    for (int i = 0; i < n; i++) {
        float diff = Z_c[i] - Z_asm[i];
        if (diff < 0) diff = -diff;
        if (diff > epsilon) {
            if (errors < 10) {
                printf("Error at index %d: C=%.6f, ASM=%.6f, diff=%.6f\n", 
                       i, Z_c[i], Z_asm[i], diff);
            }
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("\nCORRECT - All elements match!\n");
    } else {
        printf("\nERROR - %d elements do not match!\n", errors);
    }
    
    printf("\nPerformance Summary:\n");
    printf("  C:        %.6f seconds\n", c_avg_time);
    printf("  Assembly: %.6f seconds\n", asm_avg_time);
    if (asm_avg_time < c_avg_time) {
        printf("  Speedup: %.2fx (Assembly faster)\n", c_avg_time / asm_avg_time);
    } else {
        printf("  Speedup: %.2fx (C faster)\n", asm_avg_time / c_avg_time);
    }
    
    free(X);
    free(Y);
    free(Z_c);
    free(Z_asm);
    
    return 0;
}

int test() {
    int sizes[] = { 1 << 20, 1 << 24, 1 << 28 };
    int num_sizes = 3;
    
    for (int s = 0; s < 3; s++) {
        int n = sizes[s];
        printf("\n--- n = 2^%d (%d elements) ---\n", (s == 0 ? 20 : (s == 1 ? 24 : 28)), n);
        
        float* X = (float*)malloc(n * sizeof(float));
        float* Y = (float*)malloc(n * sizeof(float));
        float* Z_c = (float*)malloc(n * sizeof(float));  
        float* Z_asm = (float*)malloc(n * sizeof(float));
        
        if (!X || !Y || !Z_c || !Z_asm) {
            printf("Memory allocation failed for n = %d\n", n);
            free(X);
            free(Y);
            free(Z_c);
            free(Z_asm);
            continue;
        }
        
        float A = 3.0f;
        srand(67);
        for (int i = 0; i < n; i++) {
            X[i] = (float)rand() / (float)RAND_MAX;
            Y[i] = (float)rand() / (float)RAND_MAX;
        }
        
        double c_total_time = 0.0;
        for (int run = 0; run < 30; run++) {
            double start = get_time();
            saxpy_c(n, A, X, Y, Z_c);
            double end = get_time();
            c_total_time += (end - start);
        }
        double c_avg_time = c_total_time / 30.0;
        
        printf("C Version: %.6f seconds\n", c_avg_time);
        printf("First 10 elements:\n");
        for (int i = 0; i < 10 && i < n; i++) {
            printf("  Z[%d] = %.6f\n", i, Z_c[i]);
        }
        
        double asm_total_time = 0.0;
        for (int run = 0; run < 30; run++) {
            double start = get_time();
            saxpy_asm(n, A, X, Y, Z_asm);
            double end = get_time();
            asm_total_time += (end - start);
        }
        double asm_avg_time = asm_total_time / 30.0;
        
        printf("\nAssembly Version: %.6f seconds\n", asm_avg_time);
        printf("First 10 elements:\n");
        for (int i = 0; i < 10 && i < n; i++) {
            printf("  Z[%d] = %.6f\n", i, Z_asm[i]);
        }
        
        int errors = 0;
        float epsilon = 1e-5f;
        for (int i = 0; i < n; i++) {
            float diff = Z_c[i] - Z_asm[i];
            if (diff < 0) diff = -diff;
            if (diff > epsilon) {
                if (errors < 10) {
                    printf("Error at index %d: C=%.6f, ASM=%.6f, diff=%.6f\n", 
                           i, Z_c[i], Z_asm[i], diff);
                }
                errors++;
            }
        }
        
        if (errors == 0) {
            printf("\nCORRECT\n");
        } else {
            printf("\nERROR - %d mismatches\n", errors);
        }
        
        printf("\nPerformance:\n");
        printf("  C:        %.6f seconds\n", c_avg_time);
        printf("  Assembly: %.6f seconds\n", asm_avg_time);
        if (asm_avg_time < c_avg_time) {
            printf("  Speedup: %.2fx (Assembly faster)\n", c_avg_time / asm_avg_time);
        } else {
            printf("  Speedup: %.2fx (C faster)\n", asm_avg_time / c_avg_time);
        }
        
        free(X);
        free(Y);
        free(Z_c);
        free(Z_asm);
    }
    
    printf("\n--- All tests completed ---\n");
    
    return 0;
}