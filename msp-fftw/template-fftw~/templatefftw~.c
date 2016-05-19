/**
 *
 *  @file	templatefftw~.c
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
 *
 *   FFTW3 documentation :
 *      http://www.fftw.org/fftw3.pdf
 *
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

#include "fftw3.h"


//____________________________________________________________________
//                        'Class' Definition
//____________________________________________________________________
/*
	
 'Class' decleration and a struct for the object is declared and typedef'd.

 */

// Basic Max objects are declared as C structures. The first element of the structure is a t_object, followed by whatever you want. The example below has one long structure member.
typedef struct _templatefftw	///<	A struct to hold data for our object
{
    t_pxobject  x_obj;          ///<	The object itself (t_pxobject in MSP instead of t_object)
    t_float     x_val;          ///<	Value to use for the processing
    t_bool      fftOn;          ///<    Turns on/off the FFT
    void        *x_output;      ///<    Output definition

} t_templatefftw;

// global pointer to our class definition that is setup in main()
static t_class *templatefftw_class = NULL;





//____________________________________________________________________
//                        Function Prototypes
//____________________________________________________________________

//// standard set
void *templatefftw_new( t_symbol *s, long argc, t_atom *argv);
void templatefftw_free( t_templatefftw *x);
void templatefftw_assist(t_templatefftw *x, void *b, long m, long a, char *s);

//// value specific
void templatefftw_float(t_templatefftw *x, double f);
void templatefftw_int(  t_templatefftw *x, long n);
void templatefftw_bang( t_templatefftw *x);

//// additional inlet behavious
void templatefftw_in0( t_templatefftw *x, long n);      //1st inlet
void templatefftw_in1( t_templatefftw *x, long n);      //2nd inlet

//// performance set
void templatefftw_dsp64(t_templatefftw *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void templatefftw_perform64(t_templatefftw *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);
void templatefftw_basicfft(t_templatefftw *x, long N, double **ins);
void templatefftw_dblclick(t_templatefftw *x);




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
    t_class *c;
    
    c = class_new("templatefftw~", (method)templatefftw_new, (method)dsp_free, (long)sizeof(t_templatefftw), 0L, A_GIMME, 0);
    
    //binds a C function to a text symbol.
    class_addmethod(c, (method)templatefftw_bang,       "bang",             0);
    class_addmethod(c, (method)templatefftw_int,        "int",      A_LONG, 0);
    class_addmethod(c, (method)templatefftw_float,		"float",	A_FLOAT,0);
    class_addmethod(c, (method)templatefftw_dsp64,		"dsp64",	A_CANT, 0);
    class_addmethod(c, (method)templatefftw_assist,     "assist",	A_CANT, 0);
    
    // The A_LONG, 0 args specify the type of arguments expeced by the C function
    // A_LONG   long int        A_DEFLONG   puts a 0 in the place of a mising long argument
    // A_FLOAT  double          A_DEFFLOAT  ----------------------------------float--------
    // A_SYM    symbols         A_DEFSYM    -----an empty symbol--------------symbol-------
    // A_GIMME  raw list of atoms, since mutliple A_FLOAT should be avoided (cf. MaxAPI), A_GIMME should be used for more than four arguments or with multiple floating-point arguments
    // A_CANT   used when we cannot type check the argument
    class_addmethod(c, (method)templatefftw_in0,        "int",      A_LONG, 0);
    class_addmethod(c, (method)templatefftw_in1,        "in0",      A_LONG, 0);
    class_addmethod(c, (method)templatefftw_dblclick,   "dblclick", A_CANT, 0);
    
    // if the filename on disk is different from the object name in Max, ex. w/ times
//  class_setname("*~","times~");
    
    //  Adds a set of methods to your object's class that are called by MSP to build the DSP call chain.
    class_dspinit(c);
    
    //  adds this class to the CLASS_BOX name space, meaning that it will be searched when a user tries to type it into a box.
    class_register(CLASS_BOX, c);
    
    //assign the class we've created to a global variable so we can use it when creating new instances.
    templatefftw_class = c;
    
}





//____________________________________________________________________
//                          Instance Routines
//____________________________________________________________________


void *templatefftw_new(t_symbol *s, long argc, t_atom *argv)
{
    //Setup the custom struct for our object
    t_templatefftw *x = (t_templatefftw *) object_alloc((t_class *) templatefftw_class);
    
    //Setup 2 inlets for our object
    dsp_setup((t_pxobject *)x, 2);
    
    //Give our object a signal outlet
    outlet_new((t_pxobject *)x, "signal");
    outlet_new((t_pxobject *)x, "signal");
    
    // splatted in _dsp method if optimizations are on
    x->x_val = argc;
    
    return (x);
}

// NOT CALLED!, we use dsp_free for a generic free function
void templatefftw_free(t_templatefftw *x)
{
    ;
}

//Documentation shown when hovering over an inlet/outlet
//templatefftw~: sprintf content here
void templatefftw_assist(t_templatefftw *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) {
        //inlet
        switch (a){
            case 0: sprintf(s, "(Signal) Left Input"); break;
            case 1: sprintf(s, "(Signal) Right Input"); break;
        }
    }
    else if (m == ASSIST_OUTLET) {
        // outlet
        switch (a){
            case 0: sprintf(s, "(Signal) Left Output  : L*R"); break;
            case 1: sprintf(s, "(Signal) Right Output : L+R"); break;
        }
    }
}





//____________________________________________________________________
//                          Inlet Handlers
//____________________________________________________________________

void templatefftw_in0( t_templatefftw *x, long n)
{
    object_post((t_object *)x, "in 0 got : %ld", n);
}

void templatefftw_in1( t_templatefftw *x, long n)
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
void templatefftw_int(t_templatefftw *x, long n)
{
    x-> x_val = n;
}

//This simply copies the value of the argument to the internal storage within the instance.
void templatefftw_float(t_templatefftw *x, double f)
{
    x-> x_val = f;
}

void templatefftw_bang(t_templatefftw *x)
{
    object_post((t_object *)x, "value is %ld",x->x_val);
}

void templatefftw_dblclick(t_templatefftw *x)
{
    object_post((t_object *)x, "about to fft");
    x-> fftOn = true;
}

// Note object_post will print text to the Max window, it is linked to an instance of your object
// While post(C74_CONST char *) is static, thus not linked ot a specific instance



//____________________________________________________________________
//                          Perfomance Routines
//____________________________________________________________________

// registers a function for the signal chain in Max
// Calls the appropriate functions to do the processing
/*
 The dsp64 method specifies the signal processing function your object defines along with its arguments. Your object's dsp64 method will be called when the MSP signal compiler is building a sequence of operations (known as the DSP Chain) that will be performed on each set of audio samples. The operation sequence consists of a pointers to functions (called perform routines) followed by arguments to those functions.
 */
void templatefftw_dsp64(t_templatefftw *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_post((t_object *)x, "my sample rate is: %f", samplerate);
    
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
    
    object_method(dsp64, gensym("dsp_add64"), x, templatefftw_perform64, 0, NULL);
}

// this is the 64-bit perform method audio vectors
// Perform processing on signal & float connected to inlets
/*
 The perform routine is not a "method" in the traditional sense. It will be called within the callback of an audio driver, which, unless the user is employing the Non-Real Time audio driver, will typically be in a high-priority thread. Thread protection inside the perform routine is minimal. You can use a clock, but you cannot use qelems or outlets.
 */
void templatefftw_perform64(t_templatefftw *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
    t_double *inL = ins[0];     // we get audio for each inlet of the object from the **ins argument
    t_double *inR = ins[1];
    t_double *outL = outs[0];    // we get audio for each outlet of the object from the **outs argument
    t_double *outR = outs[1];

    t_double ftmpL;
    t_double ftmpR;

    if (x->fftOn) {
        templatefftw_basicfft(x, 256, ins);
        object_post((t_object *)x, "i just ffted");
        x->fftOn = false;
    }
    
    // this perform method simply copies the input to the output, offsetting the value
    while (sampleframes--) {
        //  mult two signals
        ftmpL = *inL * *inR;
        FIX_DENORM_NAN_DOUBLE(ftmpL);
        *outL++ = ftmpL;
        
        //  add two signals
        ftmpR = *inL++ + *inR++;
        FIX_DENORM_NAN_DOUBLE(ftmpR);
        *outR++ = ftmpR;

    }
}





//____________________________________________________________________
//                          FFT Routines
//____________________________________________________________________

void templatefftw_basicfft(t_templatefftw *x, long N, double **ins)
{
    t_double *inL = ins[0];
    
    fftw_complex    *data;      // audio samples/data
    fftw_complex    *fft_out;   // result of the forward plan, i.e. the FFT
    fftw_complex    *ifft_out;  // result of the backward plan, i.e. the iFFT
    fftw_plan       p_forw;     // forward plan
    fftw_plan       p_back;     // backward plan
    int             i;          // global incrementer
    
    // Allocate mem (i/o arrays)
    /* 
     fftw_malloc behaves like malloc, except that it properly aligns the array when SIMD (Single Instruction Multiple Data) instructions. Whereas malloc() only allocates, if not properly aligned FFTW will not use SIMD extensions.
     SIMD : a set of special operations supported by some processors (cf. doc) to perform a single operation on several numbers (usually 2 or 4) simultaneously. Programs compiled w/SIMD = nonnegligible speedup for most r2c/c2r.
     Wrapper routines : 
        fftw_alloc_real(N)    == (double*)fftw_malloc(sizeof(double) * N)
        fftw_alloc_complex(N) == (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N),
     */
    data        = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    fft_out     = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    ifft_out    = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    
    // Make a plan
    /*
     Object that contains all th data that FFTW needs to compute the FFT.
      
      4th arg : FFTW_FORWARD : -/+ 1  sign of the exponent in the transform.
      5th arg : FFTW_ESTIMATE : does not run any computation and just builds a reasonable plan that is probably sub-optimal. does not touch arrays, but make a plan to be sure
                     or
                FFTW_MEASURE : measure the execution time of several FFTs in order to find the best way to compute the transform of size n. Overwrites the i/o arrays
                a lot of other flags are included, check def of FFTW_ESTIMATE to check them out.
     Note : 
      - Once the plan has been created you can use it as many as times as you like to transform the specified i/o arrays (w/fftw_execute(fftw_plan p)).
      - If you want to transform a different array of the ame size, you can create a new plan w/fftw_plan_dft_1d and FFTW automatically reuses the info from previous plan when possible.
     - FFTW also provides two routines for creating plans for 2d and 3d transforms, and one routine for creating plans of arbitrary dimensionality.
     */
    p_forw = fftw_plan_dft_1d(N, data,      fft_out,    FFTW_FORWARD,   FFTW_ESTIMATE);
    p_back = fftw_plan_dft_1d(N, fft_out,   ifft_out,   FFTW_BACKWARD,  FFTW_ESTIMATE);
    
    for( i = 0 ; i < N ; i++ ) {
        data[i][0] = *inL;
        data[i][1] = 0.0; // use this if your data is complex valued
    }
    
    
    for( i = 0 ; i < N ; i++ ) {
        object_post((t_object *)x, "data[%d] = { %2.2f, %2.2f }\n",
                i, data[i][0], data[i][1] );
    }
    
    object_post((t_object *)x, "________________________________________________________");
    
    // Compute transform
    /*
     DFT results are stored in-order in the output array (i.e. out). out[0] == DC component.
     If in != out => transform is out-of-place => in is not modified. Otherwise input array is overwritten with the transform.
     Computes an unormalized DFT, so couputing FORWARD then BACKWARD transform results in the original array scaled by n.
     
     */
    fftw_execute(p_forw);
    
    for( i = 0 ; i < N ; i++ ) {
        object_post((t_object *)x, "fft_result[%d] = { %2.2f, %2.2f }\n",
                i, fft_out[i][0], fft_out[i][1] );
    }
    
    object_post((t_object *)x, "________________________________________________________");
    
    fftw_execute(p_back);
    
    for( i = 0 ; i < N ; i++ ) {
        object_post((t_object *)x, "ifft_result[%d] = { %2.2f, %2.2f }\n",
                i, ifft_out[i][0] / N, ifft_out[i][1] / N );
    }

    
    fftw_destroy_plan(p_forw);
    fftw_destroy_plan(p_back);
    
    fftw_free(data);
    fftw_free(fft_out); fftw_free(ifft_out);
    
    
}



























