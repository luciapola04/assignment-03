package esiot.assignment_02;

import esiot.assignment_02.controller.DashboardController;
import esiot.assignment_02.view.DashboardView;
import esiot.assignment_02.view.LogView;
import jssc.SerialPortList;

class DashboardLauncher {

	static DashboardView view = null;
	static LogView log = null;

	public static void main(String[] args) throws Exception {
		view = new DashboardView();
        log = new LogView();
        
        String portName;

        //controlla prima l'argomento
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
        
        try {
            DashboardController contr = new DashboardController(portName, view, log);
            view.registerController(contr);
            
            view.display();
            log.display();
        } catch (Exception e) {
            System.err.println("Errore fatale: impossibile aprire la porta " + portName);
            e.printStackTrace();
        }
	}
}