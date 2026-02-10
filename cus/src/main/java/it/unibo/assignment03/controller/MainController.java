package it.unibo.assignment03.controller;

import java.util.LinkedList;
import java.util.List;

import io.vertx.core.Vertx;
import io.vertx.core.http.HttpServer;
import it.unibo.assignment03.comms.CommChannel;
import it.unibo.assignment03.comms.HTTPServer;
import it.unibo.assignment03.comms.TMSComm;
import it.unibo.assignment03.model.DataPoint;

public class MainController {

    private TMSComm tmsModule;
    private CommChannel serialMonitor;
    private HTTPServer httpServer;

    private boolean running;

    private volatile SystemState currentState;
    private volatile SystemMode currentMode;

    private final double L1, L2;
    private final long T1;

    private volatile double currentDistance = 0;
    private long startL1Time = 0;
    private volatile int currentValveOpening = 0;

    static final String MSG_PREFIX = "st:";
    static final String VALV_STATE = "vo:";
    static final Integer CLOSED = 0;
    static final Integer HALF_OPEN = 50;
    static final Integer OPEN = 100;

    private LinkedList<DataPoint> history = new LinkedList<>(); 
    private final int MAX_HISTORY = 20;

    public enum SystemState {
        CONNECTED, UNCONNECTED
    }

    public enum SystemMode {
        AUTOMATIC, MANUAL
    }

    public MainController(CommChannel serialMonitor, TMSComm mqttChannel, long T1, double L1, double L2) {
        this.tmsModule = mqttChannel;
        this.serialMonitor = serialMonitor;
        Vertx vertx = Vertx.vertx();
        this.httpServer = new HTTPServer(8080, this);
        vertx.deployVerticle(httpServer);
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
        httpServer.start();
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
                        setValve(OPEN);

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
                    targetValve = CLOSED;
                    startL1Time = 0;
                } else if (currentDistance >= L1 && currentDistance < L2) {
                    if (startL1Time == 0) {
                        startL1Time = now;
                        targetValve = CLOSED;
                    } else if ((now - startL1Time) > T1) {
                        targetValve = HALF_OPEN;
                    }
                } else if (currentDistance >= L2) {
                    targetValve = OPEN;
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
                double value = Double.parseDouble(val);
                this.currentDistance = value;
                System.out.println("Distanza:"+ val);

                synchronized (history) {
                    history.addFirst(new DataPoint(value, System.currentTimeMillis()));
                    if (history.size() > MAX_HISTORY) {
                        history.removeLast();
                    }
                }
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

    public synchronized void switchMode() {
        if (currentMode == SystemMode.AUTOMATIC) {
            currentMode = SystemMode.MANUAL;
            serialMonitor.sendMsg("m:3"); // Manual mode command
        } else {
            currentMode = SystemMode.AUTOMATIC;
            serialMonitor.sendMsg("m:2"); // Auto mode command
        }
    }

    public synchronized void setValveManual(int percentage) {
        if (currentMode == SystemMode.MANUAL) {
            this.currentValveOpening = percentage;
            serialMonitor.sendMsg("r:" + percentage);
        } else {
            System.out.println("[WARNING] Tentativo cambio valvola in AUTOMATIC ignorato.");
        }
    }

    private void setValve(int percentage) {
        this.currentValveOpening = percentage;
        serialMonitor.sendMsg("v:" + percentage);

    }

    public double getCurrentDistance() { return currentDistance; }
    public int getCurrentValve() { return currentValveOpening; }
    public String getMode() { 

        if(currentState == SystemState.CONNECTED){
            return currentMode.toString();
        }else{
            return currentState.toString();
        }
    
    }

    public List<DataPoint> getHistory() {
        synchronized (history) {
            return new LinkedList<>(history);
        }
    }

    public void stop() { this.running = false; }
}