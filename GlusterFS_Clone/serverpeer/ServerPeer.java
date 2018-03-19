/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package serverpeer;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author dangvinhbao
 */
public class ServerPeer {
        
    public static void main(String[] args) throws Exception {
        
        //brick path
        String brickPath = args[0];
        //String brickPath = "/home/dangvinhbao/Desktop/GlusterFS_clone/brick1";
        
        //connect socket
        System.out.println("Waiting for client");
        ServerSocket server = new ServerSocket(4333);
        Socket client = server.accept();
        System.out.println("Connected");
        
        sendAllFilesFrom(brickPath, client);
        
        //listen to change
        while(true){
            updateFileFromClient(brickPath, client);
        }
        
    }
    
    private static void sendAllFilesFrom(String brickDir, Socket sk){
        String filename = "";
        long fileLength;       
         
        DataOutputStream output;
        OutputStream outputStream;
        FileInputStream fileReader;
        byte buffer[];
        
        File dir = new File(brickDir);
        File allFiles[];
        allFiles = dir.listFiles();
        for (File allFile : allFiles) {
            try {
                
                outputStream = sk.getOutputStream();
                output = new DataOutputStream(outputStream);
                //send signal
                output.writeBoolean(true);
                //filename
                filename = allFile.getName();
                output.writeUTF(filename);
                //file length
                fileLength = allFile.length();
                output.writeLong(fileLength);
                //file data
                fileReader = new FileInputStream(allFile);
                buffer = new byte[(int)fileLength];
                fileReader.read(buffer, 0, buffer.length);
                outputStream.write(buffer);

            } catch (IOException ex) {
                Logger.getLogger(ServerPeer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        try {
            //end signal
            outputStream = sk.getOutputStream();
            output = new DataOutputStream(outputStream);
            //send signal
            output.writeBoolean(false);
        } catch (IOException ex) {
            Logger.getLogger(ServerPeer.class.getName()).log(Level.SEVERE, null, ex);
        }
        
    }
    
    private static void updateFileFromClient(String brickDir, Socket socket) throws IOException{
        //get volume
        boolean contSignal;
        String filename = "";
        String absPath = "";
        long fileLength;
        //
        DataInputStream input;
        InputStream inputStream;
        FileOutputStream fileWriter;
        byte buffer[];
        //delete file
        File deleteFile = null;
        
        while(true){
            try {
                inputStream = socket.getInputStream();
                input = new DataInputStream(inputStream);
                contSignal= input.readBoolean();
                if(contSignal){
                    //get filename
                    filename = input.readUTF();
                    absPath = brickDir +"/"+ filename;
                    //get file length
                    fileLength = input.readLong();
                    
                    if (fileLength >= 0){
                        //write to file
                        fileWriter = new FileOutputStream(absPath);
                        //buffer
                        buffer = new byte[(int) fileLength];
                        inputStream.read(buffer, 0, buffer.length);
                        fileWriter.write(buffer, 0, buffer.length);
                    }else{
                        //fileLength < 0 => delete
                        deleteFile = new File(absPath);
                        deleteFile.delete();
                    }
                }else{
                    System.out.println("Finish update volume");
                    break;
                }
            } catch (IOException ex) {
                Logger.getLogger(ServerPeer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }    
    }
}
