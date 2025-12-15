// --- Thermostat Simulation Logic ---

const HEATER_OFF_TEMP = 150.0;
const HEATER_ON_TEMP = 130.0;

// The state of our simulated thermostat
const thermostatState = {
    currentTemp: 140.0,
    isHeaterOn: false,
};

function updateThermostatLogic() {
    const { currentTemp, isHeaterOn } = thermostatState;

    if (currentTemp < HEATER_ON_TEMP && !isHeaterOn) {
        thermostatState.isHeaterOn = true;
    } else if (currentTemp > HEATER_OFF_TEMP && isHeaterOn) {
        thermostatState.isHeaterOn = false;
    }
    // No detach logic needed for the visual simulation
}


// --- UI Interaction ---

document.addEventListener('DOMContentLoaded', () => {
    const currentTempElement = document.getElementById('current-temp');
    const heaterStateElement = document.getElementById('heater-state');
    const tempSlider = document.getElementById('temp-slider');
    const sliderValueElement = document.getElementById('slider-value');

    // Function to update the UI based on the thermostat state
    function updateUI() {
        currentTempElement.textContent = thermostatState.currentTemp.toFixed(1);
        
        if (thermostatState.isHeaterOn) {
            heaterStateElement.textContent = 'ON';
            heaterStateElement.classList.remove('off');
            heaterStateElement.classList.add('on');
        } else {
            heaterStateElement.textContent = 'OFF';
            heaterStateElement.classList.remove('on');
            heaterStateElement.classList.add('off');
        }
    }

    // Event listener for the slider
    tempSlider.addEventListener('input', (event) => {
        const newTemp = parseFloat(event.target.value);
        thermostatState.currentTemp = newTemp;
        sliderValueElement.textContent = newTemp.toFixed(1);

        // Run the logic and update the UI whenever the slider changes
        updateThermostatLogic();
        updateUI();
    });

    // Initial UI setup
    sliderValueElement.textContent = thermostatState.currentTemp.toFixed(1);
    updateUI();
});
