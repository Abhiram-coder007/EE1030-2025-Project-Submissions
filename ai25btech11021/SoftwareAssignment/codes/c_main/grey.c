#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// include stb_image and stb_image_write from c_libs folder //
#define STB_IMAGE_IMPLEMENTATION
#include "../c_libs/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../c_libs/stb_image_write.h"

#define P 4    // number of power iterations here for grey images I'm using 4//


unsigned char **readPGM(const char *filename, int *width, int *height) {     //For Reading image//
    int channels;
    unsigned char *img = stbi_load(filename, width, height, &channels, 1);  // force it to only grayscale //

    if (!img) {
        fprintf(stderr, "Error loading image: %s\n", filename);
        exit(1);
    }

    unsigned char **A = (unsigned char **)malloc(*height * sizeof(unsigned char *));
    for (int i = 0; i < *height; i++) {
        A[i] = (unsigned char *)malloc(*width * sizeof(unsigned char));
        memcpy(A[i], img + i * (*width), *width * sizeof(unsigned char));
    }                                                 //Allocatioon of memory//

    stbi_image_free(img);
    return A;
}

//Write image function (using stb_image_write)//
void writePGM(const char *filename, double **A, int width, int height) {
    int quality = 40;                                    // added quality (set to 40)
    unsigned char *output = (unsigned char *)malloc(width * height);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double val = A[i][j];
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            output[i * width + j] = (unsigned char)round(val);
        }
    }

    const char *ext = strrchr(filename, '.');        //For writing it to output//
    if (ext && (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)) {
        if (stbi_write_jpg(filename, width, height, 1, output, quality))
            printf("Output JPG written: %s (quality = %d)\n", filename, quality);
        else
            printf("Error writing JPG file: %s\n", filename);
    } else if (ext && strcmp(ext, ".png") == 0) {
        if (stbi_write_png(filename, width, height, 1, output, width))
            printf("Output PNG written: %s (quality = %d)\n", filename, quality);
        else
            printf("Error writing PNG file: %s\n", filename);
    } else {
        printf("Unknown output format for: %s\n", filename);
    }

    free(output);
}

//All Matrix which Functions I'm using//
double **alloc_matrix(int m, int n) {
    double **A = (double **)malloc(m * sizeof(double *));
    A[0] = (double *)calloc(m * n, sizeof(double));             //Allocating Matrix//
    for (int i = 1; i < m; i++)
        A[i] = A[0] + i * n;
    return A;
}
void orthonormalize(double **Q, int m, int n) {
    for (int j = 0; j < n; j++) {
        for (int k = 0; k < j; k++) {
            double dot = 0;
            for (int i = 0; i < m; i++)                  //Orthonormalising Y//
                dot += Q[i][k] * Q[i][j];
            for (int i = 0; i < m; i++)
                Q[i][j] -= dot * Q[i][k];
        }
        double norm = 0;
        for (int i = 0; i < m; i++)
            norm += Q[i][j] * Q[i][j];
        norm = sqrt(norm);
        if (norm > 1e-12)
            for (int i = 0; i < m; i++)
                Q[i][j] /= norm;
    }
}
void free_matrix(double **A) {
    free(A[0]);                          //Freeing//
    free(A);
}

void transpose(double **A, double **At, int m, int n) {
    for (int i = 0; i < m; i++)                                  //Transpose//
        for (int j = 0; j < n; j++)
            At[j][i] = A[i][j];
}
void matmul(double **A, double **B, double **C, int m, int n, int p) {
    for (int i = 0; i < m; i++)
        for (int j = 0; j < p; j++) {
            double sum = 0.0;                              // Doing Matrix Multiplication//
            for (int k = 0; k < n; k++)
                sum += A[i][k] * B[k][j];
            C[i][j] = sum;
        }
}


void randomized_power_iteration(double **A, int m, int n, int k, int p_iter, double **A_k) {
    srand(time(NULL));

    double **O = alloc_matrix(n, k);
    double **Y = alloc_matrix(m, k);
    double **At = alloc_matrix(n, m);
    double **AtY = alloc_matrix(n, k);              // Taking Omega as O//
    double **Q = alloc_matrix(m, k);               //Randomized Power Iteration Algorithm//
    double **Qt = alloc_matrix(k, m);
    double **B = alloc_matrix(k, n);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < k; j++)
            O[i][j] = ((double)rand() / RAND_MAX) - 0.5;

    matmul(A, O, Y, m, n, k);

    transpose(A, At, m, n);
    for (int it = 0; it < p_iter; it++) {
        matmul(At, Y, AtY, n, m, k);
        matmul(A, AtY, Y, m, n, k);          
    }

    for (int i = 0; i < m; i++)
        for (int j = 0; j < k; j++)
            Q[i][j] = Y[i][j];
    orthonormalize(Q, m, k);

    transpose(Q, Qt, m, k);
    matmul(Qt, A, B, k, m, n);
    matmul(Q, B, A_k, m, k, n);

    free_matrix(O);
    free_matrix(Y);
    free_matrix(At);
    free_matrix(AtY);
    free_matrix(Q);                  //freeing//
    free_matrix(Qt);
    free_matrix(B);
}

int main() {
    char subfolder[128];
    printf("Enter subfolder name inside figs: ");            //for accesebility w/o cluttering in figs folder//
    scanf("%s", subfolder);                               //Input the folder name//

    int num_K;
    printf("Enter number of K values: ");
    scanf("%d", &num_K);                              //Input No of K values//

    int Ks[num_K];
    printf("Enter %d K values: ", num_K);             // K values input//
    for (int i = 0; i < num_K; i++)
        scanf("%d", &Ks[i]);

    
    char input_path[256];
    char format[8] = "";
    FILE *f = NULL;

    sprintf(input_path, "../../figs/%s/image.jpg", subfolder);
    f = fopen(input_path, "rb");
    if (f) {
        strcpy(format, "jpg");                  //Checking .jpg or .png//
        fclose(f);
    } else {
        sprintf(input_path, "../../figs/%s/image.png", subfolder);
        f = fopen(input_path, "rb");
        if (f) {
            strcpy(format, "png");
            fclose(f);
        } else {
            fprintf(stderr, "No image.jpg or image.png found in ../../figs/%s/\n", subfolder);
            exit(1);
        }
    }

    int width, height;
    unsigned char **A_uc = readPGM(input_path, &width, &height);

    int m = height, n = width;
    printf("Loaded image: %d x %d (%s)\n", m, n, format);  

    double **A = alloc_matrix(m, n);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            A[i][j] = (double)A_uc[i][j];

    double frob_A = 0.0;
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            frob_A += A[i][j] * A[i][j];
    frob_A = sqrt(frob_A);

    char error_path[256];
    sprintf(error_path, "../../figs/%s/errors.txt", subfolder);
    FILE *errors = fopen(error_path, "w");
    if (!errors) {
        perror("fopen errors.txt");              // Writing Frobenius errors in errors.txt//
        exit(1);
    }
    fprintf(errors, "K, Frobenius_Error, Frobenius_Ratio\n");

    for (int idx = 0; idx < num_K; idx++) {
        int K = Ks[idx];
        double **A_k = alloc_matrix(m, n);

        randomized_power_iteration(A, m, n, K, P, A_k);  //Doing Iteration//

        printf("Computed A_k (rank-%d approximation using %d power iterations)\n", K, P);

        double frob_diff = 0.0;
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++) {
                double diff = A[i][j] - A_k[i][j];
                frob_diff += diff * diff;
            }
        frob_diff = sqrt(frob_diff);
        double frob_ratio = frob_diff / frob_A;


        
        char filename[256];
        sprintf(filename, "../../figs/%s/%s_%d.%s", subfolder, subfolder, K, format);
        writePGM(filename, A_k, n, m);
                                                               // Output in same format as input//
        fprintf(errors, "%d, %.6f, %.6f\n", K, frob_diff, frob_ratio);

        free_matrix(A_k);
    }

    fclose(errors);
    printf("Frobenius norm and ratios written to ../../figs/%s/errors.txt\n", subfolder);

    for (int i = 0; i < m; i++) free(A_uc[i]);           
    free(A_uc);
    free_matrix(A);

    return 0;
}
