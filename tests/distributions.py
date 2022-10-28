from utils import*
# import numpy as np
from subprocess import Popen,PIPE,check_output
import struct

n=200000;
steps=100
nx,ny=1000,1000
r=0.25

if 'r' in opt:
    cmd='bin/gas2d -n%d -N%d -x%d -y%d -r%.2f' %(n,steps,nx,ny,r)
    o = check_output(cmd,shell=True).decode()
    print(o)

with open('data/vel.bin','rb') as f:d=f.read()
v = np.reshape(np.array(struct.unpack('f'*2*n,d)),(n,2))

##plot
vm=np.round(abs(v).max())
dsp.stddisp(scat=[v[:,0],v[:,1]],xylims=vm)
plt.show()
