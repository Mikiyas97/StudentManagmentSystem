#ifndef QUEUE_H
#define QUEUE_H

#include "linkedlist.h"

// ============================================================================
// Custom Queue Data Structure (FIFO)
// ============================================================================
// Implemented entirely without standard library containers using our custom
// LinkedList<T> class as the underlying storage.
//
// Time Complexity:
//   - enqueue(): O(1)
//   - dequeue(): O(1)
//   - front():   O(1)
// ============================================================================

template <typename T>
struct Queue {
    LinkedList<T> list;

    Queue() {}
    ~Queue() {}

    // Add an item to the back of the queue. O(1)
    void enqueue(const T& item) {
        // push_back is O(1) since LinkedList maintains a tail pointer
        list.push_back(item);
    }

    // Remove and return the item at the front of the queue. O(1)
    T dequeue() {
        if (list.isEmpty()) {
            throw "Queue underflow!";
        }
        
        auto* headNode = list.getHead();
        T data = headNode->data;
        list.remove(headNode);
        return data;
    }

    // Return the item at the front without removing it. O(1)
    T front() const {
        if (list.isEmpty()) {
            throw "Queue is empty!";
        }
        return list.getHead()->data;
    }

    // Check if the queue is empty. O(1)
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

#endif // QUEUE_H
