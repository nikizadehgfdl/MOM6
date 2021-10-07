//Inspired from https://github.com/wangsl/python-embedding
//#include "runpyC.h"
#include <stdio.h>
#include <iostream>
#include <cassert>

#include <Python.h>
#include <numpy/ndarrayobject.h>

// https://docs.python.org/3/extending/embedding.html
// https://github.com/dusty-nv/jetson-utils/issues/44
// export PYTHONPATH=.:$PYTHONPATH

#define FORT(x) x##_

static PyObject *py_module = 0;
static PyObject *py_function = 0;

inline int init_numpy()
{
  if(!PyArray_API) import_array();
  return PyArray_API ? 1 : 0;
}

static void runpyC_1_1d(const char *py_script_, const char *py_function_,
		             double *x, const int &n1)
{
  if(!Py_IsInitialized()) Py_Initialize();

  assert(Py_IsInitialized());
  assert(init_numpy());
  printf("runpyC_1_1d: n1,%d \n", n1);  
  printf("From runpyC_1_1d: Trying to call %s in %s \n",py_function_,py_script_);
  if(!py_module) {
    PyObject *py_script = PyUnicode_DecodeFSDefault(py_script_);
    assert(py_script);
    py_module = PyImport_Import(py_script);
//Interesting: The following assertion fails if 
//             the py_script has a python bug even outside the function that is called!
//             the py_script has import torch
    assert(py_module);
    Py_DECREF(py_script);
  }
  if(!py_function) {
    assert(py_module);
    py_function = PyObject_GetAttrString(py_module, py_function_);
    assert(py_function);// This assertation fails if py_function is not found in py_script
    assert(PyCallable_Check(py_function));
  }
  //Simle tests
  //assert(PyObject_CallFunctionObjArgs(py_function, NULL)); //This worked. Printed from the function.
  
  const npy_intp dim_x [] = { n1 };
  PyObject *x_py = PyArray_SimpleNewFromData(1, dim_x, NPY_DOUBLE, x);
  PyObject *pValue;
  assert(pValue = PyObject_CallFunctionObjArgs(py_function, x_py, NULL));
  if (pValue != NULL) {
       printf("Result of call: %f\n", PyFloat_AsDouble(pValue));
       Py_DECREF(pValue);
  }
  Py_DECREF(x_py); x_py = 0;
  
  // PyRun_SimpleString("import sys; sys.stdout.flush()");
  
  std::cout.flush();
}

static void runpyC_1_3d(const char *py_script_, const char *py_function_,
			const char *varname_,
			double *x, const int &n1, const int &n2, const int &n3 )
{
  if(!Py_IsInitialized()) Py_Initialize();

  assert(Py_IsInitialized());
  assert(init_numpy());
  printf("runpyC_1_3d: varname %s , dims %d,%d,%d \n", varname_, n1,n2,n3);  
  printf("From runpyC_1_3d: Trying to call %s in %s.py \n",py_function_,py_script_);
  if(!py_module) {
    PyObject *py_script = PyUnicode_DecodeFSDefault(py_script_);
    assert(py_script);
    py_module = PyImport_Import(py_script);
//Interesting: The following assertion fails if 
//             the py_script has a python bug even outside the function that is called!
//             the py_script has import torch
    assert(py_module && " This assertation fails if the py_script has a python bug..");
    Py_DECREF(py_script);
  }
  if(!py_function) {
    assert(py_module);
    py_function = PyObject_GetAttrString(py_module, py_function_);
    assert(py_function && " This assertation fails if the function is not found in the script."); //This assertation fails if py_function is not found in py_script
    assert(PyCallable_Check(py_function));
  }
  //Simle tests
  //assert(PyObject_CallFunctionObjArgs(py_function, NULL)); //This worked. Printed from the function.
  
  const npy_intp dim_x [] = { n1,n2,n3 };
  PyObject *x_py = PyArray_SimpleNewFromData(3, dim_x, NPY_DOUBLE, x);
  PyObject *pValue;
  assert(pValue = PyObject_CallFunctionObjArgs(py_function, x_py, NULL));
  if (pValue != NULL) {
       printf("Result of call: %f\n", PyFloat_AsDouble(pValue));
       Py_DECREF(pValue);
  }
  Py_DECREF(x_py); x_py = 0;
  
  // PyRun_SimpleString("import sys; sys.stdout.flush()");
  
  std::cout.flush();
}

static void py_finalize()
{
  if(py_function) { Py_DECREF(py_function); py_function = 0; }
  if(py_module) { Py_DECREF(py_module); py_module = 0; }
  if(Py_IsInitialized()) assert(!Py_FinalizeEx());
  std::cout.flush();
}

// Fortran interface: NOTE: the name of function should be al lowercase!
extern "C" void FORT(runpyc_1array3d) (const char *py_script, const int &len_py_script,
		  const char *py_function, const int &len_py_function,
		  const char *varname, const int &len_varname,
		  double *x, const int &n1, const int &n2, const int &n3 )

{
  char *py_script_ = new char [len_py_script+1];
  assert(py_script);
  memcpy(py_script_, py_script, len_py_script*sizeof(char));
  py_script_[len_py_script] = '\0';

  char *py_function_ = new char [len_py_function+1];
  assert(py_function);
  memcpy(py_function_, py_function, len_py_function*sizeof(char));
  py_function_[len_py_function] = '\0';

  char *varname_ = new char [len_varname+1];
  assert(varname);
  memcpy(varname_, varname, len_varname*sizeof(char));
  varname_[len_varname] = '\0';

  runpyC_1_3d(py_script_, py_function_, varname_, x, n1,n2,n3);

  if(py_script_) { delete [] py_script_; py_script_ = 0; }
  if(py_function_) { delete [] py_function_; py_function_ = 0; }
  if(varname_) { delete [] varname_; varname_ = 0; }
}

// Fortran interface: PyFinalize 
extern "C" void FORT(pyfinalize)()
{
  py_finalize();
}

