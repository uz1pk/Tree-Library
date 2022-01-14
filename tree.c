/*
Name: Usman Zaheer
Student ID: 1148583
Class: CIS 2520 (Dr. Stefan Kremer)
Date Due: 10-26-2021 11:59
This program should sufficently be able to do all basic functions,
and the bonus 20% funtionality.
*/

#include "tree.h"

void attachNode(struct memsys *memsys, int *node_ptr, void *src, unsigned int width) {
    struct Node newNode;
    int nodeData, memData; 

    nodeData = memmalloc(memsys, sizeof(struct Node));
    memData = memmalloc(memsys, width);
    if(memData == MEMNULL || nodeData == MEMNULL) {
        fprintf(stderr, "memalloc failed\n");
        exit(0);
    }
    setval(memsys, src, width, memData);
    newNode.data = memData;
    newNode.lt = MEMNULL;
    newNode.gte = MEMNULL;
    setval(memsys, &newNode, sizeof(struct Node), nodeData);
    *node_ptr = nodeData;
}

void attachChild(struct memsys *memsys, int *node_ptr, void *src, unsigned int width, int direction) {
    struct Node node;

    getval(memsys, &node, sizeof(struct Node), *node_ptr);
    if(direction >= 0) {
        attachNode(memsys, &node.gte, src, width);
    }
    else {
        attachNode(memsys, &node.lt, src, width);
    }
    setval(memsys, &node, sizeof(struct Node), *node_ptr);
}

int comparNode(struct memsys *memsys, int *node_ptr, int (*compar)(const void *, const void *), void *target, unsigned int width) {
    void *data = malloc(width);
    int comparResult;

    readNode(memsys, node_ptr, data, width);
    comparResult = (compar)(target, data);
    free(data);
    return comparResult;    
}

int next(struct memsys *memsys, int *node_ptr, int direction) {
    struct Node node;

    getval(memsys, &node, sizeof(struct Node), *node_ptr);
    if(direction < 0) {
        return node.lt;
    }
    else{
        return node.gte;
    }
    return MEMNULL;
}

void readNode(struct memsys *memsys, int *node_ptr, void *dest, unsigned int width) {
    struct Node node;

    if(*node_ptr == MEMNULL) {
        fprintf(stderr, "No node found\n");
        exit(0);
    }
    getval(memsys, &node, sizeof(struct Node), *node_ptr);
    getval(memsys, dest, width, node.data);
}

void detachNode(struct memsys *memsys, int *node_ptr) {
    struct Node node;

    if(*node_ptr == MEMNULL) {
        fprintf(stderr, "No node found\n");
        exit(0);
    }
    getval(memsys, &node, sizeof(struct Node), *node_ptr);
    memfree(memsys, node.data);
    memfree(memsys, *node_ptr);
    *node_ptr = MEMNULL;
}

void freeNodes(struct memsys *memsys, int *node_ptr) {
    int childLt, childGte;

    if(*node_ptr == MEMNULL) {
        return;
    }
    else {
        childLt = next(memsys, node_ptr, -1);
        childGte = next(memsys, node_ptr, 1);

        freeNodes(memsys, &childLt);
        freeNodes(memsys, &childLt);
        detachNode(memsys, node_ptr);
    }
}

struct Tree *newTree(struct memsys *memsys, unsigned int width) {
    struct Tree *newTree = malloc(sizeof(struct Tree));

    if(newTree == NULL) {
        fprintf(stderr, "Malloc failed\n");
        exit(0);
    }
    newTree->root = MEMNULL;
    newTree->width = width;
    return newTree;
}

void freeTree(struct memsys *memsys, struct Tree *tree) {
    freeNodes(memsys, &tree->root);
    free(tree);
}

void addItem(struct memsys *memsys, struct Tree *tree, int (*compar)(const void *, const void *), void *src) {
    int nextAddress = tree->root, currAddress = 0;
    
    if(tree->root == MEMNULL) {
        attachNode(memsys, &(tree->root), src, tree->width);
    }
    else {
        while(nextAddress != MEMNULL) {
            currAddress = nextAddress;
            nextAddress = next(memsys, &currAddress, comparNode(memsys, &currAddress, compar, src, (tree->width)));
        }
        attachChild(memsys, &currAddress, src, tree->width, comparNode(memsys, &currAddress, compar, src, (tree->width)));
    }
}

int searchItem(struct memsys *memsys, struct Tree *tree, int (*compar)(const void *, const void *), void *target) {
    int nextAddress = tree->root, currAddress = 0;

    while(nextAddress != MEMNULL) {
        currAddress = nextAddress;
        if(comparNode(memsys, &currAddress, compar, target, (tree->width)) == 0) {
            readNode(memsys, &currAddress, target, tree->width);
            return 1;
        }
        nextAddress = next(memsys, &currAddress, comparNode(memsys, &currAddress, compar, target, (tree->width)));
    }
    return 0;
}
