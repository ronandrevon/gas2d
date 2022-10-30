// #include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
// #include <vector.h>
#include "global_types.hpp"
#include "simulator.hpp"
// #include "event.hpp"

int main( )
{
  int n=10,nx=20,ny=20;//,N_steps=50;
  Real m=1.0,r=0.5;

  double *xv=new double[n*4];
  for (int i=0;i<n;i++){
    xv[4*i]   = i+0.25;
    xv[4*i+1] = 0.1;
    xv[4*i+2] = i+0.1;
    xv[4*i+3] = 1.0;
  }

  Simulator gas = Simulator(n,nx,ny,r,m,xv);
  printf("Initial Distribution : \n");
  gas.simu()->print_dist();
  // Event event = Event(COLLISION,1,0.1);
  // double info[7];// = {0,-1,0,0,-1,0,0};

  gas.next_event();
  gas.next_event();
  double *info = gas.info();
  printf("%.2f\n",info[0]);
  // gas.next_event();
  // gas.next_event();
  // gas.next_event();

  delete [] xv;

  return 0;
}
