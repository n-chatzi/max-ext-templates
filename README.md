# Max templates

Templates/basic examples, written in c, for creating **MSP** and **MSP** external objects.

Most of the code and commentary is derived from sources that are cited, with there respective links, in the c files. 

## Prerequisites

The Max/MSP SDK is required. Theses templates are made with [Max/MSP 7.1 SDK](https://cycling74.com/downloads/sdk/#.VzpdRpPbvdQ)

## How to use

Download the zipped file, extract and open the folder containing a specific project you would like to start out with.

``<project_name> ``   : **Max** object template

``<project_name>~ ``  : **MSP** object template

The tilde indicates that object treats audio ( **MSP** ) as opposed to data ( **Max** ).

### Xcode

Verify the project Info and Build *Settings*. The first step would be to create thee user defined *Settings* :


| Name                 | Content                                                                           |
|----------------------|-----------------------------------------------------------------------------------|
| C74SUPPORT           | /Max 7/Packages/max-sdk-7.1.0/source/c74supportC74_SYM_LINKER_FLAGS               |
| C74_SYM_LINKER_FLAGS | /Max 7/Packages/max-sdk-7.1.0/source/c74support/max-includes/c74_linker_flags.txt |
| Product Version      | 7.0.1                                                                             |

The `maxmspsdk.xconfig` file should automatically link theses paths to the rest of the *Settings*.     
If issues persist, compare your *Settings* to that of an example in the Max SDK folder : /Max 7/Packages/max-sdk-7.1.0/source

### Visual Studio

*in progress*

## Notes

Currently only for XCode, but the c files can be reused in any IDE that is properly configured


>Visual Studio version is in progress

>Pure Data templates will proceed
