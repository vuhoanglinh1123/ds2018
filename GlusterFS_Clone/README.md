#Compile instructions

##compile server peer packet
    
    javac serverpeer/ServerPeer.java
##compile client watch service packet
    
    javac clientwatchservice/*

##run server first
    
    java serverpeer/ServerPeer <brick directory path>
##run client
    
    java clientwatchservice/ClientWatchService <IP address of server> <mount directory path>
