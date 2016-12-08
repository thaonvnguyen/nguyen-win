/*
* Name: Thao Nguyen
* ID: 1001142617
* Programming Assignment #3
* Page replacement Algorithms
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXSIZE 255

int input[MAXSIZE];
int pageNum, inputCount=0;
int * pageSize;

//Set all frames of page table to -1
void resetPageTable(){
  int i;
  for(i=0;i<pageNum;i++)
    pageSize[i]= -1;
}
//Print the page table
void printTable(){
  int i;
  for(i=0;i<pageNum;i++)
    printf("%3d  ",pageSize[i]);
  printf("\n");
}

/*
FIFO algorithm
- print page table for each reference input
- return number of page faults when done
*/
int FIFO(){
  int pageFault=0;
  int i,k, FIFO=0, pageReplacementNeeded;
  resetPageTable();

  printf("FIFO OUTPUT:\n");
  for(i=0;i<inputCount;i++) {
    pageReplacementNeeded=1;
    for(k=0;k<pageNum;k++){
      if(pageSize[k]==input[i])
      pageReplacementNeeded=0;
    }
    if (pageReplacementNeeded){
      pageFault++;
      pageSize[FIFO]=input[i];
      FIFO=(FIFO+1)%pageNum;
    }
    printTable();
  }
  return pageFault;
}

/*
LRU algorithm
- print page table for each reference input
- return number of page faults when done
*/
int LRU(){
  int * ranking = calloc(pageNum,sizeof(int));
  int rank=0, pageFault=0;
  int i,k, LRU, pageReplacementNeeded, minRank;
  resetPageTable(); //clear page table to -1

  printf("LRU OUTPUT:\n");
  for(i=0;i<inputCount;i++) {
    minRank=ranking[0];
    LRU = 0;
    //Determine targeted LRU page
    for(k=0;k<pageNum;k++)
    if(ranking[k]<minRank){
      minRank=ranking[k];
      LRU=k;
    }
    //Dertermine if pageReplacement is needed
    pageReplacementNeeded=1;
    for(k=0;k<pageNum && pageReplacementNeeded;k++){
      if(pageSize[k]==input[i]){ //already in page table, update rank
        ranking[k]=++rank;
        pageReplacementNeeded=0;
      }else if(pageSize[k]==-1){ //unused page frame, use it
        pageFault++;
        pageSize[k]= input[i];
        ranking[k]=++rank;
        pageReplacementNeeded=0;
      }
    }
    if (pageReplacementNeeded){
      pageFault++;
      pageSize[LRU]=input[i];
      ranking[LRU]=++rank;
    }
    printTable();
  }
  return pageFault;
}

/*
LFU algorithm
- print page table for each reference input
- return number of page faults when done
*/
int LFU(){
  //set up a map where each frameValue maps to a frequency
  int* frequency= calloc(MAXSIZE, sizeof(int));
  int pageFault=0;
  int i,k, LFU, pageReplacementNeeded, minFreq;

  resetPageTable(); //clear page table to -1
  printf("LFU OUTPUT:\n");
  for(i=0;i<inputCount;i++) {
    //Determine targeted LFU page
    minFreq = frequency[pageSize[0]];
    LFU=0;
    for(k=0;k<pageNum;k++){
      if(pageSize[k]!=-1 && frequency[pageSize[k]]<minFreq){
        minFreq=frequency[pageSize[k]];
        LFU=k;
      }
    }
    //Dertermine if pageReplacement is needed
    pageReplacementNeeded=1;
    for(k=0;k<pageNum && pageReplacementNeeded;k++){
      if(pageSize[k]==-1){ //unused page frame, use it
        pageFault++;
        pageSize[k]= input[i];
        pageReplacementNeeded=0;
      }else if(pageSize[k]==input[i]){ //already in page table, update age
        pageReplacementNeeded=0;
      }}
      if (pageReplacementNeeded){ //both above cases failed
        pageFault++;
        pageSize[LFU]=input[i];
        frequency[input[i]]=0;
      }
      frequency[input[i]]++;
      printTable();
    }
    return pageFault;
  }

/*
Optimal algorithm
- print page table for each reference input
- return number of page faults when done
*/
int optimalSelection(){
    int pageFault=0;
    int i,k,j, furthest, optimal, pageReplacementNeeded;

    resetPageTable(); //clear page table to -1
    printf("OPTIMAL OUTPUT:\n");
    for(i=0;i<inputCount;i++) {
      furthest = 0;
      optimal = 0;
      //Dertermine if pageReplacement is needed
      pageReplacementNeeded=1;
      for(k=0;k<pageNum && pageReplacementNeeded;k++){
        if(pageSize[k]==-1){ //unused page frame, use it
          pageFault++;
          pageSize[k]= input[i];
          pageReplacementNeeded=0;
        }else if(pageSize[k]==input[i]){ //already in page table, update age
          pageReplacementNeeded=0;
        }}
        if (pageReplacementNeeded){ //both above cases failed, pageFault happens
          pageFault++;
          //Determine targeted optimal page
          for(k=0;k<pageNum;k++){
            int found = 0;
            for(j=i+1;j<inputCount-1 && found!=1;j++){
              if(pageSize[k]==input[j]){
                found=1;
                if(j>furthest){ // if found something better
                  optimal = k;
                  furthest=j;
                }
              }
            }
            if(!found){ //reference not found, use it as it is furthest - infinity
              optimal=k;
              break;
            }
          }
          pageSize[optimal]=input[i];
        }
        printTable();
      }
      return pageFault;
    }

int main(void){
  char strPageNum [3], numLine[1000];
  //Scan first line as the page table size, convert it to integer
  pageNum = atoi(fgets(strPageNum, 3, stdin));
  printf("Page table size: %d \n", pageNum);

  //Scan second line, tokenize all input numbers and put in an array
  fgets(numLine, 1000, stdin);
  printf("Page reference string: %s \n", numLine);
  char * token = strtok(numLine, " ");
  while(token!=NULL && inputCount<256){
    input[inputCount]=atoi(token);
    token = strtok(NULL, " ");
    inputCount++;
  }
  pageSize = calloc(pageNum,sizeof(int));
  //Run each algorithm and print out the returned number of page faults
  printf("Page Faults of FIFO: %d\n\n", FIFO());
  printf("Page Faults of LRU: %d\n\n", LRU());
  printf("Page Faults of LFU: %d\n\n", LFU());
  printf("Page Faults of Optimal: %d\n\n", optimalSelection());
  return 0;
}
