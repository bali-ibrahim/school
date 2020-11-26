import java.net.*;

public class UDPServer {
	
	public static void main(String args[]) throws Exception {
		
		DatagramSocket serverSocket = new DatagramSocket(9876);
		System.out.println("Server");
		
		byte[] receiveData = new byte[100];

		for(int i =0; i<receiveData.length; i++) {
			receiveData[i] = '|';
		}
		byte[] sendData = null;
		
		while (true) {
			
			DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
			serverSocket.receive(receivePacket);
			String sentence = new String(receivePacket.getData(), receivePacket.getOffset(), receivePacket.getLength());
			System.out.println("RECEIVED: " + sentence);
			InetAddress IPAddress = receivePacket.getAddress();
			int port = receivePacket.getPort();
			String capitalizedSentence = sentence.toUpperCase();
			sendData = capitalizedSentence.getBytes();
			DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, port);
			serverSocket.send(sendPacket);
			
		}
		
	}
	
}