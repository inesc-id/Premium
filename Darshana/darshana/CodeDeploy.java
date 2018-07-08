import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.FileReader;
import java.io.IOException;
 
public class CodeDeploy {
 
	public static void main(String[] args) throws IOException{
 
		String fileOrDirectoryToSend = args[0];
		String nodesFile = args[1];
		String user = args[2];
		String keyPairFile = args[3];

		if (args.length < 4) {
			System.out.println("-> Expected arguments: <file-to-send> <nodes-file> <username-of-host> <key-pair-file>");
			System.out.println("-> Example: TagusTrace testNodes.txt ubuntu darshana-hijacker.pem");
			return;
		}

		// To be read from nodesFile.txt
		String host;

		BufferedReader br = new BufferedReader(new FileReader(nodesFile));

		while ((host = br.readLine()) != null) {
   			String command = "scp -r -i " + keyPairFile + " " + fileOrDirectoryToSend + " " + user + "@" + host + ":/home/" + user;
			System.out.println(command);

			CodeDeploy obj = new CodeDeploy(); 
			
			String output = obj.executeCommand(command);
			System.out.println(output);
   		}
		br.close();
	}
 
	private String executeCommand(String command) {
 
		Process p;
		StringBuffer output = new StringBuffer();

		try {
			p = Runtime.getRuntime().exec(command);
			p.waitFor();
			BufferedReader reader = new BufferedReader(new InputStreamReader(p.getInputStream()));
 
			String line = "";
			while ((line = reader.readLine())!= null) {
				output.append(line + "\n");
			}
 
		} catch (Exception e) {
			e.printStackTrace();
		}
 
		return output.toString();
	}
 
}
