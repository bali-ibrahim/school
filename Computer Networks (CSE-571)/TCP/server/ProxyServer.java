package server;

import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.ConcurrentLinkedQueue;
import services.SocketConnectionService;
import services.NotReallyWellKnownPort;

final public class ProxyServer {

  final public static void main(String args[]) throws Exception {
    final ServerSocket welcomeSocket = new ServerSocket(NotReallyWellKnownPort.Chatting);

    final var connectionPool = new ConcurrentLinkedQueue<SocketConnectionService>();

    while (true) {
      System.out.println("Started accepting connections!");
      final var connectionSocket = welcomeSocket.accept();
      final var connection = new SocketConnectionService(connectionSocket);
      // final var relay = new RelayService(connectionSocket, )
      connection.start();
      connectionPool.add(connection);
      System.out.println("Accepted a connection.");
      connection.Send("Hello client!");
    }
  }
}
