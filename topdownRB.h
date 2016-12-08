// Top-down red-black tree header file
typedef int Key;
typedef Key Item;
#define key(A) (A)
#define less(A, B) (key(A) < key(B))
#define eq(A, B) (key(A) == key(B))

typedef struct STnode* link;

struct STnode
{
    Item item;  // Data for this node
    link l, r;  // left & right links
    char red;   // RB color
    int N;      // subtree size
};

extern Item NULLitem;

void STinit();          // Initialize tree with just a sentinel

Item STsearch(Key v);   // Find node for a key

int SToddRankOfMax();   // No key can have a rank larger than this

Item STselectSmall(int x, int *flag);  // Find node with smallest key for an odd rank

Item STselectLarge(int x, int *flag);  // Find node with largest key for an odd rank

Item STselect(int k);   // Treat tree as flattened into an ordered array

//void FindLargestSmallest(int x, int *max, int* min, int *flag); //Find smallest and largest keys with odd rank x

int STinvSelect(Key v); // Inverse of STselect

void extendedTraceOn(); // Full tracing of intermediate trees

void basicTraceOn();    // Just trace down and up passes for insert

void traceOff();        // Run silent

void STinsert(Item item);     // Insert an item.  No uniqueness check

void verifyRBproperties();    // Ensure that tree isn't damaged

void STprintTree();           // Print out tree

void cleanUpUnbalanced(link h);  // Includes subtree sizes and verifies a tree
                                 // built without balancing

void CheckDuplicate(int x, int *temp); //Check duplicate value to insert in the tree

void FindRank(int x, int *rank, int *temp); //Find rank of key x


