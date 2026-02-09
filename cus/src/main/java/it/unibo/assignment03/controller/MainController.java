package it.unibo.assignment03.controller;

import it.unibo.assignment03.comms.TMSModule;

public class MainController {

    private TMSModule tmsModule;
    private boolean running;
    private SystemState currentState;

    public MainController(TMSModule tmsModule) {
        this.tmsModule = tmsModule;
        this.running = true;
        this.currentState = SystemState.UNCONNECTED;
    }

    public void start() {
        System.out.println("[CONTROLLER] Avvio del sistema...");
        tmsModule.start();
        this.runLoop();
    }

    private void runLoop() {
        System.out.println("[CONTROLLER] Sistema in ascolto.");
        while (running) {

            String msg = tmsModule.getMessage(false);

            switch (currentState) {

                case NORMAL:
                    if (msg != null) processMessage(msg);

                    if (tmsModule.isTimeout()) {
                        System.out.println("[ALLARME] Timeout rilevato dal modulo!");
                        currentState = SystemState.UNCONNECTED;
                    }
                    break;

                case UNCONNECTED:

                    if (msg != null) {
                        System.out.println("[INFO] Riconnesso.");
                        processMessage(msg);
                        currentState = SystemState.NORMAL;
                    }
                    break;
            }
            
            try { Thread.sleep(100); } catch (InterruptedException e) {}
        }
        
        tmsModule.stop();
    }

    private void processMessage(String msg) {
        System.out.println("[CONTROLLER] Ricevuto dati: " + msg);
        
        if (msg.contains("Distance")) {
            
        }
    }

    
    private void handleTimeout() {
        System.err.println("[ALLARME] Il dispositivo ESP32 non risponde da troppo tempo!");
        
    }
    
    public void stop() {
        this.running = false;
    }
}