
## Classes
```C++
Class Gas_simulator{
    void gather_statistic( );
    Event crossing_time( int ic );//calculates the next crossing time event for particle ic
    Real collision_time( int ic, int jc );//

    void step( );//dispatches the top event from the event heap to crossing or collision
    void step_crossing( int ic, const Event& ev );// collision with border
    void step_collision( int ic, const Event& ev );//checks that the partner shares the same event moves the particle at the event time and computes the reflection
    void reflexion( int ic, int jc ); // computes the collision between two particles
  private:
    int np; //number particles
    int nx, ny; // grid size

    vector <Particle> m_particle;  
    vector <vector <Cell> > m_cell;
    Event_manager <Event> m_event;

}
```

```C++
template <class E>
Class Event_manager{
  public:
    int top( ) const { return heap[1]-1; }
    const E& event( int i ) const { return events[i+1]; }
    void set_event( int i, const E& new_event );
  private:
    void pull_through( int i );
    std::vector <E> events;
    std::vector <int> heap;
  }
```
