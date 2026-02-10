// Configurazione URL (Se usi CORS con Java metti http://localhost:8080)
const BASE_URL = "http://localhost:8080"; 

// Variabili di stato
let currentSystemMode = ""; 
let isDragging = false; 

// Riferimenti agli elementi DOM (per non cercarli ogni volta)
const els = {
    state: document.getElementById('state-display'),
    valveVal: document.getElementById('valve-display'),
    slider: document.getElementById('valve'),
    btnManual: document.getElementById('btn-manual'),
    btnAuto: document.getElementById('btn-auto')
};

// --- 1. Inizializzazione Grafico Chart.js ---
const ctx = document.getElementById('levelChart').getContext('2d');
const levelChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Livello Acqua (cm)',
            data: [],
            borderColor: '#0d6efd', // Bootstrap Primary Blue
            backgroundColor: 'rgba(13, 110, 253, 0.1)',
            borderWidth: 2,
            pointRadius: 3,
            fill: true,
            tension: 0.3 
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false, // Si adatta al contenitore Bootstrap
        scales: {
            y: {
                beginAtZero: true,
                grid: { color: '#e9ecef' } // Grigio chiaro Bootstrap
            },
            x: {
                grid: { display: false }
            }
        },
        plugins: {
            legend: { display: false } // Nascondiamo la legenda per pulizia
        },
        animation: false
    }
});

// --- 2. Gestione Eventi Slider ---

// Quando l'utente trascina lo slider (aggiorna solo il numero a video)
els.slider.addEventListener('input', (e) => {
    isDragging = true;
    els.valveVal.innerText = e.target.value;
});

// Quando l'utente RILASCIA lo slider (manda il comando al server)
els.slider.addEventListener('change', async (e) => {
    isDragging = false;
    await sendValveValue(e.target.value);
});


// --- 3. Logica di Aggiornamento ---

async function fetchStatus() {
    try {
        const response = await fetch(`${BASE_URL}/api/status`);
        if (!response.ok) throw new Error("Server Error");
        
        const data = await response.json();
        updateUI(data);
        
        // Recupera storico per il grafico
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
    } catch (e) { /* ignore */ }
}

function updateUI(data) {
    // 1. Aggiorna Badge Stato
    setUIState(data.mode);

    // 2. Aggiorna Numero Valvola
    els.valveVal.innerText = data.valve;
    
    // 3. Gestione Slider (Abilita solo in Manual + Connected)
    const canControl = (data.mode === "MANUAL");
    els.slider.disabled = !canControl;
    
    // Aggiorna posizione cursore solo se l'utente non lo sta toccando
    if (!isDragging) {
        els.slider.value = data.valve;
    }

    // 4. Aggiorna Bottoni (Visualizza quale è attivo)
    currentSystemMode = data.mode;
    updateButtons(data.mode);
}

function setUIState(state) {
    els.state.innerText = state;
    
    // Rimuovi vecchie classi colore
    els.state.classList.remove('bg-secondary', 'bg-success', 'bg-danger', 'bg-warning');

    // Assegna colore Bootstrap in base allo stato
    switch(state) {
        case 'MANUAL','AUTOMATIC':
            els.state.classList.add('bg-success'); // Verde
            break;
        case 'UNCONNECTED':
            els.state.classList.add('bg-danger');  // Rosso
            break;
        case 'NOT_AVAILABLE':
        default:
            els.state.classList.add('bg-secondary'); // Grigio
            break;
    }
}

function updateButtons(mode) {
    // Gestione visuale dei bottoni (Outline vs Solid)
    if (mode === "MANUAL") {
        els.btnManual.classList.add('btn-primary');
        els.btnManual.classList.remove('btn-outline-primary');
        
        els.btnAuto.classList.add('btn-outline-primary');
        els.btnAuto.classList.remove('btn-primary');
    } else {
        els.btnAuto.classList.add('btn-primary');
        els.btnAuto.classList.remove('btn-outline-primary');
        
        els.btnManual.classList.add('btn-outline-primary');
        els.btnManual.classList.remove('btn-primary');
    }
}

function updateChart(history) {
    // Aggiorna i dati del grafico
    const labels = history.map(d => new Date(d.time).toLocaleTimeString());
    const values = history.map(d => d.value);

    levelChart.data.labels = labels.reverse();
    levelChart.data.datasets[0].data = values.reverse();
    levelChart.update();
}

// --- 4. Chiamate API (Comandi Utente) ---

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
    // Evita chiamate inutili se siamo già in quella modalità
    if (currentSystemMode === targetMode) return;

    try {
        await fetch(`${BASE_URL}/api/mode/switch`, { method: 'POST' });
        fetchStatus(); // Aggiorna subito
    } catch (e) {
        console.error("Errore cambio modalità", e);
    }
}

// Avvio Loop
setInterval(fetchStatus, 1000);
fetchStatus();