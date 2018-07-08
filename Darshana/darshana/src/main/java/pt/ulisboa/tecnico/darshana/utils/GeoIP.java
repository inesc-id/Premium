package pt.ulisboa.tecnico.darshana.utils;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.maxmind.geoip.*;

import pt.ulisboa.tecnico.darshana.structures.Structures;

public class GeoIP {
	
	private static Double _lightSpeed = new Double(299792.458); //km por segundo
	private static final String GEO_LITE_CITY_DAT_FILE = "GeoLiteCity.dat";

    private static Double distance(String originAdr, String destinationAdr) {
	
    	Location l1 = null;
        Location l2 = null;
    	Double distance = new Double(0);

    	try {
		
			LookupService cl = new LookupService(GEO_LITE_CITY_DAT_FILE, LookupService.GEOIP_MEMORY_CACHE );

			l1 = cl.getLocation(originAdr);
			l2 = cl.getLocation(destinationAdr);
			cl.close();

		} catch (IOException e) {
			System.out.println(e.getMessage());
			e.printStackTrace();
		}

		// in a local setting, searching for local IP addresses the location with be null
		if (l1 != null && l2 != null) {
			distance = l1.distance(l2);
		}
		return distance;
    }
    
    public static Double calcPropDelay(String originAdr, String destinationAdr){
    	
    	Double propDelay = (double) 0;
    	Double d = distance(originAdr, destinationAdr);
    	Double c = _lightSpeed;
    	
    	propDelay = (Double.parseDouble("3") / c) * d;   // seconds
    
    	return propDelay * 1000; // ms
    }
    
    public static Double calcRemainingDelay(Double rtt, Double propDelay) {
    	return rtt - propDelay;
    }

    public static List<Double> calcTotalPropDelay(Structures.Route route) {

		List<Structures.Node> myNodes = route.getNodes();
		List<Double> myTimes = new ArrayList<Double>();
		Double thisPropDelay = (double) 0;
		String thisIP = "";
		String nextIP = "";

		myNodes.removeIf(n -> n.getIp().equals(Utils.NONE));

//		Node node = new Node(destination, "");
//		if (route.getIncomplete() == 1) {
//			myNodes.add(node);
//		}

		for (int i=0; i < route.getNodes().size() - 1; i++) {

			thisIP = myNodes.get(i).getIp();
			nextIP = myNodes.get(i + 1).getIp();

			if (thisIP.substring(0,2).equals("10") || thisIP.substring(0, 7).equals("192.168")) {
				thisPropDelay = (double) 0;
			} else {
    			if (nextIP.substring(0,2).equals("10") || nextIP.substring(0,7).equals("192.168")) {
					continue;
				} else {
					thisPropDelay = calcPropDelay(thisIP, nextIP);
				}
			}
			LogManager.print("From :" + thisIP + " to :" + nextIP + " PropDelay: " + thisPropDelay);
    		myTimes.add(thisPropDelay);
    	}
    
    	return myTimes;
    }
    
    public static Double calcDistance(Double progLatency) {
    	
    	Double distance = (double) 0;
    	progLatency = progLatency /(double)1000;
    	distance = (progLatency * _lightSpeed) / 3;  //km

    	return distance;
    }
}
