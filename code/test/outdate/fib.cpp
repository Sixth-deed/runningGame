#include <iostream>

    // Function to compute the nth Fibonacci number
    int fib(int n) {
        if (n <= 1)
            return n;
        else
            return fib(n - 1) + fib(n - 2);
    }

int main(){
    std:: cout <<fib(5)<< std::endl;
    return 0;
}
