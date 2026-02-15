package it.unibo.assignment03;

import io.vertx.core.Vertx;
import it.unibo.assignment03.comms.CommChannel;
import it.unibo.assignment03.comms.HTTPServer;
import it.unibo.assignment03.comms.SerialCommChannel;
import it.unibo.assignment03.comms.TMSComm;
import it.unibo.assignment03.controller.MainController;
import it.unibo.assignment03.model.TankState;
import jssc.SerialPortList;

public class Launcher {
    public static void main(String[] args) {
        
        String broker = "tcp://broker.mqtt-dashboard.com:1883";
        String topic = "esiot-2025";
        long T2 = 5000;
        String portName;
        CommChannel serialChannel;
        HTTPServer httpServer;

        if (args.length > 0) {
            portName = args[0];
        } else {
            String[] portNames = SerialPortList.getPortNames();
            if (portNames.length > 0) {
                portName = portNames[0];
                System.out.println("Porta trovata automaticamente: " + portName);
            } else {
                portName = "COM11"; 
                System.err.println("Nessuna porta rilevata, provo il default: " + portName);
            }
        }

        try{

            serialChannel = new SerialCommChannel(portName,115200);
            TMSComm mqttChannel = new TMSComm(broker, topic, T2);
            TankState tank = new TankState();
            MainController controller = new MainController(serialChannel,mqttChannel,tank);
            Vertx vertx = Vertx.vertx();
            httpServer = new HTTPServer(8080, controller, tank);
            vertx.deployVerticle(httpServer);
            
            httpServer.start();
            controller.start();
            
        }catch(Exception ex){
            ex.printStackTrace();
        }
        
    }
}