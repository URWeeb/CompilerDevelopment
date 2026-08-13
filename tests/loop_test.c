// CHECK: Function name: sum_even
// CHECK: RPO order:
// CHECK: Back edges:
// CHECK-NOT: none

int sum_even(int n) {
    int total = 0;
    for (int i = 0; i < n; i = i + 1) {
        if (i % 2 == 0) {
            total = total + i;
        }
    }
    return total;
}
