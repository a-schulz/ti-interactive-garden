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
  '0,0': { start: 0, end: 1 },
  '0,1': { start: 2, end: 3 },
  '0,2': { start: 4, end: 5 },
  '0,3': { start: 6, end: 7 },
  '0,4': { start: 8, end: 9 },
  '0,5': { start: 10, end: 11 },
  
  // Row 1
  '1,0': { start: 12, end: 13 },
  '1,1': { start: 14, end: 15 },
  '1,2': { start: 16, end: 17 },
  '1,3': { start: 18, end: 19 },
  '1,4': { start: 20, end: 21 },
  '1,5': { start: 22, end: 23 },
  
  // Row 2
  '2,0': { start: 24, end: 25 },
  '2,1': { start: 26, end: 27 },
  '2,2': { start: 28, end: 29 },
  '2,3': { start: 30, end: 31 },
  '2,4': { start: 32, end: 33 },
  '2,5': { start: 34, end: 35 },
  
  // Row 3
  '3,0': { start: 36, end: 37 },
  '3,1': { start: 38, end: 39 },
  '3,2': { start: 40, end: 41 },
  '3,3': { start: 42, end: 43 },
  '3,4': { start: 44, end: 45 },
  '3,5': { start: 46, end: 47 },
  
  // Row 4
  '4,0': { start: 48, end: 49 },
  '4,1': { start: 50, end: 51 },
  '4,2': { start: 52, end: 53 },
  '4,3': { start: 54, end: 55 },
  '4,4': { start: 56, end: 57 },
  '4,5': { start: 58, end: 59 },
  
  // Row 5
  '5,0': { start: 60, end: 61 },
  '5,1': { start: 62, end: 63 },
  '5,2': { start: 64, end: 65 },
  '5,3': { start: 66, end: 67 },
  '5,4': { start: 68, end: 69 },
  '5,5': { start: 70, end: 71 }
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