## Fluid simulator

#### System structure
This program uses C++ as the main computational backend, python as a scripting language, and javascript / react as a frontend visualation tool. The C++ code compiles to a shared library which python accesses using ctypes library. The resulting data is sent via WebSocket using Ably to the JavaScript frontend.

#### Fluid simulation algorithms
The fluid is modelled using smooth particle hydrodyanamics (SPH) as a collection of particles in a 2D grid which seek to repel each other. The relative densities of the particles are calculated, and a subsequent pressure force is exerted on each particle. Boundaries are treated elastically.

### To install and run
- Ensure gcc is installed
- Create a virtual environment named 'venv' and run
`bash
// create and enter venv called venv
pip install ably, ctypes
bash run.sh
`

