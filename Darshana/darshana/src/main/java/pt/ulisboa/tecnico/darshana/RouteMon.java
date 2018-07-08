package pt.ulisboa.tecnico.darshana;

import java.io.IOException;
import java.net.ConnectException;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.Stream;
import java.util.ArrayList;

import pt.ulisboa.tecnico.darshana.utils.Utils;
import pt.ulisboa.tecnico.darshana.utils.ExponentialMovingAverage;
import pt.ulisboa.tecnico.darshana.utils.MonitoringMode;
import pt.ulisboa.tecnico.darshana.utils.LogManager;
import pt.ulisboa.tecnico.darshana.structures.Structures.*;

/**
 * Main program that monitors the path between Client and Server
 */
public class RouteMon {

	// Metric's thresholds
	public static Double _latThreshold = new Double(1.3);
	public static Double _hopThreshold = new Double(1.020);
	public static Double _pathThreshold = new Double(0.8);
	public static Double _propThreshold = new Double(1.5);
	public static Double _pathNonesThreshold = new Double(0.5);
	public static Integer _probePeriod = 10000;

	// Main arguments
	private static String _sourceIP = "";
	private static String _destinationIP = "";
	private static String _destinationCryptoPingPort = "";

	private static AlertsBroadcaster _alertsBroadcaster = null;

	//	private static final Runtime runtime = Runtime.getRuntime();
	public static Integer hijack = 0;
	public static String hijacker = "";
	public static HijackedObj hijackedObj = null;
	public static List<String> hijackedPath = new ArrayList<String>();
	private static Double deviatedRTT = null;
	private static Double deviatedHopCount = null;
	private static Route deviatedRoute = null;

	private static Integer numTraceroutes = 0;
	private static Integer numHopCounts = 0;
	private static Integer numNones;

	private static Integer numExp = 0;
	private static Integer method_two = 0;
	private static Integer propCounter = 0;
	private static Double otherDelay = null;

	private static ArrayList<ArrayList<String>> myPaths = new ArrayList<ArrayList<String>>();
	private static ArrayList<Latencies> myLatencies = new ArrayList<Latencies>();
	private static ArrayList<Double> myHops = new ArrayList<Double>();
	private static ArrayList<Means> myMeans = new ArrayList<Means>();

	private static ExponentialMovingAverage av_RTT = new ExponentialMovingAverage(new Double(0.5));
	private static ExponentialMovingAverage av_HopCount = new ExponentialMovingAverage(new Double(0.5));

	public static void initialize(MoniTools tc) throws IOException, InterruptedException {

		for (int i = 0; i < 2; i++) {
			executeTrace(_destinationIP, tc);
		}

		for (int i = 0; i < 3; i++) {
			executePing(_destinationIP, _destinationCryptoPingPort, tc);
		}
	}

	public static String monitor(MoniTools tc) throws IOException, InterruptedException {

		Integer numPings = 0;
		String conclusion = Utils.EMPTY;
		Counters counter = new Counters(0,0);

		LogManager.print("|--------------------- 1. Active Probing ---------------------|");
		while (true) {

			Thread.sleep(_probePeriod);
			numExp++;
			counter = executeTool(tc, counter);

			if (rttMeasurement(tc)) {

				LogManager.print("!! WARNING !! Lat comparison is above threshold... Lets see Hop and Prop 1º phase");
				LogManager.print("|-> Prop 1º Phase ...");

				Double minOtherDelay = Utils.calcMinOtherDelay(myLatencies);
				Double minRTT = myLatencies.stream().min((m1, m2) -> Double.compare(m1.getRtt(), m2.getRtt())).get().getRtt();
				Double maxPropDelay = minRTT - minOtherDelay;

				LogManager.print("- MinRTT : " + minRTT);
				LogManager.print("- minOtherDelay : " + minOtherDelay);
				LogManager.print("- maxPropDelay : " + maxPropDelay);
				LogManager.print("- Total_Distance: " + MoniTools.localize(maxPropDelay));

				System.out.println("|-> Hop Count ...");
				if (hopCountMeasurement(_destinationIP, tc)) {

					LogManager.print("!! WARNING !! Hop count is above threshold... Possible hijacking... Lets check Path mechanism");
					LogManager.print("|------------------------- 2. Path Similarity --------------------------|");

					ArrayList<String> newPathMes;
					Route newRoute;
					Integer numHops;

					LogManager.print("|-> Getting new Path measurement ...");

					// deviatedRoute is only affected in case of hijack simulation
					if (!isDeviatedRouteSet()) {
						newRoute = tc.runTraceroute(_destinationIP, 0);
						newPathMes = (ArrayList<String>) newRoute.getASes();

					}
					// BEGIN: HIJACK STUFF
					else {

						// deviated route is a path calculated between the client and the hijacker
						newRoute = deviatedRoute;
						// concatenate deviated route with path fetched from the hijacker
						newPathMes =(ArrayList<String>)Stream.concat(newRoute.getASes().stream(), hijackedPath.stream()).collect(Collectors.toList());
						// removes duplicate nodes from the route
						//newPathMes = (ArrayList<String>) newPathMes.stream().distinct().collect(Collectors.toList());
						deviatedRoute = null;
					}
					// END: HIJACK STUFF

					myPaths.add(newPathMes);

					LogManager.print("|-> Lets check Path measurement... Checking for visibility -> none * * * results");

					numNones = newRoute.getNumNones();
					numHops = newRoute.getNumHops();

					if ((numNones / numHops) > _pathNonesThreshold) {
						// If numNones * * * surpasses pre established threshold then result is UNCONCLUSIVE
						LogManager.print("!! WARNING !! No visibility... Path results are UNConclusive!");
						conclusion = Utils.UNCONCLUSIVE;

					} else {
						LogManager.print("!! WARNING !! Some visibility... Path results are Conclusive!!");
						conclusion = Utils.CONCLUSIVE;
					}
				}
			}

			if (numExp > 150) {
				System.exit(0);
			}

			// BEGIN: HIJACK STUFF

			Thread.sleep(3000);

			// If hijack simulation was detected
			if (isOnHijackSimulation()) {
				intercepted(tc);
			}
			// END: HIJACK STUFF

			// In case conclusion its empty then there were no anomalies at hop count
			if (!conclusion.equals(Utils.EMPTY)) {
				return conclusion;
			}
		}
	}

	/*
		Hijack Simulation
	 */

	public static void intercepted(MoniTools tc) throws IOException{

		List<Double> rtts = Utils.fillRtts();

		Integer index = (int) (Math.random()*rtts.size());
		System.out.println("Index : " + index);

		LogManager.print("Hijacked .. simulation...");
		deviatedRoute = tc.runTraceroute(hijacker,1);
		deviatedRTT = rtts.get(index);
		deviatedHopCount = (double) deviatedRoute.getNumHops();

		LogManager.print("DeviatedRTT : " + deviatedRTT);
		LogManager.print("DeviatedHopCount : " + deviatedHopCount);

		try {
			SimHijack.rcvHijacked();
			//	System.out.println("HijackedRTT : " + hijackedObj.getRTT());
			//	System.out.println("HijackedHopCount : " + hijackedObj.getHopCount());
			LogManager.print("Rcved Hijack");

		} catch (ConnectException e) {
			handleHijackerGone();
		}
	}

	public static void handleHijackerGone() {

		LogManager.print("gone with the wind...");
		hijack = 0;
		new Thread(new SimHijack(_sourceIP, _destinationIP, _destinationCryptoPingPort)).start();
	}

	/*
		Decision Makers
	 */

	public static boolean verifyPropDelay() throws IOException, InterruptedException {

		Double quoProp = (double)0;

		Double anomalousRtt = myLatencies.get(myLatencies.size() - 1).getRtt();
		Double maximumRTT = myLatencies.subList(0, myLatencies.size() - 1).stream().max((m1, m2) -> Double.compare(m1.getRtt(), m2.getRtt())).get().getRtt();

		Double minOtherDelay = Utils.calcMinOtherDelay(myLatencies);
		Double maxOtherDelay = myLatencies.stream().max((m1, m2) -> Double.compare(m1.getRemainDelay(), m2.getRemainDelay())).get().getRemainDelay();

		// Compare the anomalous RTT (form the hijack) with other normal delays
		// It can be negative
		Double propDelayExp = anomalousRtt - maxOtherDelay;

		// Minimo do RTT aproxima-se mais da latencia de propagacao
		Double maxPropDelay = maximumRTT - minOtherDelay;

		quoProp = propDelayExp / maxPropDelay;

		String logText = "|INFO| Prop values ..." + "\n"
				+ "- Anomolous RTT : " + anomalousRtt + "\n"
				+ "- Max Other Delay : " + maxOtherDelay + "\n"
				+ "- Max RTT : " + maximumRTT + "\n"
				+ "- Prop_Delay Max : " + maxPropDelay + "\n"
				+ "- Prop_Delay Exp : " + propDelayExp + "\n"
				+ "- Quocient: " + quoProp;

		LogManager.logAccordingToMetric(true, MonitoringMode.PROP, logText);

		if (quoProp > _propThreshold) {
			LogManager.print("----------------------------------------------");
			LogManager.print("Distance: " + MoniTools.localize(propDelayExp));
			LogManager.print("Propagation Delay quocient -> " + quoProp + " > " + _propThreshold);
			LogManager.print("----------------------------------------------");
			return true;
		} else {
			LogManager.print("----------------------------------------------");
			LogManager.print("Not much variation in propagation time");
			LogManager.print("----------------------------------------------");
			return false;
		}
	}


	public static boolean verifyPathSimilarity(){

		Float sorensenCoef = null;

		System.out.println("Number of routers that did not answer: " + numNones);
		sorensenCoef = Utils.calcSorensen(myPaths);

		String logValues = "Paths Similarity = " + sorensenCoef + " | Path Threshold = " + _pathThreshold.toString();
		LogManager.logAccordingToMetric(true, MonitoringMode.PATH, logValues);

		// if sorensen coeficient is below the threshold then the paths are different enough to
		// suspect of an hijack attack
		boolean arePathsDifferent = sorensenCoef < _pathThreshold;
		String logConclusion;

		if (arePathsDifferent) {
			logConclusion = "!!!! Paths seem to be quite different. Possible hijack... !!!! ";
		} else {
			logConclusion = "Paths continue to be quite similar.";
		}

		LogManager.print("----------------------------------------------");
		LogManager.logAccordingToMetric(true, MonitoringMode.PATH, logConclusion);
		LogManager.print("----------------------------------------------");

		return arePathsDifferent;
	}

	/**
	 * This decides according to a final analysis if the traffic source -> destination was hijacked
	 *
	 * @param conclusion CONCLUSIVE if results from paris-traceroute are complete, UNCONCLUSIVE if otherwise
	 * @return true if hijack is detected, false if otherwise
	 * @throws IOException
	 * @throws InterruptedException
	 */
	public static boolean checkIfWasHijacked(String conclusion) throws IOException, InterruptedException {
		boolean wasHijacked = false;
		if (conclusion.equals(Utils.CONCLUSIVE)) {
			LogManager.print("|------------------------- 3. Calculate Path Similarity ------------------------|");
			wasHijacked = verifyPathSimilarity();
		} else {
			LogManager.print("|------------------------- 3. Propagation Delay Validation ----------------------|");
			wasHijacked = verifyPropDelay();
		}
		return wasHijacked;
	}

	/*
		Auxiliar methods
	 */
	public static void executeTrace(String destinationIP, MoniTools tc) throws IOException, InterruptedException{

		Route myRoute = tc.runTraceroute(destinationIP, 0);
		myPaths.add((ArrayList<String>) myRoute.getASes());

		if (myRoute.isIncomplete()) {
			method_two = 1;
		}

		myHops.add((double)tc.getHopCount());
		av_HopCount.average(myHops.get(myHops.size() - 1));

		numTraceroutes++;

		// Logging number of Traceroutes
		LogManager.persistentLogTraceroute(numTraceroutes.toString());
	}

	public static void executePing(String destinationIP, String destinationPort, MoniTools tc) throws IOException, InterruptedException {

		if (!isOnHijackSimulation()) {
			myLatencies.add(tc.runPing(_sourceIP, destinationIP, destinationPort));
		} else {
			if (deviatedRTT != null && hijackedObj != null) {
				myLatencies.add(new Latencies(deviatedRTT + hijackedObj.getRTT(), null, (double) 0));
			}
		}
	}

	/**
	 * Performs needed actions when hijack is declared
	 */
	public static void declareHijack(String metric){

		LogManager.print("!!!! ---------------------------------------------- !!!!");
		LogManager.print("!!!! DANGER !!!! High chance you have been hijacked !!!!");
		LogManager.print("!!!! ---------------------------------------------- !!!!");

		// Warn final user... communicate
		if (_alertsBroadcaster != null) {
			String alertMessage = AlertsBroadcaster.getAlertFormatted("Hijack", _sourceIP, _destinationIP, metric);
			_alertsBroadcaster.sendAlertToRemoteDAR(alertMessage);
		}
	}

	public static boolean onVerifyMetricThreshold(String metric, Double metricThreshold, Double newValue, Double oldValue) {

		boolean isExceeded;
		Double estimatedRatio = null;

		estimatedRatio = newValue / oldValue;
		isExceeded = estimatedRatio > metricThreshold;

		String logText = "|INFO| " + metric
				+ " Threshold = " + metricThreshold.toString()
				+ " | Estimated Ratio = " + newValue.toString() + " / " + oldValue.toString()
				+ " = " + estimatedRatio.toString();

		LogManager.logAccordingToMetric(true, metric, logText);

		if (isExceeded) {
			LogManager.print("!!!! DANGER !!!! " + metric + " -> Exceeded threshold !!!!");
		} else {
			LogManager.print("!!!! OK !!!! " + metric + " -> Seems to be normal under the threshold...");
		}

		return isExceeded;
	}

	/*
		Isolated Measurements
	 */

	public static void monitoringOnlyLat(MoniTools tc) throws IOException, InterruptedException {

		// Initialize latencies measurements
		executePing(_destinationIP, _destinationCryptoPingPort, tc);

		LogManager.print("Start monitoring LAT ...");
		while (true) {
			if (isOnHijackSimulation()) {
				intercepted(tc);
			}
			executePing(_destinationIP, _destinationCryptoPingPort, tc);
			if (rttMeasurement(tc)) {
				declareHijack(MonitoringMode.LAT);
			}
			Thread.sleep(_probePeriod);
		}
	}

	public static void monitoringOnlyHop(MoniTools tc) throws IOException, InterruptedException {

		// Initialize counts and decide which protocol suits best to get complete results
		executeTrace(_destinationIP, tc);

		LogManager.print("Start monitoring HOP ...");
		while (true) {
			if (isOnHijackSimulation()) {
				intercepted(tc);
			}
			if (hopCountMeasurement(_destinationIP, tc)) {
				declareHijack(MonitoringMode.HOP);
			}
			Thread.sleep(_probePeriod);
		}
	}

	public static void monitoringOnlyPath(MoniTools tc) throws IOException, InterruptedException {

		// Initialize paths list
		executeTrace(_destinationIP, tc);

		LogManager.print("Start monitoring PATH ...");
		while (true) {
			if (isOnHijackSimulation()) {
				intercepted(tc);
			}
			if (pathMeasurement(_destinationIP, tc)) {
				declareHijack(MonitoringMode.PATH);
			}
			Thread.sleep(_probePeriod);
		}
	}

	public static void monitoringOnlyProp(MoniTools tc) throws IOException, InterruptedException {

		// Initialize measurements
		initialize(tc);

		LogManager.print("Start monitoring PROP ...");
		while (true) {
			if (isOnHijackSimulation()) {
				intercepted(tc);
			}
			if (propMeasurement(_destinationIP, tc)) {
				declareHijack(MonitoringMode.PROP);
			}
			Thread.sleep(_probePeriod);

			executePing(_destinationIP,_destinationCryptoPingPort, tc);
			executeTrace(_destinationIP, tc);
		}
	}

	public static Double executeNumHops(String destination, MoniTools tc) throws IOException{

		if (method_two == 1) {
			return null;
		}

		if (!isOnHijackSimulation()) {
			tc.calcHopCount(destination);
			myHops.add((double)tc.getHopCount());
		} else {
			if (hijackedObj != null && deviatedHopCount != null) {
				myHops.add(deviatedHopCount + hijackedObj.getHopCount());
			}
		}

		Double oldHopCount =  av_HopCount.average(myHops.get(myHops.size() - 2));
//		LogManager.print("EMA Hop Count: " + oldHopCount);

		numHopCounts++;

		// Logging number of Hops
		LogManager.persistentLogHop(numHopCounts.toString());

		return oldHopCount;
	}

	/*
		Main of Measurements
	 */

	public static Boolean rttMeasurement(MoniTools tc) throws IOException, InterruptedException {

		String metric = MonitoringMode.LAT;
		Double newRtt = null;
		Double oldRtt = null;

		oldRtt = av_RTT.average(myLatencies.get(myLatencies.size() - 2).getRtt());
		newRtt = myLatencies.get(myLatencies.size() - 1).getRtt();

		LogManager.print("- EMA RTT: " + oldRtt);
		LogManager.print("- New RTT: " + newRtt);

		// Verifying Lat threshold
		return onVerifyMetricThreshold(metric, _latThreshold, newRtt, oldRtt);

	}

	public static Boolean hopCountMeasurement(String destinationIP, MoniTools tc) throws IOException{

		String metric = MonitoringMode.HOP;
		Double newHopCount = null;
		Double oldHopCount = null;

		oldHopCount = executeNumHops(destinationIP, tc);
		if (oldHopCount == null) {
			LogManager.print("- EMA Hop Count: is null");
			return true;
		}

		newHopCount = myHops.get(myHops.size() - 1);

		LogManager.print("- EMA Hop Count: " + oldHopCount);
		LogManager.print("- New Hop Count: " + newHopCount);

		// Verifying Hop threshold
		return onVerifyMetricThreshold(metric, _hopThreshold, newHopCount, oldHopCount);
	}

	public static Boolean pathMeasurement(String destinationIP, MoniTools tc) throws IOException, InterruptedException {

		String metric = MonitoringMode.PATH;
		ArrayList<String> newPathMes;
		Route newRoute;

		// simple path measurement without hijacking code embedded
//		newRoute = tc.runTraceroute(destinationIP, 0);
//		newPathMes = (ArrayList<String>) newRoute.getASes();

		// deviatedRoute is only affected in case of hijack simulation
		if (!isDeviatedRouteSet()) {
			newRoute = tc.runTraceroute(_destinationIP, 0);
			newPathMes = (ArrayList<String>) newRoute.getASes();

		}
		// BEGIN: HIJACK STUFF
		else {

			// deviated route is a path calculated between the client and the hijacker
			newRoute = deviatedRoute;
			// concatenate deviated route with path fetched from the hijacker
			newPathMes =(ArrayList<String>)Stream.concat(newRoute.getASes().stream(), hijackedPath.stream()).collect(Collectors.toList());
			// removes duplicate nodes from the route
			//newPathMes = (ArrayList<String>) newPathMes.stream().distinct().collect(Collectors.toList());
			deviatedRoute = null;
		}
		// END: HIJACK STUFF

		myPaths.add(newPathMes);

		numNones = newRoute.getNumNones();

		// Verifying Path threshold
		return verifyPathSimilarity();
	}

	public static Boolean propMeasurement(String destinationIP, MoniTools tc) throws IOException, InterruptedException {

		String metric = MonitoringMode.PROP;
		ArrayList<String> newPathMes;
		Route newRoute;

		Double minOtherDelay = Utils.calcMinOtherDelay(myLatencies);
		Double minRTT = myLatencies.stream().min((m1, m2) -> Double.compare(m1.getRtt(), m2.getRtt())).get().getRtt();
		Double maxPropDelay = minRTT - minOtherDelay;

		newRoute = tc.runTraceroute(destinationIP, 0);
		newPathMes = (ArrayList<String>) newRoute.getASes();
		myPaths.add(newPathMes);

		numNones = newRoute.getNumNones();

		// Verifying Path threshold
		return verifyPropDelay();
	}

	private static boolean isOnHijackSimulation(){
		return hijack == 1;
	}

	private static boolean isDeviatedRouteSet() {
		return deviatedRoute != null;
	}

	public static Counters executeTool(MoniTools tc, Counters counter) throws IOException, InterruptedException{

		Integer numPings = counter.getNumPings();
		Integer numHop = counter.getNumHops();

		if (isOnHijackSimulation()) {
			intercepted(tc);
		}

		if (numPings < 5) {
			executePing(_destinationIP, _destinationCryptoPingPort, tc);
			numPings++;
		} else {
			if (numHop < 5) {
				executeNumHops(_destinationIP,tc);
				numHop++;
			} else {
				executeTrace(_destinationIP, tc);
				numHop = 0;
			}
			numPings = 0;
		}

		return new Counters(numPings,numHop);
	}

	/*
		Main()
	 */
	public static void main(String[] args) throws IOException {

		int maxArgs = 10;
		if (args.length < maxArgs) {

			LogManager.print("|ERROR| Insufficient number of arguments!! " + args.length + " instead of " + maxArgs);
			LogManager.print("|ERROR| Example: <monitoring-mode> <source-ip> <destination-ip> <destination-port> <lat-threshold> <hop-threshold> <path-threshold> <prop-threshold> <path-nones-threshold> <probe-period> [dar-ip,dar-port]");
			return;
		}

		// Mode can be [full, lat, hop, path, prop]
		String modeArg = args[0].toLowerCase();
		MonitoringMode monitoringMode = new MonitoringMode();

		if (!monitoringMode.isValidMode(modeArg)) {
			LogManager.print("|ERROR| Monitoring mode \"" + modeArg + "\" is invalid!!");
			return;
		}

		// Getting IPs and ports
		_sourceIP = args[1];
		_destinationIP = args[2];
		_destinationCryptoPingPort = args[3];

		if (Utils.isPublicIp(_sourceIP)) {
			_sourceIP = Utils.runCommand(Utils.GET_PUBLIC_IP_COMMAND).get(0);
		}

		// Getting thresholds
		try {
			_latThreshold = Double.parseDouble(args[4]);
			_hopThreshold = Double.parseDouble(args[5]);
			_pathThreshold = Double.parseDouble(args[6]);
			_propThreshold = Double.parseDouble(args[7]);
			_pathNonesThreshold = Double.parseDouble(args[8]);

		} catch (NumberFormatException e) {
			LogManager.print("|ERROR| One of the thresholds is not in double format!!");
			LogManager.print(e.getMessage());
			e.printStackTrace();
			return;
		}

		// Getting probe period
		try {
			_probePeriod = Integer.parseInt(args[9]) * 1000; // sec to ms

		} catch (NumberFormatException e) {
			LogManager.print("|ERROR| Probe period is in the wrong format!!");
			LogManager.print(e.getMessage());
			e.printStackTrace();
			return;
		}

		String preliminaryConclusion = "";
		boolean wasHijacked = false;

		// Start server for receiving measures --> Hijack
		new Thread(new SimHijack(_sourceIP, _destinationIP, _destinationCryptoPingPort)).start();

		MoniTools tc = new MoniTools();

		// Check if there is a last argument [DAR_ip,DAR_port]
		if (args.length > maxArgs) {

			String[] darIpAndPort = args[maxArgs].split(",");
			try {
				_alertsBroadcaster = new AlertsBroadcaster(darIpAndPort[0], Integer.parseInt(darIpAndPort[1]));
			} catch (Exception e) {
				LogManager.print("|ERROR| Either DAR ip and port aren't well formatted or port isn't in integer format!!");
				LogManager.print(e.getMessage());
				e.printStackTrace();
				return;
			}
		}

		try {
			switch (modeArg) {
				case MonitoringMode.LAT:
					monitoringOnlyLat(tc);
					break;

				case MonitoringMode.HOP:
					monitoringOnlyHop(tc);
					break;

				case MonitoringMode.PATH:
					monitoringOnlyPath(tc);
					break;

				case MonitoringMode.PROP:
					monitoringOnlyProp(tc);
					break;

				case MonitoringMode.FULL:
					initialize(tc);

					while (true) {
						preliminaryConclusion = monitor(tc);
						wasHijacked = checkIfWasHijacked(preliminaryConclusion);

						if (wasHijacked) {
							String metric = preliminaryConclusion.equals(Utils.CONCLUSIVE) ? MonitoringMode.PATH : MonitoringMode.PROP;
							declareHijack(metric);
							//					return;
						}
					}
			}
		} catch (Exception e) {
			System.out.println(e.getMessage());
			e.printStackTrace();
		}
	}
}
