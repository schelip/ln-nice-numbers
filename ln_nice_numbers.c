#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])
#define FILE_NAME "results.csv"

typedef union {
    float f;
    unsigned long i;
} float_bits_t;

typedef struct {
    float key;
    float value;
} key_value_pair_t;

typedef struct {
    key_value_pair_t *data;
    size_t size;
} ordered_dict_t;

ordered_dict_t create_nice_numbers_lookup_table() {
    key_value_pair_t custom[] = {
        {256.0f, logf(256.0f)},
        {16.0f, logf(16.0f)},
        {4.0f, logf(4.0f)},
        {2.0f, logf(2.0f)},
        {3.0f / 2.0f, logf(3.0f / 2.0f)},
        {5.0f / 4.0f, logf(5.0f / 4.0f)},
        {9.0f / 8.0f, logf(9.0f / 8.0f)},
        {17.0f / 16.0f, logf(17.0f / 16.0f)},
        {33.0f / 32.0f, logf(33.0f / 32.0f)},
        {65.0f / 64.0f, logf(65.0f / 64.0f)},
        {129.0f / 128.0f, logf(129.0f / 128.0f)}
    };
    ordered_dict_t result;
    result.size = ARRAY_SIZE(custom);
    result.data = malloc(result.size * sizeof(key_value_pair_t));
    if (!result.data) {
        fprintf(stderr, "Failed to allocate memory for lookup table\n");
        exit(1);
    }
    memcpy(result.data, custom, result.size * sizeof(key_value_pair_t));
    return result;
}

float get_k_0(float x, float* keys, int num_keys, size_t* idx_k) {
    int k = 0;
    while ((1 << k) <= (int)x) k++;
    k = (float)(1 << k);
    *idx_k = -1;
    while (1) {
        for (size_t i = 0; i < num_keys; i++)
            if (k == keys[i]) {
                *idx_k = i;
                break;
            }
        if (*idx_k != -1) break;
        else k <<= 1;
    }
    return k;
}

float nice_multiply(float k, float x) {
    float_bits_t fi_k, fi_x;
    fi_k.f = k;
    
    int i = (int)k; // obtém o inteiro correspondente a k
    float result;
    
    if (k == (float)i) { // k é da forma 2^n e n > 0
        int n = ((fi_k.i >> 23) & 0xff) - 127;
        result = ldexp(x, n); // multiplica x por 2^n usando bitshift
    } else { // k é da forma 2^n + 1 e n < 0
        fi_k.f -= 1.0;
        int n = ((fi_k.i >> 23) & 0xff) - 127;
        result = ldexp(x, n); // multiplica x por 2^n usando bitshift
        result += x; // adiciona x mais uma vez
    }
    
    return result;
}

float ln_nice_numbers(float x, ordered_dict_t lookup_table) {
    size_t num_keys = lookup_table.size;
    float* keys = malloc(num_keys * sizeof(float));
    float* values = malloc(num_keys * sizeof(float));
    if (!keys || !values) {
        fprintf(stderr, "Failed to allocate memory for keys and values arrays\n");
        exit(1);
    }
    for (size_t i = 0; i < num_keys; i++) {
        keys[i] = lookup_table.data[i].key;
        values[i] = lookup_table.data[i].value;
    }
    size_t idx_k;
    float k_0 = get_k_0(x, keys, num_keys, &idx_k);
    float x_j = x / k_0;
    float y_j = values[idx_k];
    int j = 0;
    while (1) {
        int found = 0;
        for (size_t i = idx_k + 1; i < num_keys; i++) {
            float m = nice_multiply(keys[i], x_j);
            if (m < 1.0f) {
                found = 1;
                x_j = m;
                y_j -= values[i];
                idx_k = i;
                printf("Iter: %d, Xj: %0.20f, Yj: %0.20f, K: %0.10f\n", j++, x_j, y_j, keys[i]);
                break;
            }
        }
        if (!found) break;
    }
    free(keys);
    free(values);
    return y_j - fabsf(1.0f - x_j);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s [--x <x> | --upper <upper>]\n", argv[0]);
        return 1;
    }

    ordered_dict_t lookup_table = create_nice_numbers_lookup_table();

    if (strcmp(argv[1], "--x") == 0) {
        float x = atof(argv[2]);
        float result = ln_nice_numbers(x, lookup_table);
        float error = fabsf(result - logf(x));
        printf("ln_nice_numbers(%f) = %f (error: %g)\n", x, result, error);
    } else if (strcmp(argv[1], "--upper") == 0) {
        int upper = atoi(argv[2]);
        FILE *fp;
        errno_t error = fopen_s(&fp, FILE_NAME, "w"); // Abrir arquivo CSV para escrita no modo de acrescentamento
        if (error != 0) {
            printf("Error opening file to write.\n");
            return 1;
        }
        fprintf(fp, "Value,Result,Error\n");
        for (int i = 1; i <= upper; i++) {
            float x = (float)i;
            float result = ln_nice_numbers(x, lookup_table);
            float error = fabsf(result - logf(x));
            fprintf(fp, "%d,%f,%g\n", i, result, error);
        }
        fclose(fp);
    } else {
        fprintf(stderr, "Usage: %s [--x <x> | --upper <upper>]\n", argv[0]);
        return 1;
    }

    free(lookup_table.data);

    return 0;
}