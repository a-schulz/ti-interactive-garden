/**
 * Interactive Garden - Main Application
 * Handles UI interactions and connections between components
 */

document.addEventListener('DOMContentLoaded', () => {
    // DOM Elements
    const boardGridElement = document.getElementById('board-grid');
    const plantPaletteElement = document.getElementById('plant-palette');
    const modeToggleButton = document.getElementById('mode-toggle');
    const currentModeElement = document.getElementById('current-mode');
    const infoContentElement = document.getElementById('info-content');
    
    // Setup
    initializeBoard();
    populatePlantPalette();
    setupEventListeners();
    
    // Initialize the board grid UI
    function initializeBoard() {
        boardGridElement.innerHTML = '';
        
        for (let row = 0; row < boardController.rows; row++) {
            for (let col = 0; col < boardController.cols; col++) {
                const cell = createGridCell(row, col);
                boardGridElement.appendChild(cell);
            }
        }
    }
    
    // Create a single grid cell
    function createGridCell(row, col) {
        const cellInfo = boardController.getGridCell(row, col);
        const cell = document.createElement('div');
        cell.classList.add('grid-cell');
        cell.dataset.row = row;
        cell.dataset.col = col;
        
        // Add environment classes
        cellInfo.environmentClasses.forEach(className => {
            cell.classList.add(className);
        });
        
        // Setup drag and drop events
        cell.addEventListener('dragover', (e) => {
            e.preventDefault();
            cell.classList.add('cell-highlight');
        });
        
        cell.addEventListener('dragleave', () => {
            cell.classList.remove('cell-highlight');
        });
        
        cell.addEventListener('drop', (e) => {
            e.preventDefault();
            cell.classList.remove('cell-highlight');
            
            const plantId = e.dataTransfer.getData('text/plain');
            if (plantId) {
                placePlantOnCell(plantId, row, col);
            }
        });
        
        // Double click to remove plant
        cell.addEventListener('dblclick', () => {
            const plantId = boardController.getPlantAt(row, col);
            if (plantId) {
                boardController.removePlant(plantId);
                updateGridCell(row, col);
                updateInfoPanel(`Removed ${plantDatabase.getPlant(plantId).name} from the garden.`);
            }
        });
        
        return cell;
    }
    
    // Create plant item for the palette
    function createPlantItem(plant) {
        const plantItem = document.createElement('div');
        plantItem.classList.add('plant-item');
        plantItem.dataset.plantId = plant.id;
        plantItem.style.backgroundImage = `url(${plant.imagePath})`;
        plantItem.draggable = true;
        
        // Add label
        const label = document.createElement('div');
        label.classList.add('plant-label');
        label.textContent = plant.name;
        plantItem.appendChild(label);
        
        // Setup drag events
        plantItem.addEventListener('dragstart', (e) => {
            e.dataTransfer.setData('text/plain', plant.id);
            plantItem.classList.add('plant-dragging');
            
            // Show plant info on drag
            updateInfoPanel(`${plant.name} - Drag to place in the garden!`);
        });
        
        plantItem.addEventListener('dragend', () => {
            plantItem.classList.remove('plant-dragging');
        });
        
        // Click to show info
        plantItem.addEventListener('click', () => {
            showPlantInfo(plant);
        });
        
        return plantItem;
    }
    
    // Populate the plant palette with available plants
    function populatePlantPalette() {
        plantPaletteElement.innerHTML = '';
        
        plantDatabase.getAllPlants().forEach(plant => {
            const plantItem = createPlantItem(plant);
            plantPaletteElement.appendChild(plantItem);
        });
    }
    
    // Place a plant on a grid cell
    function placePlantOnCell(plantId, row, col) {
        const plant = plantDatabase.getPlant(plantId);
        if (!plant) return;
        
        // Check if there's already a plant in this cell
        const existingPlantId = boardController.getPlantAt(row, col);
        if (existingPlantId) {
            boardController.removePlant(existingPlantId);
        }
        
        // Place the plant
        boardController.placePlant(plantId, row, col);
        
        // Update the UI
        updateGridCell(row, col);
        
        // Evaluate the plant and show feedback
        const evaluation = boardController.evaluatePlant(plantId);
        showPlantEvaluation(evaluation);
    }
    
    // Update a grid cell's visual state
    function updateGridCell(row, col) {
        // Find the cell DOM element
        const cellElement = document.querySelector(`.grid-cell[data-row="${row}"][data-col="${col}"]`);
        if (!cellElement) return;
        
        // Clear any existing plants and LEDs
        const existingPlant = cellElement.querySelector('.plant-on-grid');
        if (existingPlant) {
            cellElement.removeChild(existingPlant);
        }
        
        const existingLedRing = cellElement.querySelector('.led-ring');
        if (existingLedRing) {
            cellElement.removeChild(existingLedRing);
        }
        
        // Check if there's a plant in this cell
        const plantId = boardController.getPlantAt(row, col);
        if (!plantId) return;
        
        // Add plant visual
        const plant = plantDatabase.getPlant(plantId);
        const plantElement = document.createElement('div');
        plantElement.classList.add('plant-on-grid');
        plantElement.style.backgroundImage = `url(${plant.imagePath})`;
        cellElement.appendChild(plantElement);
        
        // Add LED ring effect
        const evaluation = boardController.evaluatePlant(plantId);
        const ledRing = document.createElement('div');
        ledRing.classList.add('led-ring');
        
        // Set LED color based on evaluation
        if (evaluation.status === 'happy') {
            ledRing.classList.add('green');
        } else if (evaluation.status === 'unhappy') {
            ledRing.classList.add('red');
        } else {
            ledRing.classList.add('white');
        }
        
        cellElement.appendChild(ledRing);
    }
    
    // Update all cells on the grid
    function updateAllGridCells() {
        for (let row = 0; row < boardController.rows; row++) {
            for (let col = 0; col < boardController.cols; col++) {
                updateGridCell(row, col);
            }
        }
    }
    
    // Show detailed information about a plant
    function showPlantInfo(plant) {
        let html = `
            <h3>${plant.name}</h3>
            <div class="plant-info-image" style="background-image: url(${plant.imagePath})"></div>
            <h4>Growing Conditions:</h4>
            <ul>
                <li>Prefers: ${getEnvironmentText(plant.preferredEnvironment)}</li>
                <li>Tolerates: ${getEnvironmentText(plant.toleratedEnvironment)}</li>
            </ul>
            <h4>Relationships:</h4>
            <ul>
        `;
        
        // Add relationship information
        let hasRelationships = false;
        
        plantDatabase.getAllPlants().forEach(otherPlant => {
            if (otherPlant.id === plant.id) return;
            
            const relationship = plant.getRelationship(otherPlant.id);
            if (relationship === RELATIONSHIP.LIKES) {
                html += `<li><span class="color-dot green"></span> Likes growing near ${otherPlant.name}</li>`;
                hasRelationships = true;
            } else if (relationship === RELATIONSHIP.HATES) {
                html += `<li><span class="color-dot red"></span> Dislikes growing near ${otherPlant.name}</li>`;
                hasRelationships = true;
            }
        });
        
        if (!hasRelationships) {
            html += `<li>No strong preferences about neighbors</li>`;
        }
        
        html += `</ul>`;
        
        updateInfoPanel(html);
    }
    
    // Show evaluation results for a plant
    function showPlantEvaluation(evaluation) {
        if (evaluation.status === 'error') {
            updateInfoPanel(`<p>Error: ${evaluation.reason}</p>`);
            return;
        }
        
        const plant = plantDatabase.getPlant(boardController.getPlantAt(evaluation.row, evaluation.col));
        
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
            html += `
                <h4>Environment:</h4>
                <p>${evaluation.environmentStatus.reason}</p>
            `;
        }
        
        if (boardController.currentGameMode === GAME_MODE.NEIGHBORS_MODE || 
            boardController.currentGameMode === GAME_MODE.COMBINED_MODE) {
            html += `<h4>Neighbors:</h4>`;
            
            if (evaluation.neighborStatus.likedNeighbors && evaluation.neighborStatus.likedNeighbors.length > 0) {
                html += `<p><span class="color-dot green"></span> Likes: ${evaluation.neighborStatus.likedNeighbors.join(', ')}</p>`;
            }
            
            if (evaluation.neighborStatus.hatedNeighbors && evaluation.neighborStatus.hatedNeighbors.length > 0) {
                html += `<p><span class="color-dot red"></span> Dislikes: ${evaluation.neighborStatus.hatedNeighbors.join(', ')}</p>`;
            }
            
            if ((!evaluation.neighborStatus.likedNeighbors || evaluation.neighborStatus.likedNeighbors.length === 0) && 
                (!evaluation.neighborStatus.hatedNeighbors || evaluation.neighborStatus.hatedNeighbors.length === 0)) {
                html += `<p>No neighboring plants affecting this ${plant.name}.</p>`;
            }
        }
        
        updateInfoPanel(html);
    }
    
    // Update the info panel content
    function updateInfoPanel(content) {
        infoContentElement.innerHTML = content;
    }
    
    // Convert environment bits to readable text
    function getEnvironmentText(environmentBits) {
        const parts = [];
        
        // Check light conditions
        if (environmentBits & ENVIRONMENT.SUNNY) {
            parts.push('Sunny');
        }
        if (environmentBits & ENVIRONMENT.PARTIAL_SHADE) {
            parts.push('Partial Shade');
        }
        if (environmentBits & ENVIRONMENT.SHADE) {
            parts.push('Full Shade');
        }
        
        // Check moisture conditions
        if (environmentBits & ENVIRONMENT.DRY) {
            parts.push('Dry Soil');
        }
        if (environmentBits & ENVIRONMENT.MOIST) {
            parts.push('Moist Soil');
        }
        if (environmentBits & ENVIRONMENT.WET) {
            parts.push('Wet Soil');
        }
        
        return parts.join(', ');
    }
    
    // Setup event listeners
    function setupEventListeners() {
        // Mode toggle button
        modeToggleButton.addEventListener('click', () => {
            const newMode = boardController.changeGameMode();
            currentModeElement.textContent = `Current Mode: ${boardController.getGameModeText()}`;
            
            // Update all plants to reflect new mode
            updateAllGridCells();
            
            // Update info panel
            updateInfoPanel(`
                <p>Game mode changed to <strong>${boardController.getGameModeText()}</strong>!</p>
                <p>This mode ${getModeDescription(newMode)}</p>
            `);
        });
    }
    
    // Get description for the game mode
    function getModeDescription(mode) {
        switch(mode) {
            case GAME_MODE.ENVIRONMENT_MODE:
                return 'checks if plants are happy in their environment (sunlight, moisture).';
            case GAME_MODE.NEIGHBORS_MODE:
                return 'checks if plants like their neighboring plants.';
            case GAME_MODE.COMBINED_MODE:
                return 'checks both the environment and neighboring plants.';
            default:
                return '';
        }
    }
});