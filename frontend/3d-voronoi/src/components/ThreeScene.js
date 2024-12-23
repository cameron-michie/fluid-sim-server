import React, { useEffect, useRef } from 'react';
import { useChannel } from 'ably/react';
import * as THREE from 'three';
import * as d3 from 'd3';

function ThreeScene({ points }) {
  const mountRef = useRef(null);
  const { publish } = useChannel("bomb-updates");
  const sceneRef = useRef(null);
  const cameraRef = useRef(null);
  const rendererRef = useRef(null);
  const trianglesGroupRef = useRef(null);

  useEffect(() => {
    const mount = mountRef.current;

    // Scene setup
    const scene = new THREE.Scene();
    scene.background = new THREE.Color(0xffffff);
    sceneRef.current = scene;

    // Camera setup
    const camera = new THREE.PerspectiveCamera(75, 800 / 600, 0.1, 1000);
    camera.position.set(400, -100, 400); // Adjusted position
    camera.lookAt(400, 300, 0); // Adjusted lookAt target
    cameraRef.current = camera;

    // Renderer setup
    const renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setSize(800, 600);
    renderer.shadowMap.enabled = true;
    renderer.shadowMap.type = THREE.PCFSoftShadowMap; // Optional: for softer shadows
    mount.appendChild(renderer.domElement);
    rendererRef.current = renderer;

    const color = '#ffffff';
    const intensity = 1; // Reduced intensity for more balanced lighting

    const directionalLight = new THREE.DirectionalLight(color, intensity);
    directionalLight.position.set(400, 300, 1000);
    directionalLight.castShadow = true;
    
    // Configure shadow properties for better quality
    directionalLight.shadow.mapSize.width = 1024;
    directionalLight.shadow.mapSize.height = 1024;
    directionalLight.shadow.camera.near = 0.5;
    directionalLight.shadow.camera.far = 500;
    directionalLight.shadow.bias = -0.001; // Prevent shadow acne
    scene.add(directionalLight);
    
    const ambientLight = new THREE.AmbientLight(color, 0.5); // Increased ambient light for brightness
    scene.add(ambientLight);

    // Create a group for triangles
    const trianglesGroup = new THREE.Group();
    scene.add(trianglesGroup);
    trianglesGroupRef.current = trianglesGroup;

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

  useEffect(() => {
    const trianglesGroup = trianglesGroupRef.current;
    const scene = sceneRef.current;

    // Clear previous triangle meshes
    while (trianglesGroup.children.length > 0) {
      const oldMesh = trianglesGroup.children[0];
      oldMesh.geometry.dispose();
      oldMesh.material.dispose();
      trianglesGroup.remove(oldMesh);
    }

    // Draw planes connecting triangle points
    if (points && points.length >= 3) {
      // Iterate in steps of three to form triangles
      for (let i = 0; i <= points.length - 3; i += 3) {
        const p0 = points[i];
        const p1 = points[i + 1];
        const p2 = points[i + 2];

        if (p0 && p1 && p2) {
          const geometry = new THREE.BufferGeometry();
          const vertices = new Float32Array([
            ...p0,
            ...p1,
            ...p2,
          ]);
          geometry.setAttribute('position', new THREE.BufferAttribute(vertices, 3));

          // Define UV coordinates
          const uvs = new Float32Array([
            0, 0,
            1, 0,
            0.5, 1,
          ]);
          geometry.setAttribute('uv', new THREE.BufferAttribute(uvs, 2));

          // Compute normals
          geometry.computeVertexNormals();

          const material = new THREE.MeshStandardMaterial({
            color: 0x87ceeb,
            side: THREE.DoubleSide,
            metalness: 0.3,
            roughness: 0.1,
          });
          material.flatShading = true

          const mesh = new THREE.Mesh(geometry, material);
          mesh.castShadow = true; // Allow mesh to cast shadows
          mesh.receiveShadow = true; // Allow mesh to receive shadows
          trianglesGroup.add(mesh);
        }
      }
    }
  }, [points]); // Re-run this effect when points change

  return <div ref={mountRef} />;
}

export default ThreeScene;

