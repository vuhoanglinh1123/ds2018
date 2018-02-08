
#Instruction the same as Labwork 4

## IDE
We use Netbeans IDE to build the project because Netbeans is a powerful java IDE developed by oracle

## Build
Create a java project in Netbeans, import the WordCount.java in and set it as the main class in the project's Properties/Run.
In Properties/Libraries, add these JAR files in hadoop folder:
	
+ .../hadoop/share/hadoop/common/hadoop-common-3.0.0.jar (3.0.0 is the version of hadoop)
	
+ .../hadoop/share/hadoop/mapreduce/hadoop-mapreduce-client-common-3.0.0.jar
	
+ .../hadoop/share/hadoop/mapreduce/hadoop-mapreduce-client-core-3.0.0.jar
	
+ .../hadoop/share/hadoop/mapreduce/hadoop-mapreduce-client-jobclient-3.0.0.jar
Build the project then get the .jar file in "project folder"/dist

## Command line
Input folder contains the files which we want to perform WordCount on
Output folder stores the result
*"hadoop"* command is the path to .../hadoop/bin/hadoop

use alias to make shortcut
In our case:

	alias hadoop='/usr/local/hadoop/bin/hadoop'

Run the project: general case

	hadoop jar *".jar file name"* *"input folder name"* *"output folder name"*
In our case:

	hadoop jar MapReduceTest1.jar Input Output

Use: 

	cat Output/*

To quickly view the result

**!Remember output folder cannot be exist before run the project to work** 

