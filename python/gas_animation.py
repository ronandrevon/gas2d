import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import glob_colors as colors
import gas,copy,threading

class GasAnimation:
    def __init__(self,params,dt,v0,verbose=0):
        self.t    = 0
        self.coll = 0
        self.dt   = dt
        self.time_template = 'time = %.4f ns'
        self.verbose = verbose
        self.params  = params
        self.HIST_BINS = np.linspace(0,3*v0,300)#np.hstack([np.arange(0,0.5,0.05),np.arange(0.5,1.5,0.01),np.arange(1.5,2,0.05),np.arange(2,5.1,0.2)])
        self.init_simulator(v0)
        self.init_plots()
        ###
        self.animation = animation.FuncAnimation(
            self.fig, self.update, frames=100000, interval=50, blit=False)
        self.paused = False
        self.fig.canvas.mpl_connect('key_press_event', self.toggle_pause)

    def init_simulator(self,v0):
        n,nx,ny,r,m = self.params['n'],self.params['nx'],self.params['ny'],self.params['r'],self.params['m']
        x,y = np.meshgrid(range(nx),range(ny))
        x  = np.stack([np.reshape(x,nx*ny),np.reshape(y,nx*ny)])[:,:n]+0.05
        #random speeds
        v  = np.random.rand(2,n)*2-1 #np.array([[0.2]*n,[1]*n])#np.array([[1,0],[-1,0]])
        v /= np.linalg.norm(v,axis=0)
        v *= v0

        self.xv = np.array(np.vstack([x,v]),dtype=float).T
        self.xv0 = np.reshape(np.array(np.vstack([x,v]).T,dtype=float),4*n)
        self.g = gas.Simulator()
        self.g.set_xv(self.xv0)
        self.g.init_simulator(n,nx,ny,r,m)

        self.dv = np.zeros(nx,dtype=float)
        self.g.update_dv(self.dv)
        # g.print_dist()
        # for i in range(10):
        #     print(colors.red+"event %d" %i+colors.black)
        #     info=g.next_event(True)#;g.print_dist()

        # self.next_event()
        self.trd_simu = threading.Thread(target=self.g.step_until_dt(self.dt))
        self.trd_simu.start()


    def init_plots(self):
        self.fig,(ax,ax_hist) = plt.subplots(nrows=1,ncols=2,figsize=(12, 8))#,dpi=dpi[0],**f_args)

        ### visualizer
        ax.set_xlim(-1,params['nx']+1)
        ax.set_ylim(-1,params['ny']+1)
        ax.set_aspect('equal')
        ax.grid()
        self.p, = ax.plot(self.xv[:,0], self.xv[:,1],'o',markersize=92*params['r']*5/params['nx'])
        self.time_text  = ax.text(0.05, 0.90, '', transform=ax.transAxes)
        # self.T_text     = ax.text(0.05, 0.85, '', transform=ax.transAxes)
        # self.P_text     = ax.text(0.05, 0.85, '', transform=ax.transAxes)
        # self.rho_text   = ax.text(0.05, 0.75, '', transform=ax.transAxes)

        ### histogram
        _, _, self.bar_container = ax_hist.hist(self.v(), self.HIST_BINS, lw=1,density=True,
                              ec="k", fc="blue", alpha=0.5)
        ax_hist.set_ylim(top=params['n'])


    #### update functions
    def update(self,i):
        # return self.update_balls_view(i)
        self.update_data()
        return self.update_animation()

    def update_animation(self):
        self.p.set_data(self.xv[:,0],self.xv[:,1])
        self.time_text.set_text(self.time_template % (self.t ))
        return (*self.bar_container.patches,self.p,self.time_text,)

    def update_data(self):
        '''let the simulator run for a time dt '''
        self.trd_simu.join()
        self.t    = self.g.time()
        self.coll = self.g.collisions()
        self.bottom_collisions = self.g.bottom_collisions()
        self.g.update_dv(self.dv)
        # self.g.get_xv(self.xv0)
        # print(self.xv0)
        n,m,nx,ny = self.params['n'],self.params['m'],self.params['nx'],self.params['ny']
        self.xv = np.reshape(self.xv0,(n,4))
        if self.verbose:
            v = self.v()
            p = pressure(m=m,dv=self.dv,dt=self.dt,S=nx*1)
            print()
            print(colors.red+'time=%.2f ns' %self.t +colors.black)
            print('collisions particles = %.d' %self.coll)
            print('collisions bottom    = %.d' %self.bottom_collisions)
            print('v   = %.2f m/s'  %(v.mean()*100))
            print('T   = %.2f K'  %temperature(m,v))
            print('P   = %.2fbar' %p.mean()) #;print(self.dv.mean())
            print('rho = %.2E kg/m3' %rho(n=n,V=nx*ny*1,m=m))

        self.update_histogram()
        self.trd_simu = threading.Thread(target=self.g.step_until_dt(self.dt))
        self.trd_simu.start()


    def update_histogram(self):
        n, _ = np.histogram(self.v(), self.HIST_BINS,density=False)
        for count, rect in zip(n, self.bar_container.patches):
            rect.set_height(count)

    def update_balls_view(self, i):
        ''' Animates the balls on collision based.
        If a collision happens before time step the frame stops at the time of the collision
        '''
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
    def next_event(self):
        # info=[1e10]+[-1]*6
        info = self.g.next_event(self.verbose>1)
        self.info=dict(zip(['t','ic','vx1','vy1','jc','vx2','vy2'],info))
        if self.verbose>0:
            print(colors.green+'Next event at %.7f ' %self.info['t']+colors.black)



    #### misc utils
    def v(self):
        return np.linalg.norm(self.xv[:,2:],2,axis=1)

    def T(self):
        return temperature(m=self.params['m'],v=self.v())

    def toggle_pause(self,event):
        if event.key==' ':
            if self.paused:
                self.animation.resume()
            else:
                self.animation.pause()
            self.paused = not self.paused

def pressure(m,dv,dt,S):
    '''pressure (Bar) from impulsion, time, and area
    - m : g/mol
    - dv : v=1 => 100m/s
    - dt : ns
    - S : nm^2
    '''
    return (m*1e-3/Na)*dv*1e2/(dt*1e-9)/(S*1e-18)*1e-5

def temperature(m,v):
    ''' computes the temperature in K from distribution
    - m : g/mol
    - v : v=1 => v=100m/s
    '''
    # mv2 = m*1e-3*np.sqrt(np.mean(v**2))*1e2
    mv2 = np.mean(m*v**2)*10
    T  = mv2/(3*R) #K
    return T

def rho(n,V,m):
    ''' computes the volumetric mass in kg/m3
    - n : nb particle
    - V : volume in nm^3
    - m : g/mol
    - Note : air at SPT rho=1.225kg/m3
    '''
    Vm = n/(V*1e-27)/Na*1e3 #mol/L
    rho = Vm*(m*1e-3)*1e-3 #kg/m3
    return rho

atm = 101325
Na  = 6.02214076e23
R   = Na*1.38064852e-23
dt  = 5  #ns
v   = 1     #100m/s=100nm/ns=1nm/dt
mN2 = 2*14  #g/mol
n,nx,ny,r,m = 10000,500,500,0.1,28.0 # dimension unit(nm) #m(ng)

v_avg = 5
coll  = dt*v_avg/nx #number of collisions over dt by single particle
ncoll_dt = 1/2*n*coll
# print('v=%.2f m/s'  %(v_avg*100))
# print('T=%.2f K'  %temperature(m,v_avg))
# print('P=%.2fbar' %pressure(m=m,dv=2*v_avg*ncoll_dt,dt=dt,S=nx*1))
# print('rho=%.2E kg/m3' %rho(n=n,V=nx*ny*1,m=m))
params = dict(n=n, nx=nx, ny=ny, r=r, m=m)
ga = GasAnimation(params,dt,v0=v_avg,verbose=1)
plt.show()
