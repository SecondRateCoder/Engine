#include "./Public.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

char *str_normalise(char *str, bool handle_spaces, bool handle_upper){
    if (str == NULL){return  NULL;}
	size_t str_len= strlen(str);
    int i = 0, j = 0;
	char *out = malloc(sizeof(char)* strlen(str));
    // Iterate through the string until the null terminator is reached.
    while (str[i] != '\0') { 
        // Check if the current character is NOT a space
        if (IS_SPACE((unsigned char)str[i]) && handle_spaces){
			//If space write the next value.
			if(i < str_len-1){
				++i;
				out[j] = str[i];
			}
        }else if(handle_upper){
            // Convert to lowercase and copy to the new position
            out[j] = tolower((unsigned char)str[i]);
			i++;
		}
		j++;
    }
    str[j] = '\0'; // Null-terminate the modified string
    return str;
}

#define HASH_64BIT_LIMIT 12

/// @brief Return a __uint128_t variable.
/// @param str The string to be hashed.
/// @return A __uint128_t value.
size_t *str_hash_(const char *str){
	// A large, odd prime number is a good choice for the initial hash value.
    // 5381 is a common value used in the DJB2 algorithm.
    uint128_t hash = 5381;
	size_t cc =0;
    int c;

    // A simple loop to iterate through the string until the null terminator is found.
    while ((c = *str++)) {
        // This is the core of the DJB2 algorithm:
        // hash = hash * 33 + c;
        // The bitwise left shift `(hash << 5)` is an efficient way to do `hash * 32`.
        // Then we add the original hash to get `hash * 33`.
		++cc;
		if(cc > HASH_64BIT_LIMIT){
			hash[1] = ((hash[1] << 5) + hash[1]) + c;
		}else{hash[0] = ((hash[0] << 5) + hash[0]) + c;}
    }
    return hash;
}

char tolower(char c) {
    // Check if the character is an uppercase letter (A-Z)
    if (c >= 'A' && c <= 'Z') {
        // If it is, convert it to lowercase by adding the difference in ASCII values
        // For example, 'a' - 'A' = 32. So 'B' + 32 = 'b'.
        return c + ('a' - 'A');
    }
    // If the character is not an uppercase letter, return it unchanged.
    return c;
}

/**
 * @brief Converts an entire string to lowercase.
 *
 * @param str A pointer to the null-terminated string to be converted.
 */
void str_tolower(char *str) {
    // Iterate through the string until the null terminator is reached.
    for (int i = 0; str[i] != '\0'; i++) {
        // Use the my_tolower function to convert each character.
        str[i] = tolower(str[i]);
    }
}

bool uint128_t_comps(uint128_t a, size_t b){return a[1] == b;}
bool uint128_t_comp(uint128_t a, uint128_t b){return a[0] == b[0] && a[1] == b[1];}