const API_DATA_URL = "http://localhost:8080/api/data";
const API_MODE_URL = "http://localhost:8080/api/mode";

const valveSlider = document.getElementById('valve');
const valveDisplay = document.getElementById('valve-display');
const stateDisplay = document.getElementById('state-display');
const btnManual = document.getElementById('btn-manual');
const btnAuto = document.getElementById('btn-auto');

let currentMode = 'AUTOMATIC'; 

const ctx = document.getElementById('levelChart').getContext('2d');
const myChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: [], 
        datasets: [{
            label: 'Livello Acqua (cm)',
            data: [],
            borderColor: 'rgba(13, 110, 253, 1)', 
            backgroundColor: 'rgba(13, 110, 253, 0.1)',
            borderWidth: 2,
            pointRadius: 2,
            fill: true,
            tension: 0.4 
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: true,
        scales: {
            y: {
                beginAtZero: true,
                title: { display: true, text: 'Livello (cm)' }
            },
            x: {
                title: { display: true, text: 'Orario' },
                ticks: { maxTicksLimit: 10 } // Evita troppe etichette
            }
        },
        animation: false 
    }
});

// --- 2. LOGICA INTERFACCIA (UI) ---

function updateInterface(mode) {
    currentMode = mode;

    if (mode === 'MANUAL') {
        // --- MODALITÀ MANUALE ---
        // Sblocca lo slider
        valveSlider.disabled = false; 
        
        // Aggiorna bottoni: Manuale Pieno, Auto Vuoto
        btnManual.className = "btn btn-primary"; 
        btnAuto.className = "btn btn-outline-primary";
    } else {
        // --- MODALITÀ AUTOMATICA ---
        // Blocca lo slider
        valveSlider.disabled = true; 
        
        // Aggiorna bottoni: Auto Pieno, Manuale Vuoto
        btnManual.className = "btn btn-outline-primary";
        btnAuto.className = "btn btn-primary";
    }
}

// Funzione chiamata al click dei bottoni
function setMode(mode) {
    console.log("Cambio modalità richiesto:", mode);
    
    // Aggiorna subito la grafica
    updateInterface(mode);

    // Invia comando al server
    fetch(API_MODE_URL, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ mode: mode })
    })
    .catch(err => console.error("Errore invio modalità:", err));
}

// Gestione Slider (solo visuale + invio)
valveSlider.addEventListener('input', function() {
    valveDisplay.innerText = this.value;
});

valveSlider.addEventListener('change', function() {
    // Invia il valore solo quando si rilascia il mouse (se siamo in MANUAL)
    if (currentMode === 'MANUAL') {
        console.log("Invio valore valvola:", this.value);
        // Qui aggiungerai la fetch POST per la valvola se necessario
        // fetch(..., { body: JSON.stringify({ valve: this.value }) })
    }
});

// --- 3. RECUPERO DATI (Loop) ---
async function fetchData() {
    try {
        const response = await fetch(API_DATA_URL);
        if (!response.ok) throw new Error("Errore HTTP");

        const jsonArray = await response.json();
        const dataReversed = jsonArray.reverse();
        const recentData = dataReversed.slice(-20);

        // Aggiornamento Grafico
        myChart.data.labels = recentData.map(item => {
            return new Date(item.time).toLocaleTimeString('it-IT');
        });
        myChart.data.datasets[0].data = recentData.map(item => item.value);
        myChart.update();

        // Aggiornamento Dati Testuali (se presenti nel JSON)
        if (recentData.length > 0) {
            const last = recentData[recentData.length - 1];
            
            // Esempio: se il backend invia lo stato, aggiorniamo la label
            // if (last.state) stateDisplay.innerText = last.state;
            
            // Se siamo in AUTOMATICO, la valvola è controllata dal sistema.
            // Aggiorniamo lo slider per mostrare cosa sta facendo il sistema
            /* if (currentMode === 'AUTOMATIC' && last.valveValue !== undefined) {
                valveSlider.value = last.valveValue;
                valveDisplay.innerText = last.valveValue;
            }
            */
        }

    } catch (error) {
        console.error("Errore fetch dati:", error);
    }
}

// --- 4. AVVIO ---
// Imposta lo stato iniziale
updateInterface('AUTOMATIC'); 

// Avvia il loop
setInterval(fetchData, 1000);
fetchData();