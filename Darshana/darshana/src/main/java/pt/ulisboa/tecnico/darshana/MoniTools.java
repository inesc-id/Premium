package pt.ulisboa.tecnico.darshana;
 
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.ArrayList;
import java.util.stream.*;

import pt.ulisboa.tecnico.darshana.structures.Structures.Route;
import pt.ulisboa.tecnico.darshana.structures.Structures.Node;
import pt.ulisboa.tecnico.darshana.structures.Structures.Latencies;
import pt.ulisboa.tecnico.darshana.structures.Structures.ProtoResults;
import pt.ulisboa.tecnico.darshana.utils.GeoIP;
import pt.ulisboa.tecnico.darshana.utils.Utils;
import pt.ulisboa.tecnico.darshana.utils.LogManager;

/**
 * This class executes the paris traceroute command and parses the output of it,
 * providing information about the RTT of a communication and the path of the packets.
 * 
 * @author Karan Balu
 * @version 1.0
 *
 */
public class MoniTools {

	private static Integer ttl = 1;
	private static Double propDelay = (double) 0;
	private static final String FAIL = "FAIL";
	public static List<Double> myTimes;
	public static String protocol = "";

	/** REGEX to parse the output of traceroute command.
	 * @throws IOException
	 * @throws InterruptedException */

	public Latencies runPing(String sourceIp, String destinationIp, String destinationPort) throws IOException, InterruptedException {

		List<Node> myNodes = new ArrayList<Node>();
		List<String> measures = null;
		Node newNode = null;
		double rtt = 0;
		double otherDelay = 0;

		// Build cryptographic ping client command
		String cryptoPingCommand = Utils.CRYPTO_PING_CLIENT_COMMAND
				+ Utils.SPACE + destinationIp
				+ Utils.SPACE + destinationPort;

		System.out.println("-> Cryptographic Ping");
		for (int i = 0; i < 5; i++) {

			measures = Utils.runCommand(cryptoPingCommand);

			// Get ping result, which is RTT value
			String pingResult = getRttFromCryptographicPing(measures);

			if (pingResult.equals(FAIL)) {
				LogManager.print("Something failed with ping!");

			} else if(pingResult.equals("0")) {
				System.out.println("The nonce is not the same ! Traffic may have been intercepted.");

			} else {
				rtt = Double.parseDouble(pingResult);
				propDelay = GeoIP.calcPropDelay(sourceIp, destinationIp);
				otherDelay = GeoIP.calcRemainingDelay(rtt, propDelay);
				newNode = new Node(destinationIp, pingResult, ttl.toString(), propDelay, otherDelay);
				myNodes.add(newNode);
			}
		}
		return getMeanMeasuresUpgraded(myNodes);
	}

	private String getRttFromCryptographicPing(List<String> measures){
		String rtt = FAIL;
		String rttValueSeparator = "= ";
		for (String line : measures) {
			if (line.contains(rttValueSeparator)) {
				rtt = line.split(rttValueSeparator)[1];
			}
		}
		return rtt;
	}

	public Latencies getMeanMeasuresUpgraded(List<Node> measures){

		Double rttMean = new Double(0);
		Double propDelay = new Double(0);
		Double otherDelay = new Double(0);

		for(Node n : measures){
			rttMean += Double.parseDouble(n.getRtt());
			propDelay += n.getPropDelay();
			otherDelay += n.getRemainDelay();
		}

		Latencies latencyMeasure = new Latencies(
				rttMean / measures.size(),
				propDelay / measures.size(),
				otherDelay / measures.size()
		);

		return latencyMeasure;
	}

	public static Double localize(Double latProg){
		return GeoIP.calcDistance(latProg);
	}

    /**
     * Executes the traceroute command and parses the results.
     *
     * @param destinationIP is the IP of the destination
     * @param ttlFlag is 1 if an Hijack is being simulated, and 0 otherwise
	 *
     * @return most complete path measured
     * @throws IOException
     */
	public Route runTraceroute(String destinationIP, Integer ttlFlag) throws IOException {

		List<Route> everyRoute;
		List<ProtoResults> traceRouteOutput = new ArrayList<ProtoResults>();

		String parisTraceUDP = Utils.PARIS_TRACEROUTE_UDP_COMMAND + Utils.SPACE + destinationIP;
		String parisTraceTCP = Utils.PARIS_TRACEROUTE_TCP_COMMAND + Utils.SPACE + destinationIP;
		String parisTraceICMP = Utils.PARIS_TRACEROUTE_ICMP_COMMAND + Utils.SPACE + destinationIP;

		LogManager.print(parisTraceUDP);
		List<String> traceRouteOutputUDP = Utils.runCommand(parisTraceUDP);

		LogManager.print(parisTraceTCP);
		List<String> traceRouteOutputTCP = Utils.runCommand(parisTraceTCP);

		LogManager.print(parisTraceICMP);
		List<String> traceRouteOutputICMP = Utils.runCommand(parisTraceICMP);

		traceRouteOutput.add(new ProtoResults("-U", traceRouteOutputUDP));
		traceRouteOutput.add(new ProtoResults("-T", traceRouteOutputTCP));
		traceRouteOutput.add(new ProtoResults("-I", traceRouteOutputICMP));

		everyRoute = treatTracerouteData(traceRouteOutput, destinationIP);
		Route cRoute = mostCompletePath(everyRoute);
		verifyUncertanties(cRoute);

		if (ttlFlag == 0) {
			ttl = cRoute.getNumHops();
			if (!cRoute.isIncomplete()) {
				myTimes = GeoIP.calcTotalPropDelay(cRoute);
				propDelay = myTimes.stream().mapToDouble(i -> i.doubleValue()).sum();
			}
		}

		return cRoute;
	}

	public List<Route> treatTracerouteData(List<ProtoResults> traceRouteOutput, String destinationIP) {

		List<Route> everyRoute = new ArrayList<Route>();
		String ip = "";
		String rtt = "";
		Integer numNones = 0;
		List<Node> routeNodes = new LinkedList<Node>();
		Route r = null;

		for (ProtoResults p : traceRouteOutput) {

			// first traceroute line is useless
			// line1: traceroute to 10.0.2.28 (10.0.2.28), 30 hops max, 28 bytes packets
			// last line is empty
			p.getResults().remove(0);
			p.getResults().remove(p.getResults().size() - 1);

			// traceroute result: 1  10.0.2.28  0.589ms
			for (String trace : p.getResults()) {
//				LogManager.print(trace);

				ip = trace.split("  ")[1];
				if (ip.equals("*")) {
					ip = Utils.NONE;
					rtt = Utils.NONE;
					numNones++;

				} else {
					rtt = trace.split("  ")[2];
				}
				routeNodes.add(new Node(ip, rtt));
			}
			String F_rtt = routeNodes.get(routeNodes.size() - 1).getRtt().replaceAll("[^\\d.]", "");
			r = new Route(
					routeNodes.get(0).getIp(),
					destinationIP,
					routeNodes.size(),
					F_rtt,
					routeNodes,
					numNones,
					p.getProto()
			);
			everyRoute.add(r);

			// reinitialize variables
			numNones = 0;
			routeNodes = new LinkedList<Node>();
		}

		return everyRoute;
	}

    public Route mostCompletePath(List<Route> myRoutes) throws IOException{

    	 Route bestPath = myRoutes.stream().min((r1, r2) -> Integer.compare(r1.getNumNones(), r2.getNumNones())).get();
    	 List<String> myASes = new ArrayList<String>();
    	 List<Node> myNodes = bestPath.getNodes();

    	 protocol = bestPath.getProtocol();

    	 // Previously i=1 to avoid getting AS for first ip
    	 for (int i = 0; i < myNodes.size(); i++) {
    	 	String nodeIp = myNodes.get(i).getIp();
    		 if (nodeIp.equals(Utils.NONE)) {
    			 continue;
    		 } else {
    			 String asn = getAS(nodeIp);
    			 myASes.add(asn);
    		 }
    	 }

    	 myASes = myASes.stream().distinct().collect(Collectors.toList());
    	 bestPath.setASes(myASes);

    	 // Debug nodes and ases
//    	 bestPath.printNodes();
//    	 bestPath.printAses();

    	 return bestPath;
    }

	/**
	 * Gets the AS number according to a machine's ip
	 *
	 * @param ip of machine being inspected for AS number
	 * @return string representing the AS number
	 * @throws IOException
	 */
	public String getAS(String ip) throws IOException {

		List<String> result = Utils.runCommand(Utils.WHOIS_COMMAND + Utils.SPACE + ip);
		// Output example
		// AS      | IP               | AS Name
		// 1930    | 193.136.128.103  | RCCN Fundacao para a Ciencia e a Tecnologia, I.P., PT

		// Getting the AS number, returns an integer if valid AS,
		// returns NA, if can't find the AS... this happens when the ip is private
		String originASN = result.get(1).split(" ")[0];
		//String originASN = result.get(9); //para fedora

		return originASN;

	}

	/**
	 * Looks up for the last responsive AS, i.e., before * * * results appeared
	 *
	 * @param cRoute a route with nodes
	 * @throws IOException
	 */
	public void verifyUncertanties(Route cRoute) throws IOException {

    	String lastResponsiveAS = "";
    	List<Node> myNodes = cRoute.getNodes();
    	Node myNode;

    	if (myNodes.get(myNodes.size() - 1).getIp().equals(Utils.NONE)) {
    		cRoute.setIncomplete();
    		for (int i = myNodes.size() - 1; i >= 0; i--) {
    			myNode = myNodes.get(i);
    			if (!myNode.getIp().equals(Utils.NONE)) {
    				lastResponsiveAS = getAS(myNode.getIp());
    				break;
    			}
    		}
    	}
    	cRoute.setLastAS(lastResponsiveAS);
    }

    /*
    	Hop Count monitoring utils
     */

	public List<String> hopCounter(String destinationIP, Integer firstTTL) throws IOException{

		if (protocol.equals("-U")) {
			protocol = "";
		}

		String partialTracerouteCmd = Utils.getPartialTraceroute(firstTTL.toString(), protocol, destinationIP);

		LogManager.print(partialTracerouteCmd);
		List<String> traceRouteOutput = Utils.runCommand(partialTracerouteCmd);

		// first traceroute line is useless
		// traceroute to 10.0.2.28 (10.0.2.28), 30 hops max, 28 bytes packets
		// last line is empty
		traceRouteOutput.remove(0);
		traceRouteOutput.remove(traceRouteOutput.size() - 1);

		return traceRouteOutput;
	}

	public void calcHopCount(String destinationIP) throws IOException {

		Double hp = null;
		List<String> res;
		Integer firstTTL = ttl == 1 ? 1 : ttl - 1;

		res = hopCounter(destinationIP, firstTTL);

		if (res.size() == 1) {
			System.out.println("Only one?");
			res = hopCounter(destinationIP, 1);
		}

		Matcher matcher = Pattern.compile("\\d+").matcher(res.get(res.size() - 1));
		matcher.find();
		hp = Double.parseDouble(matcher.group());

		ttl = hp.intValue();
	}

	public Integer getHopCount(){
		return ttl;
	}

	public Double getLatestNumHopsRegistred(){
		return new Double(ttl);
	}
}
