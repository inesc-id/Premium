package pt.ulisboa.tecnico.darshana.structures;

import java.util.List;

public class Structures {

    public static class Latencies {

        private Double _rtt;
        private Double _progDelay;
        private Double _remainDelay;

        public Latencies(Double rtt, Double progDelay, Double remainDelay) {
            _rtt = rtt;
            _progDelay = progDelay;
            _remainDelay = remainDelay;
        }

        public Double getRtt() {
            return _rtt;
        }

        public Double getProgDelay() {
            return _progDelay;
        }

        public Double getRemainDelay() {
            return _remainDelay;
        }
    }

    public static class HijackedObj {

        private Double _hopCount;
        private Double _rtt;

        public HijackedObj(Double rtt, Double hopCount) {
            _rtt = rtt;
            _hopCount = hopCount;
        }

        public Double getHopCount() {
            return _hopCount;
        }

        public Double getRTT() {
            return _rtt;
        }
    }

    public static class ProtoResults {

        private String _protocol;
        private List<String> _results;

        public ProtoResults(String proto, List<String> results) {
            _protocol = proto;
            _results = results;
        }

        public String getProto() {
            return _protocol;
        }

        public List<String> getResults() {
            return _results;
        }
    }
    public static class Counters {

        private Integer _numPings;
        private Integer _numHops;

        public Counters(Integer numPings, Integer numHops){
            _numPings = numPings;
            _numHops = numHops;
        }

        public Integer getNumPings() {
            return _numPings;
        }

        public Integer getNumHops() {
            return _numHops;
        }

        public void setNumPings(Integer numPings) {
            _numPings = numPings;
        }

        public void setNumHops(Integer numHops) {
            _numHops = numHops;
        }
    }

	public static class Means {
	    
    	private Double _rtt;
    	private Double _hopCount;
    	private Double _progDelay;
    	private Double _remainDelay;
    	
    	public Means(Double rtt, Double hopCount, Double progDelay, Double remainDelay){
        	_rtt = rtt;
            _hopCount = hopCount;
            _progDelay = progDelay;
            _remainDelay = remainDelay;
    	}
    	
    	public Means(Double rtt, Double hopCount){
        	_rtt = rtt;
            _hopCount = hopCount;
    	}
    	
    	public Means(Double rtt, Double hopCount, Double progDelay){
        	_rtt = rtt;
            _hopCount = hopCount;
            _progDelay = progDelay;
    	}
    	
    	public Double getRtt() {
            return _rtt;
        }
    	
    	public Double getHopCount() {
            return _hopCount;
        }
    	
    	public Double getProgDelay() {
            return _progDelay;
        }
    	
    	public Double getRemainDelay() {
            return _remainDelay;
        }
    	
    	public String toString() {
            return _rtt + "-" + _hopCount;
        }
    	
    }
    
    public static class Node {

        private String _ip;
        private String _rtt;
        private String _numHops;
        private Double _propDelay;
        private Double _remainDelay;

        /**
         * Creates an instance of a Node.
         * 
         * @param ip of the node.
         * @param rtt in a communication from a given source to the node.
         */
        public Node(String ip,String rtt){
        	_ip = ip;
            _rtt = rtt;
        }
        
        public Node(String ip, String rtt,String numHops, Double propDelay, Double remainDelay) {
            _ip = ip;
            _rtt = rtt;
            _numHops = numHops;
            _propDelay = propDelay;
            _remainDelay = remainDelay;
        }
 
        public String getIp() {
            return _ip;
        }

        public String getRtt() {
            return _rtt;
        }
        
        public String getNumHops(){
        	return _numHops;
        }
        
        public Double getPropDelay(){
        	return _propDelay;
        }
        
        public Double getRemainDelay(){
        	return _remainDelay;
        }
        
        public String toString() {
            return _ip + " - " + _rtt + " ms";
        }
    }

    public static class Route {

        private List<Node> _nodes;
        private Integer _numHops;
        private String _rtt;
        private String _srcIp;
        private String _dstIp;
        private Integer _numNones;
        private List<String> _ASes;
        private String _lastAS;
        private boolean _incomplete = false;
        private String _proto = "";

        /**
         * Creates an instance of a Route.
         *
         * @param srcIp ip of the source
         * @param dstIp ip of the destination
         * @param numHops number of hops within the route, from the source to the destination
         * @param rtt round trip time
         * @param nodes nodes within the route
         * @param numNones number of nones in the route
         * @param protocol protocol of the most complete route
         */
        public Route(String srcIp, String dstIp, Integer numHops, String rtt, List<Node> nodes, Integer numNones, String protocol) {
            _srcIp = srcIp;
            _dstIp = dstIp;
            _numHops = numHops;
            _rtt = rtt;
            _nodes = nodes;
            _numNones = numNones;
            _proto = protocol;
        }

        public void setLastAS(String lastAS) {
			_lastAS = lastAS;
		}

        public String getSrcIp() {
            return _srcIp;
        }

        public String getDstIp() {
            return _dstIp;
        }

        public Integer getNumHops() {
            return _numHops;
        }

        public List<Node> getNodes() {
            return _nodes;
        }
        
        public Integer getNumNones() {
        	return _numNones;
        }
        
        public String getLastAS() {
        	return _lastAS;
        }
        
        public boolean isIncomplete(){
        	return _incomplete;
        }

        public String getProtocol(){
            return _proto;
        }

        public String getRtt() {
            return _rtt;
        }
        
        public List<String> getASes(){
        	return _ASes;
        }
        
        public void setIncomplete(){
        	_incomplete = true;
        }
        
        public void setASes(List<String> myASes){
        	_ASes = myASes;
        }

        public void printNodes() {
            if (_nodes != null) {
                System.out.println("... INIT: Printing Nodes ...");
                for (Node n: _nodes) {
                    System.out.println("Node : " + n.toString());
                }
                System.out.println("... END: Printing Nodes ...");
            }
        }

        public void printAses() {
            if (_ASes != null) {
                System.out.println("... INIT: Printing ASes ...");
                for (String as: _ASes) {
                    System.out.println("AS : " + as);
                }
                System.out.println("... END: Printing ASes ...");
            }
        }

        public String toString() {
            return "Src IP: " + _srcIp + " Dst: " +_dstIp + " NumHops: " + _numHops +" Time: " + _rtt;
        }
    }
}
