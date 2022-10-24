// Gas_simulator.cpp
Gas_simulator::Gas_simulator( int n, Real r, int max_x, int max_y, Real m )
    : np(n), nx(max_x), ny(max_y), m_time(0), m_event( n, event_max ),
         hst( m_particle, 500 ){
    // allocate cells
    // assign random speeds and locations
    // calculate next event for every particle
    for (int i=0; i<np; i++)
    {
        min_time( i );
    }
}

void Gas_simulator::step_collision( int ic, const Event& ev ){
  int jc = e.partner();
  if (m_event.event(jc).kind==COLLISION){
    int time=e.time();
    m_particle[ic].move_to(time);
    m_particle[jc].move_to(time);
    reflexion( ic, jc );
    m_collisions++;
  }
  min_time( ic );
  min_time( jc );
}


void Gas_simulator::step_collision( int ic, const Event& e ){
  Border border = Border(e.partner());
  Particle& p = m_particle[ic];
  switch (border){
    case X_LEFT:
      p.reverse_vx();
      break;
  }
  // update particle cell
  m_crossings++;
  min_time( ic );
}

void Gas_simulator::min_time( int ic ){
  //get adjacent cells i1,i2, j1,j2
  int i1 = p.i( ) - 1;
  //get next sooner collision
  for (int i=i1;i<=i2;i++){
    for (int j=j1;j<=j2;j++){
      const Cell& cell = m_cell[i][j];
      for (int k=0;k<cell.quantity();k++){
        Real t = collision_time( ic, cell[k] );
        if (t<tmin) tmin=t;next=cell[k];
      }
    }
  }
  Event e2 = Event( COLLISION, next, tmin );
  Event e1 = crossing_time(ic);
  // set the next event
  if (e1 < e2)
      m_event.set_event( ic, e1 );
  else
      m_event.set_event( ic, e2 );

}


//Particle.hpp
class Particle
{
public:
  Particle(vector_2D m_pos,vector_2D m_vel, real m_radius,real m_mass,
    int i,int j,real m_time)
  void move_to( Real t_next )
  {
    Real dt = t_next - m_time;
    m_pos += (dt * m_vel );
    m_time = t_next;
  }
}
