/*
The script to fetch the subset of the original data and add links to the node with no out going links The index is remapped to make it continuous in the resulting subset of data. Index start from 0.

As for the output file, first line is the number of the included nodes, the following lines indicating the directed links with the first number as the index of the source node and the second number as the index of the destination node.

-----
Compiling:
    > gcc datatrim.c -o datatrim

-----
Synopsis:
    datatrim [-bion]

-----
Options:
    -b    specify the upper bound index to be included in the original data (default 5300, generating data with 1112 nodes)
    -i    specify the input path (default "./web-Stanford.txt")
    -o    specify the output path prefix (default "./data_input") 
    -n    tag to shut down the auto link addition for the nodes that have no out going links

-----
Outputs:
    Output files:
    data_input_link:    the directed links with the first number as the index of the source node and the second number as the index of the destination node.
    data_input_meta:    first line indicating the number of the nodes, the following lines indicating the node index, number of incoming links, number of outgoing links. 

-----
Error returns:
    -1    unexpected options
    -2    fail to open files 
    1     upper bound (-b) too small

-----
Example:
    >datatrim
    fetch the graph from the original, add links for the nodes with no out going links and store the result in "./data_input", 
   
    >datagen -b 10000 -n
    fetch the graph with index less than 10000 and store it in "data_input"

Source data is from:
http://snap.stanford.edu/data/web-Stanford.html
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char* argv[]){
    int option;
    int b_extend = 1;
    char *INPATH = "web-Stanford.txt";
    char *OUTPATH = "data_input";
    int BOUND = 5300, src, dst;
    FILE *fp_ori, *fp_dest;
    int *flag, *num_out_links, *num_in_links;
    int Ecount = 0;
    int Ncount = 0;
    int Noncount = 0;
    int i,j;
    char* tempstore;
    char outpath_link[100], outpath_meta[100];

    while ((option = getopt(argc, argv, "b:i:o:n")) != -1)
        switch(option){
            case 'b': BOUND = strtol(optarg, NULL, 10); break;
            case 'i': INPATH = optarg; break;
            case 'o': OUTPATH = optarg; break;
            case 'n': b_extend = 0; break;
            case '?': return -1;
        }
    if ((fp_ori = fopen(INPATH,"r")) == NULL){
        printf("Fail to open the source data file. \n");
        return -2;
    } 
    strcpy(outpath_link, OUTPATH);
    strcat(outpath_link, "_link");
    strcpy(outpath_meta, OUTPATH);
    strcat(outpath_meta, "_meta");

    flag = malloc(BOUND*sizeof(int));
    tempstore = malloc(100*sizeof(char));

    // find out with nodes are engaged with the current bound
    for (i = 0; i < BOUND; ++i)
        flag[i] = 0;     
    for (j = 0; j < 4; ++j){ // ignore the firt few lines
        fgets(tempstore, 100, fp_ori);
    }
    while(!feof(fp_ori)){
        fscanf(fp_ori, "%d\t%d\n", &src, &dst);
        if (src < BOUND && dst < BOUND){
            ++Ecount;
            flag[src] = 1;
            flag[dst] = 1;
        }
    }
    for (i = 0; i < BOUND; ++i)
        Ncount+=flag[i];
    if (Ncount == 0){
        printf("Upper limit too small and no nodes are engaged. Try larger upper bound. \n");
        exit(1);
    }
    // remap the node index 
    j=0;
    if (BOUND == 1)
        --flag[0];
    else{
        for (i = 0; i< BOUND; ++i)
            if (flag[i]){
                flag[i] = j++;
            }else{
                flag[i] = -1; //tag to indicate that the current edge is not engaged.
            }
    }
    // Get the new link file and save the output to data_input_link
    rewind(fp_ori);   
    for (j = 0; j < 4; ++j){// ignore the first few lines
        fgets(tempstore, 100, fp_ori);
    }
    free(tempstore);
    if ((fp_dest = fopen(outpath_link,"w")) == NULL){
        printf("Fail to open the output file %s. \n", outpath_link);
        return -2;
    }
    if (b_extend){ 
        num_out_links = malloc(Ncount * sizeof(int));
        for (i = 0; i < Ncount; ++i) num_out_links[i] = 0;
    }
    while(!feof(fp_ori)){
        fscanf(fp_ori, "%d\t%d\n", &src, &dst);
        if (src < BOUND && dst < BOUND){
            fprintf(fp_dest, "%d\t%d\n", flag[src], flag[dst]);
            if (b_extend) ++num_out_links[flag[src]];
        }
    }
    if (b_extend){
        for (i = 0; i < Ncount; ++i)
            if (num_out_links[i] == 0){
                ++Noncount;
                for (j = 0; j < BOUND; ++j)
                    if (flag[j] >= 0)
                        fprintf(fp_dest, "%d\t%d\n", i, flag[j]);
            }
        free(num_out_links);   
    }
    fclose(fp_dest);
    fclose(fp_ori);
    // Deal with the meta data
    if ((fp_ori = fopen(outpath_link,"r")) == NULL) {
        printf("Error opening the link file: %s.\n", outpath_link);
        return -2;
    }
    num_in_links = malloc(Ncount * sizeof(int)); 
    num_out_links = malloc(Ncount * sizeof(int)); 
    for (i = 0; i < Ncount; ++i){
        num_in_links[i] = 0;
        num_out_links[i] = 0;
    }
    while(!feof(fp_ori)){
        fscanf(fp_ori, "%d\t%d\n", &src, &dst); 
        ++num_in_links[dst]; ++num_out_links[src];
    } 
    fclose(fp_ori);
    // Save the meta data into data_input_meta
    if ((fp_dest = fopen(outpath_meta,"w")) == NULL){
        printf("Fail to open the output file %s. \n", outpath_meta);
        return -2;
    }
    fprintf(fp_dest, "%d\n", Ncount);
    for (i=0; i<Ncount; ++i){
        fprintf(fp_dest, "%d\t%d\t%d\n", i, num_in_links[i], num_out_links[i]);
    }
    fclose(fp_dest);
    free(num_in_links); free(num_out_links);
    // Display the output
    if (b_extend){
        Ecount += Ncount * Noncount;
        printf("There are %d nodes and %d edges in the sub dataset including the added edges for the nodes with no outgoing links. \n", Ncount, Ecount);
    }
    else
        printf("There are %d nodes and %d edges in the sub dataset. \n", Ncount, Ecount);

    // clean up
    free(flag);
    return 0;
}
