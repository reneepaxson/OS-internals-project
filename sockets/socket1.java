// Here we are sending 100 randomly generated integers as byte arrays 
// to another process on our local address (LocalHost = 127.0.0.1)
// and the specified port to which the receiving process is bound 
// (I chose port 1234, but it doesn't matter so long as the port number is 
//  outside of the reserved range (>1023) and it is not being used
//  by any other receiving processes)

import java.net.*;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.io.*;
import java.util.Random;

public class socket1 {

    public static void main(String args[]){
        
        // declare a socket 
        DatagramSocket mySocket = null;
        
        // create a new random number generator
        Random rng = new Random();
        
        try {

            // construct the sending socket 
            mySocket = new DatagramSocket();

            // localHost = 127.0.0.1
            InetAddress localHost = InetAddress.getLocalHost();
            // random port number
            int serverPort = 1234;
            // variable to store the random number on
            int data;
            // random number generator seed to make the output truly random
            //int seed = (int) System.currentTimeMillis();

            // loop until 100 messages have been sent
            for (int i = 0; i < 100; i++){
                // generate the random number
                data = rng.nextInt();
                // convert integer to byte array
                // all ints have a maximum length of 4 bytes, so our byte array capacity is 4
                byte [] message = ByteBuffer.allocate(4).putInt(data).array();

                // create datagramPacket 
                DatagramPacket datagramMessage = new DatagramPacket(message, message.length, localHost, serverPort);
                mySocket.send(datagramMessage);

                
                // I want to save each generated value as an int to a text file
                try {

                    // define filepath to save the outgoing data on
                    String pathname = "./socketInputData.txt";
                    // create a File instance using the path name
                    File file = new File(pathname);
                    
                    // create the file writer with parameter "true" to allow it to append
                    FileWriter filewriter = new FileWriter(pathname, true);

                    // if file doesn't exist, create it
                    if (file.createNewFile()){}
                
                    // if file exists but this is the start of the loop (i=0) 
                    else if (i==0){
                        // delete the file
                        Files.deleteIfExists(file.toPath());
                        // and recreate it
                        file.createNewFile();
                        // redefine filewriter
                        filewriter = new FileWriter(pathname, true);
                    }
                    
                    filewriter.write(data + "\n");
                    filewriter.close();

                } catch (IOException e){
                    System.out.println("an error occurred");
                    e.printStackTrace();
                }
            }
        } catch(SocketException e){System.out.println("Socket: "+ e.getMessage());
        } catch (IOException e){System.out.println("IO: " + e.getMessage());
        } finally { if (mySocket != null) mySocket.close();}
    }    
}