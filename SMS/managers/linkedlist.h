#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <QVector>
#include <functional>

// ============================================================================
// LinkedList<T> — Custom Doubly-Linked List Template
// ============================================================================
// A generic doubly-linked list data structure implementing:
//   - O(1)      push_back, push_front, remove by node pointer
//   - O(n)      linear search (findFirst, findAll)
//   - O(n log n) merge sort (stable, in-place on list nodes)
//   - O(n)      toQVector conversion for UI compatibility
//
// Used by StudentManager, MarkManager, and UserManager to arrange data
// in memory after loading from the SQLite database.
// ============================================================================

template <typename T>
struct LinkedList {
    // --- Node Structure ---
    // Each node holds data and pointers to previous and next nodes.
    struct Node {
        T data;
        Node* prev;
        Node* next;

        Node(const T& d) : data(d), prev(nullptr), next(nullptr) {}
    };

    Node* head;   // Pointer to the first node
    Node* tail;   // Pointer to the last node
    int count;    // Number of elements in the list

    // ========================================================================
    // Construction / Destruction
    // ========================================================================

    // Default constructor — creates an empty list. O(1)
    LinkedList() : head(nullptr), tail(nullptr), count(0) {}

    // Destructor — frees all nodes. O(n)
    ~LinkedList() {
        clear();
    }

    // Copy constructor — deep copy of all nodes. O(n)
    LinkedList(const LinkedList& other) : head(nullptr), tail(nullptr), count(0) {
        Node* current = other.head;
        while (current) {
            push_back(current->data);
            current = current->next;
        }
    }

    // Copy assignment operator — deep copy with self-assignment check. O(n)
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            clear();
            Node* current = other.head;
            while (current) {
                push_back(current->data);
                current = current->next;
            }
        }
        return *this;
    }

    // ========================================================================
    // Core Operations
    // ========================================================================

    // Insert at the end of the list. O(1)
    // Uses the tail pointer for direct access without traversal.
    void push_back(const T& item) {
        Node* newNode = new Node(item);
        if (!tail) {
            // List is empty — new node becomes both head and tail
            head = tail = newNode;
        } else {
            // Link new node after current tail
            newNode->prev = tail;
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }

    // Insert at the beginning of the list. O(1)
    void push_front(const T& item) {
        Node* newNode = new Node(item);
        if (!head) {
            // List is empty
            head = tail = newNode;
        } else {
            // Link new node before current head
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        count++;
    }

    // Remove a specific node by pointer. O(1)
    // Caller must ensure the node belongs to this list.
    void remove(Node* node) {
        if (!node) return;

        if (node->prev) {
            node->prev->next = node->next;
        } else {
            // Node is the head
            head = node->next;
        }

        if (node->next) {
            node->next->prev = node->prev;
        } else {
            // Node is the tail
            tail = node->prev;
        }

        delete node;
        count--;
    }

    // Remove all nodes and free memory. O(n)
    void clear() {
        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = tail = nullptr;
        count = 0;
    }

    // ========================================================================
    // Access
    // ========================================================================

    // Returns the number of elements. O(1)
    int size() const { return count; }

    // Returns true if the list has no elements. O(1)
    bool isEmpty() const { return count == 0; }

    // Returns pointer to the head node (for manual traversal). O(1)
    Node* getHead() const { return head; }

    // Returns pointer to the tail node. O(1)
    Node* getTail() const { return tail; }

    // ========================================================================
    // Iterator — enables range-based for loops
    // ========================================================================
    // Usage: for (auto& item : myLinkedList) { ... }

    class Iterator {
        Node* current;
    public:
        Iterator(Node* node) : current(node) {}

        T& operator*() { return current->data; }
        const T& operator*() const { return current->data; }
        T* operator->() { return &current->data; }

        Iterator& operator++() {
            if (current) current = current->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const { return current == other.current; }
        bool operator!=(const Iterator& other) const { return current != other.current; }
    };

    // Const iterator for read-only traversal
    class ConstIterator {
        const Node* current;
    public:
        ConstIterator(const Node* node) : current(node) {}

        const T& operator*() const { return current->data; }
        const T* operator->() const { return &current->data; }

        ConstIterator& operator++() {
            if (current) current = current->next;
            return *this;
        }

        bool operator==(const ConstIterator& other) const { return current == other.current; }
        bool operator!=(const ConstIterator& other) const { return current != other.current; }
    };

    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }
    ConstIterator begin() const { return ConstIterator(head); }
    ConstIterator end() const { return ConstIterator(nullptr); }

    // ========================================================================
    // Conversion — Bridge to QVector for UI compatibility
    // ========================================================================

    // Traverse the entire list and copy elements into a QVector. O(n)
    // This allows the linked list to be used internally by managers while
    // returning QVector to page classes (no UI code changes needed).
    QVector<T> toQVector() const {
        QVector<T> vec;
        vec.reserve(count);
        Node* current = head;
        while (current) {
            vec.push_back(current->data);
            current = current->next;
        }
        return vec;
    }

    // ========================================================================
    // Algorithm: Linear Search — O(n)
    // ========================================================================
    // Traverses the list node-by-node and tests each element against a
    // predicate function. This is the natural search algorithm for linked
    // lists since random access is not possible (unlike arrays).

    // Find the first node matching the predicate. O(n) worst case.
    // Returns nullptr if no match is found.
    Node* findFirst(std::function<bool(const T&)> predicate) const {
        Node* current = head;
        while (current) {
            if (predicate(current->data)) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    // Find ALL nodes matching the predicate. O(n).
    // Returns a new LinkedList containing copies of matching elements.
    // Used for filtering operations (e.g., filter students by grade/status).
    LinkedList<T> findAll(std::function<bool(const T&)> predicate) const {
        LinkedList<T> result;
        Node* current = head;
        while (current) {
            if (predicate(current->data)) {
                result.push_back(current->data);
            }
            current = current->next;
        }
        return result;
    }

    // ========================================================================
    // Algorithm: Merge Sort — O(n log n), Stable, In-Place
    // ========================================================================
    // Merge sort is the optimal sorting algorithm for linked lists because:
    //   1. It does not require random access (unlike quicksort's partition)
    //   2. It achieves O(n log n) in ALL cases (best, average, worst)
    //   3. It is stable — equal elements maintain their relative order
    //   4. The merge operation naturally works with linked list pointers
    //
    // The algorithm:
    //   1. Split the list into two halves using slow/fast pointer technique
    //   2. Recursively sort each half
    //   3. Merge the two sorted halves by relinking node pointers
    //
    // Time Complexity:  O(n log n) — n elements, log n levels of recursion
    // Space Complexity: O(log n)   — recursion stack only, no extra arrays

    void mergeSort(std::function<bool(const T&, const T&)> comparator) {
        if (count <= 1) return;

        head = mergeSortRecursive(head, comparator);

        // After sorting, rebuild tail and prev pointers
        // (merge sort only maintains next pointers)
        Node* current = head;
        current->prev = nullptr;
        while (current->next) {
            current->next->prev = current;
            current = current->next;
        }
        tail = current;
    }

    // Recursive merge sort — splits the list and merges sorted halves.
    // Returns the new head of the sorted sub-list.
    Node* mergeSortRecursive(Node* start, std::function<bool(const T&, const T&)>& comparator) {
        // Base case: 0 or 1 elements are already sorted
        if (!start || !start->next) return start;

        // Step 1: Find the middle node using slow/fast pointer technique
        // The slow pointer advances 1 step, fast advances 2 steps.
        // When fast reaches the end, slow is at the midpoint. O(n/2)
        Node* slow = start;
        Node* fast = start->next;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }

        // Step 2: Split the list into two halves
        Node* secondHalf = slow->next;
        slow->next = nullptr;  // Terminate the first half

        // Step 3: Recursively sort each half — O(log n) levels
        Node* left = mergeSortRecursive(start, comparator);
        Node* right = mergeSortRecursive(secondHalf, comparator);

        // Step 4: Merge the two sorted halves — O(n) per level
        return merge(left, right, comparator);
    }

    // Merge two sorted sub-lists into one sorted list.
    // Compares head elements and links the smaller one first. O(n)
    Node* merge(Node* left, Node* right, std::function<bool(const T&, const T&)>& comparator) {
        // Use a dummy node to simplify edge cases
        Node dummy(T{});
        Node* current = &dummy;

        while (left && right) {
            if (comparator(left->data, right->data)) {
                current->next = left;
                left = left->next;
            } else {
                current->next = right;
                right = right->next;
            }
            current = current->next;
        }

        // Append remaining nodes (one list may still have elements)
        current->next = left ? left : right;

        return dummy.next;
    }
};

#endif // LINKEDLIST_H
