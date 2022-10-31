import importlib as imp
import copy,threading,matplotlib
import matplotlib.pyplot as plt,numpy as np
import matplotlib.animation as animation
# import matplotlib.widgets import Slider, Button
import gas
import thermo_utils as ut;imp.reload(ut)
import glob_colors as colors

class GasAnimation:
    def __init__(self,params,dt,y0,v0,dt_int,ntop=0,verbose=0):
        ''' Animate the Gas Simulator class
        - params : dict('n','nx','ny','r','m')
        - dt : Simulation time (in hundreadth of ns) between each frame
        - y0 : initial height of the container
        - v0 : initial average velocities
        - dt_int : refresh time between each frame
        - ntop : height of the histogram axis
        '''
        self.dt   = dt
        self.verbose = verbose
        self.params  = params
        nv0s = 5
        self.HIST_BINS = np.linspace(0,nv0s*v0,nv0s*100)#np.hstack([np.arange(0,0.5,0.05),np.arange(0.5,1.5,0.01),np.arange(1.5,2,0.05),np.arange(2,5.1,0.2)])
        self.init_simulator(y0,v0)
        self.init_plots(ntop)
        self.update_histogram()
        self.info()

        ###
        self.animation = animation.FuncAnimation(
            self.fig, self.update, frames=100000, interval=dt_int, blit=False)
        self.paused = False
        self.fig.canvas.mpl_connect('key_press_event', self.key_event)

        self.trd_simu = threading.Thread(target=self.g.step_until_dt(self.dt,self.y))
        self.trd_simu.start()

    def init_simulator(self,y0,v0):
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

        self.t    = 0
        self.dv   = np.zeros(2*nx,dtype=float)
        self.coll = 0
        self.collisions_bottom = 0
        self.collisions_top    = 0
        self.g.update_dv(self.dv)
        self.y = y0

        # self.next_event() # for individual collision based animations


    def init_plots(self,ntop):
        self.fig,(self.ax,_,_,self.ax_hist) = plt.subplots(nrows=1,ncols=4,figsize=(15, 10))#,dpi=dpi[0],**f_args)

        ### visualizer
        self.ax.set_xlim(-1,params['nx']+1)
        self.ax.set_ylim(-1,params['ny']+1)
        self.ax.set_aspect('equal')
        self.ax.grid()
        # self.ax.axis('off')
        self.p,        = self.ax.plot(self.xv[:,0], self.xv[:,1],'o',markersize=92*params['r']*5/params['nx'])
        self.time_text = self.ax.text(0.05, 0.90, '', transform=self.ax.transAxes)
        # self.T_text     = ax.text(0.05, 0.85, '', transform=ax.transAxes)
        # self.P_text     = ax.text(0.05, 0.85, '', transform=ax.transAxes)
        # self.rho_text   = ax.text(0.05, 0.75, '', transform=ax.transAxes)
        nx,ny = [self.params[k] for k in ['nx','ny']]
        self.patch = matplotlib.patches.Rectangle((0,0),nx,self.y,alpha=0.9,fill=0,ec='k',lw=2)
        self.ax.add_patch(self.patch)
        ### histogram
        _, _, self.bar_container = self.ax_hist.hist(self.v(), self.HIST_BINS, lw=1,density=True,
                              ec="k", fc="blue", alpha=0.5)
        if not ntop:
            self.ntop = self.params['n']/10
        self.ax_hist.set_ylim(top=self.ntop)

        ##control volume through y slider
        ax_slider = self.fig.add_axes([0.1, 0.01, 0.4, 0.03])
        self.y_slider = matplotlib.widgets.Slider(ax=ax_slider,
            label='y',valmin=1,valmax=ny,valinit=self.y,
            )
        self.y_slider.on_changed(self.update_slider)

    #### update functions
    def update(self,i):
        # return self.update_balls_view(i)
        self.update_data()
        return self.update_animation()

    def update_slider(self,val):
        self.y=int(val)
        # self.patch.set_height(self.y)
        # print(self.patch.get_height())
        # self.patch = matplotlib.patches.Rectangle((0,0),nx,self.y,alpha=0.9,fill=0,ec='k',lw=2)
        # self.fig.canvas.draw_idle()

    def update_animation(self):
        self.p.set_data(self.xv[:,0],self.xv[:,1])
        self.ax_hist.set_ylim(top=self.ntop)
        self.time_text.set_text('time = %.4f ns' % (self.t/100 ))
        self.patch.set_height(self.y)
        return (*self.bar_container.patches,self.p,self.time_text,self.patch,)

    def info(self):
        n,m,nx,ny = self.params['n'],self.params['m'],self.params['nx'],self.params['ny']
        v = self.v()
        T    = ut.temperature(m,v)
        p_bottom = ut.pressure(m=m,dv=self.dv[:nx],dt=self.dt,S=1)
        p_top    = ut.pressure(m=m,dv=self.dv[nx:],dt=self.dt,S=1)
        P_gp = ut.P_gp(n,T,V=nx*self.y*1)
        rho  = ut.rho(n=n,V=nx*self.y*1,m=m)
        S    = ut.entropy(self.nv/n)
        print()
        print(colors.red+'time=%.2f ps' %(self.t*10) +colors.black)
        print('collisions particles = %.d' %self.coll)
        print('collisions top       = %.d' %self.collisions_top)
        print('collisions bottom    = %.d' %self.collisions_bottom)
        print('v     = %.2f m/s'  %(v.mean()*100))
        print('T     = %.2f K'  %T)
        print('P_top = %.2fbar' %p_top.mean()) #;print(self.dv.mean())
        print('P_bot = %.2fbar' %p_bottom.mean()) #;print(self.dv.mean())
        print('P_gp  = %.2fbar' %P_gp)
        print('rho   = %.2E kg/m3' %rho)
        print('Entropy = %.2f' %S)

    def update_data(self):
        '''let the simulator run for a time dt '''
        self.trd_simu.join()
        self.t    = self.g.time()
        self.coll = self.g.collisions()
        self.collisions_top     = self.g.top_collisions()
        self.collisions_bottom  = self.g.bottom_collisions()
        self.g.update_dv(self.dv)
        # self.g.get_xv(self.xv0)
        # print(self.xv0)
        self.xv = np.reshape(self.xv0,(self.params['n'],4))
        if self.verbose:
            self.info()
        self.update_histogram()
        self.trd_simu = threading.Thread(target=self.g.step_until_dt(self.dt,self.y))
        self.trd_simu.start()


    def update_histogram(self):
        self.nv, _ = np.histogram(self.v(), self.HIST_BINS,density=False)
        for count, rect in zip(self.nv, self.bar_container.patches):
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

    def key_event(self,event):
        print(event.key)
        if event.key==' ':
            self.toggle_pause()
        elif event.key == '/':
            self.ntop/=2
        elif event.key == '.':
            self.ntop*=2

        elif event.key in ['down','left','up','right']:
            if event.key =='up':
                self.y+=5
            if event.key =='right':
                self.y+=1
            elif event.key in ['down']:
                self.y-=5
            elif event.key in ['left']:
                self.y-=1
            self.y=min(self.params['ny'],max(10,self.y))
            self.y_slider.set_val(self.y)

    def toggle_pause(self):
        if self.paused:
            self.animation.resume()
        else:
            self.animation.pause()
        self.paused = not self.paused



atm = 101325
dt  = 10    #dt=1 => dt=10ps    (t0=10ps)
v0  = 5     #v=1  => v=100m/s   (v0=100nm/ns=1nm/t0)
mN2 = 2*14  #g/mol
n,nx,ny,r,m = 2000,50,200,0.1,mN2 # dimension unit(nm) #m(ng)

params = dict(n=n, nx=nx, ny=ny, r=r, m=m)
ga = GasAnimation(params,dt,y0=int(ny/2),v0=v0,dt_int=100,ntop=0,verbose=1)
plt.show()
