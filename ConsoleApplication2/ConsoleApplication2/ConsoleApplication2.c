#include <stdio.h>
#include <fcntl.h>
#include "port.h"
#include "stdint.h"  

#define MY_BUFFER_SIZE (1024*1024)//velicina bafera = 1M
#define HASHLENGTH  4096 //hes tabela sa 4096 particija
#define HASHDEPTH     32 //32 pokazivača na ulaz
#define CONTEXT        2 //dva najnovija simbola u baferu pretrage
#define MEDMATCH       8 /* Expressible lengths are [2..MEDMATCH,MAXMATCH]. */
#define MAXMATCH      16 //duzina podudaranja, tabela 3.17
#define NOSHIFTTHRESH  4 /* This or longer match and we don't shift  */

main(argc, argv)
int argc;
char** argv;
{
    typedef unsigned char* queue[HASHDEPTH];
    /* Element zero is most recent element. */

    unsigned char buf[MY_BUFFER_SIZE + 2000];
    queue hash[HASHLENGTH];

    char* input_file_name;
    int infile;

    unsigned int bits_in = 0;
    unsigned int bits_out = 0;
    if (argc != 2)  //org !=2
    {
        printf("To run lzcontext alg: x <filename>\n");
        exit(0);
    }

    //printf("Welcome to LZCONTEXT\n");
    printf("--------------------\n");
    printf("Buffer size        = %u\n", MY_BUFFER_SIZE);
    printf("Hash table length  = %u\n", HASHLENGTH);
    printf("Hash table depth   = %u\n", HASHDEPTH);
    printf("Context            = %u\n", CONTEXT);
    printf("Med match          = %u\n", MEDMATCH);
    printf("Max match          = %u\n", MAXMATCH);
    printf("No shift threshold = %u\n", NOSHIFTTHRESH);

    input_file_name = argv[1];  //org
    //input_file_name = "C:\\Users\\38165\\Desktop\\MISOS\\ConsoleApplication2\\ConsoleApplication2\\nesto.txt";

    infile = open(input_file_name, O_RDONLY); /* O_BINARY*/
    while (1)
    {
        unsigned int  i, j, k;      //ULONG == unsigned long 
        unsigned char* p_next_byte;
        unsigned long bytes_in;

        /* Read in a block of bytes to compress. */
        bytes_in = read(infile, buf, (unsigned int)MY_BUFFER_SIZE);
        if (bytes_in == 0)
            break;

        /* Initialize the hash table so all entries point to a defined string. */
        for (i = 0; i < HASHLENGTH; i++)
            for (j = 0; j < HASHDEPTH; j++)
                hash[i][j] = (unsigned char*)"0123456789ABCDEF";


        /* Get past the first few context bytes. */
        bits_in += CONTEXT * 8;
        bits_out += CONTEXT * 9;
        p_next_byte = buf + CONTEXT;

        /* Loop once for each item. */
        /* The end is sloppy but this is only approximate so who cares? */
        while (p_next_byte < (buf + bytes_in))
        {
            unsigned long index;
            unsigned int maxlength;  // UWORD == unsigned int
            unsigned char** this_queue;
            unsigned char* old_p_next_byte = p_next_byte;
            unsigned long longest_match_index;

            index = (*(p_next_byte - 2) << 8) ^ (*(p_next_byte - 1));
            index = (40543 * index) >> 4;
            index &= 0xFFF;
            this_queue = &hash[index][0];

            /* Find the longest match in the 16 recent positions.           */
            /* Note: Because we are only calculating entropy here, we don't */
            /* actually need to know the number of the best position.       */
            maxlength = 0;
            for (j = 0; j < HASHDEPTH; j++)
            {
                unsigned char* p = this_queue[j];
                for (k = 0; k < MAXMATCH; k++)
                    if (p_next_byte[k] != p[k])
                        break;
                if (k > maxlength)
                {
                    maxlength = k;
                    longest_match_index = j;
                }
            }

            /* Both literals and copies output one control bit and eight
               data bits. They differ on how much input they consume. */
            if (maxlength == 0) maxlength = 1;
            if (maxlength > MEDMATCH && maxlength < MAXMATCH) maxlength = MEDMATCH;

            p_next_byte += maxlength;
            bits_in += 8 * maxlength;
            bits_out += 9;

            /* If there was a match of 2 or greater, swap the matching
               element with the one half the distance to the head. */
            if (maxlength > 1)
            {
                unsigned char* t;
                unsigned long half = longest_match_index / 2;
                t = this_queue[half];
                this_queue[half] = this_queue[longest_match_index];
                this_queue[longest_match_index] = t;
            }

            /* Shift the queue and put the new value in the recent end. */
            if (maxlength < NOSHIFTTHRESH)
            {
                for (j = HASHDEPTH - 1; j > 0; j--)
                    this_queue[j] = this_queue[j - 1];
                this_queue[0] = old_p_next_byte;
            } 
        }
    }

    close(infile);
    printf("Bytes in  = %lu.\n", bits_in / 8);
    printf("Bytes out = %lu.\n", bits_out / 8);
    printf("Percentage remaining=%5.1f\n", 100.0 * ((float)bits_out) / ((float)bits_in));

    printf("Compressed rate=%5.1f\n", 100 - (100.0 * ((float)bits_out) / ((float)bits_in)));


    //IZ BITS_OUT U TEXT FILE! bits_out je samo broj koji broji bitove... treba buffer/array
    FILE* fp = fopen("output.txt", "w");  //w ili wb isto je
    
    char str[sizeof(bits_out)]; 
    sprintf(str, "%u", bits_out);

    fputs(str, fp);
    fclose(fp);
}

void error(mess)
/* Prints out the string and terminates. */
char mess[];
{
    printf("%s\n", mess);
    exit(0);
}