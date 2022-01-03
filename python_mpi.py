from mpi4py import MPI
import numpy as np

TEST = True
SIZE = 20
numProcs = MPI.COMM_WORLD.Get_size()
rank = MPI.COMM_WORLD.Get_rank()
newSize=SIZE
n = int(SIZE/numProcs);
if SIZE%numProcs:
	n = n+1;
	newSize = n * numProcs;

a=[]
b=[]
c=[]

if rank==0:
	if TEST:
		a = np.loadtxt('a.txt')
		b = np.loadtxt('b.txt')
		c_test = np.loadtxt('c.txt')
	else:
		a = np.random.randint(20, size=(newSize, newSize))
		b = np.random.randint(20, size=(newSize, newSize))
	for i in range(newSize):
		for j in  range(newSize):
			if i>=SIZE or j>= SIZE:
				a[i,j]=b[i,j]=0;
	rows = np.split(a,numProcs)
	pid =0
	for row in rows:
		MPI.COMM_WORLD.send(row,dest = pid,tag=1)
		MPI.COMM_WORLD.send(b,dest = pid,tag=2)
		pid +=1
start = MPI.Wtime()
x = MPI.COMM_WORLD.recv(source=0, tag=1)
y = MPI.COMM_WORLD.recv(source=0, tag=2)
z = np.zeros((n,newSize), dtype=int)
for i in range(n):
	for j in range(newSize):
		for k in range(newSize):
			z[i,j] += x[i,k]*y[k,j]
MPI.COMM_WORLD.send(z,dest=0,tag=rank)
MPI.COMM_WORLD.barrier()
if rank ==0:
	for pid in range(numProcs):
		row = MPI.COMM_WORLD.recv(source=pid,tag=pid)
		if len(c):
			c = np.concatenate((c, row), axis=0)
		else:
			c = row
	end = MPI.Wtime()
	t = end - start
	print(f'Time: {t}')
	if TEST:
		if (c==c_test).all():
			print("Correct answer!\n")
		else:
			print("Wrong answer!\n")
