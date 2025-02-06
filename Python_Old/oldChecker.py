from fractions import Fraction
from sympy.ntheory.primetest import is_square

import subprocess

import time
from tqdm import tqdm
import logging

# import os
# import psutil
 
# # inner psutil function
# def process_memory():
#     process = psutil.Process(os.getpid())
#     mem_info = process.memory_info()
#     return mem_info.rss
 
# # decorator function
# def profile(func):
#     def wrapper(*args, **kwargs):
 
#         mem_before = process_memory()
#         result = func(*args, **kwargs)
#         mem_after = process_memory()
#         print("{}:consumed memory: {:,}".format(
#             func.__name__,
#             mem_before, mem_after, mem_after - mem_before))
 
#         return result
#     return wrapper
 


# sameOne = set()


def process_result(i, j, val_i, val_j):

    product = val_i * val_j



    if is_square(product.numerator) and is_square(product.denominator):
        print("Square Free")
        print(i, j)
        print(val_i, val_j)
        print(val_i* val_j)
        logging.info("%s, %s, %s, %s", i, j, val_i, val_j)

def main():



    filename = 'Numbers/NumberAndOut1000.log'

    numberoflines = int(subprocess.check_output('wc -l ' + filename, shell=True).split()[0])
    print(numberoflines)
    i = 0
    counts = 0

    startTime = time.perf_counter()


    timeOfComputation = str(time.strftime("%Y %m %d-%H %M %S"))
    logging.basicConfig(filename=f'output/SquareFree_{timeOfComputation}.log', filemode='w', format='%(message)s', level=logging.INFO)
    
    logging.info("u, t, f(u), f(t)")

    with open(filename, 'r') as file1:
        progress = tqdm(total=numberoflines, desc="Processing", unit="line")
        for line1 in file1:
            temp1 = line1
            line1 = line1.split(',')
            with open(filename, 'r') as file2:
                for line2 in file2:
                    if temp1 == line2:
                        # i += 1
                        # if ( i % (numberoflines//100) == 0):
                        #     print( i/numberoflines * 100, "%")
                        break
                    
                    line2 = line2.split(',')
                    process_result(line1[0], line2[0], Fraction(line1[1]), Fraction(line2[1]))
                    # counts += 1

            progress.update(1)


    endTime = time.perf_counter()

    print(endTime - startTime, "seconds")
    print(i)
    print(counts)



if __name__ == "__main__":
    main()