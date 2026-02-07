package esiot.assignment_02.controller;

import esiot.assignment_02.model.SerialCommChannel;
import esiot.assignment_02.view.DashboardView;
import esiot.assignment_02.view.LogView;

public class MonitoringAgent extends Thread {

	SerialCommChannel channel;
	DashboardView view;
	LogView logger;

	static final String APP_PREFIX = "dh:";
	static final String LOG_PREFIX = "lo:";
	static final String MSG_STATE = "st:";

	public MonitoringAgent(SerialCommChannel channel, DashboardView view, LogView log) throws Exception {
		this.view = view;
		this.logger = log;
		this.channel = channel;
	}

	public void run() {
		while (true) {
			try {
				String msg = channel.receiveMsg();

				if (msg.startsWith(LOG_PREFIX)) {
					this.logger.log(msg.substring(LOG_PREFIX.length()));
					continue;
				}

				if (msg.startsWith(APP_PREFIX)) {
					String cmd = msg.substring(APP_PREFIX.length());
					if (cmd.startsWith(MSG_STATE)) {
						try {
							String args = cmd.substring(MSG_STATE.length());

							//"DroneState:HangarState:Distance"
							//ex: "DRONE INSIDE:NORMAL:0.0"
							String[] elems = args.split(":");

							if (elems.length >= 3) {
								String droneState = elems[0].trim();
								String hangarState = elems[1].trim();
								String dist = elems[2].trim();

								view.setDroneState(droneState);
								view.setHangarState(hangarState);
								view.setDistance(dist);

								
								//gestione dello stato

								boolean isNormal = hangarState.equalsIgnoreCase("NORMAL");
								boolean isInside = droneState.equalsIgnoreCase("REST");
								boolean isOutside = droneState.equalsIgnoreCase("OPERATING");

								if (!isNormal) {
									view.setControlsEnabled(false, false);
								} else {
									view.setControlsEnabled(isInside, isOutside);
								}
							}
						} catch (Exception ex) {
							ex.printStackTrace();
							System.err.println("Error parsing msg: " + cmd);
						}
					}
				}
			} catch (Exception ex) {
				ex.printStackTrace();
			}
		}
	}
}