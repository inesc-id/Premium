package pt.ulisboa.tecnico.darshana.utils;

import java.util.List;
import java.util.Arrays;

public class MonitoringMode {

    public static final String FULL = "full";
    public static final String LAT = "lat";
    public static final String HOP = "hop";
    public static final String PATH = "path";
    public static final String PROP = "prop";

    private List<String> _modes;

    public MonitoringMode() {
        this._modes = Arrays.asList(FULL, LAT, HOP, PATH, PROP);
    }

    public boolean isValidMode(String monitoringMode){
        return _modes.contains(monitoringMode);
    }
}