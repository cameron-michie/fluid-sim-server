// src/VoronoiDiagram.js
import React from 'react';
import { useChannel } from 'ably/react';
import ThreeScene from './ThreeScene';

const VoronoiDiagram = () => {
  const [points, setPoints] = React.useState([]);

  useChannel('particle-positions', (message) => {
    const coords = message.data; // Use message.data directly
    setPoints(coords);
  });

  return <ThreeScene points={points} />;
};

export default VoronoiDiagram;
