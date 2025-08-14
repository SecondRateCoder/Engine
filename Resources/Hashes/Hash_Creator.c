#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


//! ./Compilation-Scripts/Make.ps1 -SourceDirectory "./Resources\Hashes\Hash_Creator.c" -OutputExecutableName "hasher.exe"
typedef size_t uint128_t[2];

#define HASH_64BIT_LIMIT 12

// size_t *str_hash_(const char *str);

static const char* builtin_shader_typenames[38] = {
    "bool", "int", "unsignedint", "float", "vec2", "vec3", "vec4",
    "ivec2", "ivec3", "ivec4", "uvec2", "uvec3", "uvec4",
    "bvec2", "bvec3", "bvec4", "mat2", "mat3", "mat4",
    "mat2x3", "mat3x2", "mat2x4", "mat4x2", "mat3x4", "mat4x3",
    "sampler1D", "sampler2D", "sampler3D", "samplerCube",
    "sampler1DShadow", "sampler2DShadow", "sampler2DArray",
    "sampler2DArrayShadow", "isampler1D", "isampler2D",
    "usampler1D", "usampler2D", "\0"};

/// @brief Return a __uint128_t variable.
/// @param str The string to be hashed.
/// @return A __uint128_t value.
size_t *str_hash_(const char *str){
    // A large, odd prime number is a good choice for the initial hash value.
    // 5381 is a common value used in the DJB2 algorithm.
    size_t *hash = malloc(sizeof(size_t)* 2), cc =0;
    hash[0] = 0;hash[1] = 0;
    int c;

    // A simple loop to iterate through the string until the null terminator is found.
    while ((c = *str++)) {
        // This is the core of the DJB2 algorithm:
        // hash = hash * 33 + c;
        // The bitwise left shift `(hash << 5)` is an efficient way to do `hash * 32`.
        // Then we add the original hash to get `hash * 33`.
        ++cc;
        if(hash[0] >= (SIZE_MAX-10)){
            hash[1] = ((hash[1] << 5) + hash[1]) + c;
        }else{hash[0] = ((hash[0] << 5) + hash[0]) + c;}
    }
    return hash;
}

int main(){
    FILE *hashes = fopen("C:/Users/olusa/OneDrive/Documents/GitHub/Engine/Resources/Hashes/hashes.txt", "w");
    for(size_t cc =0; builtin_shader_typenames[cc] != NULL; ++cc){
        size_t *print = str_hash_(builtin_shader_typenames[cc]);
        char *temp = malloc(sizeof(char)* 100);
        // fwrite(print, sizeof(size_t), 2, hashes);
        snprintf(temp, sizeof(size_t)* 20, "#0: %zu, ", print[0]);
        snprintf(&temp[50], sizeof(size_t)* 20, "#1: %zu", print[1]);
        fwrite(temp, sizeof(char), 100, hashes);
        free(print);
        free(temp);
    }
    return EXIT_SUCCESS;
}
