#include "equal-paths.h"
#include <algorithm>
using namespace std;

// // You may add any prototypes of helper functions here


// Helper function to calculate the height of a subtree
int getHeight(Node* node) {
    if (node == nullptr) {
        return 0;
    }
    int leftHeight = getHeight(node->left);
    int rightHeight = getHeight(node->right);
    return 1 + max(leftHeight, rightHeight);
}

// helper function to determine if all paths from leaves to root are the same length
// function will update the height of the current path and return true if all paths are of equal length
bool checkEqualPaths(Node* node, int currentHeight, int& pathHeight) {
    // base case: if current node is null, we're at a leaf, return the current height
    if (node == nullptr) {
        // if this is the first leaf we've reached, record its height in pathHeight
        if (pathHeight == -1) {
            pathHeight = currentHeight;
        }
        // return true if the current path height matches the first leaf path height we've recorded
        return pathHeight == currentHeight;
    }

    // Recursively check the left and right subtrees, incrementing the currentHeight
    if (node->left == nullptr && node->right != nullptr) {
        return checkEqualPaths(node->right, currentHeight + 1, pathHeight);
    } else if (node->left != nullptr && node->right == nullptr) {
        return checkEqualPaths(node->left, currentHeight + 1, pathHeight);
    } else {
        return checkEqualPaths(node->left, currentHeight + 1, pathHeight) &&
               checkEqualPaths(node->right, currentHeight + 1, pathHeight);
    }
}

bool equalPaths(Node* root) {
    int pathHeight = -1; // Initialize path height to -1 to indicate that no leaf has been visited yet
    return checkEqualPaths(root, 0, pathHeight);
}

