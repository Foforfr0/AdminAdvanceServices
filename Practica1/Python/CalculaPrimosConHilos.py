# CalculaPrimosConHilos.py

import math
import threading

def is_prime(n: int) -> bool:
    if n < 2:
        return False
    if n % 2 == 0:
        return n == 2
    r = int(math.isqrt(n))
    for d in range(3, r + 1, 2):
        if n % d == 0:
            return False
    return True

def count_primes(limit: int) -> int:
    count = 0
    for x in range(2, limit + 1):
        if is_prime(x):
            count += 1
    return count

def thread_prime_function(name) :
    limit = 10000000
    total = count_primes(limit)
    print(f"Hilo {name} -> Primos : {total}")
    

def main():
    maxThreads = 10
    for i in range(maxThreads) :
        x = threading.Thread(target=thread_prime_function, args=(i,))
        x.start()

if __name__ == "__main__":
    main()
