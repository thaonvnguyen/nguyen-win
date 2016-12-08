#include <stdlib.h>
#include <stdio.h>
#include "topdownRB.h"

link z,head;               // Pointers to sentinel and root
Item NULLitem=(-9999999);  // Data for sentinel

int trace=0;  // Controls trace output for insert

// Allocates and fills in a node
link NEW(Item item, link l, link r, int N){
    link x = malloc(sizeof *x);
    x->item = item;
    x->l = l;
    x->r = r;
    x->red = 1;
    x->N = N;
    return x;
}


// Root/sentinel (head/z) starts out red, but first insert makes root black
// and second insert makes sentinel black.
void STinit(){
    head = (z = NEW(NULLitem, 0, 0, 0));
}

Item searchR(link h, Key v)
// Recursive search for a key
{
    Key t = key(h->item);
    if (h == z)
        return NULLitem;
    if (eq(v, t))
        return h->item;
    if (less(v, t))
        return searchR(h->l, v);
    return searchR(h->r, v);
}

Item STsearch(Key v){
    return searchR(head, v);
}


// See Sedgewick - implements "zero-based indexing".
// Returns the kth smallest key where k=0 returns the smallest
// key.  Thus, this is like flattening the tree inorder into an array
// and applying k as a subscript.
Item selectR(link h, int k){
    int t = h->l->N;

    if (h == z){    //return NULLitem;
        printf("Impossible situation in selectR\n");
        STprintTree();
        exit(0);
    }
    if (t > k)
        return selectR(h->l, k);
    if (t < k)
        return selectR(h->r, k-t-1);
    return h->item;
}

Item STselect(int k){
    if (k<0 || k>=head->N){
        printf("Range error in STselect() k %d N %d\n",k,head->N);
        exit(0);
    }
    return selectR(head, k);
}

int invSelectR(link h, Key v){ // Inverse of selectR
    Key t = key(h->item);
    int work;

    if (h==z)
        return -1;  // v doesn't appear as a key
    if (eq(v, t))
        return h->l->N;
    if (less(v, t))
        return invSelectR(h->l,v);
    work=invSelectR(h->r,v);
    if (work==(-1))
        return -1;  // v doesn't appear as a key
    return 1 + h->l->N + work;
}

int STinvSelect(Key v){
    return invSelectR(head,v);
}

void fixN(link h){ // Fixes subtree size of h, assuming that subtrees have correct sizes
    if(h->item % 2 != 0)
        h->N = h->l->N + h->r->N + 1;
    else
        h->N = h->l->N + h->r->N;
}

link rotR(link h){ // Rotate right at h, i.e. flip edge between h & h->l
    link x = h->l;
    h->l = x->r;
    x->r = h;

    x->N = x->r->N;
    fixN(x->r);
    return x;
}

link rotL(link h){// Rotate left at h, i.e. flip edge between h & h->r
    link x = h->r;
    h->r = x->l;
    x->l = h;

    x->N = x->l->N;
    fixN(x->l);
    return x;
}

link rsRBinsert(link h, Item item, int sw){ // Sedgewick's code, with details included - NOT TESTED
    Key v = key(item);

    if (h == z)
        return NEW(item, z, z, 1);

    if ((h->l->red) && (h->r->red)){
        h->red = 1;
        h->l->red = 0;
        h->r->red = 0;
    }

    if (less(v, key(h->item))){
        h->l = rsRBinsert(h->l, item, 0);
        if (h->red && h->l->red && sw)
            h = rotR(h);
        if (h->l->red && h->l->l->red){
            h = rotR(h);
            h->red = 0;
            h->r->red = 1;
        }
    }else{
        h->r = rsRBinsert(h->r, item, 1);
        if (h->red && h->r->red && !sw)
            h = rotL(h);
        if (h->r->red && h->r->r->red){
            h = rotL(h);
            h->red = 0;
            h->l->red = 1;
        }
    }
    fixN(h);
    return h;
}

void extendedTraceOn(){
    trace=2;
}

void basicTraceOn(){
    trace=1;
}

void traceOff(){
    trace=0;
}

void tracePrint(char *s,link h){
    if (trace){
        if (h==z)
            printf("%s at sentinel\n",s);
        else
            printf("%s at %d\n",s,key(h->item));
    }
}

link RBinsert(link h, Item item, int sw)
// Program 13.6 coded to be a bit clearer and make mutually exclusive
// cases obvious.  Also includes tracing.  See 2320 notes.  BPW
// h is present node in search down tree.
// Returns root of modified subtree.
// item is the Item to be inserted.
// sw == 1 <=> h is to the right of its parent.
{
    Key v = key(item);
    link before;  // Used to trigger printing of an intermediate tree

    tracePrint("Down",h);
    if (h == z)
        return NEW(item, z, z, 1);  // Attach red leaf

    if ((h->l->red) && (h->r->red)){      // Flip colors before searching down
        tracePrint("Color flip",h);
        h->red = 1;
        h->l->red = 0;
        h->r->red = 0;
        if (trace==2)
            STprintTree();
    }

    if (less(v, key(h->item))){
        tracePrint("Insert left",h);
        before=h->l;
        h->l = RBinsert(h->l, item, 0);    // Insert in left subtree
        if (trace==2 && before!=h->l)      // Has a rotation occurred?
            STprintTree();
        if (h->l->red){
            if (h->red){
                if (sw){
                    tracePrint("Case ~1",h);
                    h = rotR(h);                 // Set up case ~2 after return
                }
                else
                ;
            }
            else if (h->l->l->red){
                tracePrint("Case 2",h);
                h = rotR(h);
                h->red = 0;
                h->r->red = 1;
            }
        }
    }else{
        tracePrint("Insert right",h);
        before=h->r;
        h->r = RBinsert(h->r, item, 1);    // Insert in right subtree
        if (trace==2 && before!=h->r)      // Has a rotation occurred?
            STprintTree();
        if (h->r->red){
            if (h->red){
                if (!sw){
                    tracePrint("Case 1",h);
                    h = rotL(h);                 // Set up case 2 after return
                }
                else
                ;}
            else if (h->r->r->red){
                tracePrint("Case ~2",h);
                h = rotL(h);
                h->red = 0;
                h->l->red = 1;
            }
        }
    }

    fixN(h);
    tracePrint("Up",h);
    return h;
}

void STinsert(Item item){
    head = RBinsert(head, item, 0);
    if (head->red)
        printf("red to black reset has occurred at root!!!\n");
    head->red = 0;
}

void checkRed(link h,int redParent){ // Verifies property 3 in notes
    if (redParent && h->red){
        printf("Red property problem at %d\n",key(h->item));
        STprintTree();
        exit(0);
    }
    if (h==z)
        return;
    checkRed(h->l,h->red);
    checkRed(h->r,h->red);
}

int leftPathBlackHeight(link h){ // Counts black nodes on path to the minimum
    if (h==z)
        return !(h->red);
    return leftPathBlackHeight(h->l) + !(h->red);
}

void checkBlack(link h,int blackCount){ // Checks that all paths downward from a node have the same number of black nodes
    if (h==z){
        if (blackCount==!(h->red))
            return;
        else{
            printf("Black height problem!\n");
            STprintTree();
            exit(0);
        }
    }
    if (h->red){
        checkBlack(h->l,blackCount);
        checkBlack(h->r,blackCount);
    }else{
        checkBlack(h->l,blackCount-1);
        checkBlack(h->r,blackCount-1);
    }
}

Key lastInorder;    // Saves key from last node processed

void checkInorder(link h){ // Checks that inorder yields keys in ascending order
    if (h==z)
        return;

    checkInorder(h->l);
        if (less(h->item,lastInorder)){
            printf("Inorder error\n");
            STprintTree();
            exit(0);
        }
    lastInorder=key(h->item);
    checkInorder(h->r);
}

int checkN(link h){ // Verifies that subtree sizes are correct
    int work;

    if (h==z){
        if (h->N!=0 ){
            printf("Count for sentinel is %d, should be 0\n",h->N);
            STprintTree();
            exit(0);
        }
    }else{
        if( h->item % 2 != 0)
            work = checkN(h->l) + checkN(h->r) + 1;
        else
            work = checkN(h->l) + checkN(h->r);
        if (h->N!=work){
            h->N = work;
            //exit(0);
        }
    }
    return h->N;
}
// Checks all required properties.
// If a fatal problem is found, the tree is printed before exit(0)
void verifyRBproperties(){
    int lpbHeight;

    if (head->red)
        printf("Root is not black!\n");
    if (z->red)
        printf("Sentinel is not black!\n");
    lastInorder=(-99999999);
    checkInorder(head);
    checkRed(head,0);
    lpbHeight=leftPathBlackHeight(head);
    checkBlack(head,lpbHeight);
    checkN(head);
}

//Print out tree function
void printTree(link h,int depth,int bhAbove){
    int i,bhBelow;

    if (h==z){
        if (bhAbove!=1){
            for (i=0;i<depth;i++)
            printf("     ");
            printf("Black-height issue detected at sentinel\n");
        }
        return;
    }

    if ((h->red))
        bhBelow=bhAbove;
    else
        bhBelow=bhAbove-1;
    printTree(h->r,depth+1,bhBelow);
    for (i=0;i<depth;i++)
        printf("     ");
    if (h->red)
        printf("[%d %d]\n",key(h->item),h->N);
    else
        printf("(%d %d)\n",key(h->item),h->N);
    printTree(h->l,depth+1,bhBelow);
}

void STprintTree(){
    printTree(head,0,leftPathBlackHeight(head));
}


// Recomputes subtree sizes for an otherwise correct tree
void fixAllN(link h){
    if (h->l)
        fixAllN(h->l);
    else
        h->l=z;
    if (h->r)
        fixAllN(h->r);
    else
        h->r=z;
    fixN(h);
}

// Checks a tree constructed elsewhere
void cleanUpUnbalanced(link h){
    fixAllN(h);
    head=h;
    z->red=0;
    verifyRBproperties();
}
//Check duplicate value in the tree
void ChD(link h, int x, int *temp){
    if(h != z){
        if(h->item > x)
            ChD(h->l, x, temp);
        else if (h->item == x){
            *temp = *temp + 1;
            ChD(h->r, x, temp);
        }
        else
            ChD(h->r, x, temp);
    }
}

void CheckDuplicate(int x, int *temp){
    ChD(head, x, temp);
}

//Find rank of a key
void FR(link h, int x, int *rank, int *temp){
    if(h != z){
        if(h->item > x)
            FR(h->l, x, rank, temp); //Go left if key is greater than the value
        else{
            if(h->item == x)
                *temp = *temp + 1;
            if(h->item % 2 != 0 && h->item != x) //Check odd-even of the key of the current node
                *rank = *rank + 1;
            if(h->l != z)
                *rank = *rank + h->l->N; //Add the odd size of the node on the left of the current node to the rank
            FR(h->r, x, rank, temp); //Go right
        }
    }
}

void FindRank(int x, int *rank, int *temp){
    FR(head, x, rank, temp);
}

//Find smallest and Largest key of a rank
void FLS(link h, int x, int *max, int *min, int *flag){
    int rank = 0, temp = 0;

    if(h != z){
        FindRank(h->item, &rank, &temp); //Find the rank of the current node.
        if(rank > x)
            FLS(h->l, x, max, min, flag); //Go left if the current rank is greater than the value.
        else if (rank == x){
            *flag = *flag + 1;
            if( *flag == 1 ){
                *max = h->item;
                *min = h->item;
                FLS(h->l, x, max, min, flag);
                FLS(h->r, x, max, min, flag);
            }else{
                if( h->item > *max)
                    *max = h->item;
                if( h->item < *min)
                    *min = h->item;
                FLS(h->l, x, max, min, flag); //Go left
                FLS(h->r, x, max, min, flag); //Go right
            }
        }else{
            FLS(h->l, x, max, min, flag); //Go left.
            FLS(h->r, x, max, min, flag); //Go right.
        }
    }
}
Item STselectSmall(int x, int *flag){ // Find node with smallest key for an odd rank
    int max, min;
    FLS(head,x, &max, &min, flag);
    return min;
}
Item STselectLarge(int x, int *flag){  // Find node with largest key for an odd rank
    int max, min;
    FLS(head, x, &max, &min, flag);
    return max;
}

int SToddRankOfMax(){  // No key can have a rank larger than this

return 0;
}
