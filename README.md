# Max templates

Templates/basic examples, written in ``c``, for creating **MSP** and **MSP** external objects.

This serves as a synthesis of information for writing **Max/MSP** externals. It is a quick starting point for developing simple objects with a good dose of commentary. If you want to go further, the [Max API](https://cycling74.com/sdk/MaxSDK-7.1.0/html/index.html) is a great resource, as well as ther documents cited in the comments.

Most of the code and commentary is derived from sources that are cited, with there respective links, in the ``c`` files.

### Main sources
- [Cycling74 : Max/MSP SDK examples](https://github.com/Cycling74/max6-sdk/tree/master/examples)
- [UC Berkeley : CNMAT](https://github.com/CNMAT/CNMAT-Externs)
- [McGill : Ichiro Fujinaga](http://www.music.mcgill.ca/~ich/classes/mumt402_06/MaxMSPExternalsTutorials/MaxMSPExternalsTutorial3.2.pdf)

## Prerequisites

The Max/MSP SDK is required. Theses templates are made with [Max/MSP 7.1 SDK](https://cycling74.com/downloads/sdk/#.VzpdRpPbvdQ)

## How to use

Download the zipped file, extract and open the folder containing a specific project you would like to start out with.

``<project_name> ``   : **Max** object template

``<project_name>~ ``  : **MSP** object template

The tilde indicates that object treats audio ( **MSP** ) as opposed to data ( **Max** ).

In **Max**, in the *Options* menu go to *File Preferences* and add the path to your externals folder. There should already be a folder named externals in the max-sdk-7 folder.
Once built, restart **Max** and test your object.

### Xcode

Verify the project Info and Build *Settings*. If not present, the first step would be to create thee user defined *Settings* :


| Name                 | Content                                                                           |
|----------------------|-----------------------------------------------------------------------------------|
| C74SUPPORT           | /Max 7/Packages/max-sdk-7.1.0/source/c74supportC74_SYM_LINKER_FLAGS               |
| C74_SYM_LINKER_FLAGS | /Max 7/Packages/max-sdk-7.1.0/source/c74support/max-includes/c74_linker_flags.txt |
| Product Version      | 7.0.1                                                                             |

Remember to specify, in the ``DSTROOT`` field, the folder in which the external will be built.  
The `maxmspsdk.xconfig` file should automatically link theses paths to the rest of the *Settings*.  
If issues persist, compare your *Settings* to that of an example in the Max SDK folder : ``/Max 7/Packages/max-sdk-7.1.0/source``

### Visual Studio

Similarly to Xcode with its Info.plist, the Max/MSP SDK uses a ``.props`` file to configure the project variables such as ``$(C74SUPPORT)`` and ``$(OutDir)``.

So, to the following parameters need to be configured :

| File Name                 | Variable Name                     |
|---------------------------|-----------------------------------|
| max_extern_common.props   | ``<OutDir>__PATH__</OutDir>``         |
| max_extern_common.props   | ``<C74SUPPORT>__PATH__</C74SUPPORT>`` |

Visual Studio should find the ``.props`` by himself. If not, open the ``.vcxproj`` and change the ``<Import Project= PathToProps>`` for the ``max_extern_common``, ``max_extern_x86`` and ``max_extern_x64``.

### Alternate Solution

Alternatively, Considering the previous guidelines, the easiest solution would be to simply make a copy of a Max/MSP SDK project example next to the other examples. *Most* of the settings will be correct. But where is the fun in that.

### FFTW

To use the MSP template that use the FFTW library, you need to install [FFTW 3.3.4](http://www.fftw.org) which is the latest stable release. Then link it to your project on [Xcode](http://ofdsp.blogspot.fr/2011/07/installing-fftw3-with-xcode-and.html) or on [Windows](http://www.fftw.org/install/windows.html).

## Notes

Currently only for XCode, but the ``c`` files can be reused in any IDE that is properly configured.

Cycling '74 suggests to use their [max-test](https://github.com/Cycling74/max-test) tool to properly test your objects.


>Visual Studio version is in progress

>Pure Data templates will proceed
