import gas
import numpy as np
import glob_colors as colors

n=2
# x = np.vstack([np.arange(n)+0.25,[0.1]*n]).T
# v = np.vstack([[0.25,0.7]]*n)#np.ones((5,2))
x = np.array([[0.75,0.1],[1.75,0.1]])
v = np.array([[1,0],[-1,0]])
xv = np.reshape(np.array(np.hstack([x,v]),dtype=float),4*n)

g = gas.Simulator()
g.set_xv(xv)
g.init_simulator(2,10,10,0.2,2.0)
g.print_dist()
for i in range(10):
    print(colors.red+"event %d" %i+colors.black)
    info=g.next_event(False)#;g.print_dist()
