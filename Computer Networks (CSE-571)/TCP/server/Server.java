package server;

import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.ConcurrentLinkedQueue;
import services.SocketConnectionService;

public class Server {

  public static void main(String args[]) throws Exception {
    var welcomeSocket = new ServerSocket(6789);

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
