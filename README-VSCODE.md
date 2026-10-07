
# VS Code Setup

To quickly configure your build and debugging environment in Visual Studio Code, you can use the configurations provided below.

## Configuration Files

Create a `.vscode` directory in your project's root folder if it doesn't already exist, and add the following two files:

### 1. `tasks.json`
Defines tasks for compiling the project directly from VS Code.

<details>
<summary>Click to view tasks.json example</summary>

```json
{
    // See https://go.microsoft.com/fwlink/?LinkId=733558
    // for the documentation about the tasks.json format
    "version": "2.0.0",
    "tasks": [
        {
            "type"   : "shell",
            "label"  : "Build Project (make)", //< Descriptive label for the new task
            "command": "sh",                   //< The command to execute
            "args"   : [ "-c", "make" ],       //< Arguments for the command
            "options": {
                "cwd": "${workspaceFolder}"    //< Directory where the shell command should be executed
            },
            "group": {
                "kind"     : "build",
                "isDefault": true
            },
            "problemMatcher": {
                "base": "$gcc", 
                "fileLocation": ["relative", "${workspaceFolder}"]
            }
        }
    ]
}
```

### 2. `launch.json`

Provides the configuration for the VS Code debugger (GDB/LLDB).

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name"   : "C: Debug (GDB)",
            "type"   : "cppdbg",
            "request": "launch",

            // Executable path and arguments
            "program": "${workspaceFolder}/bin/promptex",
            "args"   : [],

            "cwd"        : "${workspaceFolder}",
            "stopAtEntry": false,
            "environment": [],
            "MIMode"     : "gdb",
            "setupCommands": [
            {
                "description"   : "Enable pretty-printing for gdb",
                "text"          : "-enable-pretty-printing",
                "ignoreFailures": true
            }
            //{
            //    "description"   : "Set Disassembly Flavor to Intel",
            //    "text"          : "-gdb-set disassembly-flavor intel",
            //    "ignoreFailures": true
            //}
            ],
            //"externalConsole": false,
            "preLaunchTask": "Build Project (make)"  //< Matches the task name in tasks.json
        }
    ]
}
```

## How to Use

1. **Create the folder:** Create a `.vscode/` directory at the root of your project if you haven't already.
2. **Create the files:** Inside `.vscode/`, create two files named `tasks.json` and `launch.json`.
3. **Copy the content:** Copy the JSON snippets above into their respective files.
4. **Customize:** Adjust paths or compiler flags if your local toolchain differs from the defaults.

> **Note:** Ensure you have the necessary build tools (such as `gcc`, `clang`, or `gdb`) installed on your system and available in your `PATH`.

```
