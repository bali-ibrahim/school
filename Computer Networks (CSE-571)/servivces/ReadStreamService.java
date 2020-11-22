package servivces;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;

public class ReadStreamService extends Thread {

    private Socket socket;

    public ReadStreamService(Socket c) {
        socket = c;
    }

    public void run() {
        try {
            var inFromServer = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            while (true) {
                System.out.println(inFromServer.readLine());
            }
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
}
