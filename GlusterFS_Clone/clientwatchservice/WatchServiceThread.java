/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package clientwatchservice;

import java.io.IOException;
import java.nio.file.FileSystems;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardWatchEventKinds;
import java.nio.file.WatchEvent;
import java.nio.file.WatchKey;
import java.nio.file.WatchService;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author dangvinhbao
 */
public class WatchServiceThread extends Thread{
    public Integer modifyFile = 1;
    public Integer deleteFile = 2;
    public String mountPath = "";
    
    private List<String> filenames = new ArrayList<String>();
    private List<Integer> changeType = new ArrayList<Integer>();
    
    public void run(){
        //SET UP WatchService
        WatchService watchService;
        Path dir = null;
        WatchKey watchKey = null;
        try {
            //watch service
            watchService = FileSystems.getDefault().newWatchService();
            //path
            dir = Paths.get(mountPath);
            //register
            watchKey = dir.register(watchService, StandardWatchEventKinds.ENTRY_CREATE,  
                    StandardWatchEventKinds.ENTRY_MODIFY,    
                    StandardWatchEventKinds.ENTRY_DELETE);
        } catch (IOException ex) {
            Logger.getLogger(WatchServiceThread.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        //infinite loop
        while(true) {
            for(WatchEvent<?> event : watchKey.pollEvents()){
                //kind
                WatchEvent.Kind<?> kind = event.kind();
                //overflow case
                if (kind == StandardWatchEventKinds.OVERFLOW){
                    continue;
                }
                //context of the event
                WatchEvent<Path> ev = (WatchEvent<Path>)event;
                Path filename = ev.context();
                //check file name to know if it is temporary file
                String filenameStr = filename.toString();
                if ( filenameStr.startsWith(".") ){
                    continue;
                }
                //get absolute path
                String filenameAbs = mountPath + "/" + filenameStr;
                //delete => send delete signal to delete
                //create or modify => send all the file content and write over
                if (kind == StandardWatchEventKinds.ENTRY_DELETE){
                    System.out.println("Send delete signal for: " + filenameAbs);
                    
                    addChangeLog(filenameStr, deleteFile);
                }
                else if (kind == StandardWatchEventKinds.ENTRY_MODIFY ||
                        kind == StandardWatchEventKinds.ENTRY_CREATE){
                    System.out.println("Send all content of: " + filenameAbs);
                    
                    addChangeLog(filenameStr, modifyFile);
                    //File file = new File(filenameAbs);
                    //long fileLength = file.length();
                }
            }
        }
    }
    public void setMountPath(String path){
        mountPath = path;
    }
    
    public List<String> getChangedFile(){
        return filenames;
    }
    public List<Integer> getChangeType(){
        return changeType;
    }
    public void emptyLog(){
        this.filenames.clear();
        this.changeType.clear();
    }
    
    private void addChangeLog(String filename, Integer type){
        String element;
        int length = filenames.size();
        
        for(int i = 0; i < length; i++){
            element = filenames.get(i);
            if (Objects.equals(element, filename)){
                changeType.set(i, type);
                return;
            }
        }
        //filename appear for the first time
        filenames.add(filename);
        changeType.add(type);
    }

}
