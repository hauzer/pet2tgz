#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MD5_STR_LEN 32

int main(int argc, char **argv)
{
    FILE *in, *out;
    char *in_s, *out_s;
    struct stat statbuf;
    void *data;
    off_t size;
    int i;

    /* Get arguments */
    if(argc < 5) {
        if(argc > 1 && strcasecmp("--help", argv[1]) == 0) {
            printf("Usage: %s -i infile -o outfile\n", argv[0]);
            return EXIT_SUCCESS;
        }

        return EXIT_FAILURE;
    }
    for(i = 1; i < argc; ++i) {
        if(strcasecmp("-i", argv[i]) == 0) {
            if(i < argc) in_s = argv[i+1];
            else return EXIT_FAILURE;
        }
        else if(strcasecmp("-o", argv[i]) == 0) {
            if(i < argc) out_s = argv[i+1];
            else return EXIT_FAILURE;
        }
    }

    /* Open input file */
    in = fopen(in_s, "r");
    if(in == NULL) {
        fprintf(stderr, "Error: Could not open file \"%s\"\n", in_s);
        return EXIT_FAILURE;
    }

    /* Get input file's size and glob it whole */
    if(stat(in_s, &statbuf) == -1) {
        fprintf(stderr, "Error: Could not stat file \"%s\"\n", in_s);
        return EXIT_FAILURE;
    }
    size = statbuf.st_size - MD5_STR_LEN;

    data = malloc(size);
    fread(data, size, 1, in);
    if(ferror(in)) {
        fprintf(stderr, "Error: Reading from \"%s\"\n", in_s);
        return EXIT_FAILURE;
    }

    if(fclose(in) == EOF) fprintf(stderr,"Warning: Could not close file \"%s\"\n", in_s);

    /* Open output file */
    out = fopen(out_s, "w");
    if(out == NULL) {
        fprintf(stderr, "Error: Could not open file \"%s\"\n", out_s);
        return EXIT_FAILURE;
    }

    /* Cut off the last 32 bytes from the data and write it down to the new one */
    fwrite(data, size, 1, out);
    if(ferror(out)) {
        fprintf(stderr, "Error: Writing to \"%s\"\n", out_s);
        return EXIT_FAILURE;
    }

    if(fclose(out) == EOF) fprintf(stderr,"Warning: Could not close file \"%s\"\n", out_s);

    /* Clean up */
    free(data);

    return EXIT_SUCCESS;
}
