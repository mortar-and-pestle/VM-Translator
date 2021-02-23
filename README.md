# VM-Translator
Translates VM code that is produced by a Jack compiler into Hack assembly.

For more information on Jack, it's VM counterpart, and Hack assembly, please visit https://www.nand2tetris.org/

Build Instructions:
-Compile using any Windows compiler


Execution Instructions:

-Two arguments can be passes to the executable
-One is mandatory and one is optional

  The first argument, which is required, can either be:

    1. The name of a .vm file. The .vm file must reside in the same directory as the .exe.
    2. The name of a directory that contains one or more .vm files. The .exe does not have to be in the same folder as the .vm files.
       Please note that the produced .asm file will be found in the directory containing the .vm files, not the directory of the .exe.
     
  The second argument, which is optional, is -n.
    If -n is passed, the bootstrap code will NOT be inserted at the beginning of the .asm file.
    This is necessary due to some VM tests taking care of RAM initialization.
    
   
   Examples of commands:
      
    VMTranslator.exe SimpleFunction.vm -n
    
    VMTranslator.exe C:\Users\User\VMFiles 

    Note: If directory contains spaces, quotes must be added to path

    VMTranslator.exe "C:\Users\User\Important Files\VMFiles"


Testing Instructions:

Testing can be accomplished using the CPUEmualtor included with the nand2tetris project tools.
Sample .vm files as well as testing materials can be found there.
