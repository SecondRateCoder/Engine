#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

typedef size_t uint128_t[2];

#define HASH_64BIT_LIMIT 12
#define IS_NUM(c)    (((c) >= '0') && ((c) <= '9'))
#define IS_EVEN(x)   (((x) % 2) == 0)
#define INT_SIMP(x)  (((x) < 0) ? -1 : 1)
#define IS_SPACE(c)  ((c) == ' ')

static const char* builtin_shader_typenames[38] = {
    "bool", "int", "unsignedint", "float", "vec2", "vec3", "vec4",
    "ivec2", "ivec3", "ivec4", "uvec2", "uvec3", "uvec4",
    "bvec2", "bvec3", "bvec4", "mat2", "mat3", "mat4",
    "mat2x3", "mat3x2", "mat2x4", "mat4x2", "mat3x4", "mat4x3",
    "sampler1D", "sampler2D", "sampler3D", "samplerCube",
    "sampler1DShadow", "sampler2DShadow", "sampler2DArray",
    "sampler2DArrayShadow", "isampler1D", "isampler2D",
    "usampler1D", "usampler2D", NULL
};

char* str_normalise(const char *str, bool handle_spaces, bool handle_upper) {
    if (!str) return NULL;

    size_t str_len = strlen(str);
    char *out = malloc(str_len + 1);
    if (!out) return NULL;

    size_t i = 0, j = 0;
    while (str[i] != '\0') {
        char c = str[i];
        if (handle_spaces && IS_SPACE(c)) {
            i++;
            continue;
        }

        if(handle_upper){c = (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;}
        out[j++] = c;
        i++;
    }

    out[j] = '\0';
    return out;
}

size_t* str_hash(const char *str) {
    if(!str){return NULL;}
    size_t *hash = malloc(sizeof(uint128_t));
    if(!hash){return NULL;}
    hash[0] = 5381;
    hash[1] = 0;
    size_t cc = 0;
    int c;
    while((c = *str++)){
        ++cc;
        if(cc > HASH_64BIT_LIMIT){hash[1] = ((hash[1] << 5) + hash[1]) + c;
        }else{hash[0] = ((hash[0] << 5) + hash[0]) + c;}
    }
    return hash;
}

int main() {
    FILE *hashes = fopen("C:/Users/olusa/OneDrive/Documents/GitHub/Engine/Resources/Hashes/hashes.txt", "w");
    if (!hashes) {
        fprintf(stderr, "Failed to open output file.\n");
        return EXIT_FAILURE;
    }

    for (size_t cc = 0; builtin_shader_typenames[cc] != NULL; ++cc) {
        char *normalized = str_normalise(builtin_shader_typenames[cc], false, true);
        size_t *hash = str_hash(normalized);
        free(normalized);

        if (!hash){continue;}

        fprintf(hashes, "#0: %zu, #1: %zu\n", hash[0], hash[1]);
        free(hash);
    }
    fclose(hashes);
    return EXIT_SUCCESS;
}

