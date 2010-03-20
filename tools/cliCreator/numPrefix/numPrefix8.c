#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	if(argc < 4) {
		fprintf(stderr, "usage: %s [in filename] [number] [out filename]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	FILE *input = fopen(argv[1], "rb");
	if( input == NULL) {
		fprintf(stderr, "failed to open file %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	
	FILE *output = fopen(argv[3], "wb");
	if( output == NULL) {
		fprintf(stderr, "failed to create file %s\n", argv[2]);
		exit(EXIT_FAILURE);
	}
	
	int8_t num = atoi(argv[2]);
	fwrite(&num, sizeof(int8_t), 1, output);
	
	int8_t c = 0;
	while( fread(&c, sizeof(int8_t), 1, input) )
	{
		fwrite(&c, sizeof(int8_t), 1, output);
	}
	
	printf("done\n");
	
	fclose(input);
	fclose(output);
	return 0;
}
