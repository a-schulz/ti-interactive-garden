import React, { useState, useEffect } from 'react';
import { boardController } from './models/BoardController';
import { plantDatabase } from './models/PlantDatabase';
import { GAME_MODE, ENVIRONMENT } from './models/constants';
import { 
  ESP_CONFIG, 
  getLedSegmentForGrid, 
  statusToColor, 
  sendLedCommand, 
  clearAllLeds 
} from './models/LedConfig';
import './App.css';

function App() {
  const [plants, setPlants] = useState([]);
  const [infoContent, setInfoContent] = useState('');
  const [currentMode, setCurrentMode] = useState(boardController.currentGameMode);
  const [evaluations, setEvaluations] = useState({});
  const [espEnabled, setEspEnabled] = useState(ESP_CONFIG.enabled);
  const [espIpAddress, setEspIpAddress] = useState(ESP_CONFIG.ipAddress);

  // Initialize plant data and set up board
  useEffect(() => {
    setPlants(plantDatabase.getAllPlants());
  }, []);

  // Toggle ESP control and update configuration
  const toggleEspControl = () => {
    const newState = !espEnabled;
    setEspEnabled(newState);
    ESP_CONFIG.enabled = newState;
    
    if (newState) {
      setInfoContent(`<p>ESP LED control enabled. Connected to ${espIpAddress}</p>`);
      // Update all LEDs based on current evaluations
      Object.entries(evaluations).forEach(([plantId, evaluation]) => {
        const { row, col, status } = evaluation;
        updateEspLed(row, col, status);
      });
    } else {
      setInfoContent(`<p>ESP LED control disabled.</p>`);
      // Clear all LEDs when disabling
      clearAllLeds();
    }
  };

  // Update ESP IP address
  const handleEspIpChange = (e) => {
    const newIp = e.target.value;
    setEspIpAddress(newIp);
    ESP_CONFIG.ipAddress = newIp;
  };

  // Send LED update to ESP controller
  const updateEspLed = (row, col, status) => {
    if (!espEnabled) return;
    
    const segment = getLedSegmentForGrid(row, col);
    const color = statusToColor(status);
    
    // Send command to ESP
    sendLedCommand(segment, color);
  };

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

    // Get environment icons for the cell
    const getEnvironmentIcons = () => {
      const env = cell.environment;
      const lightIcon = env & ENVIRONMENT.SUNNY ? '☀️' : 
                       env & ENVIRONMENT.PARTIAL_SHADE ? '🌤️' : 
                       env & ENVIRONMENT.SHADE ? '☁️' : '';
      
      const moistureIcon = env & ENVIRONMENT.DRY ? '🏜️' : 
                          env & ENVIRONMENT.MOIST ? '💧' : 
                          env & ENVIRONMENT.WET ? '💦' : '';
      
      return (
        <div className="absolute top-1 left-1 flex flex-col items-start">
          <span className="text-xs">{lightIcon}</span>
          <span className="text-xs">{moistureIcon}</span>
        </div>
      );
    };

    return (
      <div 
        className={`grid-cell ${cell.environmentClasses.join(' ')}`}
        onDrop={handleDrop}
        onDragOver={handleDragOver}
        onDoubleClick={handleDoubleClick}
        title={boardController.getEnvironmentDescription(row, col)}
      >
        {getEnvironmentIcons()}
        
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

  // Create an environment legend component
  const EnvironmentLegend = () => {
    return (
      <div className="bg-white rounded-lg shadow p-3 mt-4">
        <h3 className="text-sm font-semibold mb-2">Environment Legend</h3>
        <div className="grid grid-cols-2 gap-2 text-xs">
          <div className="flex items-center">
            <span className="mr-1">☀️</span> Sunny
          </div>
          <div className="flex items-center">
            <span className="mr-1">🏜️</span> Dry
          </div>
          <div className="flex items-center">
            <span className="mr-1">🌤️</span> Partial Shade
          </div>
          <div className="flex items-center">
            <span className="mr-1">💧</span> Moist
          </div>
          <div className="flex items-center">
            <span className="mr-1">☁️</span> Shade
          </div>
          <div className="flex items-center">
            <span className="mr-1">💦</span> Wet
          </div>
        </div>
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
    
    if (environmentBits & ENVIRONMENT.SUNNY) parts.push('Sunny');
    if (environmentBits & ENVIRONMENT.PARTIAL_SHADE) parts.push('Partially Shaded');
    if (environmentBits & ENVIRONMENT.SHADE) parts.push('Shaded');
    
    if (environmentBits & ENVIRONMENT.DRY) parts.push('Dry');
    if (environmentBits & ENVIRONMENT.MOIST) parts.push('Moist');
    if (environmentBits & ENVIRONMENT.WET) parts.push('Wet');
    
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
    
    // Update ESP LED if enabled
    updateEspLed(row, col, evaluation.status);
  };

  // Update evaluations for all plants on the grid
  const updateEvaluations = () => {
    const newEvaluations = boardController.evaluateAllPlants();
    setEvaluations(newEvaluations);
    
    // Update ESP LEDs for all plants
    if (espEnabled) {
      Object.entries(newEvaluations).forEach(([plantId, evaluation]) => {
        updateEspLed(evaluation.row, evaluation.col, evaluation.status);
      });
    }
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

  // Create ESP control panel component
  const EspControlPanel = () => {
    return (
      <div className="bg-white rounded-lg shadow p-3 mt-4">
        <h3 className="text-sm font-semibold mb-2">ESP LED Control</h3>
        <div className="flex items-center justify-between mb-2">
          <label className="inline-flex items-center cursor-pointer">
            <input 
              type="checkbox" 
              checked={espEnabled} 
              onChange={toggleEspControl} 
              className="sr-only peer"
            />
            <div className="relative w-11 h-6 bg-gray-200 peer-focus:outline-none peer-focus:ring-4 peer-focus:ring-green-300 rounded-full peer peer-checked:after:translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-[2px] after:left-[2px] after:bg-white after:border-gray-300 after:border after:rounded-full after:h-5 after:w-5 after:transition-all peer-checked:bg-green-600"></div>
            <span className="ml-3 text-sm font-medium text-gray-700">
              {espEnabled ? 'Enabled' : 'Disabled'}
            </span>
          </label>
        </div>
        <div className="mb-2">
          <label className="block text-xs mb-1">ESP IP Address:</label>
          <input
            type="text"
            value={espIpAddress}
            onChange={handleEspIpChange}
            className="w-full px-2 py-1 text-sm border border-gray-300 rounded"
            placeholder="192.168.4.1"
          />
        </div>
        <div className="text-xs text-gray-500">
          <p>When enabled, LED updates will be sent to the ESP controller.</p>
        </div>
      </div>
    );
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
          
          <EnvironmentLegend />
          
          <EspControlPanel />
          
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
