import importlib as imp
import time,copy,threading,matplotlib
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
        Tmax=0,
        anim=True,cmap='jet',verbose=0):
        ''' Animate the Gas Simulator class
        - params : dict('n','nx','ny','r','m')
        - dt : Simulation time (in hundreadth of ns) between each frame
        - y0 : initial height of the container
        - v0 : initial average velocities
        - dt_int : refresh time between each frame
        - ntop   : height of the histogram axis
        - plot_rho : plots rho if True, else shows the particles (can be changed with shortcut "n")
        - Nx   : size of imshow grid along x(automatic calculation along y)
        - Tmax : max temperature to display
        - avg  : if True computes rho and T as averages on a a Nx x Ny grid
        '''
        self.dt       = dt
        self.params   = params
        self.plot_rho = plot_rho
        self.avg      = avg
        self.cmap     = cmap
        self.anim     = anim
        self.verbose  = verbose
        self.init_simulator(y0,v0,Nx,nv0s=5)
        self.update_data()
        self.init_plots(ntop,Tmax)
        if self.verbose:
            self.info('')

        ###
        self.run_simu()
        if self.anim:
            self.animation = animation.FuncAnimation(
                self.fig, self.update, frames=100000, interval=dt_int, blit=False)
            self.paused = False
        self.fig.canvas.mpl_connect('key_press_event', self.key_event)

    def init_simulator(self,y0,v0,Nx,nv0s):
        n,nx,ny,r,m = self.params['n'],self.params['nx'],self.params['ny'],self.params['r'],self.params['m']
        self.state = dict()
        self.dv    = np.zeros(2*nx,dtype=float)
        self.xv2D  = np.zeros(2*ny*nx,dtype=float)
        self.t  = 0
        self.y  = y0
        self.V  = nx*self.y*1
        self.Nx = Nx
        self.Ny = int(np.ceil(self.Nx*self.y/nx))
        self.HIST_BINS = np.linspace(0,nv0s*v0,nv0s*100)


        ## initial locations
        x,y = np.meshgrid(range(nx),range(ny))
        x  = np.stack([np.reshape(x,nx*ny),np.reshape(y,nx*ny)])[:,:n]+0.05
        ## initial random speeds
        v  = np.random.rand(2,n)*2-1 #np.array([[0.2]*n,[1]*n])#np.array([[1,0],[-1,0]])
        v /= np.linalg.norm(v,axis=0)
        v *= v0
        self.xv  = np.array(np.vstack([x,v]),dtype=float).T
        self.xv0 = np.reshape(np.array(np.vstack([x,v]).T,dtype=float),4*n) #pack it as single array for c++ simulator

        ## init the c++ simulator
        print(colors.blue+'... init simulator ...'+colors.black)
        self.g = gas.Simulator()
        self.g.set_xv(self.xv0)
        self.g.init_simulator(n,nx,ny,r,m,self.y)
        # print(colors.blue+'...update ...'+colors.black)
        # self.g.update_dv(self.dv)
        # self.g.step_by(1);

        # self.next_event() # for individual collision based animations

    def init_plots(self,ntop,Tmax):
        n,nx,ny = [self.params[k] for k in ['n','nx','ny']]
        wm,hm,W,H = 0.025,0.15,0.2,0.8
        w_cb = 0.1
        print(colors.blue+'... init plots ...'+colors.black)

        #### create axes
        self.fig,ax = plt.subplots(nrows=1,ncols=3,figsize=(15, 10))#,dpi=dpi[0],**f_args)
        #axes limits
        xylims = [-1,nx+1,-1,ny+1]
        if not ntop : ntop = n/10
        self.ntop   = ntop
        xylimsN     = [0,1.05*self.HIST_BINS[-1],0,ntop]
        #properties dict
        self.ax_props = {
            'rho':dict(axpos=[wm,hm,W,H]                 ,xylims=xylims ,label=r'$\rho(kg/m^3)$'    ,equal=True ,axisOff=False,grid=False,patchOn=True  ),
            'T'  :dict(axpos=[wm+W+wm+w_cb+wm,hm,W,H]    ,xylims=xylims ,label=r'$T(K)$'            ,equal=True ,axisOff=False,grid=False,patchOn=True  ),
            'v'  :dict(axpos=[wm+2*(W+wm+w_cb+wm),hm,W,H],xylims=xylimsN,label=r'speed distribution',equal=False,axisOff=False,grid=False,patchOn=False ),
            'cb_rho':{'axpos':[wm+W+wm,hm,w_cb,H]},
            'cb_T'  :{'axpos':[wm+2*(W+wm)+w_cb+wm,hm,w_cb,H]},
            'slider':{'axpos':[2*wm,0.025,1-4*wm,0.05]}
        }
        self.ax = dict(zip(['rho','T','v'],ax))

        ### create artists
        self.artists = dict()
        self.patches = {k:matplotlib.patches.Rectangle((0,0),nx,self.y,alpha=0.9,fill=0,ec='k',lw=2) for k in ['rho','T']}
        self.init_axN()
        self.init_ax1()
        self.init_axT(Tmax)

        #### set axis properties
        for s in ['rho','T','v']:
            self.init_ax(s)

        #### control volume through y slider
        ax_slider = self.fig.add_axes(self.ax_props['slider']['axpos'])
        self.y_slider = matplotlib.widgets.Slider(ax=ax_slider,
            label='y',valmin=1,valmax=ny,valinit=self.y,
            )
        self.y_slider.on_changed(self.update_slider)

    def init_ax(self,s):
        props = self.ax_props[s]
        self.ax[s].set_xlim(props['xylims'][:2])
        self.ax[s].set_ylim(props['xylims'][2:])
        if props['equal']:self.ax[s].set_aspect('equal')
        if props['grid']:self.ax[s].grid()
        if props['axisOff']:self.ax[s].axis('off')
        if props['patchOn']:
            self.ax[s].add_patch(self.patches[s])
        self.ax[s].set_title(props['label'])
        self.ax[s].set_position(props['axpos'])

    def init_axN(self):
        '''create the histogram patches'''
        _, _, self.bar_container = self.ax['v'].hist(self.v(), self.HIST_BINS,
            lw=1,density=True,ec="k", fc="blue", alpha=0.5)

    def init_axT(self,Tmax):
        nx,ny,m = self.params['nx'],self.params['ny'],self.params['m']
        if not Tmax:
            Tmax = ut.temperature(m,self.HIST_BINS[int(2/3*self.HIST_BINS.size)])
            print('Tmax=%2.f' %Tmax)
        artist = self.ax['T'].imshow(self.T(),extent=[0,nx,0,self.y],origin='lower',
            interpolation='bilinear',vmin=0,vmax=Tmax,cmap=self.cmap)
        cb = self.fig.colorbar(artist,ax=self.ax['T'])
        cb.ax.set_position(self.ax_props['cb_T']['axpos'])
        self.artists['T'] = (artist,cb,)

    def init_ax1(self):
        self.update_ax1=[self.update_n,self.update_rho][self.plot_rho]
        n,nx,ny,m = self.params['n'],self.params['nx'],self.params['ny'],self.params['m']
        ymin = ny/10
        rho_max  = ut.rho(n=n,V=nx*ymin*1,m=m)
        print('rho max=%.2f kg/m^3' %rho_max)
        if self.plot_rho:
            artist = self.ax['rho'].imshow(self.rho(),extent=[0,nx,0,self.y],origin='lower',
                interpolation='bilinear',vmin=0,vmax=rho_max,cmap=self.cmap)
            cb = self.fig.colorbar(artist,ax=self.ax['rho'])
            cb.ax.set_position(self.ax_props['cb_rho']['axpos'])
            self.artists['rho'] = (artist,cb,)
        else:
            artist,= self.ax['rho'].plot(self.xv[:,0], self.xv[:,1],'o',markersize=92*self.params['r']*5/nx)
            self.artists['rho'] = (artist,)
        # self.time_text = self.ax['rho'].text(0.05, 0.90, '', transform=self.ax['rho'].transAxes)

    ############################################################################
    #### update functions
    ############################################################################
    def update(self,i):
        self.trd_simu.join()
        self.update_data()
        # return self.update_balls_view(i)
        # print(colors.blue+'...update anim...'+colors.black)
        artists = self.update_animation()
        if self.verbose:
            self.info(i)
        self.run_simu()
        # self.trd_simu = threading.Thread(target=self.g.step_until_dt(self.dt,self.y))
        # self.trd_simu.start()
        return artists

    def update_data(self):
        '''gather information'''
        n,m,nx,ny = self.params['n'],self.params['m'],self.params['nx'],self.params['ny']

        self.t    = self.g.time()
        ## particles pos,vel array (nparticles x 4). Automatically modified by
        ## the c++ simulator as the pointer to this array is set at the initialization
        self.xv   = np.reshape(self.xv0,(n,4))
        self.nv,_ = np.histogram(self.v(), self.HIST_BINS,density=False)
        ## pressure info
        ## reset to 0 inside the c++ simulator
        self.g.update_dv(self.dv)
        ## spatial distributions (n(ny,nx), v(ny,nx))
        ## Redundant info to xv but arranged on the grid
        self.xv2D = np.zeros(2*nx*ny,dtype=float)
        self.g.get_distribution(self.xv2D)
        # print(self.xv2D.mean())
        # self.g.get_xv(self.xv0)
        # print(self.xv0)

        v = self.v()
        self.state['coll']               = self.g.collisions()
        self.state['collisions_top']     = self.g.top_collisions()
        self.state['collisions_bottom']  = self.g.bottom_collisions()
        self.state['v']        = v.mean()*100
        self.state['v_rms']    = np.sqrt((v**2).mean())*100
        self.state['T']        = ut.temperature(m,v)
        self.state['P_bottom'] = ut.pressure(m=m,dv=self.dv[:nx],dt=self.dt,S=1).mean()
        self.state['P_top']    = ut.pressure(m=m,dv=self.dv[nx:],dt=self.dt,S=1).mean()
        self.state['P_gp']     = ut.P_gp(n,self.state['T'],V=self.V)
        self.state['rho']      = ut.rho(n=n,V=self.V,m=m)
        self.state['V']        = self.V
        self.state['S']        = ut.entropy(self.nv/n)

    def run_simu(self):
        self.trd_simu = threading.Thread(target=self.g.step_until_dt(self.dt,self.y))
        self.trd_simu.start()

    ############################################################################
    #### Animation update functions
    ############################################################################
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

    def update_histogram_v(self):
        for count, rect in zip(self.nv, self.bar_container.patches):
            rect.set_height(count)
        self.ax['v'].set_ylim(top=self.ntop)

    def update_animation(self):
        self.update_ax1()
        self.update_T()
        self.update_histogram_v()
        for p in self.patches.values():
            p.set_height(self.y)
        return (*self.artists['rho'],*self.artists['T'],
            *self.bar_container.patches,*self.patches)


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
                self.update_histogram_v()
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
        # print(event.key)
        if event.key==' ':
            if self.anim:
                self.toggle_pause()
        elif event.key=='enter':
            if self.anim:
                self.pause = True
                self.animation.pause()
            t=time.time()
            self.update(0)
            self.info(time.time()-t)
            # self.update_animation()
            self.fig.canvas.draw_idle()
        elif event.key == '/':
            self.ntop/=2
        elif event.key == '.':
            self.ntop*=2
        elif event.key == 'n':
            self.plot_rho=not self.plot_rho
            for a in self.artists['rho']:
                if isinstance(a,matplotlib.colorbar.Colorbar):
                      self.fig.delaxes(a.ax)
                else:
                    a.remove()
                # print(type(a[]))
            self.init_ax1()
            self.init_ax('rho')

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
    def rho(self,opt='rho'):
        nx,ny,m,n = self.params['nx'],self.params['ny'],self.params['m'],self.params['n']
        n_d = (np.reshape(self.xv2D[:nx*ny],(ny,nx)))
        n_d = n_d[:self.y,:]
        # print('rho check_sum',rho_.sum())#rho.mean(),self.artists['rho'][0].get_array().mean())
        if self.avg:
            dx,dy = np.array([nx/self.Nx,ny/self.Ny],dtype=int)
            V = dx*dy*1
            # print('Vcell=%d nm^3' %V)
            f    = lambda nd:ut.rho(nd,1,m).mean()
            rho_ = compute_blocks(n_d,self.Ny,self.Nx,f)
            # N    = average_blocks(n_d,self.Ny,self.Nx,lambda nd:1/n*np.sum(nd))
            # print('N density check_sum=%.2f' %N.sum())
        else:
            rho_= n_d/n
        self.state['rho_mean'] = rho_.mean()
        return rho_

    def T(self):
        nx,ny = self.params['nx'],self.params['ny']
        n_d = (np.reshape(self.xv2D[:nx*ny],(ny,nx)))
        n_d = n_d[:self.y,:]
        v2 = np.reshape(self.xv2D[nx*ny:],(ny,nx))
        v2 = v2[:self.y,:]
        ## v2 is actually the sum of v^2 of the particles inside the cell
        ## so we divide by n particles to get the average quadratic speed
        ## <v^2>
        v2[n_d>0] /= n_d[n_d>0]
        ## Note that the temperature average must only be counted where particles
        ## are present. We assume that this usually coincides with the locations where
        ## the rms velocity is non zeros
        if self.avg:
            def f(v2):
                v2 = v2[v2>0]
                if v2.shape[0]:
                    return ut.T(self.params['m'],v2).mean()
                else:
                    return 0
            T_ = compute_blocks(v2,self.Ny,self.Nx,f)
        else:
            T_ = ut.T(self.params['m'],v2)
        self.state['T_mean'] = T_[T_>0].mean()
        return T_

    def info(self,i):
        print()
        print(i)
        print(colors.red+'time=%.2f ps' %(self.t*10) +colors.black)
        print('collisions particles = %.d' %self.state['coll'])
        print('collisions top       = %.d' %self.state['collisions_top'])
        print('collisions bottom    = %.d' %self.state['collisions_bottom'])
        print('v        = %.2f m/s'  %self.state['v']        )
        print('v_rms    = %.2f m/s'  %self.state['v_rms']    )
        print('T        = %.2f K'    %self.state['T']        )
        print('T mean   = %.2f'      %self.state['T_mean']   )
        print('P_top    = %.2f bar'  %self.state['P_top']    )
        print('P_bot    = %.2f bar'  %self.state['P_bottom'] )
        print('P_gp     = %.2f bar'  %self.state['P_gp']     )
        print('rho      = %.2f kg/m3'%self.state['rho']      )
        print('rho mean = %.2f'      %self.state['rho_mean'] )
        print('V        = %d nm^3'   %self.state['V']        )
        print('Entropy  = %.2f a.u.' %self.state['S']        )

def compute_blocks(M,Nx,Ny,f=np.mean):
    A = np.zeros((Nx,Ny),dtype=float)
    nx,ny = M.shape
    # dx,dy = np.array(np.ceil([nx/Nx,ny/Ny]),dtype=int)
    dx,dy = np.array([nx/Nx,ny/Ny],dtype=int)
    # print(nx,ny,Nx,Ny,dx,dy)
    for i in range(Nx):
        for j in range(Ny):
            A[i,j] =f(M[i*dx:min((i+1)*dx,nx),j*dy:(j+1)*dy])
    return A



if __name__=="__main__":
    atm = 101325
    #dt=1 => dt=10ps    (t0=10ps)
    #v=1  => v=100m/s   (v0=100nm/ns=1nm/t0)
    vi  = 5
    mN2 = 2*14  #g/mol
    # n,nx,ny,r,m,dt,dt_int = 10 ,10,40 ,0.1,mN2,10,50
    # n,nx,ny,r,m,dt,dt_int = 500,40,160,0.1,mN2,1 ,100
    n,nx,ny,r,m,dt,dt_int = 50000,200,800,0.1,mN2,0.5,500
    y0,Nx = int(ny/2),10

    params = dict(n=n, nx=nx, ny=ny, r=r, m=m)
    ga = GasAnimation(params,dt,y0=y0,v0=vi,
        dt_int=dt_int,ntop=0,plot_rho=True,Nx=Nx,avg=1,anim=1,
        Tmax=700,
        verbose=1)
    plt.show()
