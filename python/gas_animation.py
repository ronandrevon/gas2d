import importlib as imp
import copy,threading,matplotlib
import matplotlib.pyplot as plt,numpy as np
import matplotlib.animation as animation
from matplotlib.image import NonUniformImage
# import matplotlib.widgets import Slider, Button
import gas
import thermo_utils as ut;imp.reload(ut)
import glob_colors as colors
matplotlib.rc('text', usetex=False)

class GasAnimation:
    def __init__(self,params,dt,y0,v0,dt_int,
        ntop=0,plot_rho=True,avg=True,Nx=10,
        verbose=0):
        ''' Animate the Gas Simulator class
        - params : dict('n','nx','ny','r','m')
        - dt : Simulation time (in hundreadth of ns) between each frame
        - y0 : initial height of the container
        - v0 : initial average velocities
        - dt_int : refresh time between each frame
        - ntop   : height of the histogram axis
        - plot_rho : plots rho if True, else shows the particles (can be changed with shortcut "n")
        - Nx  : size of imshow grid along x(automatic calculation along y)
        - avg : if True computes rho and T as averages on a a Nx x Ny grid
        '''
        self.dt   = dt
        self.verbose  = verbose
        self.params   = params
        self.plot_rho = plot_rho
        self.Nx  = Nx
        self.avg = avg
        self.init_simulator(y0,v0)
        self.init_plots(v0,ntop)
        self.update_histogram()
        self.info()
        ###
        self.animation = animation.FuncAnimation(
            self.fig, self.update, frames=100000, interval=dt_int, blit=False)
        self.paused = False
        self.fig.canvas.mpl_connect('key_press_event', self.key_event)

        self.run_simu()

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
        self.xv2D = np.zeros(2*ny*nx,dtype=float)
        self.coll = 0
        self.collisions_bottom = 0
        self.collisions_top    = 0
        self.g.update_dv(self.dv)
        self.y = y0
        self.V = nx*self.y*1
        self.Ny = int(np.ceil(self.Nx*self.y/nx))

        # self.next_event() # for individual collision based animations

    def init_ax(self,s,label):
        self.ax[s].set_xlim(-1,self.params['nx']+1)
        self.ax[s].set_ylim(-1,self.params['ny']+1)
        self.ax[s].set_aspect('equal')
        self.ax[s].grid()
        self.ax[s].axis('off')
        self.ax[s].add_patch(self.patches[s])
        self.ax[s].set_title(label)

    def init_plots(self,v0,ntop):
        self.fig,ax = plt.subplots(nrows=1,ncols=3,figsize=(15, 10))#,dpi=dpi[0],**f_args)
        self.ax = dict(zip(['rho','T','v'],ax))

        nx,ny = [self.params[k] for k in ['nx','ny']]
        self.artists = dict()
        self.patches = {k:matplotlib.patches.Rectangle((0,0),nx,self.y,alpha=0.9,fill=0,ec='k',lw=2) for k in ['rho','T']}
        self.init_ax('rho'  ,label=r'$\rho(kg/m^3)$')
        self.init_ax('T'    ,label=r'$T(K)$')

        self.init_axN(v0,ntop,nv0s=5)
        self.init_ax1()
        self.init_axT()

        #### control volume through y slider
        ax_slider = self.fig.add_axes([0.1, 0.03, 0.65, 0.03])
        self.y_slider = matplotlib.widgets.Slider(ax=ax_slider,
            label='y',valmin=1,valmax=ny,valinit=self.y,
            )
        self.y_slider.on_changed(self.update_slider)

    def init_axN(self,v0,ntop,nv0s):
        self.HIST_BINS = np.linspace(0,nv0s*v0,nv0s*100)
        _, _, self.bar_container = self.ax['v'].hist(self.v(), self.HIST_BINS, lw=1,density=True,
                              ec="k", fc="blue", alpha=0.5)
        if not ntop:
            self.ntop = self.params['n']/10
        self.ax['v'].set_ylim(top=self.ntop)
        self.ax['v'].set_title('Speed distribution')

    def init_axT(self):
        nx,ny,m = self.params['nx'],self.params['ny'],self.params['m']
        Tmax = ut.temperature(m,self.HIST_BINS[-1])
        artist = self.ax['T'].imshow(self.T(),extent=[0,nx,0,self.y],origin='lower',
            interpolation='bilinear',vmin=0,vmax=Tmax,cmap='jet')
        self.artists['T'] = (artist,)

    def init_ax1(self):
        self.update_ax1=[self.update_n,self.update_rho][self.plot_rho]
        n,nx,ny,m = self.params['n'],self.params['nx'],self.params['ny'],self.params['m']
        ymin = 5
        rho_max  = ut.rho(n=n,V=nx*ymin*1,m=m)
        if self.plot_rho:
            p = self.ax['rho'].imshow(self.rho(),extent=[0,nx,0,self.y],origin='lower',
                interpolation='bilinear',vmin=0,vmax=rho_max,cmap='jet')
        else:
            p,= self.ax['rho'].plot(self.xv[:,0], self.xv[:,1],'o',markersize=92*self.params['r']*5/nx)
        self.artists['rho'] = (p,)
        # self.time_text = self.ax['rho'].text(0.05, 0.90, '', transform=self.ax['rho'].transAxes)

    ############################################################################
    #### update functions
    ############################################################################
    def update(self,i):
        # return self.update_balls_view(i)
        self.update_data()
        artists = self.update_animation()
        self.run_simu()
        return artists

    def update_slider(self,val):
        self.y  = int(val)
        self.Ny = int(np.ceil(self.Nx*self.y/self.params['nx']))
        self.V  = self.y*self.params['nx']*1
        if self.plot_rho:
            self.artists['rho'][0].set_extent([0,self.params['nx'],0,self.y])
        self.artists['T'][0].set_extent([0,self.params['nx'],0,self.y])

    def update_n(self):
        self.artists['rho'][0].set_data(self.xv[:,0],self.xv[:,1])
        # self.time_text.set_text('time = %.4f ns' % (self.t/100 ))

    def update_rho(self):
        self.artists['rho'][0].set_data(self.rho())

    def update_T(self):
        # nx,ny = self.params['nx'],self.params['ny']
        # T = np.reshape(self.xv2D[nx*ny:],(nx,ny))**2
        self.artists['T'][0].set_data(self.T())
        # print(T.mean(),self.artists['T'][0].get_array().mean())

    def update_histogram(self):
        self.nv, _ = np.histogram(self.v(), self.HIST_BINS,density=False)
        for count, rect in zip(self.nv, self.bar_container.patches):
            rect.set_height(count)
        self.ax['v'].set_ylim(top=self.ntop)

    def update_animation(self):
        self.update_ax1()
        self.update_T()
        self.update_histogram()
        for p in self.patches.values():
            p.set_height(self.y)
        return (*self.artists['rho'],*self.artists['T'],
            *self.bar_container.patches,*self.patches)

    def update_data(self):
        '''let the simulator run for a time dt '''
        self.trd_simu.join()
        self.t    = self.g.time()
        self.coll = self.g.collisions()
        self.collisions_top     = self.g.top_collisions()
        self.collisions_bottom  = self.g.bottom_collisions()
        self.g.update_dv(self.dv)#dv is reset to 0 in the c++ simulator
        self.xv2D = np.zeros(2*self.params['ny']*self.params['nx'],dtype=float)
        self.g.get_distribution(self.xv2D)
        # print(self.xv2D.mean())
        # self.g.get_xv(self.xv0)
        # print(self.xv0)
        self.xv = np.reshape(self.xv0,(self.params['n'],4))
        if self.verbose:
            self.info()

    def run_simu(self):
        self.trd_simu = threading.Thread(target=self.g.step_until_dt(self.dt,self.y))
        self.trd_simu.start()

    ############################################################################
    #### old update
    ############################################################################
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

    ################################################################################
    #### event callbacks
    ################################################################################
    def toggle_pause(self):
        if self.paused:
            self.animation.resume()
        else:
            self.animation.pause()
        self.paused = not self.paused

    def key_event(self,event):
        print(event.key)
        if event.key==' ':
            self.toggle_pause()
        elif event.key == '/':
            self.ntop/=2
        elif event.key == '.':
            self.ntop*=2
        elif event.key == 'n':
            self.plot_rho=not self.plot_rho
            for a in self.artists['rho']:a.remove()
            self.init_ax1()

        elif event.key in ['down','left','up','right']:
            if event.key =='up':
                self.y+=5
            if event.key =='right':
                self.y+=1
            elif event.key in ['down']:
                self.y-=5
            elif event.key in ['left']:
                self.y-=1
            self.y=min(self.params['ny'],max(2,self.y))
            self.y_slider.set_val(self.y)

    ################################################################################
    #### misc utils
    ################################################################################
    def v(self):
        return np.linalg.norm(self.xv[:,2:],2,axis=1)
    def rho(self):
        nx,ny,m,n = self.params['nx'],self.params['ny'],self.params['m'],self.params['n']
        rho_ = (np.reshape(self.xv2D[:nx*ny],(ny,nx)))/n
        rho_[:self.y,:]
        # print('rho check_sum',rho_.sum())#rho.mean(),self.artists['rho'][0].get_array().mean())
        if self.avg:
            f    = lambda n:ut.rho(n,self.V,m).mean()
            f    = np.sum
            rho_ = average_blocks(rho_,self.Ny,self.Nx,f)
            # print(rho_.shape,rho_[0,0])
            # print('rho check_sum',rho_.sum(),rho_.shape)
        return rho_

    def T(self):
        nx,ny = self.params['nx'],self.params['ny']
        T_ = np.reshape(self.xv2D[nx*ny:],(ny,nx))
        T_ = T_[:self.y,:]
        if self.avg:
            f  = lambda v:ut.temperature(self.params['m'],v)
            T_ = average_blocks(T_,self.Ny,self.Nx,f)
        return T_

    def info(self):
        n,m,nx,ny = self.params['n'],self.params['m'],self.params['nx'],self.params['ny']
        v = self.v()
        T    = ut.temperature(m,v)
        p_bottom = ut.pressure(m=m,dv=self.dv[:nx],dt=self.dt,S=1)
        p_top    = ut.pressure(m=m,dv=self.dv[nx:],dt=self.dt,S=1)
        P_gp = ut.P_gp(n,T,V=nx*self.y*1)
        rho  = ut.rho(n=n,V=self.V,m=m)
        S    = ut.entropy(self.nv/n)
        print()
        print(colors.red+'time=%.2f ps' %(self.t*10) +colors.black)
        print('collisions particles = %.d' %self.coll)
        print('collisions top       = %.d' %self.collisions_top)
        print('collisions bottom    = %.d' %self.collisions_bottom)
        print('v       = %.2f m/s'  %(v.mean()*100))
        print('T       = %.2f K'  %T)
        print('P_top   = %.2fbar' %p_top.mean()) #;print(self.dv.mean())
        print('P_bot   = %.2fbar' %p_bottom.mean()) #;print(self.dv.mean())
        print('P_gp    = %.2fbar' %P_gp)
        print('rho     = %.2E kg/m3' %rho)
        print('Entropy = %.2f' %S)


def average_blocks(M,Nx,Ny,f=np.mean):
    A = np.zeros((Nx,Ny),dtype=float)
    nx,ny = M.shape
    # dx,dy = np.array(np.ceil([nx/Nx,ny/Ny]),dtype=int)
    dx,dy = np.array([nx/Nx,ny/Ny],dtype=int)
    # print(nx,ny,Nx,Ny,dx,dy)
    for i in range(Nx):
        for j in range(Ny):
            A[i,j] =f(M[i*dx:(i+1)*dx,j*dy:min((j+1)*dy,ny)])
    return A

if __name__=="__main__":
    atm = 101325
    dt  = 10    #dt=1 => dt=10ps    (t0=10ps)
    v0  = 5     #v=1  => v=100m/s   (v0=100nm/ns=1nm/t0)
    mN2 = 2*14  #g/mol
    # n,nx,ny,r,m = 2000,50,200,0.1,mN2 # dimension unit(nm) #m(ng)
    n,nx,ny,r,m = 5000,100,400,0.1,mN2 # dimension unit(nm) #m(ng)
    y0 = int(ny/2)

    params = dict(n=n, nx=nx, ny=ny, r=r, m=m)
    ga = GasAnimation(params,dt,y0=y0,v0=v0,dt_int=50,ntop=0,plot_rho=True,Nx=10,avg=True,verbose=1)
    plt.show()
