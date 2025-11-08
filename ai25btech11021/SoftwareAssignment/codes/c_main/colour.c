#include <stdio.h>
#include <stdlib.h>
#include <string.h>           
#include <math.h>            
#include <time.h>           // for allocating a random matrix//

// include stb_image and stb_image_write from c_libs folder //
#define STB_IMAGE_IMPLEMENTATION
#include "../c_libs/stb_image.h"               //for image reading//
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../c_libs/stb_image_write.h"         //for image writing//

#define P 2    // number of power iterations here Im using 2 for speed//

//All Matrix Functions I'm using//
double **alloc_matrix(int m, int n) {
    double **A = (double **)malloc(m * sizeof(double *));
    A[0] = (double *)calloc(m * n, sizeof(double));
    for (int i = 1; i < m; i++)
        A[i] = A[0] + i * n;
    return A;
}

void free_matrix(double **A) {
    free(A[0]);                        //Freeing//
    free(A);
}

void matmul(double **A, double **B, double **C, int m, int n, int p) {
    for (int i = 0; i < m; i++)
        for (int j = 0; j < p; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++)           //Matrix multplication//
                sum += A[i][k] * B[k][j];
            C[i][j] = sum;
        }
}

void orthonormalize(double **Q, int m, int n) {
    for (int j = 0; j < n; j++) {
        for (int k = 0; k < j; k++) {
            double dot = 0;
            for (int i = 0; i < m; i++)
                dot += Q[i][k] * Q[i][j];
            for (int i = 0; i < m; i++)            //Orthonormalising matrix Y (QR)//
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


void transpose(double **A, double **At, int m, int n) {
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)             //Transpose//
            At[j][i] = A[i][j];
}

void randomized_power_iteration(double **A, int m, int n, int k, int p_iter, double **A_k) {
    srand(time(NULL));

    double **Omega = alloc_matrix(n, k);
    double **Y = alloc_matrix(m, k);
    double **At = alloc_matrix(n, m);
    double **AtY = alloc_matrix(n, k);
    double **Q = alloc_matrix(m, k);
    double **Qt = alloc_matrix(k, m);              //Randomized Power Iteration//
    double **B = alloc_matrix(k, n);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < k; j++)
            Omega[i][j] = ((double)rand() / RAND_MAX) - 0.5;

    matmul(A, Omega, Y, m, n, k);

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

    free_matrix(Omega);
    free_matrix(Y);
    free_matrix(At);
    free_matrix(AtY);              //Freeing//
    free_matrix(Q);
    free_matrix(Qt);
    free_matrix(B);
}

//Read image stb_image//
unsigned char *readImage(const char *filename, int *width, int *height, int *channels) {
    unsigned char *data = stbi_load(filename, width, height, channels, 0);
    if (!data) {
        fprintf(stderr, "Error loading image: %s\n", filename);
        exit(1);
    }
    return data;
}

//Write image stb_image_write//

void writeImage(const char *filename, unsigned char *data, int width, int height, int channels) {
    int quality = 40;                                      // fixed quality of output image//
    const char *ext = strrchr(filename, '.');

    if (ext && (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)) {
        if (stbi_write_jpg(filename, width, height, channels, data, quality))
            printf("Output JPG written: %s (quality = %d)\n", filename, quality);
        else
            printf("Error writing JPG file: %s\n", filename);
    } else if (ext && strcmp(ext, ".png") == 0) {
        if (stbi_write_png(filename, width, height, channels, data, width * channels))
            printf("Output PNG written: %s\n", filename, quality);
        else
            printf("Error writing PNG file: %s\n", filename);
    } else {
        printf("Unknown format: %s\n", filename);
    }
}

int main() {
    char subfolder[128];
    printf("Enter subfolder name inside figs: ");
    scanf("%s", subfolder);

    int num_K;
    printf("Enter number of K values: ");               //For input of folder name, K values//
    scanf("%d", &num_K);

    int Ks[num_K];
    printf("Enter %d K values: ", num_K);
    for (int i = 0; i < num_K; i++)
        scanf("%d", &Ks[i]);

    
    char input_path[256];
    char format[8] = "";
    FILE *f = NULL;

    sprintf(input_path, "../../figs/%s/image.jpg", subfolder);
    f = fopen(input_path, "rb");
    if (f) {
        strcpy(format, "jpg");
        fclose(f);
    } else {                                    // checking .jpg or .png//
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

    int width, height, channels;
    unsigned char *img = readImage(input_path, &width, &height, &channels);

    printf("Loaded image: %d x %d (%d channels, %s)\n", height, width, channels, format);

    double **A = alloc_matrix(height, width);
    double **A_k = alloc_matrix(height, width);

    unsigned char *output = (unsigned char *)malloc(width * height * channels);

    char error_path[256];
    sprintf(error_path, "../../figs/%s/errors.txt", subfolder);
    FILE *errors = fopen(error_path, "w");
    if (!errors) {                        //Writing Frobenius error and ratio in error.txt for all K values//
        perror("fopen errors.txt");
        exit(1);
    }
    fprintf(errors, "K, Frobenius_Error, Relative_Error\n");

    for (int idx = 0; idx < num_K; idx++) {
        int K = Ks[idx];
        double frob_total = 0.0, frob_A_total = 0.0;

        for (int ch = 0; ch < channels; ch++) {
            for (int i = 0; i < height; i++)
                for (int j = 0; j < width; j++)
                    A[i][j] = (double)img[(i * width + j) * channels + ch];

            randomized_power_iteration(A, height, width, K, P, A_k); //Doing Power Iteration for A//

            for (int i = 0; i < height; i++)
                for (int j = 0; j < width; j++) {
                    double diff = A[i][j] - A_k[i][j];
                    frob_total += diff * diff;
                    frob_A_total += A[i][j] * A[i][j];
                    double val = A_k[i][j];
                    if (val < 0) val = 0;
                    if (val > 255) val = 255;
                    output[(i * width + j) * channels + ch] = (unsigned char)round(val);
                }
        }

        frob_total = sqrt(frob_total);
        frob_A_total = sqrt(frob_A_total);
        double rel_error = frob_total / frob_A_total;

        printf("K=%d → Frobenius norm ||A - A_k||_F = %.6f\n", K, frob_total);
        printf("Relative Frobenius error = %.6f\n", rel_error);

        fprintf(errors, "%d, %.6f, %.6f\n", K, frob_total, rel_error);

                                                             
        char filename[256];                             //Output renamed//
        
        sprintf(filename, "../../figs/%s/%s_%d.%s", subfolder, subfolder, K, format);
        writeImage(filename, output, width, height, channels);
    }

    fclose(errors);
    printf("errors written to ../../figs/%s/errors.txt\n", subfolder);

    stbi_image_free(img);
    free(output);
    free_matrix(A);                                //Freeing the memory//
    free_matrix(A_k);

    return 0;
}
