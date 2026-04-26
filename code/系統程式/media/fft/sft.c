#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define PI 3.14159265358979323846

typedef struct {
    double real;
    double imag;
} Complex;

void dft(Complex *input, Complex *output, int N) {
    for (int k = 0; k < N; k++) {
        output[k].real = 0.0;
        output[k].imag = 0.0;
        for (int n = 0; n < N; n++) {
            double angle = -2.0 * PI * k * n / N;
            output[k].real += input[n].real * cos(angle) - input[n].imag * sin(angle);
            output[k].imag += input[n].real * sin(angle) + input[n].imag * cos(angle);
        }
    }
}

void idft(Complex *input, Complex *output, int N) {
    for (int n = 0; n < N; n++) {
        output[n].real = 0.0;
        output[n].imag = 0.0;
        for (int k = 0; k < N; k++) {
            double angle = 2.0 * PI * k * n / N;
            output[n].real += input[k].real * cos(angle) - input[k].imag * sin(angle);
            output[n].imag += input[k].real * sin(angle) + input[k].imag * cos(angle);
        }
        output[n].real /= N;
        output[n].imag /= N;
    }
}

double magnitude(Complex c) {
    return sqrt(c.real * c.real + c.imag * c.imag);
}

double phase(Complex c) {
    return atan2(c.imag, c.real);
}

void print_usage(const char *prog) {
    printf("Usage: %s [options]\n", prog);
    printf("Options:\n");
    printf("  -n <size>    Set sample size (default: 64)\n");
    printf("  -f <file>    Input file (one value per line)\n");
    printf("  -i           Perform inverse DFT\n");
    printf("  -m           Output magnitude spectrum\n");
    printf("  -p           Output phase spectrum\n");
    printf("  -c           Output complex values\n");
    printf("  -s <signal>  Signal type: sine, square, sawtooth, impulse (default: sine)\n");
    printf("  -h           Show this help\n");
}

int main(int argc, char *argv[]) {
    int N = 64;
    char *input_file = NULL;
    int inverse = 0;
    int output_mag = 1;
    int output_phase = 0;
    int output_complex = 0;
    char *signal_type = "sine";

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'n':
                    if (i + 1 < argc) N = atoi(argv[++i]);
                    break;
                case 'f':
                    if (i + 1 < argc) input_file = argv[++i];
                    break;
                case 'i':
                    inverse = 1;
                    break;
                case 'm':
                    output_mag = 1;
                    break;
                case 'p':
                    output_phase = 1;
                    break;
                case 'c':
                    output_complex = 1;
                    break;
                case 's':
                    if (i + 1 < argc) signal_type = argv[++i];
                    break;
                case 'h':
                default:
                    print_usage(argv[0]);
                    return 0;
            }
        }
    }

    Complex *input = malloc(N * sizeof(Complex));
    Complex *output = malloc(N * sizeof(Complex));

    if (!input || !output) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    if (input_file) {
        FILE *fp = fopen(input_file, "r");
        if (!fp) {
            fprintf(stderr, "Cannot open file: %s\n", input_file);
            return 1;
        }
        for (int i = 0; i < N; i++) {
            double val;
            if (fscanf(fp, "%lf", &val) != 1) val = 0.0;
            input[i].real = val;
            input[i].imag = 0.0;
        }
        fclose(fp);
    } else {
        double freq = 3.0;
        for (int i = 0; i < N; i++) {
            double t = (double)i / N;
            input[i].imag = 0.0;
            if (strcmp(signal_type, "sine") == 0) {
                input[i].real = sin(2.0 * PI * freq * t);
            } else if (strcmp(signal_type, "square") == 0) {
                input[i].real = (fmod(2.0 * PI * freq * t, 2.0 * PI) < PI) ? 1.0 : -1.0;
            } else if (strcmp(signal_type, "sawtooth") == 0) {
                input[i].real = 2.0 * fmod(freq * t, 1.0) - 1.0;
            } else if (strcmp(signal_type, "impulse") == 0) {
                input[i].real = (i == N / 4) ? 1.0 : 0.0;
            } else {
                input[i].real = sin(2.0 * PI * freq * t);
            }
        }
    }

    if (inverse) {
        idft(input, output, N);
        printf("# Inverse DFT (N=%d):\n", N);
        for (int i = 0; i < N; i++) {
            if (output_complex) {
                printf("%d: %.6f %.6f\n", i, output[i].real, output[i].imag);
            } else {
                printf("%d: %.6f\n", i, output[i].real);
            }
        }
    } else {
        dft(input, output, N);
        printf("# DFT (N=%d):\n", N);
        for (int i = 0; i < N; i++) {
            if (output_complex) {
                printf("%d: %.6f %.6f\n", i, output[i].real, output[i].imag);
            } else {
                if (output_mag) {
                    printf("%d: %.6f\n", i, magnitude(output[i]));
                }
                if (output_phase) {
                    printf("%d_phase: %.6f\n", i, phase(output[i]));
                }
                if (!output_mag && !output_phase) {
                    printf("%d: %.6f %.6f\n", i, output[i].real, output[i].imag);
                }
            }
        }
    }

    free(input);
    free(output);
    return 0;
}