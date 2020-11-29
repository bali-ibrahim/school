package services.http;

import java.io.*;
import java.net.*;
import java.util.HashMap;
import java.util.regex.Pattern;
import java.util.stream.Stream;

import services.NotReallyWellKnownPort;

final public class HTTPService {

    final public static void main(String args[]) throws Exception {

        final var inFromUser = new BufferedReader(new InputStreamReader(System.in));
        System.out.print("Please Enter the URL address of the web page: ");
        var address = inFromUser.readLine();

        if (!address.startsWith("http://")) {
            address = "http://" + address;
        }

        final var url = new URL(address);
        final var clientSocket = new Socket("172.22.0.2", NotReallyWellKnownPort.AlternateWebServer);
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

    final public static HashMap<String, String> parseResponse(String responseMesssage) {
        /*
         * HTTP/1.1 404 Not Found
         * 
         */

        final var pattern = Pattern.compile("HTTP\\/(\\S*?) (\\S*?) (.*?)(?:\\n|\\r)+", Pattern.MULTILINE);
        final var matches = pattern.matcher(responseMesssage);
        final var responseMeta = new HashMap<String, String>();
        if (matches.find()) {
            responseMeta.put("Version", matches.group(1));
            responseMeta.put("StatusCode", matches.group(2));
            responseMeta.put("ReasonePhrase", matches.group(3));
        }

        return responseMeta;
    }

    final public static String getMethod(String request) {
        // https://tools.ietf.org/html/rfc2616#section-5.1
        final var firstSPIndex = request.indexOf(' ');
        if (firstSPIndex == -1)
            return null;
        final var method = request.substring(0, firstSPIndex);
        return method;
    }
}