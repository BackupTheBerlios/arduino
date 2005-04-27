/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  PdeDownloader - default downloader class that connects to uisp
  Part of the Wiring project - http://wiring.uniandes.edu.co

  Copyright (c) 2005
  Hernando Barragan, Interaction Design Institute Ivrea

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

import java.io.*;
import java.util.*;
import java.util.zip.*;
import javax.swing.*;
//#ifndef RXTX
//import javax.comm.*;
//#else
// rxtx uses package gnu.io, but all the class names
// are the same as those used by javax.comm
import gnu.io.*;
//#endif


public class PdeDownloader implements PdeMessageConsumer {
  static final String BUGS_URL =
    "http://wiring.uniandes.edu.co/discourse/";
  static final String SUPER_BADNESS =
    "Compiler error, please submit this code to " + BUGS_URL;

  String buildPath;
  String className;
  File includeFolder;
  PdeException exception;
  PdeSketch sketch;
  //PdePreferences preferences;

  static SerialPort serialPort;
  static InputStream serialInput;
  static OutputStream serialOutput;
  //int serial; // last byte of data received

  private String serial_port = "COM1";
  private int serial_rate = 9600;
  private char serial_parity = 'N';
  private int serial_databits = 8;
  private float serial_stopbits = 1;

  public void serialPreferences() {
    //System.out.println("setting serial properties");
    serial_port = PdePreferences.get("serial.port");
    serial_rate = PdePreferences.getInteger("serial.download_rate");
    serial_parity = PdePreferences.get("serial.parity").charAt(0);
    serial_databits = PdePreferences.getInteger("serial.databits");
    serial_stopbits = new Float(PdePreferences.get("serial.stopbits")).floatValue();
  }

  public PdeDownloader(String buildPath, String className,
                      PdeSketch sketch) {
    this.buildPath = buildPath;
    this.includeFolder = includeFolder;
    this.className = className;
    this.sketch = sketch;
  }

  private boolean wiringliteDownload(String userdir) {
    System.out.println("Downloading With Wiring Lite");
    Process process;
    int result = 0;
    try {
      serialPreferences();
      String command = userdir + "lib/wiringlite/bin/gnumake SERIAL=" + serial_port + " -s -C " + userdir + "lib/wiringlite program";
      System.out.println(command);
      process = Runtime.getRuntime().exec(command);
      new PdeMessageSiphon(process.getInputStream(), this);
      new PdeMessageSiphon(process.getErrorStream(), this);
      boolean compiling = true;
      while (compiling) {
        try {
          result = process.waitFor();
          compiling = false;
        } catch (InterruptedException ignored) { }
      }
    } catch (Exception e) {
      e.printStackTrace();
      System.out.println("Error: GNUMake probably couldn't be found");
      result = 99;
    }
    if(0 == result){
      System.out.println("Wiring Lite Download Successful");
    }else{
      System.out.println("Wiring Lite Download Unsuccessful (error: " + result + ")");
    }
    return (result == 0);
  }

  public boolean downloadJava(PrintStream leechErr) {
    String userdir = System.getProperty("user.dir") + File.separator;
    
    // if lib/wiringlite exists, then use it instead
    File newDir = new File(userdir + "lib/wiringlite");
    if(newDir.exists()){
      return wiringliteDownload(userdir);
    }
    
    String commandDownloader[] = new String[] {
    ((PdeBase.platform != PdeBase.MACOSX) ?  "tools/avr/bin/uisp" :
      userdir + "tools/avr/bin/uisp"),
    //[2] Serial port
    //[6] hex class file
     "-dprog=stk500",
     " ",
     "-dspeed=115200",
     "-dpart=atmega128",
     "--upload",
     " "
    };

    firstErrorFound = false;  // haven't found any errors yet
    secondErrorFound = false;

    int result=0; // pre-initialized to quiet a bogus warning from gcc
    try {

      serialPreferences();
      commandDownloader[2] = ((PdeBase.platform != PdeBase.MACOSX) ? "-dserial=/dev/" + serial_port.toLowerCase() : "-dserial=" + serial_port );
      commandDownloader[6] = "if=" + buildPath + File.separator + className + ".hex";
      /*for(int i = 0; i < commandDownloader.length; i++) {
	System.out.println(commandDownloader[i]);
      }*/
      Process process = Runtime.getRuntime().exec(commandDownloader);
      new PdeMessageSiphon(process.getInputStream(), this);
      new PdeMessageSiphon(process.getErrorStream(), this);

      // wait for the process to finish.  if interrupted
      // before waitFor returns, continue waiting
      //
      boolean compiling = true;
      while (compiling) {
        try {
          result = process.waitFor();
          compiling = false;
        } catch (InterruptedException intExc) {
        }
      }    
    } catch (Exception e) {
      String msg = e.getMessage();
      if ((msg != null) && (msg.indexOf("uisp: not found") != -1)) {
        //System.err.println("gcc is missing");
        //JOptionPane.showMessageDialog(editor.base,
        //                              "Could not find the downloader.\n" +
        //                              "uisp is missing from your PATH,\n" +
        //                              "see readme.txt for help.",
        //                              "Downloade error",
        //                              JOptionPane.ERROR_MESSAGE);
        return false;
      }
      e.printStackTrace();
      result = -1;
    }

    // if the result isn't a known, expected value it means that something
    // is fairly wrong, one possibility is that gcc has crashed.
    //
    if (result != 0 && result != 1 ) {
      exception = new PdeException(SUPER_BADNESS);
      //editor.error(exception);
      //PdeBase.openURL(BUGS_URL);
    }

    return (result == 0) ? true : false;      

  }

  boolean firstErrorFound;
  boolean secondErrorFound;

  // part of the PdeMessageConsumer interface
  //
  public void message(String s) {
    //System.err.println("MSG: " + s);
    //System.err.print(s);

    // ignore cautions
    if (s.indexOf("Caution") != -1) return;

    // gcc always uses a forward slash character as its separator, so
    // we need to replace any platform-specific separator characters before
    // attemping to compare
    //
    String partialTempPath = buildPath.replace(File.separatorChar, '/')
      + "/" + className + ".c";

    // if the partial temp path appears in the error message...
    //
    int partialStartIndex = s.indexOf(partialTempPath);
    //System.out.println(partialStartIndex);
    if (partialStartIndex != -1) {

      // skip past the path and parse the int after the first colon
      //
      String s1 = s.substring(partialStartIndex + partialTempPath.length()
                              + 1);
      int colon = s1.indexOf(':');
      int lineNumber = Integer.parseInt(s1.substring(0, colon));
      //System.out.println("pde / line number: " + lineNumber);

      //String s2 = s1.substring(colon + 2);
      int err = s1.indexOf("Error:");
      if (err != -1) {

        // if the first error has already been found, then this must be
        // (at least) the second error found
        if (firstErrorFound) {
          secondErrorFound = true;
          return;
        }

        // if we're here at all, this is at least the first error
        firstErrorFound = true;

        //err += "error:".length();
        String description = s1.substring(err + "Error:".length());
        description = description.trim();
        //System.out.println("description = " + description);
        exception = new PdeException(description, lineNumber-1);
        //editor.error(exception);

      } else {
        System.err.println("i suck: " + s);
      }

    } else {

      // this isn't the start of an error line, so don't attempt to parse
      // a line number out of it.

      // if we're not yet at the second error, these lines are probably
      // associated with the first error message, which is already in the
      // status bar, and are likely to be of interest to the user, so
      // spit them to the console.
      //
      if (!secondErrorFound) {
        System.err.println(s);
      }
    }
  }


}

