import { ENVIRONMENT, GAME_MODE, RELATIONSHIP } from "./constants";
import { plantDatabase } from "./PlantDatabase";

/**
 * BoardController manages the game board, plant positions, and game logic
 */
class BoardController {
  constructor() {
    // Board configuration
    this.rows = 6;
    this.cols = 6;
    this.grid = [];
    this.plants = {}; // Plants placed on the grid
    this.currentGameMode = GAME_MODE.ENVIRONMENT_MODE;

    // Create the grid data structure with predefined environments
    this.initializeGrid();
  }

  // Initialize the grid with predefined environments
  initializeGrid() {
    this.grid = [];

    // Predefined environment settings - 4x4 grid
    // Each cell has a defined light condition and moisture level
    const predefinedEnvironments = [
      // Row 0
      [
        { light: ENVIRONMENT.PARTIAL_SHADE, moisture: ENVIRONMENT.DRY },
        {},
        {},
        { light: ENVIRONMENT.PARTIAL_SHADE, moisture: ENVIRONMENT.WET },
        {},
        { light: ENVIRONMENT.PARTIAL_SHADE, moisture: ENVIRONMENT.DRY },
      ],
      // Row 1
      [
        {},
        { light: ENVIRONMENT.PARTIAL_SHADE, moisture: ENVIRONMENT.MOIST },
        {},
        {},
        { light: ENVIRONMENT.PARTIAL_SHADE, moisture: ENVIRONMENT.MOIST },
        {},
      ],
      // Row 2
      [
        { light: ENVIRONMENT.PARTIAL_SHADE, moisture: ENVIRONMENT.DRY },
        {},
        { light: ENVIRONMENT.PARTIAL_SHADE, moisture: ENVIRONMENT.MOIST },
        {},
        {},
        { light: ENVIRONMENT.PARTIAL_SHADE, moisture: ENVIRONMENT.DRY },
      ],
      // Row 3
      [
        {},
        { light: ENVIRONMENT.PARTIAL_SHADE, moisture: ENVIRONMENT.MOIST },
        {},
        {},
        { light: ENVIRONMENT.PARTIAL_SHADE, moisture: ENVIRONMENT.MOIST },
        {},
      ],
      // Row 4
      [
        { light: ENVIRONMENT.PARTIAL_SHADE, moisture: ENVIRONMENT.DRY },
        {},
        {},
        { light: ENVIRONMENT.PARTIAL_SHADE, moisture: ENVIRONMENT.WET },
        {},
        { light: ENVIRONMENT.PARTIAL_SHADE, moisture: ENVIRONMENT.DRY },
      ],
      // Row 5
      [
        {},
        { light: ENVIRONMENT.PARTIAL_SHADE, moisture: ENVIRONMENT.MOIST },
        {},
        {},
        { light: ENVIRONMENT.PARTIAL_SHADE, moisture: ENVIRONMENT.MOIST },
        {},
      ],
    ];

    for (let row = 0; row < this.rows; row++) {
      this.grid[row] = [];
      for (let col = 0; col < this.cols; col++) {
        const cellEnv = predefinedEnvironments[row][col];
        const lightCondition = cellEnv.light;
        const moistureCondition = cellEnv.moisture;

        this.grid[row][col] = {
          row: row,
          col: col,
          environment: lightCondition | moistureCondition,
          plantId: null,
          environmentClasses: this.getEnvironmentClasses(
            lightCondition,
            moistureCondition
          ),
        };
      }
    }
  }

  // Get a random light condition (sunny, partial shade, or shade)
  getRandomLightCondition() {
    const lightConditions = [
      ENVIRONMENT.SUNNY,
      ENVIRONMENT.PARTIAL_SHADE,
      ENVIRONMENT.SHADE,
    ];
    return lightConditions[Math.floor(Math.random() * lightConditions.length)];
  }

  // Get a random moisture condition (dry, moist, or wet)
  getRandomMoistureCondition() {
    const moistureConditions = [
      ENVIRONMENT.DRY,
      ENVIRONMENT.MOIST,
      ENVIRONMENT.WET,
    ];
    return moistureConditions[
      Math.floor(Math.random() * moistureConditions.length)
    ];
  }

  // Get CSS classes for the environment
  getEnvironmentClasses(lightCondition, moistureCondition) {
    const classes = [];

    // Add light condition class
    if (lightCondition === ENVIRONMENT.SUNNY) {
      classes.push("sunny");
    } else if (lightCondition === ENVIRONMENT.PARTIAL_SHADE) {
      classes.push("partial-shade");
    } else if (lightCondition === ENVIRONMENT.SHADE) {
      classes.push("shade");
    }

    // Add moisture condition class
    if (moistureCondition === ENVIRONMENT.DRY) {
      classes.push("dry");
    } else if (moistureCondition === ENVIRONMENT.MOIST) {
      classes.push("moist");
    } else if (moistureCondition === ENVIRONMENT.WET) {
      classes.push("wet");
    }

    return classes;
  }

  // Place a plant on the grid
  placePlant(plantId, row, col) {
    if (row < 0 || row >= this.rows || col < 0 || col >= this.cols) {
      console.error("Invalid grid position:", row, col);
      return false;
    }

    // Remove plant from previous position if it exists
    this.removePlant(plantId);

    // Update the grid cell
    this.grid[row][col].plantId = plantId;

    // Save plant position for easy reference
    this.plants[plantId] = { row, col };

    console.log(`Placed plant ${plantId} at (${row}, ${col})`);
    return true;
  }

  // Remove a plant from the grid
  removePlant(plantId) {
    if (!this.plants[plantId]) {
      return false;
    }

    const { row, col } = this.plants[plantId];
    this.grid[row][col].plantId = null;
    delete this.plants[plantId];

    return true;
  }

  // Change the current game mode
  changeGameMode() {
    switch (this.currentGameMode) {
      case GAME_MODE.ENVIRONMENT_MODE:
        this.currentGameMode = GAME_MODE.NEIGHBORS_MODE;
        console.log("Game Mode changed to: Neighbors Mode");
        break;
      case GAME_MODE.NEIGHBORS_MODE:
        this.currentGameMode = GAME_MODE.COMBINED_MODE;
        console.log("Game Mode changed to: Combined Mode");
        break;
      case GAME_MODE.COMBINED_MODE:
      default:
        this.currentGameMode = GAME_MODE.ENVIRONMENT_MODE;
        console.log("Game Mode changed to: Environment Mode");
        break;
    }

    // Re-evaluate all plants based on the new mode
    this.evaluateAllPlants();

    return this.currentGameMode;
  }

  // Get the current game mode text
  getGameModeText() {
    switch (this.currentGameMode) {
      case GAME_MODE.ENVIRONMENT_MODE:
        return "Environment";
      case GAME_MODE.NEIGHBORS_MODE:
        return "Neighbors";
      case GAME_MODE.COMBINED_MODE:
        return "Combined (Environment & Neighbors)";
      default:
        return "Unknown";
    }
  }

  // Check if a plant is happy in its environment
  checkEnvironment(plantId, row, col) {
    const plant = plantDatabase.getPlant(plantId);
    const cellEnvironment = this.grid[row][col].environment;

    const thrives = plant.thrives(cellEnvironment);
    const tolerates = plant.tolerates(cellEnvironment);

    if (thrives) {
      return {
        status: "happy",
        reason: `${plant.name} thrives in this environment!`,
      };
    } else if (tolerates) {
      return {
        status: "neutral",
        reason: `${plant.name} tolerates this environment.`,
      };
    } else {
      return {
        status: "unhappy",
        reason: `${plant.name} doesn't like this environment.`,
      };
    }
  }

  // Check how a plant interacts with its neighbors
  checkNeighbors(plantId, row, col) {
    const plant = plantDatabase.getPlant(plantId);
    let likedNeighbors = [];
    let hatedNeighbors = [];

    // Check all 8 surrounding cells (including diagonals)
    for (
      let r = Math.max(0, row - 1);
      r <= Math.min(this.rows - 1, row + 1);
      r++
    ) {
      for (
        let c = Math.max(0, col - 1);
        c <= Math.min(this.cols - 1, col + 1);
        c++
      ) {
        // Skip the center cell (the plant itself)
        if (r === row && c === col) continue;

        // Check if there's a plant in this cell
        const neighborPlantId = this.grid[r][c].plantId;
        if (!neighborPlantId) continue;

        // Get the relationship
        const relationship = plantDatabase.getRelationship(
          plantId,
          neighborPlantId
        );
        const neighborPlant = plantDatabase.getPlant(neighborPlantId);

        if (relationship === RELATIONSHIP.LIKES) {
          likedNeighbors.push(neighborPlant.name);
        } else if (relationship === RELATIONSHIP.HATES) {
          hatedNeighbors.push(neighborPlant.name);
        }
      }
    }

    if (hatedNeighbors.length > 0) {
      return {
        status: "unhappy",
        reason: `${plant.name} doesn't like being next to ${hatedNeighbors.join(
          ", "
        )}.`,
        likedNeighbors,
        hatedNeighbors,
      };
    } else if (likedNeighbors.length > 0) {
      return {
        status: "happy",
        reason: `${plant.name} likes being next to ${likedNeighbors.join(
          ", "
        )}.`,
        likedNeighbors,
        hatedNeighbors,
      };
    } else {
      return {
        status: "neutral",
        reason: `${plant.name} has no strong feelings about its neighbors.`,
        likedNeighbors,
        hatedNeighbors,
      };
    }
  }

  // Evaluate a plant based on current game mode
  evaluatePlant(plantId) {
    if (!this.plants[plantId]) {
      return { status: "error", reason: "Plant not found on grid" };
    }

    const { row, col } = this.plants[plantId];
    let environmentStatus = { status: "neutral", reason: "" };
    let neighborStatus = { status: "neutral", reason: "" };

    // Check environment
    if (
      this.currentGameMode === GAME_MODE.ENVIRONMENT_MODE ||
      this.currentGameMode === GAME_MODE.COMBINED_MODE
    ) {
      environmentStatus = this.checkEnvironment(plantId, row, col);
    }

    // Check neighbors
    if (
      this.currentGameMode === GAME_MODE.NEIGHBORS_MODE ||
      this.currentGameMode === GAME_MODE.COMBINED_MODE
    ) {
      neighborStatus = this.checkNeighbors(plantId, row, col);
    }

    // Determine overall status
    let status, reason;

    if (this.currentGameMode === GAME_MODE.ENVIRONMENT_MODE) {
      status = environmentStatus.status;
      reason = environmentStatus.reason;
    } else if (this.currentGameMode === GAME_MODE.NEIGHBORS_MODE) {
      status = neighborStatus.status;
      reason = neighborStatus.reason;
    } else {
      // Combined mode - unhappy if either is unhappy, happy if both are happy
      if (
        environmentStatus.status === "unhappy" ||
        neighborStatus.status === "unhappy"
      ) {
        status = "unhappy";
        reason = "Plant is unhappy with either environment or neighbors.";
      } else if (
        environmentStatus.status === "happy" &&
        neighborStatus.status === "happy"
      ) {
        status = "happy";
        reason = "Plant is thriving with both environment and neighbors!";
      } else {
        status = "neutral";
        reason = "Plant is content but not thriving.";
      }
    }

    return {
      status,
      reason,
      environmentStatus,
      neighborStatus,
      row,
      col,
    };
  }

  // Evaluate all plants on the grid
  evaluateAllPlants() {
    const results = {};

    for (const plantId in this.plants) {
      results[plantId] = this.evaluatePlant(plantId);
    }

    return results;
  }

  // Get the plant at a specific position
  getPlantAt(row, col) {
    if (row < 0 || row >= this.rows || col < 0 || col >= this.cols) {
      return null;
    }

    return this.grid[row][col].plantId;
  }

  // Check if a cell has a plant
  hasPlant(row, col) {
    return this.getPlantAt(row, col) !== null;
  }

  // Get all plants on the grid
  getAllPlants() {
    return this.plants;
  }

  // Get grid cell information
  getGridCell(row, col) {
    if (row < 0 || row >= this.rows || col < 0 || col >= this.cols) {
      return null;
    }

    return this.grid[row][col];
  }

  // Get environment description for a cell
  getEnvironmentDescription(row, col) {
    const cell = this.getGridCell(row, col);
    if (!cell) return "";

    const env = cell.environment;
    const light =
      env & (ENVIRONMENT.SUNNY | ENVIRONMENT.PARTIAL_SHADE | ENVIRONMENT.SHADE);
    const moisture =
      env & (ENVIRONMENT.DRY | ENVIRONMENT.MOIST | ENVIRONMENT.WET);

    let lightDesc = "Unknown";
    if (light === ENVIRONMENT.SUNNY) lightDesc = "Sunny";
    else if (light === ENVIRONMENT.PARTIAL_SHADE)
      lightDesc = "Partially Shaded";
    else if (light === ENVIRONMENT.SHADE) lightDesc = "Shaded";

    let moistureDesc = "Unknown";
    if (moisture === ENVIRONMENT.DRY) moistureDesc = "Dry";
    else if (moisture === ENVIRONMENT.MOIST) moistureDesc = "Moist";
    else if (moisture === ENVIRONMENT.WET) moistureDesc = "Wet";

    return `${lightDesc}, ${moistureDesc}`;
  }

  // Update the grid with a new predefined layout
  updateGridLayout(environmentLayout) {
    if (
      !environmentLayout ||
      environmentLayout.length !== this.rows ||
      environmentLayout[0].length !== this.cols
    ) {
      console.error("Invalid environment layout");
      return false;
    }

    // Clear existing plants
    this.plants = {};

    // Apply the new layout
    for (let row = 0; row < this.rows; row++) {
      for (let col = 0; col < this.cols; col++) {
        const cellEnv = environmentLayout[row][col];
        const lightCondition = cellEnv.light;
        const moistureCondition = cellEnv.moisture;

        this.grid[row][col] = {
          row: row,
          col: col,
          environment: lightCondition | moistureCondition,
          plantId: null,
          environmentClasses: this.getEnvironmentClasses(
            lightCondition,
            moistureCondition
          ),
        };
      }
    }

    return true;
  }
}

// Create and export a singleton instance
export const boardController = new BoardController();
