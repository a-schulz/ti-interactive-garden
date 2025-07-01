/**
 * Constants for the Interactive Garden Web Application
 */

// Environment constants
export const ENVIRONMENT = {
  // Light conditions
  SUNNY: 1,
  PARTIAL_SHADE: 2,
  SHADE: 4,
  // Moisture conditions
  DRY: 8,
  MOIST: 16,
  WET: 32
};

// Relationship types between plants
export const RELATIONSHIP = {
  NEUTRAL: 0,
  LIKES: 1,
  HATES: 2
};

// Game modes
export const GAME_MODE = {
  ENVIRONMENT_MODE: 'environment',
  NEIGHBORS_MODE: 'neighbors',
  COMBINED_MODE: 'combined'
};