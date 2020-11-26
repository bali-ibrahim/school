import java.net.*;
import java.util.*;
import java.io.*;

// Java code for thread creation by extending
// the Thread class
class SocketThread extends Thread {

  private Socket connectionSocket;
  private String clientMessage;
  public Queue<String> messageQueue;
  private DataOutputStream outToClient;

  public SocketThread(Socket c) throws IOException {
    connectionSocket = c;
    messageQueue = new LinkedList<String>();
    outToClient = new DataOutputStream(connectionSocket.getOutputStream());
  }

  public void outToClient(String message) throws IOException {
    outToClient.writeBytes(message);
  }

  public void run() {
    try {
      while (true) {
        // Displaying the thread that is running
        System.out.println(
          "Thread " + Thread.currentThread().getId() + " is running"
        );

        System.out.println(
          "Incoming from: " + connectionSocket.getInetAddress().getHostAddress()
        );
        BufferedReader inFromClient = new BufferedReader(
          new InputStreamReader(connectionSocket.getInputStream())
        );
        clientMessage = inFromClient.readLine();
        messageQueue.add(clientMessage);
        System.out.println("Received: " + clientMessage);
        String capitalizedSentence = clientMessage.toUpperCase() + '\n';
        outToClient(capitalizedSentence);
      }
    } catch (Exception e) {
      // Throwing an exception
      System.out.println("Exception is caught");
    }
  }
}

// // Main Class
// public class Multithread {

//   public static void main(String[] args) {
//     int n = 8; // Number of threads
//     for (int i = 0; i < n; i++) {
//       SocketThread object = new MultithreadingDemo();
//       object.start();
//     }
//   }
// }
