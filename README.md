# Max templates

Templates/basic examples, written in c, for creating **MSP** and **MSP** external objects.

This serves as a synthesis of information for writing **Max/MSP** externals. It is a quick starting point for developing simple objects with a good dose of commentary. If you want to go further, the [Max API](https://cycling74.com/sdk/MaxSDK-6.0.4/html/index.html) is a great resource, as well as ther documents cited in the comments.

Most of the code and commentary is derived from sources that are cited, with there respective links, in the c files.

### Main sources
- [Max/MSP SDK examples](https://github.com/Cycling74/max6-sdk/tree/master/examples)
- [CNMAT](https://github.com/CNMAT/CNMAT-Externs)
- [Ichiro Fujinaga](http://www.music.mcgill.ca/~ich/classes/mumt402_06/MaxMSPExternalsTutorials/MaxMSPExternalsTutorial3.2.pdf)

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

Currently only for XCode, but the c files can be reused in any IDE that is properly configured.

Cycling '74 suggests to use their [max-test](https://github.com/Cycling74/max-test) tool to properly test your objects.


>Visual Studio version is in progress

>Pure Data templates will proceed
