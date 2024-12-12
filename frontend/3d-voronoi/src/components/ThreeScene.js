import React, { useEffect, useRef } from 'react';
import { useChannel } from 'ably/react';
import * as THREE from 'three';
import * as d3 from 'd3';
import { Delaunay } from 'd3-delaunay';

const max_height = 50;
const max_length = 100;
const height_normalisation = 40;
const boundary_threshold = 15; // Distance in pixels from the boundary to consider a point as a boundary point

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

    const triangleMaterial = new THREE.MeshBasicMaterial({
      color: 0x87CEEB, // Light blue (hex code for sky blue)
      transparent: true, // Enable transparency
      opacity: 0.8, // Base opacity
      reflectivity: 0.5, // Reflectiveness of the water surface
      side: THREE.DoubleSide, // Render both sides for thin water surfacesi
      wireframe: true
    });
    function drawDelaunay(coords) {
      // Clear previous drawing
      while (scene.children.length > 0) {
        scene.remove(scene.children[0]);
      }

      // Identify boundary points
      const isBoundaryPoint = coords.map(([x, y]) => (
        x < boundary_threshold || x > 800 - boundary_threshold ||
        y < boundary_threshold || y > 600 - boundary_threshold
      ));

      // Compute the Delaunay triangulation
      const delaunay = Delaunay.from(coords);
      const triangles = delaunay.triangles;
      const densities = new Array(coords.length).fill(0);
      const counts = new Array(coords.length).fill(0);

      // Calculate the area of each triangle and accumulate the densities
      for (let i = 0; i < triangles.length; i += 3) {
        const t0 = triangles[i];
        const t1 = triangles[i + 1];
        const t2 = triangles[i + 2];

        const p0 = coords[t0];
        const p1 = coords[t1];
        const p2 = coords[t2];

        const area = Math.abs(d3.polygonArea([p0, p1, p2]));
        const density = 1 / area;

        if (!isBoundaryPoint[t0] && !isBoundaryPoint[t1] && !isBoundaryPoint[t2]) {
          densities[t0] += density;
          densities[t1] += density;
          densities[t2] += density;
          counts[t0] += 1;
          counts[t1] += 1;
          counts[t2] += 1;
        } else {
          const nonBoundaryIndices = [t0, t1, t2].filter(index => !isBoundaryPoint[index]);
          nonBoundaryIndices.forEach(index => {
            densities[index] += density;
            counts[index] += 1;
          });
        }
      }

      // Calculate the average density for each point
      const averageDensities = densities.map((d, i) => height_normalisation * d / counts[i]);

      // Handle boundary points separately with weighted average over second-degree triangles
      for (let i = 0; i < coords.length; i++) {
        if (isBoundaryPoint[i]) {
          let totalDensity = 0;
          let totalCount = 0;
          const secondDegreeTriangles = delaunay.neighbors(i);
          for (const neighborIndex of secondDegreeTriangles) {
            totalDensity += densities[neighborIndex];
            totalCount += counts[neighborIndex];
          }
          averageDensities[i] = totalCount > 0 ? height_normalisation * totalDensity / totalCount : 0;
        }
      }

      // Calculate the center of the bounding box
      const centerX = 400;
      const centerY = 300;
      
      // Draw the Delaunay triangles with the z-height
      for (let i = 0; i < triangles.length; i += 3) {
        const t0 = triangles[i];
        const t1 = triangles[i + 1];
        const t2 = triangles[i + 2];

        const p0 = coords[t0];
        const p1 = coords[t1];
        const p2 = coords[t2];

        const height0 = Math.min(averageDensities[t0], max_height);
        const height1 = Math.min(averageDensities[t1], max_height);
        const height2 = Math.min(averageDensities[t2], max_height);

        const vertices = new Float32Array([
          (p0[0] - centerX) / 100, height0, (p0[1] - centerY) / 100,
          (p1[0] - centerX) / 100, height1, (p1[1] - centerY) / 100,
          (p2[0] - centerX) / 100, height2, (p2[1] - centerY) / 100,
        ]);

        const distance1 = Math.hypot(p0[0] - p1[0], p0[1] - p1[1]);
        const distance2 = Math.hypot(p1[0] - p2[0], p1[1] - p2[1]);
        const distance3 = Math.hypot(p2[0] - p0[0], p2[1] - p0[1]);

        if (distance1 > max_length || distance2 > max_length || distance3 > max_length) {
          continue;
        }

        const geometry = new THREE.BufferGeometry();
        geometry.setAttribute('position', new THREE.BufferAttribute(vertices, 3));

        const mesh = new THREE.Mesh(geometry, triangleMaterial);
        scene.add(mesh);
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
