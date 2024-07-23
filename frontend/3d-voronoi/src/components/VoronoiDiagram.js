// src/VoronoiDiagram.js
import React from 'react';
import { useChannel } from 'ably/react';
import ThreeScene from './ThreeScene';


const VoronoiDiagram = () => {
  const [points, setPoints] = React.useState([]);

  useChannel('particle-positions', (message) => {
    const coords = JSON.parse(message.data);
    setPoints(coords);
  });

  return <ThreeScene points={points} />;
};

export default VoronoiDiagram;
