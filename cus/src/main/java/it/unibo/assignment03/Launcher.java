package it.unibo.assignment03;

import it.unibo.assignment03.comms.TMSModule;
import it.unibo.assignment03.controller.MainController;

public class Launcher {
    public static void main(String[] args) {
        
        String broker = "tcp://broker.mqtt-dashboard.com:1883";
        String topic = "esiot-2025";
        long timeoutMs = 5000;

        TMSModule module = new TMSModule(broker, topic, timeoutMs);

        MainController controller = new MainController(module);

        controller.start();
    }
}