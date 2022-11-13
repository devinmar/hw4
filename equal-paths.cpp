#include "equal-paths.h"
using namespace std;

// You may add any prototypes of helper functions here
bool areLeavesSameLength(struct Node* root, int pos, int* posLeaf) {
    // checks for the lowest possible iteration
    if (root == NULL)
        return true;

    // checks if leaf position is met
    if (root->left == NULL && root->right == NULL) {
      // first iteration
        if (*posLeaf == 0) {
          // change position of leaf
            *posLeaf = pos;
            return true;
        }
        // compares position
        // as long as if it is not zero
        // or first encounter
        return (pos == *posLeaf);
    }
    // return function here recursively checks each leafs position
    return areLeavesSameLength(root->left, pos + 1, posLeaf) && areLeavesSameLength(root->right, pos + 1, posLeaf);
}

bool equalPaths(Node * root)
{
    // Add your code below
    int pos = 0, posLeaf = 0;
    return areLeavesSameLength(root, pos, &posLeaf);
}

