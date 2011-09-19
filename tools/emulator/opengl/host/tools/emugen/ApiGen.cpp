/*
* Copyright (C) 2011 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "ApiGen.h"
#include "EntryPoint.h"
#include <stdio.h>
#include <stdlib.h>
#include "strUtils.h"
#include <errno.h>
#include <sys/types.h>

/* Define this to 1 to enable support for the 'isLarge' variable flag
 * that instructs the encoder to send large data buffers by a direct
 * write through the pipe (i.e. without copying it into a temporary
 * buffer. This has definite performance benefits when using a QEMU Pipe.
 *
 * Set to 0 otherwise.
 */
#define WITH_LARGE_SUPPORT  1

EntryPoint * ApiGen::findEntryByName(const std::string & name)
{
    EntryPoint * entry = NULL;

    size_t n = this->size();
    for (size_t i = 0; i < n; i++) {
        if (at(i).name() == name) {
            entry = &(at(i));
            break;
        }
    }
    return entry;
}

void ApiGen::printHeader(FILE *fp) const
{
    fprintf(fp, "// Generated Code - DO NOT EDIT !!\n");
    fprintf(fp, "// generated by 'emugen'\n");
}

int ApiGen::genProcTypes(const std::string &filename, SideType side)
{
    FILE *fp = fopen(filename.c_str(), "wt");
    if (fp == NULL) {
        perror(filename.c_str());
        return -1;
    }
    printHeader(fp);

    const char* basename = m_basename.c_str();

    fprintf(fp, "#ifndef __%s_%s_proc_t_h\n", basename, sideString(side));
    fprintf(fp, "#define __%s_%s_proc_t_h\n", basename, sideString(side));
    fprintf(fp, "\n\n");
    fprintf(fp, "\n#include \"%s_types.h\"\n",basename);
    fprintf(fp, "#ifndef %s_APIENTRY\n",basename);
    fprintf(fp, "#define %s_APIENTRY \n",basename);
    fprintf(fp, "#endif\n");


    for (size_t i = 0; i < size(); i++) {
        EntryPoint *e = &at(i);

        fprintf(fp, "typedef ");
        e->retval().printType(fp);
        fprintf(fp, " (%s_APIENTRY *%s_%s_proc_t) (", basename, e->name().c_str(), sideString(side));
        if (side == CLIENT_SIDE) { fprintf(fp, "void * ctx"); }
        if (e->customDecoder() && side == SERVER_SIDE) { fprintf(fp, "void *ctx"); }

        VarsArray & evars = e->vars();
        size_t n = evars.size();

        for (size_t j = 0; j < n; j++) {
            if (!evars[j].isVoid()) {
                if (j != 0 || side == CLIENT_SIDE || (side == SERVER_SIDE && e->customDecoder())) fprintf(fp, ", ");
                evars[j].printType(fp);
            }
        }
        fprintf(fp, ");\n");
    }
    fprintf(fp, "\n\n#endif\n");
    return 0;
}

int ApiGen::genFuncTable(const std::string &filename, SideType side)
{
    FILE *fp = fopen(filename.c_str(), "wt");
    if (fp == NULL) {
        perror(filename.c_str());
        return -1;
    }
    printHeader(fp);

    fprintf(fp, "#ifndef __%s_%s_ftable_t_h\n", m_basename.c_str(), sideString(side));
    fprintf(fp, "#define __%s_%s_ftable_t_h\n", m_basename.c_str(), sideString(side));
    fprintf(fp, "\n\n");
    fprintf(fp, "static struct _%s_funcs_by_name {\n", m_basename.c_str());
    fprintf(fp,
            "\tconst char *name;\n" \
            "\tvoid *proc;\n" \
            "} %s_funcs_by_name[] = {\n", m_basename.c_str());


    for (size_t i = 0; i < size(); i++) {
        EntryPoint *e = &at(i);
        if (e->notApi()) continue;
        fprintf(fp, "\t{\"%s\", (void*)%s},\n", e->name().c_str(), e->name().c_str());
    }
    fprintf(fp, "};\n");
    fprintf(fp, "static int %s_num_funcs = sizeof(%s_funcs_by_name) / sizeof(struct _%s_funcs_by_name);\n",
            m_basename.c_str(), m_basename.c_str(), m_basename.c_str());
    fprintf(fp, "\n\n#endif\n");
    return 0;
}


int ApiGen::genContext(const std::string & filename, SideType side)
{
    FILE *fp = fopen(filename.c_str(), "wt");
    if (fp == NULL) {
        perror(filename.c_str());
        return -1;
    }
    printHeader(fp);

    fprintf(fp, "#ifndef __%s_%s_context_t_h\n", m_basename.c_str(), sideString(side));
    fprintf(fp, "#define __%s_%s_context_t_h\n", m_basename.c_str(), sideString(side));

    //  fprintf(fp, "\n#include \"%s_types.h\"\n", m_basename.c_str());
    fprintf(fp, "\n#include \"%s_%s_proc.h\"\n", m_basename.c_str(), sideString(side));

    StringVec & contextHeaders = side == CLIENT_SIDE ? m_clientContextHeaders : m_serverContextHeaders;
    for (size_t i = 0; i < contextHeaders.size(); i++) {
        fprintf(fp, "#include %s\n", contextHeaders[i].c_str());
    }
    fprintf(fp, "\n");

    fprintf(fp, "\nstruct %s_%s_context_t {\n\n", m_basename.c_str(), sideString(side));
    for (size_t i = 0; i < size(); i++) {
        EntryPoint *e = &at(i);
        fprintf(fp, "\t%s_%s_proc_t %s;\n", e->name().c_str(), sideString(side), e->name().c_str());
    }
    // accessors
    fprintf(fp, "\t//Accessors \n");

    for (size_t i = 0; i < size(); i++) {
        EntryPoint *e = &at(i);
        const char *n = e->name().c_str();
        const char *s = sideString(side);
        fprintf(fp, "\tvirtual %s_%s_proc_t set_%s(%s_%s_proc_t f) { %s_%s_proc_t retval = %s; %s = f; return retval;}\n", n, s, n, n, s, n, s,  n, n);
    }

    // virtual destructor
    fprintf(fp, "\t virtual ~%s_%s_context_t() {}\n", m_basename.c_str(), sideString(side));
    // accessor
    if (side == CLIENT_SIDE || side == WRAPPER_SIDE) {
        fprintf(fp, "\n\ttypedef %s_%s_context_t *CONTEXT_ACCESSOR_TYPE(void);\n",
                m_basename.c_str(), sideString(side));
        fprintf(fp, "\tstatic void setContextAccessor(CONTEXT_ACCESSOR_TYPE *f);\n");
    }

    // init function
    fprintf(fp, "\tint initDispatchByName( void *(*getProc)(const char *name, void *userData), void *userData);\n");

    //client site set error virtual func
    if (side == CLIENT_SIDE) {
        fprintf(fp, "\tvirtual void setError(unsigned int  error){};\n");
        fprintf(fp, "\tvirtual unsigned int getError(){ return 0; };\n");
    }

    fprintf(fp, "};\n");

    fprintf(fp, "\n#endif\n");
    fclose(fp);
    return 0;
}

int ApiGen::genEntryPoints(const std::string & filename, SideType side)
{

    if (side != CLIENT_SIDE && side != WRAPPER_SIDE) {
        fprintf(stderr, "Entry points are only defined for Client and Wrapper components\n");
        return -999;
    }


    FILE *fp = fopen(filename.c_str(), "wt");
    if (fp == NULL) {
        perror(filename.c_str());
        return errno;
    }

    printHeader(fp);
    fprintf(fp, "#include <stdio.h>\n");
    fprintf(fp, "#include <stdlib.h>\n");
    fprintf(fp, "#include \"%s_%s_context.h\"\n", m_basename.c_str(), sideString(side));
    fprintf(fp, "\n");

    fprintf(fp, "#ifndef GL_TRUE\n");
    fprintf(fp, "extern \"C\" {\n");

    for (size_t i = 0; i < size(); i++) {
        fprintf(fp, "\t"); at(i).print(fp, false); fprintf(fp, ";\n");
    }
    fprintf(fp, "};\n\n");
    fprintf(fp, "#endif\n");

    fprintf(fp, "#ifndef GET_CONTEXT\n");
    fprintf(fp, "static %s_%s_context_t::CONTEXT_ACCESSOR_TYPE *getCurrentContext = NULL;\n",
            m_basename.c_str(), sideString(side));

    fprintf(fp,
            "void %s_%s_context_t::setContextAccessor(CONTEXT_ACCESSOR_TYPE *f) { getCurrentContext = f; }\n",
            m_basename.c_str(), sideString(side));
    fprintf(fp, "#define GET_CONTEXT %s_%s_context_t * ctx = getCurrentContext() \n",
                m_basename.c_str(), sideString(side));
    fprintf(fp, "#endif\n\n");


    for (size_t i = 0; i < size(); i++) {
        EntryPoint *e = &at(i);
        e->print(fp);
        fprintf(fp, "{\n");
        fprintf(fp, "\tGET_CONTEXT; \n");

        bool shouldReturn = !e->retval().isVoid();
        bool shouldCallWithContext = (side == CLIENT_SIDE);
        //param check
        if (shouldCallWithContext) {
            for (size_t j=0; j<e->vars().size(); j++) {
                if (e->vars()[j].paramCheckExpression() != "") 
                    fprintf(fp, "\t%s\n", e->vars()[j].paramCheckExpression().c_str());
            }
        }
        fprintf(fp, "\t %sctx->%s(%s",
                shouldReturn ? "return " : "",
                e->name().c_str(),
                shouldCallWithContext ? "ctx" : "");
        size_t nvars = e->vars().size();

        for (size_t j = 0; j < nvars; j++) {
            if (!e->vars()[j].isVoid()) {
                fprintf(fp, "%s %s",
                        j != 0 || shouldCallWithContext ? "," : "",
                        e->vars()[j].name().c_str());
            }
        }
        fprintf(fp, ");\n");
        fprintf(fp, "}\n\n");
    }
    fclose(fp);
    return 0;
}


int ApiGen::genOpcodes(const std::string &filename)
{
    FILE *fp = fopen(filename.c_str(), "wt");
    if (fp == NULL) {
        perror(filename.c_str());
        return errno;
    }

    printHeader(fp);
    fprintf(fp, "#ifndef __GUARD_%s_opcodes_h_\n", m_basename.c_str());
    fprintf(fp, "#define __GUARD_%s_opcodes_h_\n\n", m_basename.c_str());
    for (size_t i = 0; i < size(); i++) {
        fprintf(fp, "#define OP_%s \t\t\t\t\t%u\n", at(i).name().c_str(), (unsigned int)i + m_baseOpcode);
    }
    fprintf(fp, "#define OP_last \t\t\t\t\t%u\n", (unsigned int)size() + m_baseOpcode);
    fprintf(fp,"\n\n#endif\n");
    fclose(fp);
    return 0;

}
int ApiGen::genAttributesTemplate(const std::string &filename )
{
    FILE *fp = fopen(filename.c_str(), "wt");
    if (fp == NULL) {
        perror(filename.c_str());
        return -1;
    }

    for (size_t i = 0; i < size(); i++) {
        if (at(i).hasPointers()) {
            fprintf(fp, "#");
            at(i).print(fp);
            fprintf(fp, "%s\n\n", at(i).name().c_str());
        }
    }
    fclose(fp);
    return 0;
}

int ApiGen::genEncoderHeader(const std::string &filename)
{
    FILE *fp = fopen(filename.c_str(), "wt");
    if (fp == NULL) {
        perror(filename.c_str());
        return -1;
    }

    printHeader(fp);
    std::string classname = m_basename + "_encoder_context_t";

    fprintf(fp, "\n#ifndef GUARD_%s\n", classname.c_str());
    fprintf(fp, "#define GUARD_%s\n\n", classname.c_str());

    fprintf(fp, "#include \"IOStream.h\"\n");
    fprintf(fp, "#include \"%s_%s_context.h\"\n\n\n", m_basename.c_str(), sideString(CLIENT_SIDE));

    for (size_t i = 0; i < m_encoderHeaders.size(); i++) {
        fprintf(fp, "#include %s\n", m_encoderHeaders[i].c_str());
    }
    fprintf(fp, "\n");

    fprintf(fp, "struct %s : public %s_%s_context_t {\n\n",
            classname.c_str(), m_basename.c_str(), sideString(CLIENT_SIDE));
    fprintf(fp, "\tIOStream *m_stream;\n\n");

    fprintf(fp, "\t%s(IOStream *stream);\n\n", classname.c_str());
    fprintf(fp, "\n};\n\n");

    fprintf(fp,"extern \"C\" {\n");

    for (size_t i = 0; i < size(); i++) {
        fprintf(fp, "\t");
        at(i).print(fp, false, "_enc", /* classname + "::" */"", "void *self");
        fprintf(fp, ";\n");
    }
    fprintf(fp, "};\n");
    fprintf(fp, "#endif");

    fclose(fp);
    return 0;
}

// Format the byte length expression for a given variable into a user-provided buffer
// If the variable type is not a pointer, this is simply its size as a decimal constant
// If the variable is a pointer, this will be an expression provided by the .attrib file
// through the 'len' attribute.
//
// Returns 1 if the variable is a pointer, 0 otherwise
//
static int getVarEncodingSizeExpression(Var&  var, EntryPoint* e, char* buff, size_t bufflen)
{
    int ret = 0;
    if (!var.isPointer()) {
        snprintf(buff, bufflen, "%u", (unsigned int) var.type()->bytes());
    } else {
        ret = 1;
        const char* lenExpr = var.lenExpression().c_str();
        const char* varname = var.name().c_str();
        if (e != NULL && lenExpr[0] == '\0') {
            fprintf(stderr, "%s: data len is undefined for '%s'\n",
                    e->name().c_str(), varname);
        }
        if (var.nullAllowed()) {
            snprintf(buff, bufflen, "((%s != NULL) ? %s : 0)", varname, lenExpr);
        } else {
            snprintf(buff, bufflen, "%s", lenExpr);
        }
    }
    return ret;
}

static int writeVarEncodingSize(Var& var, FILE* fp)
{
    int ret = 0;
    if (!var.isPointer()) {
        fprintf(fp, "%u", (unsigned int) var.type()->bytes());
    } else {
        ret = 1;
        fprintf(fp, "__size_%s", var.name().c_str());
    }
    return ret;
}



static void writeVarEncodingExpression(Var& var, FILE* fp)
{
    const char* varname = var.name().c_str();

    if (var.isPointer()) {
        // encode a pointer header
        fprintf(fp, "\t*(unsigned int *)(ptr) = __size_%s; ptr += 4;\n", varname);

        Var::PointerDir dir = var.pointerDir();
        if (dir == Var::POINTER_INOUT || dir == Var::POINTER_IN) {
            if (var.nullAllowed()) {
                fprintf(fp, "\tif (%s != NULL) ", varname);
            } else {
                fprintf(fp, "\t");
            }

            if (var.packExpression().size() != 0) {
                fprintf(fp, "%s;", var.packExpression().c_str());
            } else {
                fprintf(fp, "memcpy(ptr, %s, __size_%s);",
                        varname, varname);
            }

            fprintf(fp, "ptr += __size_%s;\n", varname);
        }
    } else {
        // encode a non pointer variable
        if (!var.isVoid()) {
            fprintf(fp, "\t*(%s *) (ptr) = %s; ptr += %u;\n",
                    var.type()->name().c_str(), varname,
                    (uint) var.type()->bytes());
        }
    }
}

#if WITH_LARGE_SUPPORT
static void writeVarLargeEncodingExpression(Var& var, FILE* fp)
{
    const char* varname = var.name().c_str();

    fprintf(fp, "\tstream->writeFully(&__size_%s,4);\n", varname);
    if (var.nullAllowed()) {
        fprintf(fp, "\tif (%s != NULL) ", varname);
    } else {
        fprintf(fp, "\t");
    }
    fprintf(fp, "stream->writeFully(%s, __size_%s);\n", varname, varname);
}
#endif /* WITH_LARGE_SUPPORT */

int ApiGen::genEncoderImpl(const std::string &filename)
{
    FILE *fp = fopen(filename.c_str(), "wt");
    if (fp == NULL) {
        perror(filename.c_str());
        return -1;
    }

    printHeader(fp);
    fprintf(fp, "\n\n#include <string.h>\n");
    fprintf(fp, "#include \"%s_opcodes.h\"\n\n", m_basename.c_str());
    fprintf(fp, "#include \"%s_enc.h\"\n\n\n", m_basename.c_str());
    fprintf(fp, "#include <stdio.h>\n");
    std::string classname = m_basename + "_encoder_context_t";
    size_t n = size();

    // unsupport printout
    fprintf(fp, "static void enc_unsupported()\n{\n\tLOGE(\"Function is unsupported\\n\");\n}\n\n");

    // entry points;
    for (size_t i = 0; i < n; i++) {
        EntryPoint *e = &at(i);

        if (e->unsupported()) continue;


        e->print(fp, true, "_enc", /* classname + "::" */"", "void *self");
        fprintf(fp, "{\n");

//      fprintf(fp, "\n\tDBG(\">>>> %s\\n\");\n", e->name().c_str());
        fprintf(fp, "\n\t%s *ctx = (%s *)self;\n",
                classname.c_str(),
                classname.c_str());
        fprintf(fp, "\tIOStream *stream = ctx->m_stream;\n\n");
        VarsArray & evars = e->vars();
        size_t  maxvars = evars.size();
        size_t  j;

        char    buff[256];

        // Define the __size_XXX variables that contain the size of data
        // associated with pointers.
        for (j = 0; j < maxvars; j++) {
            Var& var = evars[j];

            if (!var.isPointer())
                continue;

            const char* varname = var.name().c_str();
            fprintf(fp, "\tconst unsigned int __size_%s = ", varname);

            getVarEncodingSizeExpression(var, e, buff, sizeof(buff));
            fprintf(fp, "%s;\n", buff);
        }

#if WITH_LARGE_SUPPORT
        // We need to take care of 'isLarge' variable in a special way
        // Anything before an isLarge variable can be packed into a single
        // buffer, which is then commited. Each isLarge variable is a pointer
        // to data that can be written to directly through the pipe, which
        // will be instant when using a QEMU pipe

        size_t  nvars   = 0;
        size_t  npointers = 0;

        // First, compute the total size, 8 bytes for the opcode + payload size
        fprintf(fp, "\t unsigned char *ptr;\n");
        fprintf(fp, "\t const size_t packetSize = 8");

        for (j = 0; j < maxvars; j++) {
            fprintf(fp, " + ");
            npointers += writeVarEncodingSize(evars[j], fp);
        }
        if (npointers > 0) {
            fprintf(fp, " + %u*4", npointers);
        }
        fprintf(fp, ";\n");

        // We need to divide the packet into fragments. Each fragment contains
        // either copied arguments to a temporary buffer, or direct writes for
        // large variables.
        //
        // The first fragment must also contain the opcode+payload_size
        //
        nvars = 0;
        while (nvars < maxvars || maxvars == 0) {

            // Skip over non-large fields
            for (j = nvars; j < maxvars; j++) {
                if (evars[j].isLarge())
                    break;
            }

            // Write a fragment if needed.
            if (nvars == 0 || j > nvars) {
                const char* plus = "";

                if (nvars == 0 && j == maxvars) {
                    // Simple shortcut for the common case where we don't have large variables;
                    fprintf(fp, "\tptr = stream->alloc(packetSize);\n");

                } else {
                    // allocate buffer from the stream until the first large variable
                    fprintf(fp, "\tptr = stream->alloc(");
                    plus = "";

                    if (nvars == 0) {
                        fprintf(fp,"8"); plus = " + ";
                    }
                    if (j > nvars) {
                        npointers = 0;
                        for (j = nvars; j < maxvars && !evars[j].isLarge(); j++) {
                            fprintf(fp, "%s", plus); plus = " + ";
                            npointers += writeVarEncodingSize(evars[j], fp);
                        }
                        if (npointers > 0) {
                            fprintf(fp, "%s%u*4", plus, npointers); plus = " + ";
                        }
                    }
                    fprintf(fp,");\n");
                }

                // encode packet header if needed.
                if (nvars == 0) {
                    fprintf(fp, "\t*(unsigned int *)(ptr) = OP_%s; ptr += 4;\n", e->name().c_str());
                    fprintf(fp, "\t*(unsigned int *)(ptr) = (unsigned int) packetSize; ptr += 4;\n");
                }

                if (maxvars == 0)
                    break;

                // encode non-large fields in this fragment
                for (j = nvars; j < maxvars && !evars[j].isLarge(); j++) {
                    writeVarEncodingExpression(evars[j],fp);
                }

                // Ensure the fragment is commited if it is followed by a large variable
                if (j < maxvars) {
                    fprintf(fp, "\tstream->flush();\n");
                }
            }

            // If we have one or more large variables, write them directly.
            // As size + data
            for ( ; j < maxvars && evars[j].isLarge(); j++) {
                writeVarLargeEncodingExpression(evars[j], fp);
            }

            nvars = j;
        }

#else /* !WITH_LARGE_SUPPORT */
        size_t nvars = evars.size();
        size_t npointers = 0;
        fprintf(fp, "\t const size_t packetSize = 8");
        for (size_t j = 0; j < nvars; j++) {
            npointers += getVarEncodingSizeExpression(evars[j],e,buff,sizeof(buff));
            fprintf(fp, " + %s", buff);
        }
        fprintf(fp, " + %u * 4;\n", (unsigned int) npointers);

        // allocate buffer from the stream;
        fprintf(fp, "\t unsigned char *ptr = stream->alloc(packetSize);\n\n");

        // encode into the stream;
        fprintf(fp, "\t*(unsigned int *)(ptr) = OP_%s; ptr += 4;\n",  e->name().c_str());
        fprintf(fp, "\t*(unsigned int *)(ptr) = (unsigned int) packetSize;  ptr += 4;\n\n");

        // out variables
        for (size_t j = 0; j < nvars; j++) {
            writeVarEncodingExpression(evars[j], fp);
        }
#endif /* !WITH_LARGE_SUPPORT */

        // in variables;
        for (size_t j = 0; j < nvars; j++) {
            if (evars[j].isPointer()) {
                Var::PointerDir dir = evars[j].pointerDir();
                if (dir == Var::POINTER_INOUT || dir == Var::POINTER_OUT) {
                    const char* varname = evars[j].name().c_str();
                    if (evars[j].nullAllowed()) {
                        fprintf(fp, "\tif (%s != NULL) ",varname);
                    } else {
                        fprintf(fp, "\t");
                    }
                    fprintf(fp, "stream->readback(%s, __size_%s);\n",
                            varname, varname);
                }
            }
        }
//XXX       fprintf(fp, "\n\tDBG(\"<<<< %s\\n\");\n", e->name().c_str());
        // todo - return value for pointers
        if (e->retval().isPointer()) {
            fprintf(stderr, "WARNING: %s : return value of pointer is unsupported\n",
                    e->name().c_str());
            fprintf(fp, "\t return NULL;\n");
        } else if (e->retval().type()->name() != "void") {
            fprintf(fp, "\n\t%s retval;\n", e->retval().type()->name().c_str());
            fprintf(fp, "\tstream->readback(&retval, %u);\n",(uint) e->retval().type()->bytes());
            fprintf(fp, "\treturn retval;\n");
        }
        fprintf(fp, "}\n\n");
    }

    // constructor
    fprintf(fp, "%s::%s(IOStream *stream)\n{\n", classname.c_str(), classname.c_str());
    fprintf(fp, "\tm_stream = stream;\n\n");

    for (size_t i = 0; i < n; i++) {
        EntryPoint *e = &at(i);
        if (e->unsupported()) {
            fprintf(fp, "\tset_%s((%s_%s_proc_t)(enc_unsupported));\n", e->name().c_str(), e->name().c_str(), sideString(CLIENT_SIDE));
        } else {
            fprintf(fp, "\tset_%s(%s_enc);\n", e->name().c_str(), e->name().c_str());
        }
        /**
           if (e->unsupsported()) {
           fprintf(fp, "\tmemcpy((void *)(&%s), (const void *)(&enc_unsupported), sizeof(%s));\n",
           e->name().c_str(),
           e->name().c_str());
           } else {
           fprintf(fp, "\t%s = %s_enc;\n", e->name().c_str(), e->name().c_str());
           }
        **/
    }
    fprintf(fp, "}\n\n");

    fclose(fp);
    return 0;
}


int ApiGen::genDecoderHeader(const std::string &filename)
{
    FILE *fp = fopen(filename.c_str(), "wt");
    if (fp == NULL) {
        perror(filename.c_str());
        return -1;
    }

    printHeader(fp);
    std::string classname = m_basename + "_decoder_context_t";

    fprintf(fp, "\n#ifndef GUARD_%s\n", classname.c_str());
    fprintf(fp, "#define GUARD_%s\n\n", classname.c_str());

    fprintf(fp, "#include \"IOStream.h\" \n");
    fprintf(fp, "#include \"%s_%s_context.h\"\n\n\n", m_basename.c_str(), sideString(SERVER_SIDE));

    for (size_t i = 0; i < m_decoderHeaders.size(); i++) {
        fprintf(fp, "#include %s\n", m_decoderHeaders[i].c_str());
    }
    fprintf(fp, "\n");

    fprintf(fp, "struct %s : public %s_%s_context_t {\n\n",
            classname.c_str(), m_basename.c_str(), sideString(SERVER_SIDE));
    fprintf(fp, "\tsize_t decode(void *buf, size_t bufsize, IOStream *stream);\n");
    fprintf(fp, "\n};\n\n");
    fprintf(fp, "#endif\n");

    fclose(fp);
    return 0;
}

int ApiGen::genContextImpl(const std::string &filename, SideType side)
{
    FILE *fp = fopen(filename.c_str(), "wt");
    if (fp == NULL) {
        perror(filename.c_str());
        return -1;
    }
    printHeader(fp);

    std::string classname = m_basename + "_" + sideString(side) + "_context_t";
    size_t n = size();
    fprintf(fp, "\n\n#include <string.h>\n");
    fprintf(fp, "#include \"%s_%s_context.h\"\n\n\n", m_basename.c_str(), sideString(side));
    fprintf(fp, "#include <stdio.h>\n\n");

    // init function;
    fprintf(fp, "int %s::initDispatchByName(void *(*getProc)(const char *, void *userData), void *userData)\n{\n", classname.c_str());
    fprintf(fp, "\tvoid *ptr;\n\n");
    for (size_t i = 0; i < n; i++) {
        EntryPoint *e = &at(i);
        fprintf(fp, "\tptr = getProc(\"%s\", userData); set_%s((%s_%s_proc_t)ptr);\n",
                e->name().c_str(),
                e->name().c_str(),
                e->name().c_str(),
                sideString(side));

    }
    fprintf(fp, "\treturn 0;\n");
    fprintf(fp, "}\n\n");
    fclose(fp);
    return 0;
}

int ApiGen::genDecoderImpl(const std::string &filename)
{
    FILE *fp = fopen(filename.c_str(), "wt");
    if (fp == NULL) {
        perror(filename.c_str());
        return -1;
    }

    printHeader(fp);

    std::string classname = m_basename + "_decoder_context_t";

    size_t n = size();

    fprintf(fp, "\n\n#include <string.h>\n");
    fprintf(fp, "#include \"%s_opcodes.h\"\n\n", m_basename.c_str());
    fprintf(fp, "#include \"%s_dec.h\"\n\n\n", m_basename.c_str());
    fprintf(fp, "#include <stdio.h>\n\n");

    // decoder switch;
    fprintf(fp, "size_t %s::decode(void *buf, size_t len, IOStream *stream)\n{\n", classname.c_str());
    fprintf(fp,
            "                           \n\
\tsize_t pos = 0;\n\
\tif (len < 8) return pos; \n\
\tunsigned char *ptr = (unsigned char *)buf;\n\
\tbool unknownOpcode = false;  \n\
#ifdef CHECK_GL_ERROR \n\
\tchar lastCall[256] = {0}; \n\
#endif \n\
\twhile ((len - pos >= 8) && !unknownOpcode) {   \n\
\t\tvoid *params[%u]; \n\
\t\tint opcode = *(int *)ptr;   \n\
\t\tunsigned int packetLen = *(int *)(ptr + 4);\n\
\t\tif (len - pos < packetLen)  return pos; \n\
\t\tswitch(opcode) {\n",
            (uint) m_maxEntryPointsParams);

    for (size_t f = 0; f < n; f++) {
        enum Pass_t { PASS_TmpBuffAlloc = 0, PASS_MemAlloc, PASS_DebugPrint, PASS_FunctionCall, PASS_Epilog, PASS_LAST };
        EntryPoint *e = &at(f);

        // construct a printout string;
        std::string printString = "";
        for (size_t i = 0; i < e->vars().size(); i++) {
            Var *v = &e->vars()[i];
            if (!v->isVoid())  printString += (v->isPointer() ? "%p(%u)" : v->type()->printFormat()) + " ";
        }
        printString += "";
        // TODO - add for return value;

        fprintf(fp, "\t\t\tcase OP_%s:\n", e->name().c_str());
        fprintf(fp, "\t\t\t{\n");

        bool totalTmpBuffExist = false;
        std::string totalTmpBuffOffset = "0";
        std::string *tmpBufOffset = new std::string[e->vars().size()];

        // construct retval type string
        std::string retvalType;
        if (!e->retval().isVoid()) {
            retvalType = e->retval().type()->name();
        }

        for (int pass = PASS_TmpBuffAlloc; pass < PASS_LAST; pass++) {
            if (pass == PASS_FunctionCall && !e->retval().isVoid() && !e->retval().isPointer()) {
                fprintf(fp, "\t\t\t*(%s *)(&tmpBuf[%s]) = ", retvalType.c_str(),
                        totalTmpBuffOffset.c_str());
            }


            if (pass == PASS_FunctionCall) {
                fprintf(fp, "\t\t\tthis->%s(", e->name().c_str());
                if (e->customDecoder()) {
                    fprintf(fp, "this"); // add a context to the call
                }
            } else if (pass == PASS_DebugPrint) {
                fprintf(fp, "#ifdef DEBUG_PRINTOUT\n");
                fprintf(fp, "\t\t\tfprintf(stderr,\"%s: %s(%s)\\n\"", m_basename.c_str(), e->name().c_str(), printString.c_str());
                if (e->vars().size() > 0 && !e->vars()[0].isVoid()) fprintf(fp, ",");
            }

            std::string varoffset = "8"; // skip the header
            VarsArray & evars = e->vars();
            // allocate memory for out pointers;
            for (size_t j = 0; j < evars.size(); j++) {
                Var *v = & evars[j];
                if (!v->isVoid()) {
                    if ((pass == PASS_FunctionCall) && (j != 0 || e->customDecoder())) fprintf(fp, ", ");
                    if (pass == PASS_DebugPrint && j != 0) fprintf(fp, ", ");

                    if (!v->isPointer()) {
                        if (pass == PASS_FunctionCall || pass == PASS_DebugPrint) {
                            fprintf(fp, "*(%s *)(ptr + %s)", v->type()->name().c_str(), varoffset.c_str());
                        }
                        varoffset += " + " + toString(v->type()->bytes());
                    } else {
                        if (v->pointerDir() == Var::POINTER_IN || v->pointerDir() == Var::POINTER_INOUT) {
                            if (pass == PASS_MemAlloc && v->pointerDir() == Var::POINTER_INOUT) {
                                fprintf(fp, "\t\t\tsize_t tmpPtr%uSize = (size_t)*(unsigned int *)(ptr + %s);\n",
                                        (uint) j, varoffset.c_str());
                                fprintf(fp, "unsigned char *tmpPtr%u = (ptr + %s + 4);\n",
                                        (uint) j, varoffset.c_str());
                            }
                            if (pass == PASS_FunctionCall) {
                                if (v->nullAllowed()) {
                                    fprintf(fp, "*((unsigned int *)(ptr + %s)) == 0 ? NULL : (%s)(ptr + %s + 4)",
                                            varoffset.c_str(), v->type()->name().c_str(), varoffset.c_str());
                                } else {
                                    fprintf(fp, "(%s)(ptr + %s + 4)",
                                            v->type()->name().c_str(), varoffset.c_str());
                                }
                            } else if (pass == PASS_DebugPrint) {
                                fprintf(fp, "(%s)(ptr + %s + 4), *(unsigned int *)(ptr + %s)",
                                        v->type()->name().c_str(), varoffset.c_str(),
                                        varoffset.c_str());
                            }
                            varoffset += " + 4 + *(size_t *)(ptr +" + varoffset + ")";
                        } else { // out pointer;
                            if (pass == PASS_TmpBuffAlloc) {
                                fprintf(fp, "\t\t\tsize_t tmpPtr%uSize = (size_t)*(unsigned int *)(ptr + %s);\n",
                                        (uint) j, varoffset.c_str());
                                if (!totalTmpBuffExist) {
                                    fprintf(fp, "\t\t\tsize_t totalTmpSize = tmpPtr%uSize;\n", (uint)j);
                                } else {
                                    fprintf(fp, "\t\t\ttotalTmpSize += tmpPtr%uSize;\n", (uint)j);
                                }
                                tmpBufOffset[j] = totalTmpBuffOffset;
                                char tmpPtrName[16];
                                sprintf(tmpPtrName," + tmpPtr%uSize", (uint)j);
                                totalTmpBuffOffset += std::string(tmpPtrName);
                                totalTmpBuffExist = true;
                            } else if (pass == PASS_MemAlloc) {
                                fprintf(fp, "\t\t\tunsigned char *tmpPtr%u = &tmpBuf[%s];\n",
                                        (uint)j, tmpBufOffset[j].c_str());
                            } else if (pass == PASS_FunctionCall) {
                                if (v->nullAllowed()) {
                                    fprintf(fp, "tmpPtr%uSize == 0 ? NULL : (%s)(tmpPtr%u)",
                                            (uint) j, v->type()->name().c_str(), (uint) j);
                                } else {
                                    fprintf(fp, "(%s)(tmpPtr%u)", v->type()->name().c_str(), (uint) j);
                                }
                            } else if (pass == PASS_DebugPrint) {
                                fprintf(fp, "(%s)(tmpPtr%u), *(unsigned int *)(ptr + %s)",
                                        v->type()->name().c_str(), (uint) j,
                                        varoffset.c_str());
                            }
                            varoffset += " + 4";
                        }
                    }
                }
            }

            if (pass == PASS_FunctionCall || pass == PASS_DebugPrint) fprintf(fp, ");\n");
            if (pass == PASS_DebugPrint) fprintf(fp, "#endif\n");

            if (pass == PASS_TmpBuffAlloc) {
                if (!e->retval().isVoid() && !e->retval().isPointer()) {
                    if (!totalTmpBuffExist)
                        fprintf(fp, "\t\t\tsize_t totalTmpSize = sizeof(%s);\n", retvalType.c_str());
                    else
                        fprintf(fp, "\t\t\ttotalTmpSize += sizeof(%s);\n", retvalType.c_str());

                    totalTmpBuffExist = true;
                }
                if (totalTmpBuffExist) {
                    fprintf(fp, "\t\t\tunsigned char *tmpBuf = stream->alloc(totalTmpSize);\n");
                }
            }

            if (pass == PASS_Epilog) {
                // send back out pointers data as well as retval
                if (totalTmpBuffExist) {
                    fprintf(fp, "\t\t\tstream->flush();\n");
                }

                fprintf(fp, "\t\t\tpos += *(int *)(ptr + 4);\n");
                fprintf(fp, "\t\t\tptr += *(int *)(ptr + 4);\n");
            }

        } // pass;
        fprintf(fp, "\t\t\t}\n");
        fprintf(fp, "#ifdef CHECK_GL_ERROR\n");
        fprintf(fp, "\t\t\tsprintf(lastCall, \"%s\");\n", e->name().c_str());
        fprintf(fp, "#endif\n");
        fprintf(fp, "\t\t\tbreak;\n");

        delete [] tmpBufOffset;
    }
    fprintf(fp, "\t\t\tdefault:\n");
    fprintf(fp, "\t\t\t\tunknownOpcode = true;\n");
    fprintf(fp, "\t\t} //switch\n");
    if (strstr(m_basename.c_str(), "gl")) {
        fprintf(fp, "#ifdef CHECK_GL_ERROR\n");
        fprintf(fp, "\tint err = this->glGetError();\n");
        fprintf(fp, "\tif (err) fprintf(stderr, \"%s Error: 0x%%X in %%s\\n\", err, lastCall);\n", m_basename.c_str());
        fprintf(fp, "#endif\n");
    }
    fprintf(fp, "\t} // while\n");
    fprintf(fp, "\treturn pos;\n");
    fprintf(fp, "}\n");

    fclose(fp);
    return 0;
}

int ApiGen::readSpec(const std::string & filename)
{
    FILE *specfp = fopen(filename.c_str(), "rt");
    if (specfp == NULL) {
        return -1;
    }

    char line[1000];
    unsigned int lc = 0;
    while (fgets(line, sizeof(line), specfp) != NULL) {
        lc++;
        EntryPoint ref;
        if (ref.parse(lc, std::string(line))) {
            push_back(ref);
            updateMaxEntryPointsParams(ref.vars().size());
        }
    }
    fclose(specfp);
    return 0;
}

int ApiGen::readAttributes(const std::string & attribFilename)
{
    enum { ST_NAME, ST_ATT } state;

    FILE *fp = fopen(attribFilename.c_str(), "rt");
    if (fp == NULL) {
        perror(attribFilename.c_str());
        return -1;
    }
    char buf[1000];

    state = ST_NAME;
    EntryPoint *currentEntry = NULL;
    size_t lc = 0;
    bool globalAttributes = false;
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        lc++;
        std::string line(buf);
        if (line.size() == 0) continue; // could that happen?

        if (line.at(0) == '#') continue; // comment

        size_t first = line.find_first_not_of(" \t\n");
        if (state == ST_ATT && (first == std::string::npos || first == 0)) state = ST_NAME;

        line = trim(line);
        if (line.size() == 0 || line.at(0) == '#') continue;

        switch(state) {
        case ST_NAME:
            if (line == "GLOBAL") {
                globalAttributes = true;
            } else {
                globalAttributes = false;
                currentEntry = findEntryByName(line);
                if (currentEntry == NULL) {
                    fprintf(stderr, "WARNING: %u: attribute of non existant entry point %s\n", (unsigned int)lc, line.c_str());
                }
            }
            state = ST_ATT;
            break;
        case ST_ATT:
            if (globalAttributes) {
                setGlobalAttribute(line, lc);
            } else  if (currentEntry != NULL) {
                currentEntry->setAttribute(line, lc);
            }
            break;
        }
    }
    return 0;
}


int ApiGen::setGlobalAttribute(const std::string & line, size_t lc)
{
    size_t pos = 0;
    size_t last;
    std::string token = getNextToken(line, pos, &last, WHITESPACE);
    pos = last;

    if (token == "base_opcode") {
        std::string str = getNextToken(line, pos, &last, WHITESPACE);
        if (str.size() == 0) {
            fprintf(stderr, "line %u: missing value for base_opcode\n", (uint) lc);
        } else {
            setBaseOpcode(atoi(str.c_str()));
        }
    } else  if (token == "encoder_headers") {
        std::string str = getNextToken(line, pos, &last, WHITESPACE);
        pos = last;
        while (str.size() != 0) {
            encoderHeaders().push_back(str);
            str = getNextToken(line, pos, &last, WHITESPACE);
            pos = last;
        }
    } else if (token == "client_context_headers") {
        std::string str = getNextToken(line, pos, &last, WHITESPACE);
        pos = last;
        while (str.size() != 0) {
            clientContextHeaders().push_back(str);
            str = getNextToken(line, pos, &last, WHITESPACE);
            pos = last;
        }
    } else if (token == "server_context_headers") {
        std::string str = getNextToken(line, pos, &last, WHITESPACE);
        pos = last;
        while (str.size() != 0) {
            serverContextHeaders().push_back(str);
            str = getNextToken(line, pos, &last, WHITESPACE);
            pos = last;
        }
    } else if (token == "decoder_headers") {
        std::string str = getNextToken(line, pos, &last, WHITESPACE);
        pos = last;
        while (str.size() != 0) {
            decoderHeaders().push_back(str);
            str = getNextToken(line, pos, &last, WHITESPACE);
            pos = last;
        }
    }
    else {
        fprintf(stderr, "WARNING: %u : unknown global attribute %s\n", (unsigned int)lc, line.c_str());
    }

    return 0;
}
