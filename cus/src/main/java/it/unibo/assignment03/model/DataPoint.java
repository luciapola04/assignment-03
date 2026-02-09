package it.unibo.assignment03.model;

public class DataPoint {

    private double level;
    private long timestamp;

    public DataPoint(final double level,final long time) {
        this.level = level;
        this.timestamp = time;
    }

    public double getValue() {
        return level;
    }

    public double getTimestamp() {
        return timestamp;
    }

} 
