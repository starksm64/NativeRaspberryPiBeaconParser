#include <atomic>
#include <stdio.h>

using namespace std;

int main() {
    atomic<int> counter(0);

    int value = counter.fetch_add(1);
    printf("fetch_add -> %d\n", value);
    value = counter.fetch_sub(1);
    printf("fetch_sub -> %d\n", value);
    printf("counter.load() -> %d\n", counter.load());

}
