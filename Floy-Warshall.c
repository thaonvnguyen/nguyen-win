#include <stdio.h>
#include <stdlib.h>
#define SIZE 10
#define CEILING 999

int dist[SIZE][SIZE], n;
int Odddist[SIZE][SIZE], Oddsucc[SIZE][SIZE], Evendist[SIZE][SIZE], Evensucc[SIZE][SIZE];
int newDist;

void initialoddMatrix(){/*Initialize Odd Matrix*/
    int i,j,k;
    while (1){
        scanf("%d %d %d",&i,&j,&k);
        if (i==(-1))
            break;
        dist[i][j] = Odddist[i][j] = k;
    }
    printf("Initial Odd Matrix:\n");
    for (i=0;i<n;i++){
        for (k=0;k<n;k++)
            printf("%3d %d ",Odddist[i][k],Oddsucc[i][k]);
        printf("\n");
    }
    printf("----------------------------------------\n");
}

void printMatrices(){ /*Print Matrices*/
        int i,k;
        printf("Odd matrix:\n");
        for (i=0;i<n;i++){
            for (k=0;k<n;k++)
                printf("%3d %d ",Odddist[i][k],Oddsucc[i][k]);
            printf("\n");
        }
        printf("Even matrix:\n");
        for (i=0;i<n;i++){
            for (k=0;k<n;k++)
                printf("%3d %d ",Evendist[i][k],Evensucc[i][k]);
            printf("\n");
        }
        printf("----------------------------------------\n");
    }

void printPath(){ /*Print out Odd Path*/
    int i,j,k;
    printf("OUTPUTING ODD PATH:\n");
    for (i=0;i<n;i++)
        for (j=0;j<n;j++){
            if (Odddist[i][j]==CEILING)
                printf("No odd path from %d to %d\n",i,j);
            else{
                int temp = dist[i][Oddsucc[i][j]], count = 1;
                k = Oddsucc[i][j];
                printf("Distance %d for odd path: %d ", Odddist[i][j], i);
                while(temp != Odddist[i][j]){
                    if(count%2 != 0){
                        printf("%d ", k);
                        temp = temp + dist[k][Evensucc[k][j]];
                        k = Evensucc[k][j];
                    }else{
                        printf("%d ", k);
                        temp = temp + dist[k][Oddsucc[k][j]];
                        k = Oddsucc[k][j];
                    }
                    count++;
                }
                printf("%d\n",j);
            }
        }

    /*Print out Even Path*/
    printf("----------------------------------------\n");
    printf("OUTPUTING EVEN PATH:\n");
    for (i=0;i<n;i++)
        for (j=0;j<n;j++){
            if (Evendist[i][j]==CEILING)
                printf("No even path from %d to %d.\n",i,j);
            else{
                int temp = dist[i][Evensucc[i][j]], count = 2;
                k = Evensucc[i][j];
                printf("Distance %d for even path: %d ", Evendist[i][j], i);
                while(temp != Evendist[i][j]){
                    if(count%2 == 0){
                        printf("%d ", k);
                        temp = temp + dist[k][Oddsucc[k][j]];
                        k = Oddsucc[k][j];
                    }else{
                        printf("%d ", k);
                        temp = temp + dist[k][Evensucc[k][j]];
                        k = Evensucc[k][j];
                    }
                    count++;
                }
                printf("%d\n",j);
            }
        }
}

int main(void){
    int i, j, k;

    scanf("%d",&n);

    for (i=0;i<n;i++)
        for (j=0;j<n;j++){
            dist[i][j] = Odddist[i][j] = Evendist[i][j] = CEILING;
            Oddsucc[i][j] = Evensucc[i][j] = j;
        }

    initialoddMatrix();

    /*Floyd-Warshall*/
    for (j=0;j<n;j++){
        printf("PROCESS COLUMN %d:\n", j);

        /*Calculate Even Matrix*/
        for (i=0;i<n;i++)
        if (CEILING>Odddist[i][j])
            for (k=0;k<n;k++){
                newDist = Odddist[i][j]+Odddist[j][k];
                if (newDist < Evendist[i][k]){
                    Evendist[i][k]=newDist;
                    Evensucc[i][k]=Oddsucc[i][j];
                }
            }
        for (i=0;i<n;i++)
            if (CEILING>Evendist[i][j])
                for (k=0;k<n;k++){
                    newDist=Evendist[i][j]+Evendist[j][k];
                    if (newDist<Evendist[i][k]){
                        Evendist[i][k]=newDist;
                        Evensucc[i][k]=Evensucc[i][j];
                    }
                }

        /*Calculate Odd Matrix*/
        for (i=0;i<n;i++)
            if (CEILING > Evendist[i][j])
                for (k=0;k<n;k++){
                    newDist=Evendist[i][j]+Odddist[j][k];
                    if (newDist<Odddist[i][k]){
                        Odddist[i][k]=newDist;
                        Oddsucc[i][k]=Evensucc[i][j];
                    }
                }

        for (i=0;i<n;i++)
            if (CEILING > Odddist[i][j])
                for (k=0;k<n;k++){
                    newDist=Odddist[i][j]+Evendist[j][k];
                    if (newDist<Odddist[i][k]){
                        Odddist[i][k]=newDist;
                        Oddsucc[i][k]=Oddsucc[i][j];
                    }
                }
    printMatrices();
    }
    printPath();
    return 0;
    }
