package it.unibo.assignment03;

import it.unibo.assignment03.comms.CommChannel;
import it.unibo.assignment03.comms.SerialCommChannel;
import it.unibo.assignment03.comms.TMSModule;
import it.unibo.assignment03.controller.MainController;
import jssc.SerialPortList;

public class Launcher {
    public static void main(String[] args) {
        
        String broker = "tcp://broker.mqtt-dashboard.com:1883";
        String topic = "esiot-2025";
        long T1 = 6000;
        long T2 = 5000;
        double L1 = 0.10;
        double L2 = 0.20;
        String portName;
        CommChannel serialChannel;

        if (args.length > 0) {
            portName = args[0];
        } else {
            //sennò la cerca
            String[] portNames = SerialPortList.getPortNames();
            if (portNames.length > 0) {
                portName = portNames[0];
                System.out.println("Porta trovata automaticamente: " + portName);
            } else {
                //scelta fissa se non trova nulla
                portName = "COM11"; 
                System.err.println("Nessuna porta rilevata, provo il default: " + portName);
            }
        }

        try{
            serialChannel = new SerialCommChannel(portName,115200);
            TMSModule mqttChannel = new TMSModule(broker, topic, T2);

            MainController controller = new MainController(serialChannel,mqttChannel,T1,L1,L2);
            controller.start();
            
        }catch(Exception ex){
            ex.printStackTrace();
        }
        
    }
}