// CHECK: Function classify():
// CHECK-DAG: switch: 1
// CHECK-DAG: ret:

int classify(int x) {
    switch (x) {
        case 0:
            return 10;
        case 1:
            return 20;
        default:
            return -1;
    }
}
