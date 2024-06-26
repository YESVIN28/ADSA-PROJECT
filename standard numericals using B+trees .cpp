#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

// Structure to represent an interval
struct Interval {
    int start;
    int end;

    Interval(int s, int e) : start(s), end(e) {}
};

// Node structure for the Interval B+ Tree
struct Node {
    bool leaf;
    vector<Interval> keys;
    vector<Node*> pointers;

    Node(bool isLeaf) : leaf(isLeaf) {}
};

// Interval B+ Tree class
class IntervalBPlusTree {
private:
    Node* root;

    // Helper function to find the appropriate child node index for a given interval
    int findChildIndex(Node* node, const Interval& interval) {
        int index = 0;
        while (index < node->keys.size() && interval.start > node->keys[index].start) {
            ++index;
        }
        return index;
    }

    // Helper function to split a full node
    Node* splitNode(Node* parentNode, Node* fullNode) {
        int mid = fullNode->keys.size() / 2;
        Node* newNode = new Node(fullNode->leaf);

        // Move half of the keys and pointers to the new node
        newNode->keys.assign(fullNode->keys.begin() + mid, fullNode->keys.end());
        fullNode->keys.erase(fullNode->keys.begin() + mid, fullNode->keys.end());
        
        if (!fullNode->leaf) {
            newNode->pointers.assign(fullNode->pointers.begin() + mid, fullNode->pointers.end());
            fullNode->pointers.erase(fullNode->pointers.begin() + mid, fullNode->pointers.end());
        }

        // Update parent node's pointers
        int index = findChildIndex(parentNode, newNode->keys[0]);
        parentNode->pointers.insert(parentNode->pointers.begin() + index + 1, newNode);

        return newNode;
    }

    // Helper function to insert an interval into a node
    void insertIntoNode(Node* node, const Interval& interval) {
        int index = lower_bound(node->keys.begin(), node->keys.end(), interval, 
            [](const Interval& a, const Interval& b) { return a.start < b.start; }) - node->keys.begin();
        
        node->keys.insert(node->keys.begin() + index, interval);
    }

public:
    IntervalBPlusTree() {
        root = new Node(true);
    }

    // Function to insert an interval into the tree
    void insert(const Interval& interval) {
        Node* currentNode = root;

        // Traverse the tree to find the appropriate leaf node
        while (!currentNode->leaf) {
            int index = findChildIndex(currentNode, interval);
            currentNode = currentNode->pointers[index];
        }

        // Insert the interval into the leaf node
        insertIntoNode(currentNode, interval);

        // Check if the leaf node is full and split if necessary
        if (currentNode->keys.size() > 2) {
            Node* parentNode = root;
            while (!parentNode->leaf) {
                int index = findChildIndex(parentNode, currentNode->keys[0]);
                parentNode = parentNode->pointers[index];
            }
            splitNode(parentNode, currentNode);
        }
    }

    // Function to search for intervals containing a given point
    vector<Interval> search(int point) {
        vector<Interval> result;
        Node* currentNode = root;

        // Traverse the tree to find the leaf node containing the point
        while (!currentNode->leaf) {
            int index = 0;
            while (index < currentNode->keys.size() && point > currentNode->keys[index].start) {
                ++index;
            }
            currentNode = currentNode->pointers[index];
        }

        // Search for intervals containing the point in the leaf node
        for (const auto& interval : currentNode->keys) {
            if (interval.start <= point && interval.end >= point) {
                result.push_back(interval);
            }
        }

        return result;
    }
};

int main() {
    // Create an Interval B+ Tree
    IntervalBPlusTree tree;

    // Generate random student marks and use them to insert intervals into the tree
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> marks(0, 100);

    cout << "Inserting random student marks into the Interval B+ Tree..." << endl;
    for (int i = 0; i < 10; ++i) {
        int start = marks(gen);
        int end = marks(gen);
        tree.insert(Interval(min(start, end), max(start, end)));
        cout << "Inserted interval [" << min(start, end) << ", " << max(start, end) << "]" << endl;
    }

    // Search for intervals containing a point
    int searchPoint = marks(gen);
    vector<Interval> result = tree.search(searchPoint);

    // Display the search result
    cout << "\nIntervals containing point " << searchPoint << ":" << endl;
    if (result.empty()) {
        cout << "No intervals found." << endl;
    } else {
        for (const auto& interval : result) {
            cout << "[" << interval.start << ", " << interval.end << "]" << endl;
        }
    }

    return 0;
}
