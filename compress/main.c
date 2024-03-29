#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEBUG_MODE

char* data = "abcddddd";

typedef struct node {
    char c;
    int freq;
    struct node *l;
    struct node *r;
} nodeStructure;

nodeStructure *getNode(char c, int freq, nodeStructure *l, nodeStructure *r) { // alloc, init...
    nodeStructure* n = (nodeStructure*)malloc(sizeof(nodeStructure));
    n->c = c;
    n->freq = freq;
    n->l = l;
    n->r = r;
#ifdef DEBUG_MODE
    printf("node created:%p %c - %d; l: %p, r: %p;\n", n,  ((c != 0)?  c : '^'), freq, l, r);
#endif
    return n;
}

int cmp(const void* p1, const void* p2) { // qsort comparator for node pointers
    
#ifdef DEBUG_MODE
    if(p1 == NULL || p2 == NULL){ printf("\nComparation error!\n"); exit(1); }
#endif
    
    int fr1 = (*(nodeStructure**)p1)->freq;
    int fr2 = (*(nodeStructure**)p2)->freq;
    return fr1 - fr2;
}

int* countChars(char* data, int len) { // creating the array freq(char)
    int* freq;
    freq = malloc(0x100 * sizeof(int));
    for (int i = 0; i < 0x100; i++) freq[i] = 0;
    for (int i = 0; i < len; i++) freq[data[i]]++;
    return freq;
}

int printInputCharTable(int *freq) {
    int i;
    for (i = 0; i < 0x100; i++) {
        (freq[i] > 0) && printf("%c - %d; ", (char)i, freq[i]); // skip the missings w/o if :)
    }
    puts("");
    return 0;
}

nodeStructure** getFirstNodePointers(int* freq) {
    
    int nodeCounter = 0;
    for (int i = 0; i < 0x100; i++) {
        if ( *(freq + i) != 0) nodeCounter++;
    }
    
    int j = 0;
    nodeStructure **nodePointer;
    nodePointer = malloc((nodeCounter + 1) * sizeof(void*));
    for (int i = 0; i < 0x100; i++) {
        (freq[i] != 0) && (*(nodePointer + j++) = getNode((char)i, *(freq + i), NULL, NULL));
    }
    *(nodePointer + j) = NULL;
    return nodePointer;
}

int pointerCounter(nodeStructure** p) {
    int counter;
    for (counter = 0; *p++ != NULL; counter++); // imbecile!
#ifdef DEBUG_MODE
    printf("\n%d NODES\n", counter);
#endif
    return counter;
}

nodeStructure* makeTree(nodeStructure** node, int len) {
    if (len == 1) return *node;
    
    qsort(node, len, sizeof(void*), cmp);
    *node = getNode(0, (*node)->freq + (*(node+1))->freq, (*node), (*(node+1)));
    
    int i = 1;
    while (node[i] != NULL) {
        node[i] = node[i+1];
        i++;
    }
    return makeTree(node, len - 1);
}

int drawTree(nodeStructure* rootNode) {
    if (rootNode->l == NULL && rootNode->r == NULL) {
        printf("(%c - %d)", rootNode->c, rootNode->freq);
        return 0;
    }
    printf("%p\n", rootNode);
    drawTree(rootNode->l);
    drawTree(rootNode->r);
    return 0;
}


int printCodes(nodeStructure* rootNode) {

    static char buff[0xFF];
    

    if ((rootNode->l == NULL) != (rootNode->r == NULL)) { // (possible logical XOR?)
        printf("MUDDLE ELEMENT! - %p\n", rootNode);
        return 1;
    }

    
    if (rootNode->l == NULL && rootNode->r == NULL) {    // leaf reached
        if (strlen(buff) == 0) {
            buff[strlen(buff)] = '0'; buff[strlen(buff)+1] = '\0';
        }
        printf("%s - %c\n", buff, rootNode->c);
        buff[(strlen(buff) > 0)? strlen(buff) - 1: 0]='\0';
        return 0;
    }
    
    buff[strlen(buff)] = '0'; buff[strlen(buff)+1] = '\0'; printCodes(rootNode->l);
    buff[strlen(buff)] = '1'; buff[strlen(buff)+1] = '\0'; printCodes(rootNode->r);
    buff[(strlen(buff) > 0)? strlen(buff) - 1: 0]='\0';
    
    return 0;
}


char codes[0xff][0xff];


int makeCodes(nodeStructure* rootNode, int depth) {
    
    static char buff[0xFF];
    
    if (rootNode->l == NULL && rootNode->r == NULL) { // leaf
        if (depth == 0) { buff[0] = '0'; buff[1] ='\0'; // single
        } else buff[depth] = '\0';
    
    strcpy(codes[rootNode->c], buff);
    printf("%s - %c\n", buff, rootNode->c);
    depth--;
    return depth;
    }
    
    buff[depth] = '0'; makeCodes(rootNode->l, ++depth);  buff[--depth] = '\0';
    buff[depth] = '1'; makeCodes(rootNode->r, ++depth);  buff[--depth] = '\0';
    
    return depth;
}


char* bitStreamOut(int length, char* data) {
    char *out = malloc(length/8 + 1);
    char tmp = 0;
    int j;
    int k = 0;
    for (int i = 0; i <=length; i++) {
        j = i % 8;
        tmp+=((data[j] == '1')? 1 : 0 ) * pow(2,j);
        if ( j == 0 ) {(out[k++]) = tmp;}
    }
    return out;
}



/*  *   *    ***    *   *   *
    ** **   *   *   *   **  *
    * * *   *****   *   * * *
    *   *   *   *   *   *  **
    *   *   *   *   *   *   *  */

int main(int argc, char **argv) {

    int* freq = countChars(data, (int)strlen(data));
    
#ifdef DEBUG_MODE
    printInputCharTable(freq);
    puts("");
#endif

    nodeStructure** nodePointers = getFirstNodePointers(freq);
    int len =pointerCounter(nodePointers);
    
#ifdef DEBUG_MODE
    printf("\nTotal: %d.\n\n", len);
#endif
    
    nodeStructure *rootNode = makeTree(nodePointers, len);

#ifdef DEBUG_MODE
    printf("\nrooot %p found.\n", rootNode);
#endif
   
#ifdef DEBUG_MODE
//    puts("\nnow backwards...\n");
//    drawTree(rootNode);
//    puts("\n\ncodes:\n");
//    printCodes(rootNode);
#endif
    
    puts("\ncodes:\n");
    
    makeCodes(rootNode, 0);
    
//    for(int i = 0; printf("%s\n", (char*)codes[i]), i < 0x100; i++); // it works! 😱
    
    puts("");
    
    printf("%s - %lu bytes\n", data, strlen(data));
    bitStreamOut((int)strlen(data), data);
    
    puts("");
  
    return 0;
}