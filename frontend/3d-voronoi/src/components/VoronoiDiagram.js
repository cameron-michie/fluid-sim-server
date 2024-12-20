// src/VoronoiDiagram.js
import React from 'react';
import { useChannel } from 'ably/react';
import ThreeScene from './ThreeScene';
import pako from 'pako';

const VoronoiDiagram = () => {
  const [points, setPoints] = React.useState([]);

  useChannel('triangulated-coords', (message) => {
    const coords = JSON.parse(pako.inflate(message.data, { to: 'string' }));
    setPoints(coords);
    // console.log(coords)
  });

  return <ThreeScene points={points} />;
};

export default VoronoiDiagram;
