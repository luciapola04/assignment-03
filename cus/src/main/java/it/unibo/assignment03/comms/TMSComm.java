package it.unibo.assignment03.comms;

import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class TMSComm implements MqttCallback {

    private String broker;
    private String clientId;
    private String topic;
    private MqttClient client;
    
    private BlockingQueue<String> dataQueue;
    
    private long lastMessageTime;
    private boolean tmsConnected;
    private long timeoutThresholdMs;

    public TMSComm(String broker, String topic, long timeoutMs) {
        this.broker = broker;
        this.topic = topic;
        this.clientId = "Java-Client-" + System.currentTimeMillis();
        this.dataQueue = new LinkedBlockingQueue<>();

        this.tmsConnected = false;
        this.timeoutThresholdMs = timeoutMs;

        this.lastMessageTime = System.currentTimeMillis();

        try {
            
            this.client = new MqttClient(broker, clientId, new MemoryPersistence());
            this.client.setCallback(this);

        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    public void start() {
        try {
            MqttConnectOptions connOpts = new MqttConnectOptions();
            connOpts.setCleanSession(true);
            connOpts.setConnectionTimeout(10);
    
            System.out.println("[TMS] Connecting to broker: " + broker);
            client.connect(connOpts);
            
            System.out.println("[TMS] Connected. Subscribing to: " + topic);
            client.subscribe(topic);
            
            System.out.println("[TMS] TMS module ready (waiting for data).");

        } catch (MqttException e) {
            System.err.println("[TMS] Error starting connection: " + e.getMessage());
        }
    }

    public void stop() {
        try {
            if (client != null && client.isConnected()) {
                client.disconnect();
                client.close();
                System.out.println("[TMS] Disconnected.");
            }
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    /**
     * Recupera un messaggio dalla coda.
     * @param block se true, aspetta che arrivi un messaggio.
     * @return il messaggio stringa o null se la coda è vuota.
     */
    public String getMessage(boolean block) {
        try {
            if (block) {
                return dataQueue.take();
            } else {
                return dataQueue.poll();
            }
        } catch (InterruptedException e) {
            return null;
        }
    }

    public boolean isTimeout() {
        long currentTime = System.currentTimeMillis();
        if ((currentTime - lastMessageTime) > timeoutThresholdMs) {
            if (tmsConnected) {
                System.out.println("[TMS] Timeout detected! Device connection considered lost.");
            }
            tmsConnected = false;
            return true;
        }
        return false;
    }

    public boolean isConnected() {
        return client.isConnected() && tmsConnected;
    }

    //override implementations
    @Override
    public void connectionLost(Throwable cause) {
        System.out.println("[TMS] Connection to broker lost! " + cause.getMessage());
        this.tmsConnected = false;
    }

    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        String payload = new String(message.getPayload());
        dataQueue.put(payload);
        
        lastMessageTime = System.currentTimeMillis();
        
        if (!tmsConnected) {
            System.out.println("[TMS] First message received. Module is ONLINE.");
            tmsConnected = true;
        }

    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {
        //Dobbiamo solo ricevere
    }
}