package servivces;

import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

public class SocketConnectionService extends Thread {
    private Socket socket;
    private Queue<String> messageQueue;

    public SocketConnectionService(Socket c) {
        socket = c;
        messageQueue = new ConcurrentLinkedQueue<String>();
    }

    public void run() {
        try {
            var reader = new ReadStreamService(socket);
            reader.start();
            while (true) {
                var message = messageQueue.poll();
                if (message.isEmpty())
                    continue;
                DataOutputStream outToServer = new DataOutputStream(socket.getOutputStream());
                outToServer.writeBytes(message);
            }
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    public void Send(String message) {
        messageQueue.add(message);
    }
}
