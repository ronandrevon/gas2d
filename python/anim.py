import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import glob_colors as colors
import gas

n,nx,ny,r,m = 10,50,50,0.3,2.0
dt = 0.05
x  = np.array([np.arange(n)*3+0.25,[0.1]*n])#np.array([[0.75,0.1],[1.75,0.1]])
v  = np.array([[1]*n,[0.1]*n])#np.array([[1,0],[-1,0]])
xv = np.array(np.vstack([x,v]),dtype=float).T
params = dict(n=n,nx=nx,ny=ny,r=r,m=m)
xv0 = np.reshape(np.array(np.vstack([x,v]).T,dtype=float),4*n)
g = gas.Simulator()
g.set_xv(xv0)
g.init_simulator(n,nx,ny,r,m)
# g.print_dist()
# for i in range(10):
#     print(colors.red+"event %d" %i+colors.black)
#     info=g.next_event(False)#;g.print_dist()

time_template = 'time = %.1fs'

class PauseAnimation:
    def __init__(self,params,xv,dt,verbose=0):
        self.dt=dt
        self.verbose=verbose

        fig = plt.figure(figsize=(5, 5))
        ax = fig.add_subplot(autoscale_on=False, xlim=(-1, params['nx']+1), ylim=(-1, params['ny']+1))

        ax.set_aspect('equal')
        ax.grid()

        self.t = 0
        self.xv=xv
        # self.g = gas.Simulator(int(params['n']),int(params['nx']),int(params['ny']),float(params['r']),float(params['m']));
        self.next_event()

        self.p, = ax.plot(self.xv[:,0], self.xv[:,1],'o',markersize=92*params['r']*5/params['nx'])
        self.time_text = ax.text(0.05, 0.9, '', transform=ax.transAxes)

        self.animation = animation.FuncAnimation(
            fig, self.update, frames=100000, interval=5*self.dt, blit=True)
        self.paused = False

        fig.canvas.mpl_connect('key_press_event', self.toggle_pause)

    def toggle_pause(self,event):
        if event.key==' ':
            if self.paused:
                self.animation.resume()
            else:
                self.animation.pause()
            self.paused = not self.paused


    def next_event(self):
        # info=[1e10]+[-1]*6
        if self.verbose>0:
            print(colors.green+'Next event'+colors.black)
        info = g.next_event(self.verbose>1)
        # print(info)
        self.info=dict(zip(['t','ic','vx1','vy1','jc','vx2','vy2'],info))

    def update(self, i):

        if self.t+self.dt>=self.info['t']:
            if self.verbose>0:
                print(colors.red+'Event reached animation paused'+colors.black)
            t,ic,vx1,vy1,jc,vx2,vy2=self.info.values()
            ##update positions before event
            dt=self.info['t']-t
            self.xv[:,:2] += self.xv[:,2:]*dt
            ##update speeds of particles at the event
            self.xv[int(ic),2:]=[vx1,vy1]
            if jc>=0:
                self.xv[int(jc),2:]=[vx2,vy2]
            self.t=self.info['t']

            self.next_event()

            # print(colors.blue+'Press space to resume'+colors.black)
            # self.animation.pause()

        else:
            self.xv[:,:2] += self.xv[:,2:]*self.dt
            self.t+=self.dt

        self.p.set_data(self.xv[:,0],self.xv[:,1])
        self.time_text.set_text(time_template % (i*self.dt))

        return (self.p,self.time_text,)


pa = PauseAnimation(params,xv,dt,verbose=0)
plt.show()
