// CHECK: Function name: sum_with_skip
// CHECK: RPO order:
// CHECK: Back edges:
// CHECK-NOT: none

int sum_with_skip(int n) {
    int total = 0;
    for (int i = 0; i < n; i = i + 1) {
        if (i % 3 == 0) {
            continue;
        }
        if (i % 5 == 0) {
            continue;
        }
        total = total + i;
    }
    return total;
}
