# filepath: /Users/cameronoscarmichie/fluid-sim-server-ably/scripts/main.py
import asyncio
from sim_wrapper import Simulation
from ably_client import AblyClient
from simulation import process_simulation, process_messages, listener

async def main():
    api_key = '0GHh-Q.M8fW0w:YJKOVLZO7f0pj6z6BRvCbPzM_absDlhfXjdb7AYgpo4'
    ably_client = AblyClient(api_key)

    # Get channels
    channel = ably_client.get_rest_channel('triangulated-coords')
    raw_channel = ably_client.get_rest_channel('particle-coords')
    bomb_channel = ably_client.get_realtime_channel('bomb-updates')

    # Create simulation instance
    simulation = Simulation()
    bombs_ticker = []

    # Set up message queue and subscribers
    message_queue = asyncio.Queue()
    await bomb_channel.subscribe(lambda msg: asyncio.create_task(listener(msg, message_queue)))

    # Start simulation and message processing tasks
    simulation_task = asyncio.create_task(process_simulation(simulation, channel, raw_channel, bombs_ticker))
    message_processing_task = asyncio.create_task(process_messages(message_queue, simulation, bombs_ticker))

    try:
        await asyncio.gather(simulation_task, message_processing_task)
    finally:
        del simulation  # Clean up the simulation instance

if __name__ == "__main__":
    asyncio.run(main())