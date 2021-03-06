package server;

import java.io.*;
import java.net.*;
import java.util.HashSet;

import services.NotReallyWellKnownPort;
import services.MimeHeader;
import services.http.HTTPService;

public class Proxy {

	public static void main(String args[]) throws Exception {

		final var welcomeSocket = new ServerSocket(NotReallyWellKnownPort.AlternateWebServer);

		while (true) {
			final var connectionSocket = welcomeSocket.accept();
			new Thread(new ServerHandler(connectionSocket)).start();
		}

	}

}

class ServerHandler implements Runnable {

	Socket clientSocket;
	DataInputStream inFromClient;
	DataOutputStream outToClient;

	String host;

	String address;

	String hd;

	public ServerHandler(Socket s) {
		clientSocket = s;
		try {
			inFromClient = new DataInputStream(s.getInputStream());
			outToClient = new DataOutputStream(s.getOutputStream());
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private void dispose() throws IOException {
		inFromClient.close();
		outToClient.close();
	}

	@Override
	public void run() {

		try {
			hd = getHeader(inFromClient);

			int sp1 = hd.indexOf(' ');
			int sp2 = hd.indexOf(' ', sp1 + 1);
			int eol = hd.indexOf('\r');

			address = hd.substring(sp1 + 1, sp2);

			String pairs = hd.substring(eol + 2);
			MimeHeader inMH = new MimeHeader(pairs);

			host = inMH.get("Host");

			System.out.println("Requested a connection to " + host);
			{
				// TODO: move to somewhere modifiable from outside
				final var prohibitedHostNames = new HashSet<String>();
				prohibitedHostNames.add("yandex.com.tr");
				prohibitedHostNames.add("yandex.com");
				prohibitedHostNames.add("youtube.com");
				if (prohibitedHostNames.contains(host)) {
					final var status = "Connection blocked to the host due to the proxy policy.";
					System.out.println(status);
					final var response = HTTPService.toHTMLResponse(
							"Error when fetching URL: " + host + HTTPService.CRLF, HTTPService.forbiddenStatus);
					outToClient.writeBytes(response);
					dispose();
					return;
				}
			}
			{
				final var method = HTTPService.getMethod(hd);
				if (!method.equals("GET")) {
					final var status = "Requested method " + method + " is not allowed on proxy server.";
					System.out.println(status);
					final var response = HTTPService.toHTMLResponse("", HTTPService.methodNotAllowedStatus);
					outToClient.writeBytes(response);
					dispose();
					return;
				}
			}

			System.out.println("Initiating the server connection");

			// no cache
			inMH.put("Connection", "close");
			handleProxy(address, inMH, outToClient);
			dispose();

		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	public void handleProxy(final String address, final MimeHeader inMH, final DataOutputStream outToClient)
			throws Exception {

		// final var url = new URL("http://" + host + address);
		final var url = new URL(address);
		final var hostName = url.getHost();
		final var path = url.getPath().isEmpty() ? "/" : url.getPath();

		final var sSocket = new Socket(hostName, 80);
		final var inFromServer = new DataInputStream(sSocket.getInputStream());
		final var outToServer = new DataOutputStream(sSocket.getOutputStream());

		inMH.put("User-Agent", inMH.get("User-Agent") + " via CSE571 Proxy");

		String request = "GET " + path + " HTTP/1.1\r\n" + inMH + "\r\n";

		System.out.println(request);

		outToServer.writeBytes(request);

		System.out.println("HTTP request sent to: " + host + "\nWaiting response...");

		final var responseHeader = getHeader(inFromServer);
		System.out.println("Response header:\n" + responseHeader);
		outToClient.writeBytes(responseHeader);

		final var meta = HTTPService.parseResponse(responseHeader);

		if (meta.get("StatusCode").equals("200")) {
			final var eol = responseHeader.indexOf('\r');
			final var pairs = responseHeader.substring(eol + 2);
			final var responseMH = new MimeHeader(pairs);
			final var length = Integer.parseInt(responseMH.get("Content-Length"));
			final var dataArr = new byte[length];
			inFromServer.readFully(dataArr);
			System.out.println("Data arrived.");
			System.out.println(length + " bytes");
			outToClient.write(dataArr);
			System.out.println("Sent data back to client. Proxy for " + host + " is done.");
		}

		sSocket.close();

	}

	public String getHeader(DataInputStream in) throws Exception {

		final var header = new byte[1024];

		int data;
		int h = 0;

		while ((data = in.read()) != -1) {
			header[h++] = (byte) data;

			if (header[h - 1] == '\n' && header[h - 2] == '\r' && header[h - 3] == '\n' && header[h - 4] == '\r') {
				break;
			}
		}
		return new String(header, 0, h);
	}
}