
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package clientwatchservice;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.io.OutputStream;
import java.util.List;

import clientwatchservice.*;


/**
 *
 * @author dangvinhbao
 */
public class ClientWatchService {
    static Socket socket;
    static String mountPath = "";
    public static Integer modifyFile = 1;
    public static Integer deleteFile = 2;
    //MAIN
    
    public static void main(String[] args) throws Exception{
        
        String serverID = args[0];
        //mount
        mountPath = args[1];
        //mountPath = "/home/dangvinhbao/Desktop/GlusterFS_clone/mount1";
        //set up Socket
        socket = new Socket(serverID, 4333);
        
        System.out.println("Connected");
                
        //get volume from server
        purgeDirectory(new File(mountPath));
        updateVolume(mountPath, socket);
        
        //create listening thread
        WatchServiceThread listenThread = new WatchServiceThread();
        listenThread.setMountPath(mountPath);
        listenThread.start();
        
        while(true){
            System.out.println("Enter update to send change to server");
            BufferedReader bufferRead = new BufferedReader(new InputStreamReader(System.in));
            String userInput = bufferRead.readLine();

            List<String> filenames = listenThread.getChangedFile();
            List<Integer> changeType = listenThread.getChangeType();
            
            updateAll(filenames, changeType);
            listenThread.emptyLog();
        }
    }
    //SEND ALL FILE UPDATE
    private static void updateAll(List<String> filenames, List<Integer> changeType){
        int length = filenames.size();
        String filename;
        
        File file;
        String absPath;
        long fileLength;
        
        DataOutputStream output;
        OutputStream outputStream;
        FileInputStream fileReader;
        byte buffer[];
        
        for(int i = 0; i < length; i++){
            try {
                outputStream = socket.getOutputStream();
                output = new DataOutputStream(outputStream);
                //get filename
                filename = filenames.get(i);
                //send signal
                output.writeBoolean(true);
                //send file name
                output.writeUTF(filename);
                if(changeType.get(i) == modifyFile){
                    absPath = mountPath + "/" + filename;                
                    file = new File(absPath);
                    fileLength = file.length();
                    //send file length
                    output.writeLong(fileLength);
                    //file data
                    fileReader = new FileInputStream(file);
                    buffer = new byte[(int)fileLength];
                    fileReader.read(buffer, 0, buffer.length);
                    outputStream.write(buffer);
                }else if (changeType.get(i) == deleteFile){   
                    //send fileLength == -1 for delete
                    output.writeLong(-1);
                }
            } catch (IOException ex) {
                Logger.getLogger(ClientWatchService.class.getName()).log(Level.SEVERE, null, ex);
            }
        }try {
            //end signal
            outputStream = socket.getOutputStream();
            output = new DataOutputStream(outputStream);
            //send signal
            output.writeBoolean(false);
        } catch (IOException ex) {
            Logger.getLogger(ClientWatchService.class.getName()).log(Level.SEVERE, null, ex);
        }
        System.out.println("Finish update");
    }
    
    //GET VOLUME FILES
    private static void updateVolume(String mountDir, Socket socket){
        //get volume
        boolean contSignal;
        String filename = "";
        long fileLength;
        //
        DataInputStream input;
        InputStream inputStream;
        FileOutputStream fileWriter;
        byte buffer[];
        //
        
        while(true){
            try {
                inputStream = socket.getInputStream();
                input = new DataInputStream(inputStream);
                contSignal= input.readBoolean();
                if(contSignal){
                    //get filename
                    filename = input.readUTF();
                    //get file length
                    fileLength = input.readLong();
                    //write to file
                    fileWriter = new FileOutputStream(mountDir +"/"+ filename);
                    //buffer
                    buffer = new byte[(int) fileLength];
                    inputStream.read(buffer, 0, buffer.length);
                    fileWriter.write(buffer, 0, buffer.length);
                }else{
                    System.out.println("Finish get volume");
                    break;
                }
            } catch (IOException ex) {
                Logger.getLogger(ClientWatchService.class.getName()).log(Level.SEVERE, null, ex);
            }
        }      
    }
    private static void purgeDirectory(File dir) {
    for (File file: dir.listFiles()) {
        file.delete();
    }
}
}
