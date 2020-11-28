package services.http;

import java.io.*;
import java.net.*;
import java.util.stream.Stream;

final public class HTTPService {

    final public static void main(String args[]) throws Exception {

        final var inFromUser = new BufferedReader(new InputStreamReader(System.in));
        System.out.print("Please Enter the URL address of the web page: ");
        var address = inFromUser.readLine();

        if (!address.startsWith("http://")) {
            address = "http://" + address;
        }

        final var url = new URL(address);
        final var clientSocket = new Socket("172.22.0.3", 6789);
        final var response = get(url, clientSocket);
        response.forEach(System.out::println);
        clientSocket.close();

    }

    final private static String toRequest(final URL url, final RequestType requestType) {
        final var hostName = url.getHost();
        final var path = url.getPath().isEmpty() ? "/" : url.getPath();

        final var CRLF = "\r\n";
        final var request = "" + requestType.name().toUpperCase() + " " + path + " HTTP/1.1" + CRLF + "Host: "
                + hostName + CRLF + "Connection: close" + CRLF + CRLF;
        return request;
    }

    final public static Stream<String> get(final URL url) throws IOException {
        final var hostName = url.getHost();
        final var defaultHTTPPort = 80;
        final var socket = new Socket(hostName, defaultHTTPPort);
        final var response = get(url, socket);
        socket.close();
        return response;
    }

    final private static Stream<String> get(final URL url, final Socket socket) throws IOException {
        System.out.println("Connection established successfully!");
        final var request = toRequest(url, RequestType.get);
        System.out.println("----Request----");
        System.out.println(request);
        final var outStream = new DataOutputStream(socket.getOutputStream());
        outStream.writeBytes(request);
        final var hostName = url.getHost();
        System.out.println("HTTP request sent to: " + hostName);
        final var inStream = (new BufferedReader(new InputStreamReader(socket.getInputStream()))).lines();
        return inStream;
    }
}