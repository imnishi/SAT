from __future__ import print_function
from pysat.solvers import Solver
import csv
import random

def check(k,sudoku1,sudoku2,lsr) :

    ks = k*k
    k4 = k*k*k*k
    k6 = k*k*k*k*k*k

    solver = Solver()
    for sudoku in range(0,2):

        #all the cells should have exactly 1 value
        # print("1")

        for row in range(0,ks):
            for col in range(0,ks):
                cl = []
                for val in range(1,ks+1):
                    cl.append(int(sudoku*k6 + row*k4 + col*ks+ val))
                solver.add_clause(cl)
                #cl.clear()
        
        for row in range(0,ks):
            for col in range(0,ks):
                for val1 in range(1,ks+1):
                    for val2 in range(val1+1,ks+1):
                        cl = []
                        cl.append(int(-(sudoku*k6 + row*k4 + col*ks+ val1)))
                        cl.append(int(-(sudoku*k6 + row*k4 + col*ks+ val2)))
                        solver.add_clause(cl)
                        # cl.clear()

        
        #all the boxes should have each value exactly once
        # print("2")

        for brow in range(0,k):
            for bcol in range(0,k):
                for val in range(1,ks+1):
                    cl =[]
                    for row in range(0,k):
                        for col in range(0,k):
                            cl.append(int(sudoku*k6 + (brow*k + row)*k4 + (bcol*k + col)*ks+ val))
                    solver.add_clause(cl)
                    # cl.clear()

        for brow in range(0,k):
            for bcol in range(0,k):
                for val in range(1,ks+1):
                    for row1 in range(0,k):
                        for col1 in range(0,k):
                            for row2 in range(row1,k):
                                for col2 in range(col1,k):
                                    if (row1==row2 and col1==col2) :
                                        continue
                                    else:
                                        cl = []
                                        cl.append(int(-(sudoku*k6 + (brow*k + row1)*k4 + (bcol*k + col1)*ks+ val)))
                                        cl.append(int(-(sudoku*k6 + (brow*k + row2)*k4 + (bcol*k + col2)*ks+ val)))
                                        solver.add_clause(cl)
                                        # cl.clear()


        #all the coloumns should have each value exactly once
        # print("3")

        for col in range(0,ks):
            for val in range(1,ks+1):
                cl = []
                for row in range(0,ks):
                    cl.append(int(sudoku*k6 + row*k4 + col*ks+ val))
                solver.add_clause(cl)
                # cl.clear()

        for col in range(0,ks):
            for val in range(1,ks+1):
                for row1 in range(0,ks):
                    for row2 in range(row1+1,ks):
                        cl = []
                        cl.append(int(-(sudoku*k6 + row1*k4 + col*ks+ val)))
                        cl.append(int(-(sudoku*k6 + row2*k4 + col*ks+ val)))
                        solver.add_clause(cl)
                        # cl.clear()


        #all the rows should have each value exactly once
        # print("4")

        for row in range(0,ks):
            for val in range(1,ks+1):
                cl = []
                for col in range(0,ks):
                    cl.append(int(sudoku*k6 + row*k4 + col*ks+ val))
                solver.add_clause(cl)
                # cl.clear()

        for row in range(0,ks):
            for val in range(1,ks+1):
                for col1 in range(0,ks):
                    for col2 in range(col1+1,ks):
                        cl = []
                        num = -(sudoku*k6 + row*k4 + col1*ks+ val)
                        cl.append(int(-(sudoku*k6 + row*k4 + col1*ks+ val)))
                        cl.append(int(-(sudoku*k6 + row*k4 + col2*ks+ val)))
                        solver.add_clause(cl)
                        # cl.clear()


    #both sudoku should have unique values at same location
    # print("5")

    for row in range(0,ks):
        for col in range(0,ks):
            for val in range(1,ks+1):
                cl = []
                cl.append(int(-(0*k6 + row*k4 + col1*ks+ val)))
                cl.append(int(-(1*k6 + row*k4 + col2*ks+ val)))
                solver.add_clause(cl)
                # cl.clear()


    #adding already known values of both sudoku
    # print("6")

    for row in range(0,ks):
        for col in range(0,ks):
            if (sudoku1[row][col]!=0) :
                cl = []
                cl.append(int(0*k6 + row*k4 + col*ks+ sudoku1[row][col]))
                solver.add_clause(cl)
                # cl.clear()

    for row in range(0,ks):
        for col in range(0,ks):
            if (sudoku2[row][col]!=0) :
                cl = []
                cl.append(int(1*k6 + row*k4 + col*ks+ sudoku2[row][col]))
                solver.add_clause(cl)
                # cl.clear()

    for itr in range(0,len(lsr)) :
        cl = []
        cl.append(int(-(lsr[itr][0]*k6 + lsr[itr][1]*k4 + lsr[itr][2]*ks+ lsr[itr][3])))
        solver.add_clause(cl)

    # print(solver.solve())
    return solver.solve()

if __name__=="__main__" :

    k = int(input("parameter k: "))
    ks = k*k
    k4 = k*k*k*k
    k6 = k*k*k*k*k*k

    if k==2:
        with open('a2.csv') as csvfile:
            rows = csv.reader(csvfile)
            res = list(zip(*rows))
    elif k==3:
        with open('a3.csv') as csvfile:
            rows = csv.reader(csvfile)
            res = list(zip(*rows))
    elif k==4:
        with open('a4.csv') as csvfile:
            rows = csv.reader(csvfile)
            res = list(zip(*rows))
    elif k==5:
        with open('a5.csv') as csvfile:
            rows = csv.reader(csvfile)
            res = list(zip(*rows))
    else :
        print("data set not present for this k")
        quit()

    sudoku1 = []
    for j in range(0,ks):
        l = []
        for i in range(0,ks):
            l.append(int(res[i][j]))
        sudoku1.append(l)

    sudoku2 = []
    for j in range(ks,2*ks):
        l = []
        for i in range(0,ks):
            l.append(int(res[i][j]))
        sudoku2.append(l)

    flag = 0
    res = False
    lsr = []


    while res==False :
        # print("a")
        row = random.randint(0,ks-1)
        col = random.randint(0,ks-1)
        # print(row)
        # print(col)

        if flag==0 :
            if sudoku1[row][col]==0 :
                continue
            lsr.append([0,row,col,sudoku1[row][col]])
            sudoku1[row][col] = 0
            res = check(k, sudoku1, sudoku2, lsr)
            # print(res)
            flag = 1
            # print(flag)
            continue
        else :
            if sudoku2[row][col]==0 :
                continue
            lsr.append([1,row,col,sudoku2[row][col]])
            sudoku2[row][col] = 0
            res = check(k, sudoku1, sudoku2, lsr)
            # print(res)
            flag = 0
            # print(flag)
            continue
    
    if (lsr[len(lsr)-1][0]==0) :
        sudoku1[lsr[len(lsr)-1][1]][lsr[len(lsr)-1][2]] = lsr[len(lsr)-1][3]
    else :
        sudoku2[lsr[len(lsr)-1][1]][lsr[len(lsr)-1][2]] = lsr[len(lsr)-1][3]

    print("sudoku1")
    for i in range(0,ks):
        for j in range(0,ks):
            if sudoku1[i][j]<10 :
                print(" ", end='')
            print(sudoku1[i][j],end=" ")
        print()

    print()
    print("sudoku2")
    for i in range(0,ks):
        for j in range(0,ks):
            if sudoku2[i][j]<10 :
                print(" ", end='')
            print(sudoku2[i][j],end=" ")
        print()