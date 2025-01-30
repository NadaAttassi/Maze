import React, { useState } from 'react';
import axios from 'axios';
import './App.css';

function App() {
  const [maze, setMaze] = useState([
    [0, 1, 0, 0, 0, 0, 1, 0, 0, 0],
    [0, 1, 0, 1, 0, 0, 1, 0, 1, 0],
    [0, 0, 0, 1, 0, 0, 0, 0, 1, 0],
    [0, 1, 1, 1, 0, 1, 1, 1, 1, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 1, 1, 1, 1, 1, 1, 1, 1, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 1, 0],
    [0, 1, 1, 1, 0, 1, 1, 0, 1, 0],
    [0, 0, 0, 1, 0, 0, 0, 0, 1, 0],
    [0, 1, 0, 0, 0, 1, 0, 0, 0, 0]
  ]);

  const [start, setStart] = useState([0, 0]);
  const [end, setEnd] = useState([9, 9]);
  const [path, setPath] = useState([]);
  const [isSolving, setIsSolving] = useState(false);

  const animatePath = (path) => {
    let i = 0;
    const interval = setInterval(() => {
      if (i < path.length) {
        setPath((prevPath) => [...prevPath, path[i]]);
        i++;
      } else {
        clearInterval(interval);
        setIsSolving(false);
      }
    }, 100);
  };

  const handleSolve = async () => {
    setIsSolving(true);
    setPath([]);
  
    try {
      const response = await axios.post('http://localhost:8080/solve', {
        maze,
        start,
        end
      }, {
        headers: {
          'Content-Type': 'application/json'
        }
      });
  
      console.log("Réponse du backend:", response.data); // Log pour déboguer
  
      if (response.data && Array.isArray(response.data.path)) {
        animatePath(response.data.path);
      } else {
        console.error('Réponse invalide du backend:', response.data);
      }
    } catch (error) {
      console.error('Erreur lors de la résolution du labyrinthe:', error);
      setIsSolving(false);
    }
  };

  const renderMaze = () => {
    return maze.map((row, rowIndex) => (
      <div className="maze-row" key={rowIndex}>
        {row.map((cell, colIndex) => {
          const isStart = rowIndex === start[0] && colIndex === start[1];
          const isEnd = rowIndex === end[0] && colIndex === end[1];
          const isOnPath = Array.isArray(path) && path.some((p) => 
            Array.isArray(p) && p[0] === rowIndex && p[1] === colIndex
          );
          return (
            <div
              key={colIndex}
              className={`maze-cell ${cell === 1 ? 'wall' : 'path'} ${isStart ? 'start' : ''} ${isEnd ? 'end' : ''} ${isOnPath ? 'path-found' : ''}`}
            />
          );
        })}
      </div>
    ));
  };

  return (
    <div>
      <h1>Maze Solver</h1>
      <div>
        <h2>Labyrinthe</h2>
        <button onClick={handleSolve} disabled={isSolving}>
          {isSolving ? 'Solving...' : 'Solve Maze'}
        </button>
        <div className="maze-container">
          {renderMaze()}
        </div>
      </div>
    </div>
  );
}

export default App;