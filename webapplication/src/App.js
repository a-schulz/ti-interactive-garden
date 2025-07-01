import React, { useState, useEffect } from 'react';
import { boardController } from './models/BoardController';
import { plantDatabase } from './models/PlantDatabase';
import { GAME_MODE } from './models/constants';
import './App.css';

function App() {
  const [plants, setPlants] = useState([]);
  const [infoContent, setInfoContent] = useState('');
  const [currentMode, setCurrentMode] = useState(boardController.currentGameMode);
  const [evaluations, setEvaluations] = useState({});

  // Initialize plant data and set up board
  useEffect(() => {
    setPlants(plantDatabase.getAllPlants());
  }, []);

  // Create a grid cell component
  const GridCell = ({ row, col }) => {
    const cell = boardController.getGridCell(row, col);
    const plantId = cell.plantId;
    const plant = plantId ? plantDatabase.getPlant(plantId) : null;
    const evaluation = evaluations[plantId];
    
    // Determine LED color based on evaluation status
    let ledClass = '';
    if (evaluation?.status === 'happy') {
      ledClass = 'green';
    } else if (evaluation?.status === 'unhappy') {
      ledClass = 'red';
    } else if (evaluation?.status === 'neutral') {
      ledClass = 'white';
    }

    // Handle drop event
    const handleDrop = (e) => {
      e.preventDefault();
      const plantId = e.dataTransfer.getData('text/plain');
      if (plantId) {
        placePlant(plantId, row, col);
      }
    };

    // Handle drag over event
    const handleDragOver = (e) => {
      e.preventDefault();
    };

    // Handle double click to remove plant
    const handleDoubleClick = () => {
      if (plantId) {
        boardController.removePlant(plantId);
        updateEvaluations();
        setInfoContent(`Removed ${plant.name} from the garden.`);
      }
    };

    return (
      <div 
        className={`grid-cell ${cell.environmentClasses.join(' ')}`}
        onDrop={handleDrop}
        onDragOver={handleDragOver}
        onDoubleClick={handleDoubleClick}
        title={boardController.getEnvironmentDescription(row, col)}
      >
        {plant && (
          <img 
            src={plant.imagePath} 
            alt={plant.name} 
            className="plant-on-grid"
            style={{ zIndex: 10 }}
          />
        )}
        {ledClass && <div className={`led-ring ${ledClass}`}></div>}
      </div>
    );
  };

  // Create a plant item for the palette
  const PlantItem = ({ plant }) => {
    // Handle drag start
    const handleDragStart = (e) => {
      e.dataTransfer.setData('text/plain', plant.id);
    };

    // Handle click to show plant info
    const handleClick = () => {
      setInfoContent(`
        <h3>${plant.name}</h3>
        <p>Prefers: ${getEnvironmentText(plant.preferredEnvironment)}</p>
        <p>Tolerates: ${getEnvironmentText(plant.toleratedEnvironment)}</p>
        <h4>Relationships:</h4>
        <ul>
          ${getPlantRelationships(plant)}
        </ul>
      `);
    };

    return (
      <div 
        className="plant-item" 
        draggable="true"
        onDragStart={handleDragStart}
        onClick={handleClick}
        style={{ borderColor: plant.color }}
      >
        <img src={plant.imagePath} alt={plant.name} className="plant-image" />
        <span className="plant-name">{plant.name}</span>
      </div>
    );
  };

  // Convert environment bits to readable text
  const getEnvironmentText = (environmentBits) => {
    const parts = [];
    
    if (environmentBits & 1) parts.push('Sunny');
    if (environmentBits & 2) parts.push('Partially Shaded');
    if (environmentBits & 4) parts.push('Shaded');
    
    if (environmentBits & 8) parts.push('Dry');
    if (environmentBits & 16) parts.push('Moist');
    if (environmentBits & 32) parts.push('Wet');
    
    return parts.join(', ');
  };

  // Get plant relationships as HTML
  const getPlantRelationships = (plant) => {
    let html = '';
    for (const otherPlant of plants) {
      if (plant.id === otherPlant.id) continue;
      
      const relationship = plant.getRelationship(otherPlant.id);
      let relationText = '';
      
      if (relationship === 1) {
        relationText = `<span class="text-green-600">Likes</span>`;
      } else if (relationship === 2) {
        relationText = `<span class="text-red-600">Dislikes</span>`;
      } else {
        relationText = `<span class="text-gray-600">Neutral</span>`;
      }
      
      html += `<li>${relationText} ${otherPlant.name}</li>`;
    }
    return html;
  };

  // Place a plant on the grid
  const placePlant = (plantId, row, col) => {
    const plant = plantDatabase.getPlant(plantId);
    if (!plant) return;
    
    boardController.placePlant(plantId, row, col);
    updateEvaluations();
    
    // Show evaluation for this plant
    const evaluation = boardController.evaluatePlant(plantId);
    showPlantEvaluation(evaluation);
  };

  // Update evaluations for all plants on the grid
  const updateEvaluations = () => {
    setEvaluations(boardController.evaluateAllPlants());
  };

  // Show evaluation results for a plant
  const showPlantEvaluation = (evaluation) => {
    if (evaluation.status === 'error') {
      setInfoContent(`<p>Error: ${evaluation.reason}</p>`);
      return;
    }
    
    const plant = plantDatabase.getPlant(
      boardController.getPlantAt(evaluation.row, evaluation.col)
    );
    
    let html = `<h3>${plant.name} Evaluation</h3>`;
    
    // Add status icon
    let statusIcon;
    if (evaluation.status === 'happy') {
      statusIcon = '😊';
    } else if (evaluation.status === 'unhappy') {
      statusIcon = '😟';
    } else {
      statusIcon = '😐';
    }
    
    html += `<div class="evaluation-status ${evaluation.status}">${statusIcon} ${evaluation.reason}</div>`;
    
    // Add details based on game mode
    if (boardController.currentGameMode === GAME_MODE.ENVIRONMENT_MODE || 
        boardController.currentGameMode === GAME_MODE.COMBINED_MODE) {
      const cell = boardController.getGridCell(evaluation.row, evaluation.col);
      html += `
        <h4>Environment:</h4>
        <p>${boardController.getEnvironmentDescription(evaluation.row, evaluation.col)}</p>
      `;
    }
    
    if (boardController.currentGameMode === GAME_MODE.NEIGHBORS_MODE || 
        boardController.currentGameMode === GAME_MODE.COMBINED_MODE) {
      if (evaluation.neighborStatus.likedNeighbors && evaluation.neighborStatus.likedNeighbors.length > 0) {
        html += `
          <h4>Likes neighbors:</h4>
          <p>${evaluation.neighborStatus.likedNeighbors.join(', ')}</p>
        `;
      }
      
      if (evaluation.neighborStatus.hatedNeighbors && evaluation.neighborStatus.hatedNeighbors.length > 0) {
        html += `
          <h4>Dislikes neighbors:</h4>
          <p>${evaluation.neighborStatus.hatedNeighbors.join(', ')}</p>
        `;
      }
    }
    
    setInfoContent(html);
  };

  // Handle game mode change
  const handleModeChange = () => {
    const newMode = boardController.changeGameMode();
    setCurrentMode(newMode);
    updateEvaluations();
    
    // Show mode description
    setInfoContent(`
      <h3>Game Mode Changed!</h3>
      <p>Now in <strong>${boardController.getGameModeText()}</strong> mode.</p>
      <p>${getModeDescription(newMode)}</p>
    `);
  };

  // Get description for the game mode
  const getModeDescription = (mode) => {
    switch(mode) {
      case GAME_MODE.ENVIRONMENT_MODE:
        return 'This mode checks if plants are happy in their environment based on sunlight and moisture conditions.';
      case GAME_MODE.NEIGHBORS_MODE:
        return 'This mode checks if plants like their neighboring plants (including diagonal neighbors).';
      case GAME_MODE.COMBINED_MODE:
        return 'This mode combines both environment and neighbor checks. Plants must be happy with both to thrive!';
      default:
        return '';
    }
  };

  return (
    <div className="app-container">
      <header className="mb-8 text-center">
        <h1 className="text-3xl font-bold text-green-800">Interactive Garden</h1>
        <p className="text-gray-600">Learn how plants interact with their environment and each other!</p>
      </header>
      
      <div className="game-area">
        {/* Board */}
        <div className="board-container">
          <div className="board-header">
            <h2 className="text-xl font-semibold text-green-700">Garden Board</h2>
            <button 
              className="mode-toggle"
              onClick={handleModeChange}
            >
              Change Mode
            </button>
          </div>
          
          <p className="current-mode mb-2">Current Mode: <strong>{boardController.getGameModeText()}</strong></p>
          
          <div className="board-grid">
            {/* Generate grid cells */}
            {Array.from({ length: boardController.rows }).map((_, row) => (
              Array.from({ length: boardController.cols }).map((_, col) => (
                <GridCell key={`${row}-${col}`} row={row} col={col} />
              ))
            ))}
          </div>
          
          <div className="info-panel">
            <div className="info-header">Information</div>
            <div 
              className="info-content" 
              dangerouslySetInnerHTML={{ __html: infoContent || 'Drag plants onto the grid to get started!' }}
            />
          </div>
        </div>
        
        {/* Plant palette */}
        <div className="w-full lg:w-64">
          <h2 className="text-xl font-semibold text-green-700 mb-2">Plants</h2>
          <p className="text-sm text-gray-600 mb-4">Drag plants to place them on the garden</p>
          
          <div className="plant-palette">
            {plants.map(plant => (
              <PlantItem key={plant.id} plant={plant} />
            ))}
          </div>
          
          <div className="mt-6 text-sm">
            <h3 className="font-semibold mb-2">Tips:</h3>
            <ul className="list-disc pl-5 space-y-1">
              <li>Double-click to remove plants</li>
              <li>Click plants in the palette to see details</li>
              <li>Hover over grid cells to see environment</li>
              <li>Change modes to test different aspects</li>
            </ul>
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;
