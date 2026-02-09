package it.unibo.assignment03.controller;

import it.unibo.assignment03.comms.CommChannel;
import it.unibo.assignment03.comms.TMSModule;

public class MainController {

    private TMSModule tmsModule;
    private CommChannel serialMonitor;

    private boolean running;

    private SystemState currentState;
    private SystemMode currentMode;

    private final double L1, L2;
    private final long T1;

    private double currentDistance = 0;
    private long startL1Time = 0;
    private int currentValveOpening = 0;

    static final String MSG_PREFIX = "st:";
	static final String VALV_STATE = "vo:";

    public enum SystemState {
        CONNECTED, UNCONNECTED
    }

    public enum SystemMode {
        AUTOMATIC, MANUAL
    }

    public MainController(CommChannel serialMonitor, TMSModule mqttChannel, long T1, double L1, double L2) {
        this.tmsModule = mqttChannel;
        this.serialMonitor = serialMonitor;
        this.T1 = T1;
        this.L1 = L1;
        this.L2 = L2;

        this.running = true;
        this.currentState = SystemState.UNCONNECTED;
        this.currentMode = SystemMode.AUTOMATIC;
        serialMonitor.sendMsg("m:1");
    }

    public void start() {
        System.out.println("[CONTROLLER] Avvio del sistema...");
        tmsModule.start();
        this.runLoop();
    }

    private void runLoop() {
        System.out.println("[CONTROLLER] Sistema in ascolto (MQTT + Serial).");

        while (running) {
            long now = System.currentTimeMillis();

            String mqttMsg = tmsModule.getMessage(false);

            String serialMsg = null;
            if (serialMonitor.isMsgAvailable()) {
                try {
                    serialMsg = serialMonitor.receiveMsg();
                    System.out.println(serialMsg);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            switch (currentState) {

                case CONNECTED:

                    if (mqttMsg != null)
                        processMqttMessage(mqttMsg);
                    if (serialMsg != null)
                        processSerialMessage(serialMsg);

                    if (tmsModule.isTimeout()) {
                        
                        System.err.println("[ALLARME] Timeout MQTT! Passo a UNCONNECTED");
                        currentState = SystemState.UNCONNECTED;
                        serialMonitor.sendMsg("m:1");
                        setValve(100);

                    }else {
            
                        runOperatingLogic(now);

                    }
                    break;

                case UNCONNECTED:

                    //quando mi riconnetto
                    if (mqttMsg != null) {
                        System.out.println("[INFO] Connessione ripristinata.");
                        processMqttMessage(mqttMsg);
                        currentState = SystemState.CONNECTED;
                        serialMonitor.sendMsg("m:2");
                    }
                    break;
            }

            try {
                Thread.sleep(50);
            } catch (InterruptedException e) {
            }
        }

        tmsModule.stop();
    }

    private void runOperatingLogic(long now) {

        switch(currentMode){

            case AUTOMATIC:

                int targetValve = 0;

                if (currentDistance < L1) {
                    targetValve = 0;
                    startL1Time = 0;
                } else if (currentDistance >= L1 && currentDistance < L2) {
                    if (startL1Time == 0) {
                        startL1Time = now;
                        targetValve = 0;
                    } else if ((now - startL1Time) > T1) {
                        targetValve = 50;
                    }
                } else if (currentDistance >= L2) {
                    targetValve = 100;
                    startL1Time = 0;
                }

                if (targetValve != currentValveOpening) {
                    setValve(targetValve);
                }

            break;

            case MANUAL:

                break;

        } 
    }

    private void processMqttMessage(String msg) {
        if (msg.contains("Distance")) {
            try {
                String val = msg.split(":")[1];
                this.currentDistance = Double.parseDouble(val);
                System.out.println("Distanza:"+ val);
            } catch (Exception e) {
                System.err.println("Errore parsing MQTT: " + msg);
            }
        }
    }

    private void processSerialMessage(String msg) {

        if (msg.startsWith(MSG_PREFIX)) {
            String cmd = msg.substring(MSG_PREFIX.length());
            if (cmd.startsWith(VALV_STATE)) {
                try {
                    String args = cmd.substring(VALV_STATE.length());
                    // "st:vo"
                    // ex: "MANUAL:50"
                    String[] elems = args.split(":");

                    if (elems.length >= 2) {
                        String WCSState = elems[0].trim();
                        String valveOpening = elems[1].trim();

                        if(WCSState.equalsIgnoreCase("MANUAL")){
                            currentMode = SystemMode.MANUAL;
                        }
                        if(WCSState.equalsIgnoreCase("AUTOMATIC")){
                            currentMode = SystemMode.AUTOMATIC;
                        }
                        this.currentValveOpening = Integer.parseInt(valveOpening);

                    }
                } catch (Exception ex) {
                    ex.printStackTrace();
                    System.err.println("Error parsing msg: " + cmd);
                }
            }
        }
    }

    // --- HELPER ---

    private void setValve(int percentage) {

        this.currentValveOpening = percentage;
        serialMonitor.sendMsg("v:" + percentage);

    }

    //lo usermo per il web
    private void toggleMode() {
        if (currentMode == SystemMode.AUTOMATIC) {
            currentMode = SystemMode.MANUAL;
            serialMonitor.sendMsg("m:3");
        } else {
            currentMode = SystemMode.AUTOMATIC;
            serialMonitor.sendMsg("m:2");
        }
    }

    public void stop() {
        this.running = false;
    }
}