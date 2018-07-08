package pt.ulisboa.tecnico.darshana.utils;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.LinkedList;
import java.util.List;
import java.util.ArrayList;

import pt.ulisboa.tecnico.darshana.structures.Structures.Latencies;

public class Utils {

	// Commands
	public static final String GET_PUBLIC_IP_COMMAND = "sh getPubIP.sh";
	public static final String WHOIS_COMMAND = "whois -h whois.cymru.com";
	public static final String WHOIS_OLD_COMMAND = "whois -h riswhois.ripe.net";

	public static final String CRYPTO_PING_CLIENT_COMMAND = "python CryptoPingClient.py";
	public static final String PARIS_TRACEROUTE_UDP_COMMAND = "paris-traceroute -M 10 -n -q 1";
	public static final String PARIS_TRACEROUTE_TCP_COMMAND = "paris-traceroute -M 10 -n -q 1 -T";
	public static final String PARIS_TRACEROUTE_ICMP_COMMAND = "paris-traceroute -M 10 -n -q 1 -I";

	public static String getPartialTraceroute(String ttl, String protocol, String destinationIP) {
		return "paris-traceroute -f " + ttl + " -n -q 1 " + protocol + SPACE + destinationIP;
	}

	// General constants
	public static final String SPACE = " ";
	public static final String EMPTY = "";
	public static final String NONE = "NONE";
	public static final String CONCLUSIVE = "CONCLUSIVE";
	public static final String UNCONCLUSIVE = "UNCONCLUSIVE";

	private static final Runtime runtime = Runtime.getRuntime();

    public static List<String> runCommand(String command) throws IOException {

    	List<String> lines = new LinkedList<String>();
    	Process process = runtime.exec(command);

    	BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
    	String line;

    	while ((line = reader.readLine()) != null) {
    		//System.out.println(line);
    		lines.add(line);
    	}

		//System.out.println("Size: " + lines.size());
		return lines;
	}

	public static Double calcMinOtherDelay(List<Latencies> myLatencies){

		Double min = myLatencies.get(0).getRemainDelay();

		for(Latencies m : myLatencies){
			if(m.getRemainDelay() == (double)0) {
				continue;
			}
			if(m.getRemainDelay() < min) {
				min = m.getRemainDelay();
			}
		}
		return min;
	}

	/**
	 * This gives the similarity in a number that ranges from [0,1].
	 * 0 means that there is no similarity at all and 1 means that the items of the two
	 * paths are the same
	 */
	public static float calcSorensen(ArrayList<ArrayList<String>> myPaths){

		Float sorensenCoef = null;
		ArrayList<String> oldPath = new ArrayList<String>();
		ArrayList<String> newPath = new ArrayList<String>();
		ArrayList<String> path = new ArrayList<String>();
		Float interPathSize = null;
		Float newPathSize = null;
		Float oldPathSize = null;

		oldPath = myPaths.get(myPaths.size() - 2);
		path = myPaths.get(myPaths.size() - 1);
		newPath = path;

		LogManager.logPaths(oldPath, newPath);

		oldPathSize = (float) oldPath.size();
		newPathSize = (float) newPath.size();

		// Performs an intersection between oldPath and newPath
		path.retainAll(oldPath);
		interPathSize = (float) path.size();

		String logText = "IntersectionPathSize = " + interPathSize + " | OldPathSize = " + oldPathSize + " | NewPathSize = " + newPathSize;
		LogManager.print(logText);
		LogManager.persistentLogPath(logText);

		sorensenCoef = (float) (2*interPathSize / (newPathSize + oldPathSize));

		return sorensenCoef;
	}

	/**
	 * Rtts values for hijacking
	 */
	public static List<Double> fillRtts(){

		List<Double> rtts = new ArrayList<Double>();

		rtts.add((double)935.693979263305664062);
		rtts.add((double)902.637004852294921875);
		rtts.add((double)873.034954071044921875);
		rtts.add((double)839.745044708251953125);
		rtts.add((double)935.590982437133789062);
		rtts.add((double)876.423120498657226562);
		rtts.add((double)936.041831970214843750);
		rtts.add((double)878.150939941406250000);
		rtts.add((double)874.613046646118164062);
		rtts.add((double)935.104846954345703125);
		rtts.add((double)837.863922119140625000);
		rtts.add((double)840.775012969970703125);
		rtts.add((double)872.767925262451171875);
		rtts.add((double)901.504993438720703125);
		rtts.add((double)874.640941619873046875);
		rtts.add((double)940.517902374267578125);
		rtts.add((double)873.646020889282226562);

		return rtts;
	}

	/**
	 * Check if second argument is an ip to use or an indicator to use public ip
	 *
	 * @param address ip address to use for the communications with the hijacked client
	 * @return true if the program should use public ip, false if should use the ip provided
	 */
	public static boolean isPublicIp(String address) {
		return address.toLowerCase().equals("publicip") || address.toLowerCase().equals("p");
	}
}
