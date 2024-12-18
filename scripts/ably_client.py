#filepath: /Users/cameronoscarmichie/fluid-sim-server-ably/scripts/ably_client.py
from ably import AblyRest, AblyRealtime

class AblyClient:
    def __init__(self, api_key):
        self.rest = AblyRest(api_key)
        self.realtime = AblyRealtime(api_key)
        self.channels = {}

    def get_rest_channel(self, channel_name):
        if channel_name not in self.channels:
            self.channels[channel_name] = self.rest.channels.get(channel_name)
        return self.channels[channel_name]

    def get_realtime_channel(self, channel_name):
        if channel_name not in self.channels:
            self.channels[channel_name] = self.realtime.channels.get(channel_name)
        return self.channels[channel_name]