package it.unibo.assignment03.model;

import java.util.LinkedList;
import java.util.List;

public class TankState {

    private double waterLevel = 0;
    private int valveOpening = 0;
    
    public enum State { CONNECTED, UNCONNECTED }
    public enum Mode { AUTOMATIC, MANUAL }

    private State currentState = State.UNCONNECTED;
    private Mode currentMode = Mode.AUTOMATIC;

    private LinkedList<DataPoint> history = new LinkedList<>();
    private final int MAX_HISTORY = 20;

    public TankState() {
    }

    public synchronized void updateLevel(double value) {
        this.waterLevel = value;
        history.addFirst(new DataPoint(value, System.currentTimeMillis()));
        if (history.size() > MAX_HISTORY) {
            history.removeLast();
        }
    }

    public synchronized void setValveOpening(int value) {
        this.valveOpening = value;
    }

    public synchronized void setState(State state) {
        this.currentState = state;
    }

    public synchronized void setMode(Mode mode) {
        this.currentMode = mode;
    }

    public synchronized double getLevel() {
        return waterLevel;
    }

    public synchronized int getValveOpening() {
        return valveOpening;
    }

    public synchronized State getState() {
        return currentState;
    }

    public synchronized Mode getMode() {
        return currentMode;
    }

    public synchronized List<DataPoint> getHistory() {
        return new LinkedList<>(history);
    }
}