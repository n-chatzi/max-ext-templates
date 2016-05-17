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
    t_atom      val;        ///<	Value to use for the processing
    t_symbol    name;       ///<    Name of the Max objct
    void        *out0;      ///<    Output definition
    void        *out1;      ///<    Output definition

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
    t_class *c = class_new("template", (method)template_new, (method)template_free, (long)sizeof(t_template), 0L, A_GIMME, 0);
    
    //binds a C function to a text symbol. The two methods defined here are int and bang.
    class_addmethod(c, (method)template_bang,       "bang",             0);
    class_addmethod(c, (method)template_int,        "int",      A_LONG, 0);
    class_addmethod(c, (method)template_float,		"float",	A_FLOAT,0);
    
    class_addmethod(c, (method)template_assist,     "assist",	A_CANT, 0);
    class_addmethod(c, (method)template_anything,   "anything", A_GIMME, 0);
    class_addmethod(c, (method)template_dblclick,   "dbclick",  A_CANT, 0);
    
    //Add a new attribute to the specified attribute to specify that it should appear in the inspector's Basic tab.
    class_addmethod(c, (method)template_identify,   "identify",         0);
    
    class_addmethod(c, (method)template_in0,        "int",      A_LONG, 0);
    class_addmethod(c, (method)template_in1,        "in0",      A_LONG, 0);
    
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
    
    //Setup 2 inlets for our object
    
    //Give our object a signal outlet
    outlet_new((t_object *)x, "signal");
    
    
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
    atom_setlong(&x->val, n);
    template_bang(x);
}

//This simply copies the value of the argument to the internal storage within the instance.
void template_float(t_template *x, double f)
{
    atom_setfloat(&x->val, f);
    template_bang(x);
}

void template_bang(t_template *x)
{
    switch (x->val.a_type) {
        case A_LONG:
            outlet_int(x->out0, atom_getlong(&x->val));
            outlet_int(x->out1, atom_getlong(&x->val));
            break;
        case A_FLOAT:
            outlet_float(x->out0, atom_getfloat(&x->val));
            outlet_float(x->out1, atom_getfloat(&x->val));
            break;
        case A_SYM:
            outlet_anything(x->out0, atom_getsym(&x->val), 0, NULL);
            outlet_anything(x->out1, atom_getsym(&x->val), 0, NULL);
            break;
        default: break;
    }
}





//____________________________________________________________________
//                          Perfomance Routines
//____________________________________________________________________
void template_anything(t_template *x, t_symbol *s, long ac, t_atom *av)
{
    
}

void template_identify(t_template *x)
{
    object_post((t_object *)x, "my name is %s", x->name.s_name);
}

void template_dblclick(t_template *x)
{
    object_post((t_object *)x, "I got a double-click");
}





//____________________________________________________________________
//                          Additional Routines
//____________________________________________________________________


