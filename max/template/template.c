/**
 *
 *  @file	template.c
 *
 *
 *  Sources :
 *
 *   Cylcing 74'
 *    - Max 7.1 API :
 *          https://cycling74.com/sdk/MaxSDK-6.0.4/html/index.html
 *
 *    - Max/MSP 7.1 SDK examples :
 *          https://cycling74.com/downloads/sdk/#.Vzn0OpPbugw
 *
 *    - Writing External Objects for Max 4.0 and MSP 2.0 (written by David Zicarelli) :
 *          http://peabody.sapp.org/class/dmp2/read/WritingMax_MSPExternals.pdf
 *
 *   Ichiro Fujinaga
 *    - Max/MSP Externals Tutorial :
 *          http://www.music.mcgill.ca/~ich/classes/mumt402_06/MaxMSPExternalsTutorials/MaxMSPExternalsTutorial3.2.pdf
 *
 *   Beoit Bouchez
 *    - Writing-MaxMSP-Externals-on-Windows :
 *          http://1cyjknyddcx62agyb002-c74projects.s3.amazonaws.com/files/2013/11/Writing-MaxMSP-Externals-on-Windows.pdf
 *
 */

//____________________________________________________________________
//                         External Libraries
//____________________________________________________________________
/*
 
 Headears and Platform specific elements
 
 */
#ifdef MAC_VERSION
    // do something specific to the Mac
#endif
#ifdef WIN_VERSION
    // do something specific to Windows
#endif

#include "ext_byteorder.h"  // provides cross-platform tools for manipulating memory in an endian-independent way.

#include "ext.h"            // should always be first, then ext_obex.h + other files.
#include "ext_obex.h"		// required for "new" style objects
#include "z_dsp.h"			// required for MSP objects





//____________________________________________________________________
//                        'Class' Definition
//____________________________________________________________________
/*
	
 'Class' decleration and a struct for the object is declared and typedef'd.

 */

// Basic Max objects are declared as C structures. The first element of the structure is a t_object, followed by whatever you want. The example below has one long structure member.
typedef struct _template	///<	A struct to hold data for our object
{
    t_pxobject x_obj;       ///<	The object itself (t_pxobject in MSP instead of t_object)
    t_float x_val;          ///<	Value to use for the processing
    void *x_output;         ///<    Output definition

} t_template;

// global pointer to our class definition that is setup in main()
static t_class *template_class = NULL;





//____________________________________________________________________
//                        Function Prototypes
//____________________________________________________________________

//// standard set
void *template_new( t_symbol *s, long argc, t_atom *argv);
void template_free( t_template *x);
void template_assist(t_template *x, void *b, long m, long a, char *s);

//// value specific
void template_float(t_template *x, double f);
void template_int(  t_template *x, long n);
void template_bang( t_template *x);

//// additional inlet behavious
void template_in0( t_template *x, long n);      //1st inlet
void template_in1( t_template *x, long n);      //2nd inlet

//// performance set
void template_dsp64(t_template *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void template_perform64(t_template *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);





//____________________________________________________________________
//                          Initialisation Routine
//____________________________________________________________________

/*
 
The initialization routine, which must be called main, is called when Max loads your object for the first time. In the initialization routine, you define one or more classes. Defining a class consists of the following:

        1) telling Max about the size of your object's structure and how to create and destroy an instance
        2) defining methods that implement the object's behavior 
        3) in some cases, defining attributes that describe the object's data 
        4) registering the class in a name space
 
*/

void ext_main(void *r)
{
    // object initialization, note the use of dsp_free for the freemethod, which is required
    // unless you need to free allocated memory, in which case you should call dsp_free from
    // your custom free function.
    
    // creates a class with the new instance routine (see below), a free function (in this case there isn't one, so we pass NULL), the size of the structure, a no-longer used argument, and then a description of the arguments you type when creating an instance (in this case, there are no arguments, so we pass 0).
    t_class *c = class_new("template", (method)template_new, (method)dsp_free, (long)sizeof(t_template), 0L, A_GIMME, 0);
    
    //binds a C function to a text symbol. The two methods defined here are int and bang.
    class_addmethod(c, (method)template_bang,       "bang",             0);
    class_addmethod(c, (method)template_int,        "int",      A_LONG, 0);
    class_addmethod(c, (method)template_float,		"float",	A_FLOAT,0);
    class_addmethod(c, (method)template_dsp64,		"dsp64",	A_CANT, 0);
    class_addmethod(c, (method)template_assist,     "assist",	A_CANT, 0);
    
    class_addmethod(c, (method)template_in0,        "int",      A_LONG, 0);
    class_addmethod(c, (method)template_in1,        "in0",      A_LONG, 0);
    
    // if the filename on disk is different from the object name in Max, ex. w/ times
//  class_setname("*~","times~");
    
    //  Adds a set of methods to your object's class that are called by MSP to build the DSP call chain.
    class_dspinit(c);
    
    //  adds this class to the CLASS_BOX name space, meaning that it will be searched when a user tries to type it into a box.
    class_register(CLASS_BOX, c);
    
    //assign the class we've created to a global variable so we can use it when creating new instances.
    template_class = c;
    
}





//____________________________________________________________________
//                          Instance Routines
//____________________________________________________________________


void *template_new(t_symbol *s, long argc, t_atom *argv)
{
    //Setup the custom struct for our object
    t_template *x = (t_template *) object_alloc((t_class *) template_class);
    
    //Setup 2 inlets for our object
    dsp_setup((t_pxobject *)x, 2);
    
    //Give our object a signal outlet
    outlet_new((t_pxobject *)x, "signal");
    
    // splatted in _dsp method if optimizations are on
    x->x_val = argc;
    
    return (x);
}

// NOT CALLED!, we use dsp_free for a generic free function
void template_free(t_template *x)
{
    ;
}

//Documentation shown when hovering over an inlet/outlet
//template: sprintf content here
void template_assist(t_template *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) {
        //inlet
        switch (a){
            case 0: sprintf(s, "(Signal) Left Input"); break;
            case 1: sprintf(s, "(Signal) Right Input"); break;
        }
    }
    else {
        // outlet
        switch (a){
            case 2: sprintf(s, "(Signal) Left Output"); break;
            case 3: sprintf(s, "(Signal) Right Output"); break;
        }
    }
}





//____________________________________________________________________
//                          Inlet Handlers
//____________________________________________________________________

void template_in0( t_template *x, long n)
{
    object_post((t_object *)x, "in 0 got : %ld", n);
}

void template_in1( t_template *x, long n)
{
    object_post((t_object *)x, "in 1 got : %ld", n);
}





//____________________________________________________________________
//                          Message Handlers
//____________________________________________________________________

/*
 
 The C functions you write will be declared according to the arguments the message requires. All functions are passed a pointer to your object as the first argument.
 
 */

//This simply copies the value of the argument to the internal storage within the instance.
void template_int(t_template *x, long n)
{
    x-> x_val = n;
}

//This simply copies the value of the argument to the internal storage within the instance.
void template_float(t_template *x, double f)
{
    x-> x_val = f;
}

void template_bang(t_template *x)
{
    post("value is %ld",x->x_val);
}





//____________________________________________________________________
//                          Perfomance Routines
//____________________________________________________________________

// registers a function for the signal chain in Max
// Calls the appropriate functions to do the processing
void template_dsp64(t_template *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    post("my sample rate is: %f", samplerate);
    
    /* 
        instead of calling dsp_add(), we send the "dsp_add64" message to the object representing the dsp chain
     the arguments passed are:
            1: the dsp64 object passed-in by the calling function
            2: the symbol of the "dsp_add64" message we are sending
            3: a pointer to your object
            4: a pointer to your 64-bit perform method
            5: flags to alter how the signal chain handles your object -- just pass 0
            6: a generic pointer that you can use to pass any additional data to your perform method
     */
    
    object_method(dsp64, gensym("dsp_add64"), x, template_perform64, 0, NULL);
}

// this is the 64-bit perform method audio vectors
// Perform processing on signal & float connected to inlets
void template_perform64(t_template *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
    t_double *inL = ins[0];     // we get audio for each inlet of the object from the **ins argument
    t_double *inR = ins[1];
    t_double *out = outs[0];    // we get audio for each outlet of the object from the **outs argument

    t_double ftmp;

    // this perform method simply copies the input to the output, offsetting the value
    while (sampleframes--) {
        ftmp = *inL++ * *inR++;
        FIX_DENORM_NAN_DOUBLE(ftmp);
        *out++ = ftmp;
    }
}





//____________________________________________________________________
//                          Additional Routines
//____________________________________________________________________


