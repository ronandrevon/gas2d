import numpy as np

kB = 1.38064852e-23
Na = 6.02214076e23
R  = Na*kB

def entropy(pi):
    '''Computes the entropy of the distribution v_dist
    - pi : normalized speed distribution/histogram
    '''
    if not isinstance(pi,np.ndarray):
        pi=np.array(pi)
    return -np.log(pi[pi>0]).mean()

def P_gp(n,T,V):
    ''' computes the pressure in bar of the ideal gas from :
    - n : nb particles
    - T : temperature
    - V : Volume (nm^3)
    '''
    return n/Na*R*T/(V*1e-27)/1e5

def pressure(m,dv,dt,S):
    '''pressure (Bar) from impulsion, time, and area
    - m : g/mol
    - dv : v=1 => 100m/s
    - dt : 10ps
    - S : nm^2
    '''
    return (m*1e-3/Na)*dv*1e2/(dt*1e-11)/(S*1e-18)/1e5

def T(m,v2):
    ''' computes the temperature in K from v2
    '''
    return  m*v2*10/(2*R) #K

def temperature(m,v):
    ''' computes the temperature in K from distribution v
    - m : g/mol
    - v : v=1 => v=100m/s
    Note : in 2D we have mv^2/2 = k_B*T
    '''
    mv2 = np.mean(m*v**2)*10
    T   = mv2/(2*R) #K
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
