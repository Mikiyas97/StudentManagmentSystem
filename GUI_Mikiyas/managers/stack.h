#ifndef STACK_H
#define STACK_H

#include "linkedlist.h"

// ============================================================================
// Custom Stack Data Structure (LIFO)
// ============================================================================
// Implemented entirely without standard library containers using our custom
// LinkedList<T> class as the underlying storage.
//
// Time Complexity:
//   - push(): O(1)
//   - pop():  O(1)
//   - peek(): O(1)
// ============================================================================

template <typename T>
class Stack {
private:
    LinkedList<T> list;

public:
    Stack() {}
    ~Stack() {}

    // Push an item onto the top of the stack. O(1)
    void push(const T& item) {
        // We push to the front of the linked list so that pop is O(1)
        list.push_front(item);
    }

    // Remove and return the top item. O(1)
    T pop() {
        if (list.isEmpty()) {
            throw "Stack underflow!";
        }
        
        auto* headNode = list.getHead();
        T data = headNode->data;
        list.remove(headNode);
        return data;
    }

    // Return the top item without removing it. O(1)
    T peek() const {
        if (list.isEmpty()) {
            throw "Stack is empty!";
        }
        return list.getHead()->data;
    }

    // Check if the stack is empty. O(1)
    bool isEmpty() const {
        return list.isEmpty();
    }

    // Return the number of elements. O(1)
    int size() const {
        return list.size();
    }
    
    // Clear all elements. O(n)
    void clear() {
        list.clear();
    }
};

#endif // STACK_H
