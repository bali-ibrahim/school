package server;

import java.net.*;
import java.util.Date;
import java.util.Random;

public class UDP {
	public static void main(String args[]) {
		byte[] buf = new byte[1024];
		try {
			DatagramSocket ds = new DatagramSocket(NotReallyWellKnownPorts.UDPDefault, InetAddress.getByName("127.0.0.1"));
			while (true) {
				DatagramPacket rPacket = new DatagramPacket(buf, buf.length);
				ds.receive(rPacket);
				String str = new String(rPacket.getData(), rPacket.getOffset(), rPacket.getLength());
				System.out.println("Frame " + str + " received");
				byte[] sbuf = String.valueOf(str).getBytes();
				DatagramPacket sPacket = new DatagramPacket(sbuf, sbuf.length, rPacket.getAddress(), rPacket.getPort());
				ds.send(sPacket);
				System.out.println("ACK sent for " + str);
			}
		} catch (Exception ex) {
			System.out.println("Error" + ex);
		}
	}
}