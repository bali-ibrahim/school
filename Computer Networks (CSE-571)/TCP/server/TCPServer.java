import java.net.*;
import java.util.concurrent.ConcurrentLinkedQueue;

import servivces.SocketConnectionService;

public class TCPServer {

  public static void main(String args[]) throws Exception {

    var welcomeSocket = new ServerSocket(8080);

    var connectionPool = new ConcurrentLinkedQueue<SocketConnectionService>();

    while (true) {

      System.out.println("Started accepting connections!");
      Socket connectionSocket = welcomeSocket.accept();
      var connection = new SocketConnectionService(connectionSocket);
      connection.start();
      connectionPool.add(connection);
      System.out.println("Accepted a connection.");
      connection.Send("Hello client!");
    }
  }
}
