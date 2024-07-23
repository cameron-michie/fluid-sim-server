import './App.css';
import { ChannelProvider } from 'ably/react';

import VoronoiDiagram from './components/VoronoiDiagram';

function App() {
  return (
    <div className="App">
      <ChannelProvider channelName="particle-positions">
          <ChannelProvider channelName="bomb-updates">
              <VoronoiDiagram/>
          </ChannelProvider>
      </ChannelProvider>
    </div>
  );
}

export default App;
