# TODO

## Python
- [x] interface with C++ properly swig (seems to work using a wrapper/interface class)
- [x] animate with matplotlib
    - [x] animation framework
    - [x] get info from simulator
- [x] provide initial distribution
- [ ] compute pressure, temperature, entropy, distribution
- [ ] allow change in volume,exchange of energy
- [ ] external pressure/force

## Identify pure collisions simulation classes - 1h30
- [x] where the border collisions happens  : Gas_simulator::step_crossing()
- [x] where the particle collision happens : Gas_simulator::step_collision()
- [x] velocities updated after particle collisions : Gas_simulator::reflexion()
- [x] where events are registered/detected Gas_simulator::min_time (called after handling collisions) which eventually calls Event_Manager::set_event
- [x] cells are used to partition the collision spaces(nx,ny cell grid each 1 square unit large)
- [x] events are kept in Event_Manager.events and a heap is likely used for sorting the vector of events.
  - [x] event_manager instantiated in gas_simulator header without using constructor function
- [x] things get started in gas_simulator constructor min_time called on every particles
### Qt side
- [x] Rayleigh :tab using plotter.h for plotting and for histogram calculation
- [x] visualizer_tab : uses vizualizer to display the spheres
- [x] process tab :small tab for starting and stopping the
- [x] statistics : small tab displaying info such as events, crossings, collisions, etc...
- [ ] libs for plotting graph and rendering animation (openGL)
- [ ] where and how the spheres are rendered

## Adapt adapt to non QT for simple compilation   - 1h30
- [x] done
## Profiler
- [ ] run profiler and runtime performances       - 1h
- [x] check cython 1h
## final distributions python - 1h
- [ ] show final 1d-energy/2d-speed distributions after $n$ mean free time
