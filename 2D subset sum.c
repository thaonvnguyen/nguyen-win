#include <stdio.h>
#include <stdlib.h>
/**
* Program uses dynamic programming to solve a subset sum problem with
  2 targets, in which the solution set for each will be two different
  disjoint subset of the original set.

  Compile on Omega: gcc lab2.c -o lab2
  Run with data file: lab2 < fileName
*/
int** table;
int * s;
/*Function to backtrace the table and find out the subset solution*/
void backtrace (int n, int target1, int target2){
    int subsetSum1 = target1,subsetSum2 = target2,i =0,j =0;
	int traceUp, traceLeft;
	int trace = table[target2][target1];

	if (trace ==-1){
		printf("No solution\n");
	}
    else{
    int *sequence1 = (int*)malloc(n*sizeof(int));
    int *sequence2 = (int*)malloc(n*sizeof(int));

    while(trace!=0){
    if(subsetSum1-s[trace] >=0){
        traceLeft = table[subsetSum2][subsetSum1-s[trace]];
        if (traceLeft!=-1){
            sequence1[i]= trace;
            i++;
            subsetSum1= subsetSum1-s[trace];
            trace = traceLeft;
            continue;
        }
    }
    if(subsetSum2-s[trace]>=0){
        traceUp = table[subsetSum2-s[trace]][subsetSum1];
        if(traceUp!=-1){
        sequence2[j]= trace;
        j++;
        subsetSum2= subsetSum2-s[trace];
        trace = traceUp;
        }
    }
    }
    printf("subsequence for %d:\n", target1);
    for (subsetSum1 = i-1;subsetSum1>=0;subsetSum1--)
        printf("%d \n",s[sequence1[subsetSum1]]);
    printf("\n");
    printf("subsequence for %d:\n", target2);
    for (subsetSum1 = j-1;subsetSum1>=0;subsetSum1--)
        printf("%d \n",s[sequence2[subsetSum1]]);
    printf("\n");
}}

/*Function to go through every possible target value up to the given
target value and find the minimum subset to give solution for each*/
void subsetSum(int size, int target1, int target2){
    int i,j,k;
    for (i=1;i<=target1;i++){
		table[0][i]=-1;
		for(k=0;k<=size;k++){
			int currentSum = i-s[k];
			if (currentSum>=0 && table[0][currentSum]<k && table[0][currentSum]>=0){
					table[0][i]=k;
					break;
    }}}
    //Take care of the first row and the first column
	for (i=1;i<=target2;i++){
        table[i][0]=-1;
        for(k=0;k<=size;k++){
			int currentSum = i-s[k];
            if(currentSum>=0 && table[currentSum][0]<k && table[currentSum][0]>=0){
                    table[i][0]=k;
                    break;
                }
        }
    }
    //Fill in the rest of the table
    for(i=1; i<=target2; i++){
        for(j=1;j<=target1;j++){
            for(k=0;k<=size;k++){
                table[i][j]=-1;
                int hor = j-s[k];
                int vert = i-s[k];
                if((hor>=0 && table[i][hor]>=0 && table[i][hor]<k)
                ||(vert>=0 && table[vert][j]>=0 && table[vert][j]<k)){
                    table[i][j] = k;
                    break;
                }
            }
        }
    }
}

int main(){
    //Scan parameters
    int i,j,k, size, target1, target2;
    scanf("%d %d %d", &size, &target1, &target2);
    //allocate space for the table
    table = malloc(sizeof(int*)*(target2+1));
    for(i=0;i<=target2;i++){
        int* row = calloc(target1+1,sizeof(int));
        *(table+i) = row;
    }
    //array for values
     s = calloc(size+1, sizeof(int));
     for(i=1;i<size+1;i++){
        scanf("%d", s+i);
    }

    printf("%4s %4s\n", "i", "S");
    printf("---------------\n");
     for(i=1;i<=size;i++){
        printf("%4d %4d \n", i, *(s+i));
    }

    printf("Targets are %d and %d\n", target1, target2);
    subsetSum(size,target1,target2);
    backtrace (size,target1,target2);

    return 0;
}
