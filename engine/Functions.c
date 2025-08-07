#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define str_hash(str) str_hash_(str)

char *str_normalise(char *str, bool handle_spaces, bool handle_upper){
    if (str == NULL){return;}
	size_t str_len= strlen(str);
    int i = 0, j = 0;
	char *out = malloc(sizeof(char)* strlen(str));
    // Iterate through the string until the null terminator is reached.
    while (str[i] != '\0') { 
        // Check if the current character is NOT a space
        if (isspace((unsigned char)str[i]) && handle_spaces){
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
}

const size_t str_hash_(const char *str){
	// A large, odd prime number is a good choice for the initial hash value.
    // 5381 is a common value used in the DJB2 algorithm.
    size_t hash = 5381;
    int c;

    // A simple loop to iterate through the string until the null terminator is found.
    while ((c = *str++)) {
        // This is the core of the DJB2 algorithm:
        // hash = hash * 33 + c;
        // The bitwise left shift `(hash << 5)` is an efficient way to do `hash * 32`.
        // Then we add the original hash to get `hash * 33`.
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}