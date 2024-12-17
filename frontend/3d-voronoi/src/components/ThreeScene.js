import React, { useEffect, useRef } from 'react';
import { useChannel } from 'ably/react';
import * as THREE from 'three';
import * as d3 from 'd3';

function ThreeScene({ points }) {
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

    const triangleMaterial = new THREE.MeshBasicMaterial({
      color: 0x87CEEB, // Light blue (hex code for sky blue)
      transparent: true, // Enable transparency
      opacity: 0.8, // Base opacity
      reflectivity: 0.5, // Reflectiveness of the water surface
      side: THREE.DoubleSide, // Render both sides for thin water surfacesi
      wireframe: true
    });

    const drawTriangles = (pointsData) => {
      // Clear previous drawing
      while (scene.children.length > 0) {
        scene.remove(scene.children[0]);
      }

      // Draw triangles using pointsData
      if (!pointsData || pointsData.length < 3) {
        return; // Not enough points to form a triangle
      }

      // Draw triangles using pointsData
      for (let i = 0; i <= pointsData.length - 3; i += 3) {
        const p0 = pointsData[i];
        const p1 = pointsData[i + 1];
        const p2 = pointsData[i + 2];

        // Ensure points are defined
        if (p0 && p1 && p2) {
          const vertices = new Float32Array([
            p0[0], p0[1], p0[2],
            p1[0], p1[1], p1[2],
            p2[0], p2[1], p2[2],
          ]);

          const geometry = new THREE.BufferGeometry();
          geometry.setAttribute('position', new THREE.BufferAttribute(vertices, 3));

          const mesh = new THREE.Mesh(geometry, triangleMaterial);
          scene.add(mesh);
        }
      }

      renderer.render(scene, camera);
    };

    drawTriangles(points);

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
}

export default ThreeScene;

