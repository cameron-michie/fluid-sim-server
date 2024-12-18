import ctypes
import os
import json
import asyncio

from ably import AblyRest
from ably import AblyRealtime

# Determine the absolute path to the shared library
script_dir = os.path.dirname(os.path.abspath(__file__))
lib_path = os.path.join(script_dir, '../shared/mylib.so')

# Load the shared library
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
lib.SimulationWrapper_triangulate.argtypes = [ctypes.POINTER(SimulationWrapper), ctypes.POINTER(Coord), ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]
lib.SimulationWrapper_triangulate.restype = ctypes.POINTER(Coord)

def coord_to_compact_list(coord):
    return [round(coord.x, 2), round(coord.y, 2), round(coord.z, 2)]

def get_triangulated_coords(sim):
    size = ctypes.c_size_t()
    coords_ptr = lib.SimulationWrapper_getParticleCoords(sim, ctypes.byref(size))
    print(f"Received {size.value} particle coordinates from C++.")

    # Create an array of Coord from the pointer
    coords_array = ctypes.cast(coords_ptr, ctypes.POINTER(Coord * size.value)).contents

    # Logging input coordinates
    print("Input Coordinates for Triangulation:")
    for i in range(size.value):
        coord = coords_array[i]
        print(f"Coord {i}: ({coord.x}, {coord.y}, {coord.z})")

    # Prepare output size
    output_size = ctypes.c_size_t()
    triangulated_coords_ptr = lib.SimulationWrapper_triangulate(sim, coords_ptr, size.value, ctypes.byref(output_size))
    print(f"Received {output_size.value} triangulated coordinates from C++.")

    # Create an array of Coord from the pointer
    triangulated_coords_array = ctypes.cast(triangulated_coords_ptr, ctypes.POINTER(Coord * output_size.value)).contents

    # Logging triangulated coordinates
    print("Triangulated Coordinates:")
    for i in range(100):
        coord = triangulated_coords_array[i]
        print(f"Triangulated Coord {i}: ({coord.x}, {coord.y}, {coord.z})")

    coords_list = [coord_to_compact_list(coord) for coord in triangulated_coords_array]
    return coords_list

def get_raw_particle_coords(sim):
    size = ctypes.c_size_t()
    coords_ptr = lib.SimulationWrapper_getParticleCoords(sim, ctypes.byref(size))
    print(f"Received {size.value} raw particle coordinates from C++.")

    # Create an array of Coord from the pointer
    coords_array = ctypes.cast(coords_ptr, ctypes.POINTER(Coord * size.value)).contents

    # Logging coordinates
    print("Raw Particle Coordinates:")
    for i in range(100):
        coord = coords_array[i]
        print(f"Coord {i}: ({coord.x}, {coord.y}, {coord.z})")

    coords_list = [coord_to_compact_list(coord) for coord in coords_array]
    return coords_list

def subtract_and_shift(arr, func, sim):
    if not arr:
        return []  # Check if empty
    count_zeros_removed, result = 0, []
    print(arr)
    
    for x in arr:
        if x > 1:
            result.append(x - 1)
        else:
            count_zeros_removed += 1
    
    for _ in range(count_zeros_removed):
        func(sim)
    
    return result

async def process_simulation(sim, channel, raw_channel):
    global bombsTicker 
    for _ in range(500):
        lib.SimulationWrapper_iterate(sim)

        # Get triangulated coordinates
        coords_dict = get_triangulated_coords(sim)
        coords_json = json.dumps(coords_dict)
        await channel.publish(f"positions-{_}", coords_json)  # Commented out for testing

        # Get raw particle coordinates
        raw_coords_dict = get_raw_particle_coords(sim)
        raw_coords_json = json.dumps(raw_coords_dict)
        await raw_channel.publish(f"raw-positions-{_}", raw_coords_json)  # Commented out for testing

        bombsTicker = subtract_and_shift(bombsTicker, lib.SimulationWrapper_removeBombParticle, sim)
        await asyncio.sleep(0.25)

async def listener(message, queue):
    await queue.put(message)

async def main():
    global sim, bombsTicker
    sim = lib.SimulationWrapper_new()
    bombsTicker = []
    print("Starting")

    async def process_messages(queue):
        while True:
            message = await queue.get()
            if message is None:
                break  # Exit loop if None is received
            print("Processing message:", message.data)
            try:
                data = message.data
                position_x = round(float(data['x']), 2)
                position_y = round(float(data['y']), 2)
                lib.SimulationWrapper_addBombParticle(sim, position_x, position_y)
                bombsTicker.append(20)
                print("Bomb at", position_x, position_y)
            except Exception as e:
                print(f"Error processing message: {e}")
            finally:
                queue.task_done()


    rest = AblyRest('0GHh-Q.M8fW0w:YJKOVLZO7f0pj6z6BRvCbPzM_absDlhfXjdb7AYgpo4')
    realtime = AblyRealtime('0GHh-Q.M8fW0w:YJKOVLZO7f0pj6z6BRvCbPzM_absDlhfXjdb7AYgpo4')
    channel = rest.channels.get('particle-positions')
    raw_channel = rest.channels.get('raw-particle-positions')
    bombChannel = realtime.channels.get('bomb-updates')

    message_queue = asyncio.Queue()
    await bombChannel.subscribe(lambda msg: asyncio.create_task(listener(msg, message_queue)))

    simulation_task = asyncio.create_task(process_simulation(sim, channel, raw_channel))
    message_processing_task = asyncio.create_task(process_messages(message_queue))

    try:
        await asyncio.gather(simulation_task, message_processing_task)
    finally:
        lib.SimulationWrapper_delete(sim)

if __name__ == "__main__":
    asyncio.run(main())