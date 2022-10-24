#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
// #include <vector.h>
#include "global_types.hpp"
#include "gas_simulator.hpp"
#include "particle.hpp"

const char help[200]="\
usage:\n \
  gas2d [-n <nb particles>] [-r <radius>] [-m <mass>] \
[-nx <cell size x>] [-ny <cell size y>] [-N nb steps] \
\n";
int main( int argc, char **argv )
{
  int opt;
  int n=5,nx=10,ny=10,N_steps=5;
  Real m=1.0,r=0.5;
  while((opt = getopt(argc, argv, "hn:r:x:y:m:N:")) != -1)
  {
      switch(opt)
      {
          case 'n':n=atoi(optarg);break;
          case 'r':r=(Real)atof(optarg);break;
          case 'x':nx=atoi(optarg);break;
          case 'y':ny=atoi(optarg);break;
          case 'm':m=(Real)atof(optarg);break;
          case 'N':N_steps=atoi(optarg);break;
          case 'h':
            printf(help);
            return 0;
            break;
      }
  }
  printf("\n\t... Setup ...\n");
  printf("nb particles:%d\n" , n);
  printf("radius      :%.2f\n", (float) r);
  printf("mass        :%.2f\n", (float) m);
  printf("cell size   :%dx%d\n", nx,ny);

  Gas_simulator gas = Gas_simulator(n,r,nx,ny,m);

  printf("\n\t... Starting computation ...\n");
  printf("Nsteps      :%d\n",N_steps);
  gas.step_by(N_steps);
  // printf("step %d\n", N_steps);
  // gas.step_by(N_steps);
  // printf("step %d\n", 2*N_steps);
  printf("crossings:%d\n",gas.crossings());
  printf("collisions:%d\n",gas.collisions());

  printf("\n\t... Gathering data ...\n");
  float  *v = new float[2*n];
  const vector<Particle>& vp =  gas.vparticles( );
  // FILE* pFileTxt;
  // pFileTxt = fopen("vel.txt", "w");
  int i=0;
  for (vector<Particle>::const_iterator
          it = vp.begin( ); it != vp.end( ); it++ )
  {
      v[2*i]  =(float)it->vx();
      v[2*i+1]=(float)it->vy();
      // fprintf(pFileTxt,"%.2f %.2f\n",v[2*i],v[2*i+1] );
      i++;
  }
  // fclose(pFileTxt);

  printf("\n\t... Writing to file ...\n");
  FILE* pFile;
  pFile = fopen("vel.bin", "wb");
  fwrite(v, sizeof(float),2*n, pFile);
  delete [] v;
  fclose(pFile);
  return 0;
}
