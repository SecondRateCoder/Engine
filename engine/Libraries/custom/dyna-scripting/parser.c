// SciptableObject Handler

/// @brief
/// This expects .c files in the format:
/// Default:
/// (ScriptableObject."<NAME>"){
///		 A function called when the engine first begins setting itself up      
///		void Awake(){
///          ...
///      }
///		
///		 The function called after set-up of the engine.
///		void Start(){
///			...
///		}
///		
///		 The function called every frame
///		void Frame(){
///			...
///		}
///		
///		 An function called when the GameObject is called to react to some event
///		void Update(){
///			...
///		}
///		
///		 The function called upon whenever the attached 
///		void OnDisable(){
///			...
///		}
/// }
///
/// It can properly handle .h files as normal
/// Global properties can be defined as <TYPE>(*)(<ATTRIBUTES#0>, <ATTRIBUTES#1>, <ATTRIBUTES#2>, ..., <ATTRIBUTES#N>)<NAME>;
/// Where <ATTRIBUTES> can be:
///     SERIALIZE: This exposes the symbol so it can be accessed from outside the engine or other ScriptableObjects.
///     If has SERIALISED:
///         PRIORITY: This can be "HIGH", "LOW", <INTEGER>, where "HIGH" priorities are added 1st, "LOW" priority added last and <INTEGER> added at the index.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

FILE **files;
uint8_t num_files;
uint8_t *processed_files;
uint8_t num_processed;

char *path_out;
char *path_cwd;

FILE **group;
uint8_t num_in_group;

char *scriptobj_name;
char *prepend;

int main(int argc, char **argv){
    if(argc < 1){return EXIT_FAILURE;}
    cwd_init();
    
    files = calloc(argc, sizeof(FILE *));
    for(int cc = 0; cc < argc; ++cc){
        files[num_files] = fopen(argv[cc], "rb");
        if(files[cc]){num_files++;}
        fseek(files[num_files - 1], 0, SEEK_SET);
    }
    files = realloc(files, sizeof(FILE *) * num_files);
    group = calloc(num_files, sizeof(FILE *));

    char *buffer = calloc(100, sizeof(char));
    for(uint8_t cc =0; cc < num_files; ++cc){
        bool accept_ = true;
        for(uint8_t cc_ = 0; cc_ < num_processed; ++cc_){if(cc == processed_files[cc_]){accept_ = false;}}
        if(!accept_){continue;}else{
            fread(buffer, sizeof(char), 100, files[cc]);
            if(!strncmp(buffer, "(ScriptableObject.", 18)){
                // Is valid file
                if(scriptobj_name){
                    if(strncmp(scriptobj_name, buffer + 18, strlen(scriptobj_name))){group[num_in_group] = files[cc];}
                }else{
                    uint16_t scriptobj_name_len = 19;
                    for(; buffer[scriptobj_name_len]!='\n'&&buffer[scriptobj_name_len]!='\"'&&scriptobj_name_len<100; ++scriptobj_name_len){}
                    if(scriptobj_name_len == 19){continue;}
                    scriptobj_name = calloc(scriptobj_name_len, sizeof(char));
                    memcpy(scriptobj_name, buffer + 18, scriptobj_name_len);
                }
            }
        }
    }
    return EXIT_SUCCESS;
}

bool env_prepare(){
    if(!path_cwd){cwd_init();}
    if(scriptobj_name){
        path_out = strdup(path_cwd);
        path_out = realloc(path_out, (strlen(path_cwd) + strlen(scriptobj_name) + 2);
        path_out[strlen(path_cwd)] = "\\";
        memcpy(path_out + strlen(path_cwd) + 1, scriptobj_name, strlen(scriptobj_name));
        path_out[strlen(path_cwd) + strlen(scriptobj_name) + 1] = "\\";
        prepend = calloc(strlen(scriptobj_name), sizeof(char));
        snprintf(prepend, "%zu", idk_hash(scriptobj_name, strlen(scriptobj_name)));
    }
}

typedef struct TOKEN_t{
    uint8_t FILE_, segment;    unsigned long offset;
    uint8_t SERIALISED_, PRIORITY_;
}TOKEN_t;

TOKEN_t *tokens;

void tokens_init(FILE *file, char *buffer){
    if(!buffer){buffer = calloc(100, sizeof(char));}
    fseek(file, 19 + strlen(scriptobj_name), SEEK_SET);
    uint8_t cc = fread(buffer, sizeof(char), 100, file);
    tokens = calloc(20, sizeof(TOKEN_t));
    uint16_t size_ = 20, used_ = 0;
    if(cc == 100){
        // Continue
    }
    
}



uint64_t idk_hash(char *s, size_t len){
    uint64_t out = djb33_hash(s, len);
    out << 32;
    out +=h31_hash(s, len);
    out |= fnv32_hash(s, len);
    out += out << 32;
    out != ejb_hash(s, len);
    out = (((out >> 32) + out) | ejb_hash(s, len));
    return out;
}

//! Taken from: "https://gist.github.com/sgsfak/9ba382a0049f6ee885f68621ae86079b"
/*
 * The Dan Bernstein popuralized hash..  See
 * https://github.com/pjps/ndjbdns/blob/master/cdb_hash.c#L26 Due to hash
 * collisions it seems to be replaced with "siphash" in n-djbdns, see
 * https://github.com/pjps/ndjbdns/commit/16cb625eccbd68045737729792f09b4945a4b508
 */
uint32_t djb33_hash(const char* s, size_t len)
{
    uint32_t h = 5381;
    while (len--) {
        /* h = 33 * h ^ s[i]; */
        h += (h << 5);  
        h ^= *s++;
    }
    return h;
}


/*
 *
 * The Java hash, but really no-one seems to know where it came from, see
 * https://bugs.java.com/bugdatabase/view_bug.do?bug_id=4045622
 */
uint32_t h31_hash(const char* s, size_t len)
{
    uint32_t h = 0;
    while (len) {
        h = 31 * h + *s++;
        --len;
    }
    return h;
}

/*
 * The FNV Hash, or more precisely the "FNV-1a alternate algorithm"
 * See: http://www.isthe.com/chongo/tech/comp/fnv/
 *      https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
 */
uint32_t fnv32_hash(const char *str, size_t len)
{
    unsigned char *s = (unsigned char *)str;	/* unsigned string */

    /* See the FNV parameters at www.isthe.com/chongo/tech/comp/fnv/#FNV-param */
    const uint32_t FNV_32_PRIME = 0x01000193; /* 16777619 */

    uint32_t h = 0x811c9dc5; /* 2166136261 */
    while (len--) {
        /* xor the bottom with the current octet */
        h ^= *s++;
        /* multiply by the 32 bit FNV magic prime mod 2^32 */
        h *= FNV_32_PRIME;
    }

    return h;
}


/*
 * "This came from ejb's hsearch."
 */
uint32_t ejb_hash(const char *s, size_t len)
{
	unsigned char *key = (unsigned char*) s;
	const uint32_t PRIME1 = 37;
	const uint32_t PRIME2 = 1048583;
	uint32_t h = 0;

	while (len--) {
		h = h * PRIME1 ^ (*key++ - ' ');
	}
	h %= PRIME2;

	return h;
}

/*
 * Bob Jenkins "One-at-a-time" hash
 */
uint32_t oat_hash(const char *s, size_t len)
{
    unsigned char *p = (unsigned char*) s;
    uint32_t h = 0;

    while(len--) {
        h += *p++;
        h += (h << 10);
        h ^= (h >> 6);
    }

    h += (h << 3);
    h ^= (h >> 11);
    h += (h << 15);

    return h;
}
