#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

# include <stdio.h>
# include "gas_simulator.hpp"
// # include "global_types.hpp"


class Simulator{
  public:
    Simulator(){
    }

  void set_xv(double* seq, int n){
    xv=seq;
  };

  void init_simulator(int n, int nx, int ny, float r, float m){
    gas_simulator = new Gas_simulator(n,Real(r),nx,ny,Real(m),xv);
  };

  void step_by(int t){
    gas_simulator->step_by(t);
  };
  void print_stat(){
    printf("crossings:%d\n" ,gas_simulator->crossings());
    printf("collisions:%d\n",gas_simulator->collisions());
  };

  double* next_event(bool v=true){
    // printf("\n");
    // double info[7];// = {0,-1,0,0,-1,0,0};
    gas_simulator->step_until(m_info);
    // const double (& info)[7] = m_info.vals;

    if (v){
      printf("%.5f : \n",m_info[0]);
      printf("particle %d has new speed %.2f,%.2f collided with ",\
      (int)m_info[1],m_info[2],m_info[3]);
      if (m_info[4]>=0)
        printf("particle %d with new velocity %.2f %.2f \n",\
          (int)m_info[4],m_info[5],m_info[6]);
      else
        printf("border\n");
      print_stat();
    }
    return m_info;
  };

  double get_time() const{
    return m_info[0];
  };

  double* info(){
    return m_info;
  };

  const Gas_simulator* simu(){
    return gas_simulator;
  };
  void print_dist(){
    return gas_simulator->print_dist();
  };

  private:
    Gas_simulator *gas_simulator;
    double m_info[7]={0,-1,0,0,-1,0,0};
    double *xv;
};

#endif // _SIMULATOR_H_