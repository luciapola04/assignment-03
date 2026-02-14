package it.unibo.assignment03.controller;

import it.unibo.assignment03.comms.CommChannel;
import it.unibo.assignment03.comms.TMSComm;
import it.unibo.assignment03.model.TankState;

public class MainController {

    private TMSComm tmsModule;
    private CommChannel serialMonitor;
    private TankState tankState;

    private boolean running;

    private final double L1, L2;
    private final long T1;

    private long startL1Time = 0;
    private int lastSentValve = -1; 

    static final String MSG_PREFIX = "st:";
    static final String VALV_STATE = "vo:";
    static final Integer CLOSED = 0;
    static final Integer HALF_OPEN = 50;
    static final Integer OPEN = 100;

    public MainController(CommChannel serialMonitor, TMSComm mqttChannel, TankState tank, long T1, double L1, double L2) {
        this.tmsModule = mqttChannel;
        this.serialMonitor = serialMonitor;
        this.tankState = tank;
        this.T1 = T1;
        this.L1 = L1;
        this.L2 = L2;

        this.running = true;
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
                    System.out.println("[ARDUINO] " + serialMsg);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            TankState.State currentState = tankState.getState();

            switch (currentState) {

                case CONNECTED:
                    if (mqttMsg != null) processMqttMessage(mqttMsg);
                    if (serialMsg != null) processSerialMessage(serialMsg);

                    if (tmsModule.isTimeout()) {
                        System.err.println("[ALLARME] Timeout MQTT! Passo a UNCONNECTED");
                        
                        tankState.setState(TankState.State.UNCONNECTED);
                        serialMonitor.sendMsg("m:1"); 
                        
                        startL1Time = 0;
                        lastSentValve = -1; 
                        
                        setValve(OPEN);

                    } else {
                        runOperatingLogic(now);
                    }
                    break;

                case UNCONNECTED:
                    if (mqttMsg != null) {
                        System.out.println("[INFO] Connessione MQTT ripristinata.");
                        processMqttMessage(mqttMsg);
                        
                        tankState.setState(TankState.State.CONNECTED);
                        serialMonitor.sendMsg("m:2");

                        startL1Time = 0;
                        lastSentValve = -1; 
                    }
                    break;
            }

            try {
                Thread.sleep(50);
            } catch (InterruptedException e) {
                e.printStackTrace();
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

                if (targetValve != lastSentValve) {
                    setValve(targetValve);
                    lastSentValve = targetValve;
                }
                break;

            case MANUAL:
                if (lastSentValve != -1) {
                    lastSentValve = -1;
                    startL1Time = 0;
                }
                break;
        } 
    }

    private void processMqttMessage(String msg) {
        if (msg.contains("Distance")) {
            try {
                String val = msg.split(":")[1];
                double value = Double.parseDouble(val.trim());
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
                    String[] elems = args.split(":");

                    if (elems.length >= 2) {
                        String wcsStateStr = elems[0].trim();
                        String valveOpeningStr = elems[1].trim();

                        if(wcsStateStr.equalsIgnoreCase("MANUAL")){
                            if (tankState.getMode() != TankState.Mode.MANUAL) {
                                tankState.setMode(TankState.Mode.MANUAL);
                                startL1Time = 0;
                                lastSentValve = -1;
                            }
                        }
                        else if(wcsStateStr.equalsIgnoreCase("AUTOMATIC")){
                            if (tankState.getMode() != TankState.Mode.AUTOMATIC) {
                                tankState.setMode(TankState.Mode.AUTOMATIC);
                                startL1Time = 0;
                                lastSentValve = -1;
                            }
                        }

                        tankState.setValveOpening(Integer.parseInt(valveOpeningStr));
                    }
                } catch (Exception ex) {
                    System.err.println("Error parsing msg: " + cmd);
                    ex.printStackTrace();
                }
            }
        }
    }


    public void switchMode() {
        if (tankState.getMode() == TankState.Mode.AUTOMATIC) {
            serialMonitor.sendMsg("m:3"); //manual
        } else {
            serialMonitor.sendMsg("m:2"); //auto
        }
    }

    public void setValveManual(int percentage) {
        if (tankState.getMode() == TankState.Mode.MANUAL) {
            serialMonitor.sendMsg("r:" + percentage);
        } else {
            System.out.println("[WARNING] Ignorato comando manuale mentre in AUTOMATIC.");
        }
    }

    private void setValve(int percentage) {
        serialMonitor.sendMsg("v:" + percentage);
    }

    public void stop() { 
        this.running = false; 
    }
}