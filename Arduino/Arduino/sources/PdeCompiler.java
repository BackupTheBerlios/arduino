/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  PdeCompiler - default compiler class that connects to gcc
  Part of the Wiring project 

  Copyright (c) 2005 
  Hernando Barragan, Interaction Design institute Ivrea

  Processing PdeCompiler Copyright (c) 2001-03
  Ben Fry, Massachusetts Institute of Technology and
  Casey Reas, Interaction Design Institute Ivrea

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

public class PdeCompiler implements PdeMessageConsumer {
  static final String BUGS_URL =
    "http://processing.org/bugs/";
  static final String SUPER_BADNESS =
    "Compiler error, please submit this code to " + BUGS_URL;

  PdeSketch sketch;
  String buildPath;

  //String buildPath;
  //String className;
  //File includeFolder;
  PdeException exception;
  //PdeEditor editor;


  public PdeCompiler() { }  // consider this a warning, you werkin soon.
  
  private boolean wiringliteCompile(String userdir) {
    System.out.println("Compiling With Wiring Lite");
    Process process;
    int result = 0;
    try {
      process = Runtime.getRuntime().exec(userdir + "lib/wiringlite/bin/gnumake -s -C " + userdir + "lib/wiringlite compile");
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
      System.out.println("Wiring Lite Compilation Successful");
    }else{
      System.out.println("Wiring Lite Compilation Unsuccessful (error: " + result + ")");
    }
    return (result == 0);
  }

  public boolean compile(PdeSketch sketch, String buildPath)
    throws PdeException {

    this.sketch = sketch;
    this.buildPath = buildPath;

    // the pms object isn't used for anything but storage
    PdeMessageStream pms = new PdeMessageStream(this);

    String userdir = System.getProperty("user.dir") + File.separator;
    
    // if lib/wiringlite exists, then use it instead
    File newDir = new File(userdir + "lib/wiringlite");
    if(newDir.exists()){
      return wiringliteCompile(userdir);
    }

    String baseCommandCompiler[] = new String[] {
      ((PdeBase.platform != PdeBase.MACOSX) ? "tools/avr/bin/avr-gcc" :
        userdir + "tools/avr/bin/avr-gcc"),
      "-c",
      "-g",
      "-Os",
      "-Ilib",
      "-w",
      "-mmcu=atmega128",
      " ",
      " "
    };

    String baseCommandLinker[] = new String[] {
      ((PdeBase.platform != PdeBase.MACOSX) ? "tools/avr/bin/avr-gcc" :
        userdir + "tools/avr/bin/avr-gcc"),
      " ",
      "-mmcu=atmega128",
      "-o",
      " ",
      ((PdeBase.platform != PdeBase.MACOSX) ? "lib/BApplet.o" :
        userdir + "lib/BApplet.o"),
      ((PdeBase.platform != PdeBase.MACOSX) ? "lib/BSerial.o" :
        userdir + "lib/BSerial.o"),
      ((PdeBase.platform != PdeBase.MACOSX) ? "lib/BTimer.o" :
        userdir + "lib/BTimer.o"),
      ((PdeBase.platform != PdeBase.MACOSX) ? "lib/BServo.o" :
        userdir + "lib/BServo.o"),
      ((PdeBase.platform != PdeBase.MACOSX) ? "lib/BDisplay.o" :
        userdir + "lib/BDisplay.o"),
      ((PdeBase.platform != PdeBase.MACOSX) ? "lib/BEncoder.o" :
        userdir + "lib/BEncoder.o"),
      ((PdeBase.platform != PdeBase.MACOSX) ? "lib/BInterrupts.o" :
        userdir + "lib/BInterrupts.o"),
      ((PdeBase.platform != PdeBase.MACOSX) ? "tools/avr/avr/lib/libm.a" :
        userdir + "tools/avr/avr/lib/libm.a")
    };

    String baseCommandObjcopy[] = new String[] {
      ((PdeBase.platform != PdeBase.MACOSX) ? "tools/avr/bin/avr-objcopy" :
        userdir + "tools/avr/bin/avr-objcopy"),
      "-O",
      " ",
      "-R",
      " ",
      " ",
      " "
    };


    // make list of code files that need to be compiled
    // (some files are skipped if they contain no class)
    String preprocNames[] = new String[sketch.codeCount];
    int preprocCount = 0;
    for (int i = 0; i < sketch.codeCount; i++) {
      if (sketch.code[i].preprocName != null) {
        preprocNames[preprocCount++] = sketch.code[i].preprocName;
      }
    }

    String commandCompiler[] = new String[baseCommandCompiler.length + preprocCount];
    System.arraycopy(baseCommandCompiler, 0, commandCompiler, 0, baseCommandCompiler.length);
    // append each of the files to the command string
    for (int i = 0; i < preprocCount; i++) {
      commandCompiler[baseCommandCompiler.length + i] =
        buildPath + File.separator + preprocNames[i];
    }

    baseCommandLinker[1] = "-Os -W1,-u,vfprintf,-Map=" + 
      ((PdeBase.platform != PdeBase.MACOSX) ? buildPath : userdir + buildPath)
       + File.separator + sketch.name + ".map,--cref -lprintf_flt -lm";
    baseCommandLinker[4] = ((PdeBase.platform != PdeBase.MACOSX) ? buildPath
      : buildPath) + File.separator + sketch.name + ".elf";
    String commandLinker[] = new String[baseCommandLinker.length + preprocCount];
    System.arraycopy(baseCommandLinker, 0, commandLinker, 0, baseCommandLinker.length);
    for(int i = 0; i < preprocCount; i++) {
      commandLinker[baseCommandLinker.length + i] = buildPath + File.separator + preprocNames[i] + ".o";
    }


    /*
    String command[] = new String[baseCommand.length + sketch.codeCount];
    System.arraycopy(baseCommand, 0, command, 0, baseCommand.length);
    // append each of the files to the command string
    for (int i = 0; i < sketch.codeCount; i++) {
      command[baseCommand.length + i] =
        buildPath + File.separator + sketch.code[i].preprocName;
    }
    */

    //for (int i = 0; i < command.length; i++) {
      //System.out.println("cmd " + i + "  " + command[i]);
    //}

    firstErrorFound = false;  // haven't found any errors yet
    secondErrorFound = false;
    int result = 0; // pre-initialized to quiet a bogus warning from jikes
    try {
      // execute the compiler, and create threads to deal
      // with the input and error streams
      //
      Process process;
      boolean compiling = true;
      for(int i = 0; i < preprocCount; i++) {
        baseCommandCompiler[7] = buildPath + File.separator + preprocNames[i];
        baseCommandCompiler[8] = "-o"+ buildPath + File.separator + preprocNames[i] + ".o";  
        //System.arraycopy(baseCommandCompiler.length
        //for(int j = 0; j < baseCommandCompiler.length; j++) {
        //  System.out.println(baseCommandCompiler[j]);
        //}
        process = Runtime.getRuntime().exec(baseCommandCompiler);
        new PdeMessageSiphon(process.getInputStream(), this);
        new PdeMessageSiphon(process.getErrorStream(), this);

        // wait for the process to finish.  if interrupted
        // before waitFor returns, continue waiting
        //
        compiling = true;
        while (compiling) {
          try {
            result = process.waitFor();
            //System.out.println("result is " + result);
            compiling = false;
          } catch (InterruptedException ignored) { }
        }
        if(result!=0) 
          return false;
      }
      //for(int j = 0; j < commandLinker.length; j++) {
      //  System.out.println(commandLinker[j]);
      //}
      process = Runtime.getRuntime().exec(commandLinker);
      new PdeMessageSiphon(process.getInputStream(), this);
      new PdeMessageSiphon(process.getErrorStream(), this);
      compiling = true;
      while(compiling) {
	try {
          result = process.waitFor();
          compiling = false;
        } catch (InterruptedException intExc) { }
      }
      if(result!=0)
        return false;

      /*for(int j = 0; j < baseCommandObjcopy.length; j++) {
        System.out.println(baseCommandObjcopy[j]);
      }*/      
      baseCommandObjcopy[2] = "srec";
      baseCommandObjcopy[4] = ".eeprom";
      baseCommandObjcopy[5] = buildPath + File.separator + sketch.name + ".elf";
      baseCommandObjcopy[6] = buildPath + File.separator + sketch.name + ".rom";
      process = Runtime.getRuntime().exec(baseCommandObjcopy);
      new PdeMessageSiphon(process.getInputStream(), this);
      new PdeMessageSiphon(process.getErrorStream(), this);
      compiling = true;
      while(compiling) {
        try {
          result = process.waitFor();
          compiling = false;
        } catch (InterruptedException intExc) { }
      }
      if(result!=0)
        return false;

      baseCommandObjcopy[2] = "ihex";
      baseCommandObjcopy[4] = ".flash";
      baseCommandObjcopy[5] = buildPath + File.separator + sketch.name + ".elf";
      baseCommandObjcopy[6] = buildPath + File.separator + sketch.name + ".hex";
      process = Runtime.getRuntime().exec(baseCommandObjcopy);
      new PdeMessageSiphon(process.getInputStream(), this);
      new PdeMessageSiphon(process.getErrorStream(), this);
      compiling = true;
      while(compiling) {
        try {
          result = process.waitFor();
          compiling = false;
        } catch (InterruptedException intExc) { }
      }
      if(result!=0)
        return false; 


    } catch (Exception e) {
      String msg = e.getMessage();
      if ((msg != null) && (msg.indexOf("gcc: not found") != -1)) {
        //System.err.println("gcc is missing");
        PdeBase.showWarning("Compiler error",
                            "Could not find the compiler.\n" +
                            "gcc is missing from your PATH,\n" +
                            "see readme.txt for help.", null);
        return false;

      } else {
        e.printStackTrace();
        result = -1;
      }
    }

    // an error was queued up by message(), barf this back to build()
    // which will barf it back to PdeEditor. if you're having trouble
    // discerning the imagery, consider how cows regurgitate their food
    // to digest it, and the fact that they have five stomaches.
    //
    //System.out.println("throwing up " + exception);
    if (exception != null) throw exception;

    // if the result isn't a known, expected value it means that something
    // is fairly wrong, one possibility is that jikes has crashed.
    //
    if (result != 0 && result != 1 ) {
      //exception = new PdeException(SUPER_BADNESS);
      //editor.error(exception);  // this will instead be thrown
      PdeBase.openURL(BUGS_URL);
      throw new PdeException(SUPER_BADNESS);
    }

    // success would mean that 'result' is set to zero
    return (result == 0); // ? true : false;
  }


  boolean firstErrorFound;
  boolean secondErrorFound;

  /**
   * Part of the PdeMessageConsumer interface, this is called
   * whenever a piece (usually a line) of error message is spewed
   * out from the compiler. The errors are parsed for their contents
   * and line number, which is then reported back to PdeEditor.
   */
  public void message(String s) {
    // This receives messages as full lines, so a newline needs
    // to be added as they're printed to the console.
    System.err.println(s);

    // ignore cautions
    if (s.indexOf("Caution") != -1) return;

    // jikes always uses a forward slash character as its separator,
    // so replace any platform-specific separator characters before
    // attemping to compare
    //
    String buildPathSubst = buildPath.replace(File.separatorChar, '/') + "/";

    String partialTempPath = null;
    int partialStartIndex = -1; //s.indexOf(partialTempPath);
    int fileIndex = -1;  // use this to build a better exception

    // iterate through the project files to see who's causing the trouble
    for (int i = 0; i < sketch.codeCount; i++) {
      if (sketch.code[i].preprocName == null) continue;

      partialTempPath = buildPathSubst + sketch.code[i].preprocName;
      partialStartIndex = s.indexOf(partialTempPath);
      if (partialStartIndex != -1) {
        fileIndex = i;
        //System.out.println("fileIndex is " + fileIndex);
        break;
      }
    }

    // if the partial temp path appears in the error message...
    //
    //int partialStartIndex = s.indexOf(partialTempPath);
    //System.out.println(partialTempPath);
    if (partialStartIndex != -1) {

      // skip past the path and parse the int after the first colon
      //
      String s1 = s.substring(partialStartIndex +
                              partialTempPath.length() + 1);
      //System.out.println(s1);
      int colon = s1.indexOf(':');
      int lineNumber = Integer.parseInt(s1.substring(0, colon));
      //System.out.println("pde / line number: " + lineNumber);

      if (fileIndex == 0) {  // main class, figure out which tab
        for (int i = 1; i < sketch.codeCount; i++) {
          if (sketch.code[i].flavor == PdeSketch.PDE) {
            if (sketch.code[i].lineOffset < lineNumber) {
              fileIndex = i;
              //System.out.println("i'm thinkin file " + i);
            }
          }
        }
        if (fileIndex != 0) {  // if found another culprit
          lineNumber -= sketch.code[fileIndex].lineOffset;
          //System.out.println("i'm sayin line " + lineNumber);
        }
      }

      //String s2 = s1.substring(colon + 2);
      //System.out.println(s1);
      int err = s1.indexOf("undefined");
      if (err != -1) {

        // if the first error has already been found, then this must be
        // (at least) the second error found
        if (firstErrorFound) {
          secondErrorFound = true;
          return;
        }

        // if executing at this point, this is *at least* the first error
        firstErrorFound = true;

        //err += "error:".length();
        String description = s1.substring(err + "undefined".length());
        description = description.trim();

        //System.out.println("description = " + description);
        //System.out.println("creating exception " + exception);
        //System.out.println(lineNumber);
        exception = new PdeException(description, fileIndex, lineNumber-1, -1);

        // NOTE!! major change here, this exception will be queued
        // here to be thrown by the compile() function
        //editor.error(exception);

      } else {
        System.err.println("i suck: " + s);
      }

    } else {
      // this isn't the start of an error line, so don't attempt to parse
      // a line number out of it.

      // if the second error hasn't been discovered yet, these lines
      // are probably associated with the first error message,
      // which is already in the status bar, and are likely to be
      // of interest to the user, so spit them to the console.
      //
      if (!secondErrorFound) {
        System.err.println(s);
      }
    }
  }


  static String bootClassPath;

  static public String calcBootClassPath() {
    if (bootClassPath == null) {
      String additional = "";
      if (PdeBase.platform == PdeBase.MACOSX) {
        additional =
          contentsToClassPath(new File("/System/Library/Java/Extensions/"));
      }
      bootClassPath =  System.getProperty("sun.boot.class.path") + additional;
    }
    return bootClassPath;
  }


  ///


  /**
   * Return the path for a folder, with appended paths to
   * any .jar or .zip files inside that folder.
   * This will prepend a colon (or whatever the path separator is)
   * so that it can be directly appended to another path string.
   *
   * This will always add the root folder as well, and doesn't bother
   * checking to see if there are any .class files in the folder or
   * within a subfolder.
   */
  static public String contentsToClassPath(File folder) {
    if (folder == null) return "";

    StringBuffer abuffer = new StringBuffer();
    String sep = System.getProperty("path.separator");

    try {
      // add the folder itself in case any unzipped files
      String path = folder.getCanonicalPath();
      abuffer.append(sep);
      abuffer.append(path);

      if (!path.endsWith(File.separator)) {
        path += File.separator;
      }
      //System.out.println("path is " + path);

      String list[] = folder.list();
      for (int i = 0; i < list.length; i++) {
        if (list[i].toLowerCase().endsWith(".jar") ||
            list[i].toLowerCase().endsWith(".zip")) {
          abuffer.append(sep);
          abuffer.append(path);
          abuffer.append(list[i]);
        }
      }
    } catch (IOException e) {
      e.printStackTrace();  // this would be odd
    }
    //System.out.println("included path is " + abuffer.toString());
    //packageListFromClassPath(abuffer.toString());  // WHY?
    return abuffer.toString();
  }


  static private void packageListFromZip(String filename, Hashtable table) {
    try {
      ZipFile file = new ZipFile(filename);
      Enumeration entries = file.entries();
      while (entries.hasMoreElements()) {
        ZipEntry entry = (ZipEntry) entries.nextElement();

        if (!entry.isDirectory()) {
          String name = entry.getName();

          if (name.endsWith(".class")) {
            int slash = name.lastIndexOf('/');
            if (slash == -1) continue;

            String pname = name.substring(0, slash);
            if (table.get(pname) == null) {
              table.put(pname, new Object());
            }
          }
        }
      }
    } catch (IOException e) {
      System.err.println("Ignoring " + filename + " (" + e.getMessage() + ")");
      //e.printStackTrace();
    }
  }


  /**
   * Make list of package names by traversing a directory hierarchy.
   * Each time a class is found in a folder, add its containing set
   * of folders to the package list. If another folder is found,
   * walk down into that folder and continue.
   */
  static private void packageListFromFolder(File dir, String sofar,
                                            Hashtable table) {
                                          //String imports[],
                                          //int importCount) {
    //System.err.println("checking dir '" + dir + "'");
    boolean foundClass = false;
    String files[] = dir.list();

    for (int i = 0; i < files.length; i++) {
      if (files[i].equals(".") || files[i].equals("..")) continue;

      File sub = new File(dir, files[i]);
      if (sub.isDirectory()) {
        String nowfar =
          (sofar == null) ? files[i] : (sofar + "." + files[i]);
        packageListFromFolder(sub, nowfar, table);
        //System.out.println(nowfar);
        //imports[importCount++] = nowfar;
        //importCount = magicImportsRecursive(sub, nowfar,
        //                                  imports, importCount);
      } else if (!foundClass) {  // if no classes found in this folder yet
        if (files[i].endsWith(".class")) {
          //System.out.println("unique class: " + files[i] + " for " + sofar);
          table.put(sofar, new Object());
          foundClass = true;
        }
      }
    }
    //return importCount;
  }

}