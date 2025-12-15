// --- Thermostat Simulation Logic ---

const HEATER_OFF_TEMP = 150.0;
const HEATER_ON_TEMP = 130.0;
const MEDIAN_WINDOW_SIZE = 7;

const thermostatState = {
    isHeaterOn: false,
    tempReadings: [], // Buffer for the median calculation
};

function getMedian(numbers) {
    if (numbers.length === 0) return null;
    const sorted = Array.from(numbers).sort((a, b) => a - b);
    const middle = Math.floor(sorted.length / 2);
    return sorted[middle];
}

function updateThermostatLogic(newRawTemp) {
    // 1. Add new reading to our buffer
    thermostatState.tempReadings.push(newRawTemp);

    // 2. Trim the buffer if it's too large
    if (thermostatState.tempReadings.length > MEDIAN_WINDOW_SIZE) {
        thermostatState.tempReadings.shift(); // Remove the oldest reading
    }

    // 3. Only make decisions if the buffer is full
    if (thermostatState.tempReadings.length === MEDIAN_WINDOW_SIZE) {
        const medianTemp = getMedian(thermostatState.tempReadings);
        
        if (medianTemp < HEATER_ON_TEMP && !thermostatState.isHeaterOn) {
            thermostatState.isHeaterOn = true;
        } else if (medianTemp > HEATER_OFF_TEMP && thermostatState.isHeaterOn) {
            thermostatState.isHeaterOn = false;
        }
        
        return medianTemp;
    }

    return null; // Return null if we don't have enough data yet
}


// --- UI Interaction ---

document.addEventListener('DOMContentLoaded', () => {
    const medianTempElement = document.getElementById('current-temp');
    const heaterStateElement = document.getElementById('heater-state');
    const tempSlider = document.getElementById('temp-slider');
    const sliderValueElement = document.getElementById('slider-value');
    const bufferDisplayElement = document.getElementById('buffer-display');

    function updateUI(medianTemp) {
        // Update median temp display if available
        if (medianTemp !== null) {
            medianTempElement.textContent = medianTemp.toFixed(1);
        } else {
            medianTempElement.textContent = '---'; // Display during buffering
        }

        // Update heater status
        if (thermostatState.isHeaterOn) {
            heaterStateElement.textContent = 'ON';
            heaterStateElement.classList.remove('off');
            heaterStateElement.classList.add('on');
        } else {
            heaterStateElement.textContent = 'OFF';
            heaterStateElement.classList.remove('on');
            heaterStateElement.classList.add('off');
        }
        
        // Update buffer visualization
        bufferDisplayElement.textContent = `[ ${thermostatState.tempReadings.map(t => t.toFixed(1)).join(', ')} ]`;
    }

    // Initial buffer setup: fill with the initial slider value
    const initialRawTemp = parseFloat(tempSlider.value);
    for(let i = 0; i < MEDIAN_WINDOW_SIZE; i++) {
        thermostatState.tempReadings.push(initialRawTemp);
    }
    
    // Initial logic run and UI setup
    sliderValueElement.textContent = initialRawTemp.toFixed(1);
    const initialMedian = updateThermostatLogic(initialRawTemp);
    console.log("Initial buffer state:", thermostatState.tempReadings); // Debug log
    updateUI(initialMedian);


    // Event listener for the slider
    tempSlider.addEventListener('input', (event) => {
        const newRawTemp = parseFloat(event.target.value);
        sliderValueElement.textContent = newRawTemp.toFixed(1);

        const median = updateThermostatLogic(newRawTemp);
        console.log("Buffer state after slider:", thermostatState.tempReadings); // Debug log
        updateUI(median);
    });
});