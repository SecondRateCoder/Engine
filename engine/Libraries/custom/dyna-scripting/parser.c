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

FILE **group;
uint8_t num_in_group;

char *scriptobj_name;
char *prepend;

int main(int argc, char **argv){
    if(argc < 1){return EXIT_FAILURE;}
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
                }
            }
        }
    }
}