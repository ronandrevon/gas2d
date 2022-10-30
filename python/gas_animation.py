import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import glob_colors as colors
import gas,copy

class GasAnimation:
    def __init__(self,params,dt,verbose=0):
        self.t  = 0
        self.coll = 0
        self.dt = dt
        self.time_template = 'time = %.4fs'
        self.verbose = verbose
        self.params = params
        self.HIST_BINS = np.linspace(0,3,300)#np.hstack([np.arange(0,0.5,0.05),np.arange(0.5,1.5,0.01),np.arange(1.5,2,0.05),np.arange(2,5.1,0.2)])
        # self.g = gas.Simulator(int(params['n']),int(params['nx']),int(params['ny']),float(params['r']),float(params['m']));
        self.init_simulator()
        self.init_plots()

    def init_simulator(self):
        n,nx,ny,r,m = self.params['n'],self.params['nx'],self.params['ny'],self.params['r'],self.params['m']
        x,y = np.meshgrid(range(nx),range(ny))
        x = np.stack([np.reshape(x,nx*ny),np.reshape(y,nx*ny)])[:,:n]+0.05
        v  = np.random.rand(2,n)*2-1#np.array([[0.2]*n,[1]*n])#np.array([[1,0],[-1,0]])
        v/=np.linalg.norm(v,axis=0)

        self.xv = np.array(np.vstack([x,v]),dtype=float).T

        xv0 = np.reshape(np.array(np.vstack([x,v]).T,dtype=float),4*n)
        self.g = gas.Simulator()
        self.g.set_xv(xv0)
        self.g.init_simulator(n,nx,ny,r,m)
        self.next_event()
        # g.print_dist()
        # for i in range(10):
        #     print(colors.red+"event %d" %i+colors.black)
        #     info=g.next_event(True)#;g.print_dist()

    def init_plots(self):
        # fig = plt.figure(figsize=(5, 5))
        # ax = fig.add_subplot(autoscale_on=False, xlim=(-1, params['nx']+1), ylim=(-1, params['ny']+1))
        fig,(ax,ax_hist) = plt.subplots(nrows=1,ncols=2,figsize=(12, 8))#,dpi=dpi[0],**f_args)
        ax.set_xlim(-1,params['nx']+1)
        ax.set_ylim(-1,params['ny']+1)
        ax.set_aspect('equal')
        ax.grid()

        self.p, = ax.plot(self.xv[:,0], self.xv[:,1],'o',markersize=92*params['r']*5/params['nx'])
        self.time_text = ax.text(0.05, 0.9, '', transform=ax.transAxes)


        _, _, self.bar_container = ax_hist.hist(self.v(), self.HIST_BINS, lw=1,density=True,
                              ec="k", fc="blue", alpha=0.5)

        ax_hist.set_ylim(top=params['n'])

        self.animation = animation.FuncAnimation(
            fig, self.update, frames=100000, interval=10, blit=False)
        self.paused = False

        fig.canvas.mpl_connect('key_press_event', self.toggle_pause)

    def v(self):
        return np.linalg.norm(self.xv[:,2:],2,axis=1)


    def update(self,i):
        return self.update_balls_view(i)
        # return (*patches, p,time_text,)
        # return ( p,time_text,)

    def update_histogram(self):
        n, _ = np.histogram(self.v(), self.HIST_BINS,density=False)
        if self.verbose:
            print('collisions : %d ' %self.coll)
        for count, rect in zip(n, self.bar_container.patches):
            rect.set_height(count)

    def update_balls_view(self, i):

        if self.t+self.dt>=self.info['t']:
            if self.verbose>0:
                print(colors.red+'Event reached'+colors.black)
            t,ic,vx1,vy1,jc,vx2,vy2=self.info.values()
            ##update positions before event
            dt=self.info['t']-self.t
            if self.verbose:
                print("dt=%.8f"%dt)
            self.xv[:,:2] += self.xv[:,2:]*dt
            ##update speeds of particles at the event
            self.xv[int(ic),2:]=[vx1,vy1]
            if jc>=0:
                self.coll+=1
                self.xv[int(jc),2:]=[vx2,vy2]
                self.update_histogram()
            self.t=copy.copy(self.info['t'])

            self.next_event()
            # print(colors.blue+'Press space to resume'+colors.black)
            # self.animation.pause()

        else:
            self.xv[:,:2] += self.xv[:,2:]*self.dt
            self.t+=self.dt

        self.p.set_data(self.xv[:,0],self.xv[:,1])
        self.time_text.set_text(self.time_template % (self.t ))

        return (*self.bar_container.patches,self.p,self.time_text,)

    def toggle_pause(self,event):
        if event.key==' ':
            if self.paused:
                self.animation.resume()
            else:
                self.animation.pause()
            self.paused = not self.paused

    def next_event(self):
        # info=[1e10]+[-1]*6
        info = self.g.next_event(self.verbose>1)
        self.info=dict(zip(['t','ic','vx1','vy1','jc','vx2','vy2'],info))
        if self.verbose>0:
            print(colors.green+'Next event at %.7f ' %self.info['t']+colors.black)

dt = 0.01 #real time speed would be
n,nx,ny,r,m = 10,20,20,0.3,2.0
params = dict(n=n, nx=nx, ny=ny, r=r, m=m)
ga = GasAnimation(params,dt,verbose=1)
plt.show()
