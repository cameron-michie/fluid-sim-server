import './App.css';
import { ChannelProvider } from 'ably/react';
import { useEffect } from 'react';
import pako from 'pako';
import Ably from 'ably';

import VoronoiDiagram from './components/VoronoiDiagram';

function App() {

  return (
    <div className="App">
      <ChannelProvider channelName="triangulated-coords">
          <ChannelProvider channelName="bomb-updates">
              <VoronoiDiagram/>
          </ChannelProvider>
      </ChannelProvider>
    </div>
  );
}

export default App;
