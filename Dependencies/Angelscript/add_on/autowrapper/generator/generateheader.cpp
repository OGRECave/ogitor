//
// This generator creates a header file that implements automatic
// wrapper functions for the generic calling convention.
//
// Originally implemented by George Yohng from 4Front Technologies in 2009-03-11
// Modifications by Pierre Fortin in order to add constructor wrapper generation
//

#include <stdio.h>

// Generate templates for up to this number of function parameters
const int MAXPARAM = 10;


int main()
{
    printf("#ifndef ASWRAPPEDCALL_H\n"
           "#define ASWRAPPEDCALL_H\n\n");

	// Add some instructions on how to use this 
	printf("// Generate the wrappers by calling the macros in global scope. \n");
	printf("// Then register the wrapper function with the script engine using the asCALL_GENERIC \n");
	printf("// calling convention. The wrapper can handle both global functions and class methods.\n");
	printf("// The wrapper can also handle OBJLAST & OBJFIRST methods as well.\n");
	printf("// or a asCALL_CDECL_OBJLAST.\n");
	printf("//\n");
	printf("// Example:\n");
	printf("//\n");
	printf("// asDECLARE_FUNCTION_WRAPPER(MyGenericWrapper, MyRealFunction);\n");
	printf("// asDECLARE_FUNCTION_WRAPPERPR(MyGenericOverloadedWrapper, MyOverloadedFunction, (int), void);\n");
	printf("// asDECLARE_METHOD_WRAPPER(MyGenericMethodWrapper, MyClass, Method);\n");
	printf("// asDECLARE_METHOD_WRAPPERPR(MyGenericOverloadedMethodWrapper, MyClass, Method, (int) const, void);\n");
	printf("// asDECLARE_FUNCTION_OBJ_WRAPPER(MyGenericMethodWrapper, MyRealFunction);\n");
	printf("// asDECLARE_FUNCTION_OBJ_WRAPPERPR(MyGenericOverloadedMethodWrapper, MyOverloadedFunction, (int), void);\n");
	printf("//\n");
    printf("// This file was generated to accept functions with a maximum of %d parameters.\n\n", MAXPARAM);

	// Include files
	printf("#include <new> // placement new\n");
    printf("#include <angelscript.h>\n\n");

	// This is the macro that should be used to implement the wrappers
    printf("#define asDECLARE_FUNCTION_WRAPPER(wrapper_name,func) \\\n"
           "    static void wrapper_name(asIScriptGeneric *gen)\\\n"
           "    { \\\n"
           "        asCallWrappedFunc(&func,gen);\\\n"
           "    }\n\n" );
    printf("#define asDECLARE_FUNCTION_WRAPPERPR(wrapper_name,func,params,rettype) \\\n"
           "    static void wrapper_name(asIScriptGeneric *gen)\\\n"
           "    { \\\n"
		   "        asCallWrappedFunc((rettype (*)params)(&func),gen);\\\n"
           "    }\n\n" );
    printf("#define asDECLARE_FUNCTION_OBJ_WRAPPER(wrapper_name,func,objfirst) \\\n"
        "    static void wrapper_name(asIScriptGeneric *gen)\\\n"
        "    { \\\n"
        "        asCallWrappedFuncObj<objfirst>::Call(&func,gen);\\\n"
        "    }\n\n" );
    printf("#define asDECLARE_FUNCTION_OBJ_WRAPPERPR(wrapper_name,func,objfirst,params,rettype) \\\n"
        "    static void wrapper_name(asIScriptGeneric *gen)\\\n"
        "    { \\\n"
        "        asCallWrappedFuncObj<objfirst>::Call((rettype (*)params)(&func),gen);\\\n"
        "    }\n\n" );
    printf("#define asDECLARE_METHOD_WRAPPER(wrapper_name,cl,func) \\\n"
           "    static void wrapper_name(asIScriptGeneric *gen)\\\n"
           "    { \\\n"
		   "        asCallWrappedFunc(&cl::func,gen);\\\n"
           "    }\n\n" );
    printf("#define asDECLARE_METHOD_WRAPPERPR(wrapper_name,cl,func,params,rettype) \\\n"
           "    static void wrapper_name(asIScriptGeneric *gen)\\\n"
           "    { \\\n"
		   "        asCallWrappedFunc((rettype (cl::*)params)(&cl::func),gen);\\\n"
           "    }\n\n" );

	printf("// A helper class to accept reference parameters\n");
    printf("template<typename X>\n"
           "class as_wrapNative_helper\n"
           "{\n"
           "public:\n"
           "    X d;\n"
           "    as_wrapNative_helper(X d_) : d(d_) {}\n"
           "private:\n"
           "    // These are declared to avoid compiler warnings\n"
           "    as_wrapNative_helper(const as_wrapNative_helper&);\n"
           "    as_wrapNative_helper& operator=(const as_wrapNative_helper&);\n"
           "};\n\n");

    printf("// A helper class to statically branch on the right wrapper for constructors\n");
    printf("template<bool ObjFirst>\n"
           "class asCallWrappedFuncObj;\n"
           "\n"
           "template<>\n"
           "struct asCallWrappedFuncObj<true>\n"
           "{\n"
           "    template<typename Ctor>\n"
           "    static void Call(Ctor ctor, asIScriptGeneric *gen)\n"
           "    {\n"
           "        asCallWrappedFuncObjFirst(ctor, gen);\n"
           "    }\n"
           "};\n"
           "\n"
           "template<>\n"
           "struct asCallWrappedFuncObj<false>\n"
           "{\n"
           "    template<typename Ctor>\n"
           "    static void Call(Ctor ctor, asIScriptGeneric *gen)\n"
           "    {\n"
           "        asCallWrappedFuncObjLast(ctor, gen);\n"
           "    }\n"
           "};\n"
           "\n");

	// Iterate over the number of parameters 
    for(int t = 0; t <= MAXPARAM; t++)
    {
        int k;

		printf("// %d parameter(s)\n\n", t);

        // CDecl methods ( Function receiving a pointer to the object as input )

        // OBJFIRST
        printf("template<typename C");

        for (k = 0; k < t; ++k)
            printf(", typename T%d", k+1);

        printf(">");

        printf("\nstatic void asCallWrappedFuncObjFirst(void (*func)(C*");

        for (k = 0; k < t; ++k)
            printf(",T%d", k+1);

        printf("),asIScriptGeneric *gen)");
        printf("\n{");
        printf("\n    func(static_cast<C*>(gen->GetObject())");
        
        for (k = 0; k < t; ++k)
            printf(", ((as_wrapNative_helper<T%d> *)gen->GetAddressOfArg(%d))->d", k+1, k);
    
        printf(");\n}\n\n");

        // OBJLAST
        printf("template<typename C");

        for (k = 0; k < t; ++k)
            printf(", typename T%d", k+1);

        printf(">");

        printf("\nstatic void asCallWrappedFuncObjLast(void (*func)(");

        for (k = 0; k < t; ++k)
            printf("T%d,", k+1);

        printf("C*),asIScriptGeneric* gen)");
        printf("\n{");
        printf("\n    func(");

        for (k = 0; k < t; ++k)
            printf("((as_wrapNative_helper<T%d> *)gen->GetAddressOfArg(%d))->d,", k+1, k);

        printf("static_cast<C*>(gen->GetObject()));\n}\n\n");

		// Iterate over the different function forms
        for(int d = 0; d < 6; d++)
        {
			// Different forms of the function
            static const char *start[]=
            {"template<",                      // global function with no return type
             "template<typename R",            // global function with return type
             "template<typename C",            // class method with no return type
             "template<typename C,typename R", // class method with return type
             "template<typename C",            // const class method with no return type
             "template<typename C,typename R"  // const class method with return type
            };

            static const char *start2[]=
            {"<",
             "<R",
             "<C",
             "<C,R",
             "<C",
             "<C,R"
            };

            static const char *signature[]=
            {"_void",
             "",
             "_void_this",
             "_this",
             "_void_this_const",
             "_this_const"
            };

			static const char *constness[] =
			{"",
			 "",
			 "",
			 "",
			 " const",
			 " const"
			};

			//----------
			// Generate the function that extracts the parameters from 
			// the asIScriptGeneric interface and calls the native function

			// Build the template declaration
            if( (t>0) || (d>0) )
            {
                printf("%s",start[d]);

                for(int k=0;k<t;k++)
                    printf("%stypename T%d",(k||(d>0))?",":"",k+1);

                printf(">\n");
            }           

            printf("static void asWrapNative_p%d%s(",t,signature[d]);
			printf("%s (%s*func)(" ,(d&1)?"R":"void", (d>1)?"C::":"");
            for(k=0;k<t;k++)
                printf("%sT%d",k?",":"",k+1);
            printf(")%s", constness[d]);
			printf(",asIScriptGeneric *%s)\n",((t>0)||(d>0))?"gen":"");
            printf("{\n");
            printf("    %s%s(",(d&1)?"new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ":"",

                   (d>1)?"((*((C*)gen->GetObject())).*func)":"func");

            for( k = 0; k < t; k++ )
                printf("%s ((as_wrapNative_helper<T%d> *)gen->GetAddressOfArg(%d))->d",k?",":"",k+1,k);

            printf(" )%s;\n"
                   "}\n\n",(d&1)?")":"");

			//----------
			// Generate the function that calls the templated wrapper function.
			// This is overloads for the asCallWrappedFunc functions

			// Build the template declaration
            if( (t>0) || (d>0) )
            {
                printf("%s",start[d]);

                for(int k=0;k<t;k++)
                    printf("%stypename T%d", (k||(d>0))?",":"", k+1);

                printf(">\n");
            }

            printf("inline void asCallWrappedFunc(%s (%s*func)(", (d&1)?"R":"void", (d>1)?"C::":"");

            for( k =0; k < t; k++ )
                printf("%sT%d",k?",":"",k+1);

            printf(")%s,asIScriptGeneric *gen)\n"
                   "{\n"
                   "    asWrapNative_p%d%s",constness[d],t,signature[d]);

            if( (t>0) || (d>0) )
            {
                printf("%s",start2[d]);

                for( int k = 0; k < t; k++ )
                    printf("%sT%d",(k||(d>0))?",":"",k+1);

                printf(">");
            }
            printf("(func,gen);\n"
                   "}\n\n");
        }

    }    

    printf("#endif // ASWRAPPEDCALL_H\n\n");

    return 0;
}

