package pt.ulisboa.tecnico.darshana.utils;

import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.io.Writer;
import java.io.FileOutputStream;
import java.util.ArrayList;

/**
 * LogManager is responsible to log the events persistently
 */
public class LogManager {

    // Logging Filenames
    private static final String LAT_LOGGING_FILENAME = "Lat_Log_Dump.txt";
    private static final String HOP_LOGGING_FILENAME = "Hop_Log_Dump.txt";
    private static final String PATH_LOGGING_FILENAME = "Path_Log_Dump.txt";
    private static final String PROP_LOGGING_FILENAME = "Prop_Log_Dump.txt";
    private static final String TRACEROUTE_LOGGING_FILENAME = "Traceroute_Log_Dump.txt";

    private static final String FILE_ENCODING = "utf-8";

    public static void print(String text) {
        System.out.println(text);
    }

    public static void persistentLogLat(String text) {
        logIntoFile(LAT_LOGGING_FILENAME, text);
    }
    public static void persistentLogHop(String text) {
        logIntoFile(HOP_LOGGING_FILENAME, text);
    }
    public static void persistentLogPath(String text) {
        logIntoFile(PATH_LOGGING_FILENAME, text);
    }
    public static void persistentLogProp(String text) {
        logIntoFile(PROP_LOGGING_FILENAME, text);
    }

    public static void persistentLogTraceroute(String text) {
        logIntoFile(TRACEROUTE_LOGGING_FILENAME, text);
    }

    public static void logPaths(ArrayList<String> oldPath, ArrayList<String> newPath) {
        String oldPathLog = "- Old path: " + LogManager.preparePathToLog(oldPath);
        String newPathLog = "- New path: " + LogManager.preparePathToLog(newPath);

        print(oldPathLog + "\n" + newPathLog);
        persistentLogPath(oldPathLog + "\n" + newPathLog);
    }

    public static String preparePathToLog(ArrayList<String> pathList) {

        String path = "";
        if (pathList.size() != 0) {
            path += pathList.get(0);
            for(int i = 1; i < pathList.size(); i++) {
                path += " -> " + pathList.get(i);
            }
        }
        return path;
    }

    public static void logAccordingToMetric(boolean printToSysOut, String metric, String text) {

        String filename = Utils.EMPTY;
        switch (metric) {
            case MonitoringMode.LAT:
                filename = LAT_LOGGING_FILENAME;
                break;
            case MonitoringMode.HOP:
                filename = HOP_LOGGING_FILENAME;
                break;
            case MonitoringMode.PATH:
                filename = PATH_LOGGING_FILENAME;
                break;
            case MonitoringMode.PROP:
                filename = PROP_LOGGING_FILENAME;
                break;
        }

        if (printToSysOut) {
            print(text);
        }

        if (filename.equals(Utils.EMPTY)) {
            logIntoFile(filename, text);
        }
    }


    public static void logIntoFile(String filename, String text) {

        try {
            Writer writer = new BufferedWriter(
                    new OutputStreamWriter(
                            new FileOutputStream(filename, true),
                            FILE_ENCODING
                    )
            );
            writer.write(text + "\n");
            writer.close();

        } catch (FileNotFoundException exception) {
            exception.printStackTrace();

        } catch (UnsupportedEncodingException exception) {
            exception.printStackTrace();

        } catch (IOException exception) {
            exception.printStackTrace();
        }
    }
}





