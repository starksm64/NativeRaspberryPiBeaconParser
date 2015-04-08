#ifndef NATIVESCANNER_SMA_H
#define NATIVESCANNER_SMA_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

/**
 * http://rosettacode.org/wiki/Averages/Simple_moving_average#C.2B.2B
 */
class SMA {
public:
    SMA(unsigned int period) :
            period(period), window(new int32_t[period]), head(NULL), tail(NULL),
            total(0) {
        assert(period >= 1);
    }
    ~SMA() {
        delete[] window;
    }

    // Adds a value to the average, pushing one out if nescessary
    void add(int32_t val) {
        // Special case: Initialization
        if (head == NULL) {
            head = window;
            *head = val;
            tail = head;
            inc(tail);
            total = val;
            return;
        }

        // Were we already full?
        if (head == tail) {
            // Fix total-cache
            total -= *head;
            // Make room
            inc(head);
        }

        // Write the value in the next spot.
        *tail = val;
        inc(tail);

        // Update our total-cache
        total += val;
    }

    // Returns the average of the last P elements added to this SMA.
    // If no elements have been added yet, returns 0.0
    int32_t avg() const {
        ptrdiff_t size = this->size();
        if (size == 0) {
            return 0; // No entries => 0 average
        }
        return total / size;
    }

private:
    unsigned int period;
    int32_t * window; // Holds the values to calculate the average of.

    // Logically, head is before tail
    int32_t * head; // Points at the oldest element we've stored.
    int32_t * tail; // Points at the newest element we've stored.

    int32_t total; // Cache the total so we don't sum everything each time.

    // Bumps the given pointer up by one.
    // Wraps to the start of the array if needed.
    void inc(int32_t * & p) {
        if (++p >= window + period) {
            p = window;
        }
    }

    // Returns how many numbers we have stored.
    ptrdiff_t size() const {
        if (head == NULL)
            return 0;
        if (head == tail)
            return period;
        return (period + tail - head) % period;
    }
};


#endif //NATIVESCANNER_SMA_H
