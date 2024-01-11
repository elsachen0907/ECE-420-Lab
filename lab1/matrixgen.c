/*
Generate two same sized square matrices in which the every element is uniformly randomly selected integer within some absolute value

-----
Compiling:
    > gcc matrixgen.c -o matrixgen

-----
Synopsis:
    matrixgen [-sbpo]

-----
Options:
    -s    specify the size of the matrices (default 10) 
    -b    upper bound of the absolute value of the elements (default 5)
    -p    print the result
    -o    specify the output path (default "./data_input") 

-----
Outputs:
    Output file:
    data_input: storing the two generated matrices

-----
Error returns:
    -1    unexpected options
    -2    fail to open file 

-----
Example:
    >matrixgen 
    generate two 10 by 10 matrices with elements between -5 and 5, and store the result in "./data_input"
   
    >matrixgen -s 100 -b 10 -p
    generate two 100 by 100 matrices with elements between -10 and 10, print the result and store it in "data_input"
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


int main (int argc, char* argv[])
{
    int size = 10, bound = 5;
    char* OUTPATH = "data_input";
    int b_print = 0;    //switch for the print
    int option;
    int temp,i,j;
    FILE *op;

    while ((option = getopt(argc, argv, "s:b:po:")) != -1)
        switch(option)
        {
            case 's': size = strtol(optarg, NULL, 10); break;
            case 'b': bound = strtol(optarg, NULL, 10); ; break;
            case 'p': b_print = 1; break;
            case 'o': OUTPATH = optarg; break;
            case '?': printf("Unexpected Options. \n"); return -1;
        }
    if ((op = fopen(OUTPATH,"w")) == NULL)
    {
        printf("Cant open a file!/n");
        return -2;
    }
		
    srand(time(NULL));
    fprintf(op,"%d\n\n", size);
    for (i = 0; i < size; i++)
    {	
        for (j = 0; j < size; j++)
        {
            temp = rand() % (2 * bound) - bound + 1;
            fprintf(op,"%d\t",temp);
        }
        fprintf(op,"\n");
    }
    fprintf(op,"\n");	
    for (i = 0; i < size; i++)
    {	
        for (j = 0; j < size; j++)
        {
            temp = rand() % (2 * bound) - bound + 1;
            fprintf(op,"%d\t",temp);
        }
        fprintf(op,"\n");
    }
    fclose(op);
    if (b_print)
    {
        if ((op = fopen(OUTPATH,"r"))== NULL)
        {
            printf("Cant open a file!\n");		
            return -2;
        }
        fscanf(op,"%d\n",&size);
        printf("The matrix size is %d\n", size);
        printf("=====================================\n");
        printf("Matrix A is \n");			
        for (i = 0; i < size; i++)
        {
            for (j = 0; j < size; j++)
            {
                fscanf(op,"%d\t",&temp);
                printf("%d\t",temp);
            }
            printf("\n");
        }
        printf("=====================================\n");
        printf("Matrix B is \n");			
        for (i = 0; i < size; i++)
        {
            for (j = 0; j < size; j++)
            {
                fscanf(op,"%d\t",&temp);
                printf("%d\t",temp);
            }
            printf("\n");
        }
        fclose(op); 
        }
	return 0;
}
