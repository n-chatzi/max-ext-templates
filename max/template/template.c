/**
 *
 *  @file	template.c
 *
 *
 *  All sources used can be found here
 *
 *  Documentation :
 *      Cylcing 74'
 *          - Max 7.1 API :
 *              https://cycling74.com/sdk/MaxSDK-6.0.4/html/index.html
 *
 *          - Max/MSP 7.1 SDK examples :
 *              https://cycling74.com/downloads/sdk/#.Vzn0OpPbugw
 *
 *          - Writing External Objects for Max 4.0 and MSP 2.0 (written by David Zicarelli) :
 *              http://peabody.sapp.org/class/dmp2/read/WritingMax_MSPExternals.pdf
 *
 *      Ichiro Fujinaga
 *          - Max/MSP Externals Tutorial :
 *              http://www.music.mcgill.ca/~ich/classes/mumt402_06/MaxMSPExternalsTutorials/MaxMSPExternalsTutorial3.2.pdf
 *
 *      Beoit Bouchez
 *          - Writing-MaxMSP-Externals-on-Windows :
 *              http://1cyjknyddcx62agyb002-c74projects.s3.amazonaws.com/files/2013/11/Writing-MaxMSP-Externals-on-Windows.pdf
 *
 *
 *  Code : 
 *      
 *      Kingsley 
 *          - Kingsley’s guide to basic max object programming.
 *               http://bb-attachments.cycling74.com.s3.amazonaws.com/2169.kingsleysmaxobjectprogrammingguide.doc
 *
 *      Cylcing 74'
 *          - Max/MSP SDK dummy & simplemax by jeremy bernstein - jeremy@bootsquad.com :
 *              https://github.com/Cycling74/max-sdk/tree/master/source/basics/template
 *
 *
 *  This object has two inlets and two outlets.
 *  The left outlet multiplies the inlets.
 *  The right outlet adds the inlets.
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





//____________________________________________________________________
//                        'Class' Definition
//____________________________________________________________________
/*
	
 'Class' decleration and a struct for the object is declared and typedef'd.

 */

// Basic Max objects are declared as C structures. The first element of the structure is a t_object, followed by whatever you want. The example below has one long structure member.
typedef struct _template	///<	A struct to hold data for our object
{
    t_object    obj;        ///<	The object itself (t_object in Max instead of t_object for MSP)
    t_atom      val;        ///<    Value to use for argument
    t_atom      val_0;      ///<	Value to use for inlet 0
    t_atom      val_1;      ///<	Value to use for inlet 1
    void        *proxy;     ///<    A proxy is a small object that controls an inlet, but does not translate the message it receives. The advantage of proxies over regular inlets is that your object can respond to any message in all of its inlets.
    long        in_n;       ///<    Space for the inlet number used by all the proxies
    t_symbol    name;       ///<    Name of the Max object
    void        *out_0;     ///<    Output definition
    void        *out_1;     ///<    Output definition

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
void template_anything(t_template *x, t_symbol *s, long ac, t_atom *av);    ///< request that args be passed as an array, the routine will check the types itself.
void template_identify(t_template *x);
void template_dblclick(t_template *x);





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
    // object initialization
    // creates a class with the new instance routine (see below), a free function (in this case there isn't one, so we pass NULL), the size of the structure, a no-longer used argument, and then a description of the arguments you type when creating an instance (in this case, there are no arguments, so we pass 0).
    t_class *c ;
    c = class_new("template", (method)template_new, (method)template_free, (long)sizeof(t_template), 0L, A_GIMME, 0);
    
    //binds a C function to a text symbol. The two methods defined here are int and bang.
    class_addmethod(c, (method)template_bang,       "bang",             0);
    class_addmethod(c, (method)template_int,        "int",      A_LONG, 0);
    class_addmethod(c, (method)template_float,		"float",	A_FLOAT,0);
    
    class_addmethod(c, (method)template_assist,     "assist",	A_CANT, 0);
    class_addmethod(c, (method)template_anything,   "anything", A_GIMME, 0);
    class_addmethod(c, (method)template_dblclick,   "dblclick",  A_CANT, 0);
    
    //Add a new attribute to the specified attribute to specify that it should appear in the inspector's Basic tab.
    class_addmethod(c, (method)template_identify,   "identify",         0);
    
    class_addmethod(c, (method)template_in0,        "int",      A_LONG, 0);
    class_addmethod(c, (method)template_in1,        "in1",      A_LONG, 0);
    
    CLASS_ATTR_SYM(c, "name", 0, t_template, name);
    
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
    
    //Give our object two outlets
    // x->out0 = intout((t_object *)x);
    // Theses outlets are type-specific, meaning that we will always send the same type of message through them. If you want to create outlets that can send any message, use outlet_new().
    //outlet_new((t_object *)x, NULL);    //NULL indicates the outlet will be used to send various messages
    x->out_0 = floatout((t_object *)x);
    x->out_1 = floatout((t_object *)x);
    
    intin(x, 1);

    // passing your object, a non-zero code value associated with the proxy, and a pointer to your object's inlet number location.
    // additonally this creates the second inlet (i.e. inlet 1)
    //x->proxy = proxy_new((t_object *) x, 1, &x->in_n);
    // to make more inlets use     intin(x, 2);
    
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
            case 0: sprintf(s, "Input 0 : value 1"); break;
            case 1: sprintf(s, "Input 1 : value 2"); break;
        }
    }
    else {
        // outlet
        switch (a){
            case 0: sprintf(s, "Output 0 : Mult"); break;
            case 1: sprintf(s, "Output 1 : Add"); break;
        }
    }
}





//____________________________________________________________________
//                          Inlet Handlers
//____________________________________________________________________

// posts messages upon reception
void template_in0(t_template *x, long n)
{
    object_post((t_object *)x, "in 0 got : %ld", n);
    atom_setlong(&x->val_0, n);
}

void template_in1(t_template *x, long n)
{
    object_post((t_object *)x, "in 1 got : %ld", n);
    atom_setlong(&x->val_1, n);

}





//____________________________________________________________________
//                          Message Handlers
//____________________________________________________________________

/*
 
 The C functions you write will be declared according to the arguments the message requires. All functions are passed a pointer to your object as the first argument.
 
 */

//This simply copies the value of the argument to the internal storage within the instance. It stores it in one of the two values depending on which inlets the value was sent to. The bang function is then called, thus sending the values to the output
void template_int(t_template *x, long n)
{
    /*
    switch (proxy_getinlet((t_object *)x)) {
        case 0:
            atom_setlong(&x->val_0, n);
            post("int received in left inlet");
            break;
        case 1:
            atom_setlong(&x->val_1, n);
            post("int received in left inlet");
            break;
    }*/
    template_bang(x);
}

// Identical to previous function, used upon reception of float values.
void template_float(t_template *x, double f)
{/*
    switch (proxy_getinlet((t_object *)x)) {
        case 0:
            atom_setlong(&x->val_0, f);
            post("float received in left inlet");
            break;
        case 1:
            atom_setlong(&x->val_1, f);
            post("float received in left inlet");
            break;
    }*/
    template_bang(x);
}

// Function called upon reception of a bang on an inlet. Outputs values.
void template_bang(t_template *x)
{
    switch (x->val_0.a_type) {
        case A_LONG:
            outlet_int(x->out_0, atom_getlong(&x->val_0) * atom_getlong(&x->val_1));
            outlet_int(x->out_1, atom_getlong(&x->val_0) + atom_getlong(&x->val_1));
            break;
        case A_FLOAT:
            outlet_float(x->out_0, atom_getfloat(&x->val_0) * atom_getfloat(&x->val_1));
            outlet_float(x->out_1, atom_getfloat(&x->val_0) + atom_getfloat(&x->val_1));
            break;
        case A_SYM:
            object_post((t_object *)x, "i see symbols... not numbers");
            break;
        default: break;
    }
}





//____________________________________________________________________
//                          Perfomance Routines
//____________________________________________________________________
void template_anything(t_template *x, t_symbol *s, long ac, t_atom *av)
{
    atom_setsym(&x->val_0, s);
    template_bang(x);
}

void template_identify(t_template *x)
{
    object_post((t_object *)x, "my name is %s", x->name.s_name);
}

void template_dblclick(t_template *x)
{
    object_post((t_object *)x, "Here you go : ");
    template_bang(x);
}





//____________________________________________________________________
//                          Additional Routines
//____________________________________________________________________


