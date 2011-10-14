#define U 100
#define L 0

shared int A[U-L];

int main() {
    int t = 0;
    upc_forall(int i = L; i < U; i++; i) {
        t += A[i];
    }

    return 0;
}
