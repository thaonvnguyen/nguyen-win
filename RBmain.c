#include <stdlib.h>
#include <stdio.h>
#include "topdownRB.h"

int main(){
Key t;
int op = -1, rank, temp;
int max, min, notOutOfRange;

STinit();  // Initialize red-black tree
verifyRBproperties();
printf("clean\n");

scanf("%d",&op);
while (op){
  switch (op){
    case 1:
        temp = 0;
        scanf("%d",&t);
        //printf("1 %d\n",t);
        CheckDuplicate(t, &temp); //Check duplicate
        if( temp == 0){
            STinsert(t); //If the key is not duplicated, insert it to the tree.
            verifyRBproperties(); //Ensure the tree isn't damaged.
        }else
            printf("Error - This value is already in the tree.\n"); //Error message if the key is duplicated.
        break;
    case 2:
        temp =0, rank =0;
        scanf("%d", &t);
        //printf("2 %d\n",t);
        FindRank(t, &rank, &temp); //Find the rank of the key.
        if(temp == 1)
            printf("Key %d has odd rank %d.\n", t, rank); //Print out the key's rank.
        else
             printf("Key %d is not in tree\n",t); //Error message if no key in the tree.
        break;
    case 3:
        notOutOfRange =0;
        scanf("%d", &rank);
        //printf("3 %d\n",rank);
        //FindLargestSmallest(rank, &max, &min, &flag); //Find Smallest and Largest value of the rank.
        max = STselectSmall(rank, &notOutOfRange);
        min = STselectLarge(rank, &notOutOfRange);
        if(notOutOfRange == 0)
            printf("Range error \n"); //Error message if no such rank.
        else
            printf("Rank %d has first key %d and last key %d.\n", rank, min, max); //Print out the range.
        break;
    case 4:
      //printf("4\n");
      verifyRBproperties(); //Ensure the tree isn't damaged.
      printf("clean\n");
      break;
    default:
      printf("Bad operation %d\n",op);
  }
   scanf("%d",&op);
}
    printf("0\n");

    return 0;
}
