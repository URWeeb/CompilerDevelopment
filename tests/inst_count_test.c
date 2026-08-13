// CHECK: Function foo():
// CHECK-DAG: add: 1
// CHECK-DAG: br:
// CHECK-DAG: load:
// CHECK-DAG: ret:
// CHECK-DAG: store:

int foo(int a, int b) {
    int c = a + b;
    if (c > 0) {
        return c;
    }
    return 0;
}
