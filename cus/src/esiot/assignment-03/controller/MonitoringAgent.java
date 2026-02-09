package esiot.assignment_03.controller;

import esiot.assignment_03.model.SerialCommChannel;

public class MonitoringAgent extends Thread {

	SerialCommChannel channel;

	static final String MSG_PREFIX = "st:";
	static final String LOG_PREFIX = "lo:";
	static final String VALV_STATE = "vo:";

	public MonitoringAgent(SerialCommChannel channel) throws Exception {
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

				if (msg.startsWith(MSG_PREFIX)) {
					String cmd = msg.substring(MSG_PREFIX.length());
					if (cmd.startsWith(VALV_STATE)) {
						try {
							String args = cmd.substring(VALV_STATE.length());

							//"st:vo"
							//ex: "MANUAL:50"
							String[] elems = args.split(":");

							if (elems.length >= 2) {
								String WCSState = elems[0].trim();
								String valveOpening = elems[1].trim();
								
								//gestione dello stato
								boolean isNormal = WCSState.equalsIgnoreCase("MANUAL");
								boolean isAutomatic = WCSState.equalsIgnoreCase("AUTOMATIC");


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