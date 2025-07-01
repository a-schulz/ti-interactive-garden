/**
 * LED configuration for mapping grid positions to LED strip segments
 */

// Default ESP controller settings
export const ESP_CONFIG = {
  enabled: false,        // Whether to send LED control commands to ESP
  ipAddress: '192.168.4.1', // Default IP of the ESP access point
  port: 80,              // Default port
  endpoint: '/api/led'   // LED control endpoint
};

// Maps each grid position [row, col] to a range of LEDs on the strip
export const GRID_TO_LED_MAPPING = {
  // Row 0
  '0,0': { start: 0, end: 2 },
  '0,1': { start: 3, end: 5 },
  '0,2': { start: 6, end: 8 },
  '0,3': { start: 9, end: 11 },
  
  // Row 1
  '1,0': { start: 12, end: 14 },
  '1,1': { start: 15, end: 17 },
  '1,2': { start: 18, end: 20 },
  '1,3': { start: 21, end: 23 },
  
  // Row 2
  '2,0': { start: 24, end: 26 },
  '2,1': { start: 27, end: 29 },
  '2,2': { start: 30, end: 32 },
  '2,3': { start: 33, end: 35 },
  
  // Row 3
  '3,0': { start: 36, end: 38 },
  '3,1': { start: 39, end: 41 },
  '3,2': { start: 42, end: 44 },
  '3,3': { start: 45, end: 47 }
};

// Get LED segment for a grid position
export const getLedSegmentForGrid = (row, col) => {
  const key = `${row},${col}`;
  return GRID_TO_LED_MAPPING[key] || { start: 0, end: 0 };
};

// Convert color hex string to RGB object
export const hexToRgb = (hex) => {
  // Remove # if present
  hex = hex.replace('#', '');
  
  // Convert to RGB
  return {
    r: parseInt(hex.substring(0, 2), 16),
    g: parseInt(hex.substring(2, 4), 16),
    b: parseInt(hex.substring(4, 6), 16)
  };
};

// Convert status to color
export const statusToColor = (status) => {
  switch (status) {
    case 'happy':
      return { r: 0, g: 255, b: 0 }; // Green
    case 'unhappy':
      return { r: 255, g: 0, b: 0 }; // Red
    case 'neutral':
      return { r: 255, g: 255, b: 255 }; // White
    default:
      return { r: 0, g: 0, b: 0 }; // Off
  }
};

// Send LED control command to ESP
export const sendLedCommand = async (segment, color) => {
  if (!ESP_CONFIG.enabled) return;
  
  try {
    const url = `http://${ESP_CONFIG.ipAddress}:${ESP_CONFIG.port}${ESP_CONFIG.endpoint}`;
    const payload = {
      start: segment.start,
      end: segment.end,
      color
    };
    
    const response = await fetch(url, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(payload),
    });
    
    if (!response.ok) {
      console.error('Error controlling LED:', await response.text());
    }
    
    return await response.json();
  } catch (error) {
    console.error('Failed to send LED command:', error);
  }
};

// Clear all LEDs
export const clearAllLeds = async () => {
  if (!ESP_CONFIG.enabled) return;
  
  try {
    const url = `http://${ESP_CONFIG.ipAddress}:${ESP_CONFIG.port}/api/clear`;
    
    const response = await fetch(url, {
      method: 'POST',
    });
    
    if (!response.ok) {
      console.error('Error clearing LEDs:', await response.text());
    }
    
    return await response.json();
  } catch (error) {
    console.error('Failed to clear LEDs:', error);
  }
};