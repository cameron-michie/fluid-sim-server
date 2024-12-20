import ctypes
import os

# Load the shared library
script_dir = os.path.dirname(os.path.abspath(__file__))
lib_path = os.path.join(script_dir, '../shared/mylib.so')
lib = ctypes.CDLL(lib_path)

# Define the Coord structure
class Coord(ctypes.Structure):
    _fields_ = [("x", ctypes.c_float),
                ("y", ctypes.c_float),
                ("z", ctypes.c_float)]

# Define the SimulationWrapper structure
class SimulationWrapper(ctypes.Structure):
    pass

# Define the function prototypes
lib.SimulationWrapper_new.restype = ctypes.POINTER(SimulationWrapper)
lib.SimulationWrapper_delete.argtypes = [ctypes.POINTER(SimulationWrapper)]
lib.SimulationWrapper_iterate.argtypes = [ctypes.POINTER(SimulationWrapper)]
lib.SimulationWrapper_addBombParticle.argtypes = [ctypes.POINTER(SimulationWrapper), ctypes.c_float, ctypes.c_float]
lib.SimulationWrapper_removeBombParticle.argtypes = [ctypes.POINTER(SimulationWrapper)]
lib.SimulationWrapper_getParticleCoords.argtypes = [ctypes.POINTER(SimulationWrapper), ctypes.POINTER(ctypes.c_size_t)]
lib.SimulationWrapper_getParticleCoords.restype = ctypes.POINTER(Coord)
lib.SimulationWrapper_triangulate.argtypes = [
    ctypes.POINTER(SimulationWrapper),
    ctypes.POINTER(Coord),
    ctypes.c_size_t,
    ctypes.POINTER(ctypes.c_size_t)
]
lib.SimulationWrapper_triangulate.restype = ctypes.POINTER(Coord)

class Simulation:
    def __init__(self):
        self.sim = lib.SimulationWrapper_new()

    def __del__(self):
        lib.SimulationWrapper_delete(self.sim)

    def iterate(self):
        lib.SimulationWrapper_iterate(self.sim)

    def add_bomb_particle(self, x, y):
        lib.SimulationWrapper_addBombParticle(self.sim, ctypes.c_float(x), ctypes.c_float(y))

    def remove_bomb_particle(self):
        lib.SimulationWrapper_removeBombParticle(self.sim)

    def get_particle_coords(self):
        size = ctypes.c_size_t()
        coords_ptr = lib.SimulationWrapper_getParticleCoords(self.sim, ctypes.byref(size))
        coords_array = ctypes.cast(coords_ptr, ctypes.POINTER(Coord * size.value)).contents
        return coords_array, size.value

    def triangulate(self, coords_array, input_size):
        output_size = ctypes.c_size_t()
        triangulated_coords_ptr = lib.SimulationWrapper_triangulate(
            self.sim,
            coords_array,
            input_size,
            ctypes.byref(output_size)
        )
        triangulated_coords_array = ctypes.cast(
            triangulated_coords_ptr,
            ctypes.POINTER(Coord * output_size.value)
        ).contents
        return triangulated_coords_array, output_size.value

    def perform_triangulation(self):
        lib.SimulationWrapper_performTriangulation(self.sim)

    def get_triangulated_vertices(self):
        size = ctypes.c_size_t()
        vertices_ptr = lib.SimulationWrapper_getTriangulatedVertices(self.sim, ctypes.byref(size))
        vertices_array = ctypes.cast(vertices_ptr, ctypes.POINTER(Vertex * size.value)).contents
        return vertices_array, size.value

    def get_triangles(self):
        size = ctypes.c_size_t()
        triangles_ptr = lib.SimulationWrapper_getTriangles(self.sim, ctypes.byref(size))
        triangles_array = ctypes.cast(triangles_ptr, ctypes.POINTER(Triangle * size.value)).contents
        return triangles_array, size.value