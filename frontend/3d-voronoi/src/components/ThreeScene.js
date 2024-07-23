import React, { useEffect, useRef } from 'react';
import { useChannel } from 'ably/react';
import * as THREE from 'three';
import * as d3 from 'd3';
import { Delaunay } from 'd3-delaunay';

const ThreeScene = ({ points }) => {
  const mountRef = useRef(null);
  const { publish } = useChannel("bomb-updates");

  useEffect(() => {
    const mount = mountRef.current;

    const scene = new THREE.Scene();
    scene.background = new THREE.Color(0xffffff); // Set background color to white

    const camera = new THREE.PerspectiveCamera(75, 800 / 600, 0.1, 1000);
    camera.position.set(0, 5, 5); // Set camera position for a 30-degree angle
    camera.lookAt(0, 0, 0); // Look at the center of the scene

    const renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setSize(800, 600);
    mount.appendChild(renderer.domElement);

    const light = new THREE.DirectionalLight(0xffffff, 1);
    light.position.setScalar(100);
    scene.add(light);
    scene.add(new THREE.AmbientLight(0xffffff, 1));

    const material = new THREE.LineBasicMaterial({ color: 0xff0000 }); // Change line color to red

    function drawDelaunay(coords) {
      // Clear previous drawing
      while (scene.children.length > 0) {
        scene.remove(scene.children[0]);
      }

      // Compute the Delaunay triangulation
      const delaunay = Delaunay.from(coords);
      const triangles = delaunay.triangles;

      // Calculate the center of the bounding box
      const centerX = 400;
      const centerY = 300;

      // Draw the Delaunay triangles with density on the z-axis
      for (let i = 0; i < triangles.length; i += 3) {
        const t0 = triangles[i];
        const t1 = triangles[i + 1];
        const t2 = triangles[i + 2];

        const p0 = coords[t0];
        const p1 = coords[t1];
        const p2 = coords[t2];

        const area = Math.abs(d3.polygonArea([p0, p1, p2]));
        const height = 25 / area; // Inversely proportional to the area

        const points = [
          new THREE.Vector3((p0[0] - centerX) / 100, height, (p0[1] - centerY) / 100),
          new THREE.Vector3((p1[0] - centerX) / 100, height, (p1[1] - centerY) / 100),
          new THREE.Vector3((p2[0] - centerX) / 100, height, (p2[1] - centerY) / 100),
          new THREE.Vector3((p0[0] - centerX) / 100, height, (p0[1] - centerY) / 100)
        ];

        const geometry = new THREE.BufferGeometry().setFromPoints(points);
        const line = new THREE.Line(geometry, material);
        scene.add(line);
      }

      renderer.render(scene, camera);
    }

    drawDelaunay(points);

    const animate = () => {
      requestAnimationFrame(animate);
      renderer.render(scene, camera);
    };

    animate();

    const handleCanvasClick = (event) => {
      const [x, y] = d3.pointer(event);
      if (x >= 0 && x <= 800 && y >= 0 && y <= 600) {
        publish('bomb-update', { x, y });
      }
    };

    renderer.domElement.addEventListener('click', handleCanvasClick);

    return () => {
      renderer.domElement.removeEventListener('click', handleCanvasClick);
      mount.removeChild(renderer.domElement);
    };
  }, [points]);

  return <div ref={mountRef} />;
};

export default ThreeScene;
