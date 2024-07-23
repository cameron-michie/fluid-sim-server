import React from 'react';
import ReactDOM from 'react-dom/client';
import './index.css';
import App from './App';
import * as Ably from 'ably';
import { AblyProvider } from 'ably/react';

const client = new Ably.Realtime({ key: "0GHh-Q.M8fW0w:YJKOVLZO7f0pj6z6BRvCbPzM_absDlhfXjdb7AYgpo4", clientId: 'john' });

const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(
  <React.StrictMode>
    <AblyProvider client={client}>
      <App />
    </AblyProvider>
  </React.StrictMode>
);
