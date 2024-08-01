import java.net.*;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.io.*;

public class socket2 {
    public static void main (String args[]){
        
        // declare socket
        DatagramSocket mySocket = null;

        // define the filepath to save the received data on
        String pathname = "./socketOutputData.txt";
        // create a File instance using the path name
        File file = new File(pathname);

        try {
            // before we enter the loop, determine if the received data
            // file exists already and delete if it does
            Files.deleteIfExists(file.toPath());

            // construct the receiving socket and bind it to port 1234
            mySocket = new DatagramSocket(1234);
            
            // large byte array to receive a potentially large message 
            byte[] message = new byte[1000];
            
            int i = 0;
            
            while(true){
                
                // create empty datagramPacket to receive the message into
                DatagramPacket request = new DatagramPacket(message, message.length);
                
                mySocket.receive(request);

                // convert message from byte array to int 
                ByteBuffer wrapped = ByteBuffer.wrap(message);
                int data = wrapped.getInt();

                // the "write to file" section
                try {
                    //create filewriter with parameter "true" to allow append
                    FileWriter filewriter = new FileWriter(pathname, true);

                    // if file doesn't exist, create it
                    if (file.createNewFile()){}

                    filewriter.write(data + "\n");
                    filewriter.close();

                } catch (IOException e){
                    System.out.println("an error occurred");
                    e.printStackTrace();
                }
                i++;
                if (i > 99){ break;}
            }
        } catch (SocketException e) { System.out.println("Socket: " + e.getMessage());
        } catch (IOException e){ System.out.println("IO: " + e.getMessage());
        } finally { if (mySocket != null) mySocket.close();} 
    }
}  
