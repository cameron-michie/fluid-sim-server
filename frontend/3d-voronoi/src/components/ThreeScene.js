import React, { useEffect, useRef } from 'react';
import { useChannel } from 'ably/react';
import * as THREE from 'three';
import * as d3 from 'd3';

function ThreeScene({ points }) {
  const mountRef = useRef(null);
  const { publish } = useChannel("bomb-updates");

  useEffect(() => {
    const mount = mountRef.current;

    // Scene setup
    const scene = new THREE.Scene();
    scene.background = new THREE.Color(0xffffff);

    // Camera setup
    const camera = new THREE.PerspectiveCamera(75, 800 / 600, 0.1, 1000);
    camera.position.set(0, 250, 0);
    camera.lookAt(0, 0, 0);

    // Renderer setup
    const renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setSize(800, 600);
    mount.appendChild(renderer.domElement);

    // Lights
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.5);
    scene.add(ambientLight);

    const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
    directionalLight.position.set(10, 10, 10);
    scene.add(directionalLight);

    // Create a group for triangles
    const trianglesGroup = new THREE.Group();
    scene.add(trianglesGroup);

    // Declare triangleMaterial in a higher scope
    let triangleMaterial;

    // Load the normal map texture correctly
    const textureLoader = new THREE.TextureLoader();
    const normalMap = textureLoader.load(
      '/WATER.jpg',
      () => {
        // Texture loaded, create material and mesh
        triangleMaterial = new THREE.MeshStandardMaterial({
          color: 0x87CEEB,
          transparent: true,
          opacity: 0.8,
          side: THREE.DoubleSide,
          normalMap: normalMap,
          metalness: 0.3,
          roughness: 0.1,
        });

        // Create the mesh geometry
        const geometry = new THREE.PlaneGeometry(800, 600, 100, 100); // Adjust size as needed
        const waterMesh = new THREE.Mesh(geometry, triangleMaterial);
        waterMesh.rotation.x = -Math.PI / 2; // Rotate to make it horizontal
        scene.add(waterMesh);

        // Now that the material is ready, draw the triangles
        drawTriangles(points);
      },
      undefined,
      (error) => {
        console.error('Texture loading error:', error);
      }
    );

    const drawTriangles = (pointsData) => {
      if (!triangleMaterial) return; // Ensure material is available

      // Clear previous triangle meshes
      while (trianglesGroup.children.length > 0) {
        const oldMesh = trianglesGroup.children[0];
        oldMesh.geometry.dispose();
        oldMesh.material.dispose();
        trianglesGroup.remove(oldMesh);
      }

      // Draw triangles using pointsData
      if (!pointsData || pointsData.length < 3) {
        return; // Not enough points to form a triangle
      }

      for (let i = 0; i <= pointsData.length - 3; i += 3) {
        const p0 = pointsData[i];
        const p1 = pointsData[i + 1];
        const p2 = pointsData[i + 2];

        if (p0 && p1 && p2) {
          const vertices = new Float32Array([
            p0[0], p0[1], p0[2],
            p1[0], p1[1], p1[2],
            p2[0], p2[1], p2[2],
          ]);

          const geometry = new THREE.BufferGeometry();
          geometry.setAttribute('position', new THREE.BufferAttribute(vertices, 3));

          const mesh = new THREE.Mesh(geometry, triangleMaterial);
          trianglesGroup.add(mesh);
        }
      }

      // No need to call renderer.render here; it's handled in animate()
    };

    // Animation loop
    const animate = () => {
      requestAnimationFrame(animate);
      renderer.render(scene, camera);
    };

    animate();

    // Event handler
    const handleCanvasClick = (event) => {
      const [x, y] = d3.pointer(event);
      if (x >= 0 && x <= 800 && y >= 0 && y <= 600) {
        publish('bomb-update', { x, y });
      }
    };

    renderer.domElement.addEventListener('click', handleCanvasClick);

    // Cleanup function to dispose of WebGL resources
    return () => {
      renderer.domElement.removeEventListener('click', handleCanvasClick);
      mount.removeChild(renderer.domElement);

      // Dispose the renderer
      renderer.dispose();

      // Dispose of geometries and materials in the scene
      scene.traverse((object) => {
        if (object.isMesh) {
          object.geometry.dispose();
          if (Array.isArray(object.material)) {
            object.material.forEach((material) => material.dispose());
          } else {
            object.material.dispose();
          }
        }
      });
    };
  }, []); // Empty dependency array ensures useEffect runs once

  return <div ref={mountRef} />;
}

export default ThreeScene;

