package it.unibo.assignment03.controller;

import io.vertx.core.Vertx;
import it.unibo.assignment03.comms.CommChannel;
import it.unibo.assignment03.comms.HTTPServer;
import it.unibo.assignment03.comms.TMSComm;
import it.unibo.assignment03.model.TankState;

public class MainController {

    private TMSComm tmsModule;
    private CommChannel serialMonitor;
    private HTTPServer httpServer;
    private TankState tankState;

    private boolean running;

    private final double L1, L2;
    private final long T1;
    private long startL1Time = 0;

    static final String MSG_PREFIX = "st:";
    static final String VALV_STATE = "vo:";
    static final Integer CLOSED = 0;
    static final Integer HALF_OPEN = 50;
    static final Integer OPEN = 100;

    public MainController(CommChannel serialMonitor, TMSComm mqttChannel,TankState tank, long T1, double L1, double L2) {
        this.tmsModule = mqttChannel;
        this.serialMonitor = serialMonitor;
        this.tankState = tank;
        this.T1 = T1;
        this.L1 = L1;
        this.L2 = L2;

        
        Vertx vertx = Vertx.vertx();
        this.httpServer = new HTTPServer(8080, this,tankState);
        vertx.deployVerticle(httpServer);

        this.running = true;
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

            TankState.State currentState = tankState.getState();

            switch (currentState) {

                case CONNECTED:

                    if (mqttMsg != null)
                        processMqttMessage(mqttMsg);
                    if (serialMsg != null)
                        processSerialMessage(serialMsg);

                    if (tmsModule.isTimeout()) {
                        
                        System.err.println("[ALLARME] Timeout MQTT! Passo a UNCONNECTED");
                        tankState.setState(TankState.State.UNCONNECTED);
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
                        tankState.setState(TankState.State.CONNECTED);
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

        switch(tankState.getMode()){

            case AUTOMATIC:

                int targetValve = 0;
                double currentDistance = tankState.getLevel();

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

                if (targetValve != tankState.getValveOpening()) {
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
                System.out.println("Distanza:"+ val);

                tankState.updateLevel(value);
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
                            tankState.setMode(TankState.Mode.MANUAL);
                        }
                        if(WCSState.equalsIgnoreCase("AUTOMATIC")){
                            tankState.setMode(TankState.Mode.AUTOMATIC);
                        }
                        tankState.setValveOpening(Integer.parseInt(valveOpening));

                    }
                } catch (Exception ex) {
                    ex.printStackTrace();
                    System.err.println("Error parsing msg: " + cmd);
                }
            }
        }
    }


    public void switchMode() {
        if (tankState.getMode() == TankState.Mode.AUTOMATIC) {
            serialMonitor.sendMsg("m:3"); // Manual mode command
        } else {
            serialMonitor.sendMsg("m:2"); // Auto mode command
        }
    }

    public void setValveManual(int percentage) {
        if (tankState.getMode() == TankState.Mode.MANUAL) {
            serialMonitor.sendMsg("r:" + percentage);
        } else {
            System.out.println("[WARNING] Tentativo cambio valvola in AUTOMATIC ignorato.");
        }
    }

    private void setValve(int percentage) {
        serialMonitor.sendMsg("v:" + percentage);
        

    }

    public void stop() { this.running = false; }
}