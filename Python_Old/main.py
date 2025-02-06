from fractions import Fraction

import concurrent.futures

from sympy.ntheory.primetest import is_square 
import math
import logging
import time 
PRODUCTIONS_UPPERBOUND = 150
PRODUCTIONS_LOWERBOUND = 1


results = {}


def func(x):
    # return  (2*x-1)*(4*(x*x)-2*x-1)
    return  x*(x-1)*(x**3-8*x**2+5*x+1)

def testing():
    prior = set()
    start_time = time.perf_counter()
    for i in range(PRODUCTIONS_LOWERBOUND, PRODUCTIONS_UPPERBOUND): 
        for j in range(i, PRODUCTIONS_UPPERBOUND): #Deneominator 
            
            frac = Fraction(i, j)
            if math.gcd(i, j) != 1:
                continue

            # if frac in prior:
            #     continue
            
            out = func(frac)
            
            if out == 0:
                continue

            # prior.add(frac)
            results[frac] = out
            # logging.info("Fraction: %s, Result: %s", frac, out)

            logging.info("%s, %s", frac, out)

        if (i % (PRODUCTIONS_UPPERBOUND/100) == 0):
            print(f"Done with {i} iterations")
            curr_time = time.perf_counter()
            print(f"Time elapsed: {(curr_time - start_time)} seconds")
            
    del prior
            
def seconds_to_hms(seconds):
    hours, remainder = divmod(seconds, 3600)
    minutes, seconds = divmod(remainder, 60)
    return hours, minutes, seconds

def checkingIfSquareFree():
    sameOne = set()

    max_threads = 1

    total_iterations = len(results) * (len(results) - 1)
    completed_iterations = 0
    percenter = int((total_iterations)/100)
    start_time = time.time()

    def process_result(i, j):
        nonlocal completed_iterations

        completed_iterations += 1


        completePercent = int((completed_iterations / total_iterations) * 100)
        if (completed_iterations % percenter) == 0:
            elapsed_time = time.time() - start_time
            estimated_remaining_time = (100 - completePercent) * (elapsed_time / completePercent)
            hours, minutes, seconds = seconds_to_hms(estimated_remaining_time)
            print(f"Done with {completePercent}%. Estimated time remaining: {int(hours)} hours, {int(minutes)} minutes, {int(seconds)} seconds")




        if (i, j) in sameOne or (j, i) in sameOne:
            return

        if i == Fraction(1, 1-j) or j == Fraction(1, 1-i) or i == Fraction(1, j-1) or j == Fraction(1, i-1):
            return

        product = results[i] * results[j]

        sameOne.add((i, j))

        if is_square(product.numerator) and is_square(product.denominator):
            print("Square Free")
            print(i, j)
            print(results[i], results[j])
            print(results[i] * results[j])
            logging.info("%s, %s, %s, %s", i, j, results[i], results[j])
    
    with concurrent.futures.ThreadPoolExecutor(max_workers=max_threads) as executor:
        futures = []
        for i in results:

            for j in results:
                if i == j:
                    continue
                futures.append(executor.submit(process_result, i, j))
        
        concurrent.futures.wait(futures)


    print(f"Total time: {time.time() - start_time} seconds")

            



timeOfComputation = time.time()

# format the time into date_time
timeOfComputation = time.strftime("%Y %m %d-%H %M %S")

logging.basicConfig(filename=f'Numbers/NumberAndOut{PRODUCTIONS_UPPERBOUND}.log', filemode='w', format='%(message)s', level=logging.INFO)
startTime = time.perf_counter()
testing()

endTime = time.perf_counter()
print(f"Total time: {endTime - startTime} seconds")

# print("Done with generating the numbers")
# # logging.basicConfig(filename=f'output/SquareFree_{timeOfComputation}.log', filemode='w', format='%(message)s', level=logging.INFO)
# logging.info("u, t, f(u), f(t)")
# logging.debug("Square Free")
# checkingIfSquareFree()
            