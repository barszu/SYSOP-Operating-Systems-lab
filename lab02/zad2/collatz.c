// funkcja realizująca regułę Collatza
int collatz_conjecture(int input){
    int n = input;
    if ((n % 2) == 0){
        return n/2;
    }
    else {
        return 3*n + 1 ;
    }
}

// funkcja sprawdzająca po ilu wywołaniach collatz_conjecture zbiega się do 1
int test_collatz_convergence(int input, int max_iter){
    int iter_no = 1;
    int n = input;
    while (iter_no <= max_iter){
        n = collatz_conjecture(n);
        if (n == 1){ //found
            return iter_no ;
        }
        iter_no++ ;
    }
    return -1 ; //not found within max_iter
}