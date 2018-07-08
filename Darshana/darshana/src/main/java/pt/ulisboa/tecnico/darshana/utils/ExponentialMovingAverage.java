package pt.ulisboa.tecnico.darshana.utils;

public class ExponentialMovingAverage {
    private Double alpha;
    private Double oldValue;

    public ExponentialMovingAverage(Double alpha) {
        this.alpha = alpha;
    }

    public Double average(Double value) {

        if (oldValue == null) {
            oldValue = value;
            return value;
        }

        Double newValue = oldValue + alpha * (value - oldValue);
        oldValue = newValue;
        return newValue;
    }
}
