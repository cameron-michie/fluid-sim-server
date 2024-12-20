import asyncio
from sim_wrapper import Simulation
from simulation import process_simulation, process_messages, listener
from ably import AblyRealtime

async def main():
    api_key = '0GHh-Q.M8fW0w:YJKOVLZO7f0pj6z6BRvCbPzM_absDlhfXjdb7AYgpo4'
    ably = AblyRealtime(api_key)

    # Get channels
    triangleCoords = ably.get('triangulated-coords')
    particleCoords = ably.get('particle-coords')
    bombCoords = ably.get('bomb-updates')

    # Create simulation instance
    simulation = Simulation()
    bombs_ticker = []

    # Set up message queue and subscribers
    message_queue = asyncio.Queue()
    await bombCoords.subscribe(lambda msg: asyncio.create_task(listener(msg, message_queue)))

    # Start simulation and message processing tasks
    simulation_task = asyncio.create_task(process_simulation(simulation, triangleCoords, particleCoords, bombs_ticker))
    message_processing_task = asyncio.create_task(process_messages(message_queue, simulation, bombs_ticker))

    try:
        await asyncio.gather(simulation_task, message_processing_task)
    finally:
        del simulation  # Clean up the simulation instance

if __name__ == "__main__":
    asyncio.run(main())