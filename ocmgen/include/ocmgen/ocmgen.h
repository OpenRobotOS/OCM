#pragma once

#include <glib.h>
#include <stdint.h>

#include "getopt.h"

/////////////////////////////////////////////////

#ifndef g_ptr_array_size
  #define g_ptr_array_size(x) ((x)->len)
#endif

/////////////////////////////////////////////////
// ocm_typename_t: represents the name of a type, including package
//
//	Originally, the first field in the ocm_typename was named typename - which is a C++
//	keyword and caused much grief. Renamed to lctypename.
typedef struct ocm_typename ocm_typename_t;

struct ocm_typename {
  char *lctypename;  // fully-qualified name, e.g., "edu.mit.dgc.laser_t"
  char *package;     // package name, e.g., "edu.mit.dgc"
  char *shortname;   // e.g., "laser_t"
};

/////////////////////////////////////////////////
// ocm_dimension_t: represents the size of a dimension of an
//                  array. The size can be either dynamic (a variable)
//                  or a constant.
//
typedef enum { OCM_CONST, OCM_VAR } ocm_dimension_mode_t;

typedef struct ocm_dimension ocm_dimension_t;

struct ocm_dimension {
  ocm_dimension_mode_t mode;
  char *size;  // a string containing either a member variable name or a constant
};

/////////////////////////////////////////////////
// ocm_member_t: represents one member of a struct, including (if its
//               an array), its dimensions.
//
typedef struct ocm_member ocm_member_t;

struct ocm_member {
  ocm_typename_t *type;
  char *membername;

  // an array of ocm_dimension_t. A scalar is a 1-dimensional array
  // of length 1.
  GPtrArray *dimensions;

  // Comments in the OCM type definition immediately before a member
  // declaration are attached to that member
  char *comment;
};

/////////////////////////////////////////////////
// ocm_struct_t: a first-class OCM object declaration
//
typedef struct ocm_struct ocm_struct_t;

struct ocm_struct {
  ocm_typename_t *structname;  // name of the data type

  GPtrArray *members;  // ocm_member_t

  // recursive declaration of structs and enums
  GPtrArray *structs;    // ocm_struct_t
  GPtrArray *enums;      // locally-declared enums  DEPRECATED
  GPtrArray *constants;  // ocm_constant_t

  char *ocmfile;  // file/path of function that declared it
  int64_t hash;

  // Comments in the OCM type defition immediately before a struct is declared
  // are attached to that struct.
  char *comment;
};

/////////////////////////////////////////////////
// ocm_constant_: the symbolic name of a constant and its value.
//
typedef struct ocm_constant ocm_constant_t;

struct ocm_constant {
  char *lctypename;  // int8_t / int16_t / int32_t / int64_t / float / double
  char *membername;
  union {
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    float f;
    double d;
  } val;
  char *val_str;  // value as a string, as specified in the .ocm file

  // Comments in the OCM type definition immediately before a constant are
  // attached to the constant.
  char *comment;
};

/////////////////////////////////////////////////
// DEPRECATED
// ocm_enum_value_t: the symbolic name of an enum and its constant
//                   value.
//
typedef struct ocm_enum_value ocm_enum_value_t;

struct ocm_enum_value {
  char *valuename;
  int32_t value;
};

/////////////////////////////////////////////////
// DEPRECATED
// ocm_enum_t: an enumeration, also a first-class OCM object.
//
typedef struct ocm_enum ocm_enum_t;

struct ocm_enum {
  ocm_typename_t *enumname;  // name of the enum

  GPtrArray *values;  // legal values for the enum
  char *ocmfile;      // file/path of function that declared it

  // hash values for enums are "weak". They only involve the name of the enum,
  // so that new enumerated values can be added without breaking the hash.
  int64_t hash;
};

/////////////////////////////////////////////////
// ocmgen_t: State used when parsing OCM declarations. The gopt is
//           essentially a set of key-value pairs that configure
//           various options. structs and enums are populated
//           according to the parsed definitions.
//
typedef struct ocmgen ocmgen_t;

struct ocmgen {
  char *package;  // remembers the last-specified package name, which is prepended to other types.
  getopt_t *gopt;
  GPtrArray *structs;  // ocm_struct_t
  GPtrArray *enums;    // ocm_enum_t (declared at top level)

  gchar *comment_doc;
};

/////////////////////////////////////////////////
// Helper functions
/////////////////////////////////////////////////

// Returns 1 if the argument is a built-in type (e.g., "int64_t", "float").
int ocm_is_primitive_type(const char *t);

// Returns 1 if the argument is an array dimension type (e.g., "int*_t").
int ocm_is_array_dimension_type(const char *t);

// Returns 1 if the argument is a legal constant type (e.g., "int64_t", "float").
int ocm_is_legal_const_type(const char *t);

// Returns the member of a struct by name. Returns NULL on error.
ocm_member_t *ocm_find_member(ocm_struct_t *lr, const char *name);

// Returns the constant of a struct by name. Returns NULL on error.
ocm_constant_t *ocm_find_const(ocm_struct_t *lr, const char *name);

// Returns 1 if the "lazy" option is enabled AND the file "outfile" is
// older than the file "declaringfile"
int ocm_needs_generation(ocmgen_t *ocmgen, const char *declaringfile, const char *outfile);

// create a new parsing context.
ocmgen_t *ocmgen_create();

// for debugging, emit the contents to stdout
void ocmgen_dump(ocmgen_t *ocm);

// parse the provided file
int ocmgen_handle_file(ocmgen_t *ocm, const char *path);

// Are all of the dimensions of this array constant?
// (scalars return 1)
int ocm_is_constant_size_array(ocm_member_t *lm);
