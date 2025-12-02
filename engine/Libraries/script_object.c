// Example Script Object .c file
"(ScriptObject.(\"<NAME>\")){"
	"void Setup(){"
	"..."  
	"}"
	"void Awake(){"
	"..."
	"}"
	"void Update(){"
	"..."
	"}"
	"void Disable(){"
	"..."
	"}"
	"void Close()"
	"..."
	"}"
"}"

FILE **f_inputs;
size_t num_inputs;
void tokens_open(char **paths, uint8_t num_paths){
	f_inputs = calloc(num_paths, sizeof(FILE *));
	uint8_t counter = 0;
	for(uint8_t cc =0; cc < num_paths; ++cc){
		f_inputs[counter] = fopen(paths[cc]);
		if(f_inputs[counter]){counter++;}
	}
}
