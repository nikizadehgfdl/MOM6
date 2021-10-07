#!/bin/env python
#Inspired from https://github.com/wangsl/python-embedding
import sys
import numpy
import matplotlib.pyplot as plt
#A test function with one argument
def py_test1(x) :
    print(' **** From py_test1 ****')
    print("py_test1: Shape of the input array x : ", x.shape)
    print("py_test1: x[4,4,0] x[4,4,-1] : ", x[4,4,0],x[4,4,-1])

    plt.plot(x[4,4,:]);plt.show();

    a=numpy.max(x)
    print("py_test1: Returning the numpy.max(x): ", a)

    sys.stdout.flush()
    return a

#A test function with two arguments
def py_test2(x,y) :
    print(' **** From py_test2 ****',x,y)
    sys.stdout.flush()
    a=sum(x)
    b=sum(y)
    return a,b

#A test function with no arguments
def py_test0():
    print(' **** From py_test0 ****')
    sys.stdout.flush()
    return 

from math import sin
#import torch
#import numpy  #This gives all kinds of library errors at runtime

def my_test_torch(x, y) :
    print(' From Python test: {}'.format(x.size))
    for i in range(x.size) :
        x[i] += 1.0

    a = torch.from_numpy(x)
    print(a)

    b = torch.from_numpy(x.astype(numpy.float32))
    print(b)

    if torch.cuda.is_available() :
        b_dev = b.cuda()
        print(b_dev)

    for i in range(y.size) :
        y[i] = 2*numpy.float64(b[i]) + 0.1

    print(y)

    sys.stdout.flush()
    

if __name__ == '__main__' :
    import numpy as np

    x = np.arange(10, dtype=numpy.float64)
    print(x)

    y = np.arange(10, dtype=numpy.float64)
    print(y)
    my_test(x, y)

    print(x)


