//con CORS uso localhost
const BASE_URL = "http://localhost:8080"; 

let currentSystemMode = ""; 
let isDragging = false; 

const els = {
    state: document.getElementById('state-display'),
    valveVal: document.getElementById('valve-display'),
    slider: document.getElementById('valve'),
    btnManual: document.getElementById('btn-manual'),
    btnAuto: document.getElementById('btn-auto')
};

const ctx = document.getElementById('levelChart').getContext('2d');
const levelChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Livello Acqua (cm)',
            data: [],
            borderColor: '#0d6efd',
            backgroundColor: 'rgba(13, 110, 253, 0.1)',
            borderWidth: 2,
            pointRadius: 3,
            fill: true,
            tension: 0.3 
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            y: {
                beginAtZero: true,
                grid: { color: '#e9ecef' }
            },
            x: {
                grid: { display: false }
            }
        },
        plugins: {
            legend: { display: false }
        },
        animation: false
    }
});

els.slider.addEventListener('input', (e) => {
    isDragging = true;
    els.valveVal.innerText = e.target.value;
});

els.slider.addEventListener('change', async (e) => {
    isDragging = false;
    await sendValveValue(e.target.value);
});

async function fetchStatus() {
    try {
        const response = await fetch(`${BASE_URL}/api/status`);
        if (!response.ok) throw new Error("Server Error");
        
        const data = await response.json();
        updateUI(data);
        
        fetchHistory();

    } catch (error) {
        console.error("Errore connessione:", error);
        setUIState("NOT AVAILABLE");
    }
}

async function fetchHistory() {
    try {
        const response = await fetch(`${BASE_URL}/api/history`);
        if (response.ok) {
            const historyData = await response.json();
            updateChart(historyData);
        }
    } catch (e) { console.log(e);}
}

function updateUI(data) {

    setUIState(data.mode);
    els.valveVal.innerText = data.valve;
    
    const canControl = (data.mode === "MANUAL");
    els.slider.disabled = !canControl;
    
    if (!isDragging) {
        els.slider.value = data.valve;
    }

    currentSystemMode = data.mode;
    updateButtons(data.mode);
}

function setUIState(state) {
    els.state.innerText = state;
    els.state.classList.remove('bg-secondary', 'bg-success', 'bg-danger', 'bg-warning');

    switch(state) {
        case 'MANUAL':
        case 'AUTOMATIC':
            els.state.classList.add('bg-success');
            break;
        case 'UNCONNECTED':
            els.state.classList.add('bg-danger');
            break;
        case 'NOT_AVAILABLE':
        default:
            els.state.classList.add('bg-secondary');
            break;
    }
}

function updateButtons(mode) {
    els.btnManual.classList.remove('disabled');
    els.btnAuto.classList.remove('disabled');

    if (mode === "MANUAL") {
        els.btnManual.classList.replace('btn-outline-primary', 'btn-primary');
        els.btnAuto.classList.replace('btn-primary', 'btn-outline-primary');

    } else if (mode === "AUTOMATIC") {
        els.btnAuto.classList.replace('btn-outline-primary', 'btn-primary');
        els.btnManual.classList.replace('btn-primary', 'btn-outline-primary');

    } else {
        els.btnAuto.classList.add("disabled");
        els.btnManual.classList.add("disabled");
        
        els.btnManual.classList.replace('btn-primary', 'btn-outline-primary');
        els.btnAuto.classList.replace('btn-primary', 'btn-outline-primary');
    }
}

function updateChart(history) {
    const labels = history.map(d => new Date(d.time).toLocaleTimeString());
    const values = history.map(d => d.value);

    levelChart.data.labels = labels.reverse();
    levelChart.data.datasets[0].data = values.reverse();
    levelChart.update();
}


async function sendValveValue(val) {
    try {
        await fetch(`${BASE_URL}/api/valve/set`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ value: parseInt(val) })
        });
    } catch (e) {
        console.error("Errore invio valvola", e);
    }
}

async function setMode(targetMode) {
    if (currentSystemMode === targetMode) return;

    try {
        await fetch(`${BASE_URL}/api/mode/switch`, { method: 'POST' });
        fetchStatus();
    } catch (e) {
        console.error("Errore cambio modalità", e);
    }
}

setInterval(fetchStatus, 1000);
fetchStatus();