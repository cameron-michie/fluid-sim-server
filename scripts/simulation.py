# filepath: /Users/cameronoscarmichie/fluid-sim-server-ably/scripts/simulation.py
import asyncio
import json
import sys
import zlib

def coord_to_list(coord):
    return [round(coord.x, 1), round(coord.y, 1), round(coord.z, 1)]

def subtract_and_shift(arr, remove_func):
    if not arr:
        return []
    count_zeros_removed = 0
    result = []
    for x in arr:
        if x > 1:
            result.append(x - 1)
        else:
            count_zeros_removed += 1
    for _ in range(count_zeros_removed):
        remove_func()
    return result

async def process_simulation(simulation, channel, raw_channel, bombs_ticker):
    for iteration in range(500):
        simulation.iterate()

        # Get triangulated coordinates
        coords_array, size = simulation.get_particle_coords()
        triangulated_coords_array, output_size = simulation.triangulate(coords_array, size)
        coords_list = [coord_to_list(coord) for coord in triangulated_coords_array]
        coords_json = json.dumps(coords_list)
        compressed_coords = zlib.compress(coords_json.encode('utf-8'))
        
        # Print the size of the original and compressed lists
        # print(f"Original size: {sys.getsizeof(coords_json)} bytes")
        # print(f"Compressed size: {sys.getsizeof(compressed_coords)} bytes")

        # await channel.publish(f"positions-{iteration}", compressed_coords)

        # Get raw particle coordinates
        raw_coords_list = [coord_to_list(coord) for coord in coords_array]
        raw_coords_json = json.dumps(raw_coords_list)
        # await raw_channel.publish(f"raw-positions-{iteration}", raw_coords_json)

        # Update bomb particles
        bombs_ticker[:] = subtract_and_shift(bombs_ticker, simulation.remove_bomb_particle)

        await asyncio.sleep(0.1)

async def listener(message, queue):
    await queue.put(message)

async def process_messages(queue, simulation, bombs_ticker):
    while True:
        message = await queue.get()
        if message is None:
            break
        try:
            data = message.data
            x = round(float(data['x']), 2)
            y = round(float(data['y']), 2)
            simulation.add_bomb_particle(x, y)
            bombs_ticker.append(20)
        except Exception as e:
            print(f"Error processing message: {e}")
        finally:
            queue.task_done()