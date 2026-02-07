package esiot.assignment_02.controller;

import esiot.assignment_02.model.SerialCommChannel;
import esiot.assignment_02.view.DashboardView;
import esiot.assignment_02.view.LogView;

public class DashboardController  {
	
	SerialCommChannel channel;
	DashboardView view;
	LogView logger;
	
	public DashboardController(String port, DashboardView view, LogView logger) throws Exception {
		this.view = view;
		this.logger = logger;
		view.registerController(this);
		
		channel = new SerialCommChannel(port,115200);		
		new MonitoringAgent(channel,view,logger).start();
		
		System.out.println("Waiting Arduino for rebooting...");		
		Thread.sleep(4000);
		System.out.println("Ready.");		
	
	}
	
	
	public void notifyTakeOff() {
		  channel.sendMsg("takeoff-req");
	}

	public void notifyLanding() {
		  channel.sendMsg("landing-req");
	}

}
