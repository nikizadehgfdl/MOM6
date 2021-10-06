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

static void py_test(const char *py_script_, const char *py_function_,
		    double *x, const int &nx,
		    double *y, const int &ny)
{
  if(!Py_IsInitialized()) Py_Initialize();

  assert(Py_IsInitialized());
  assert(init_numpy());
  
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
    assert(py_function); //This assertation fails if py_function is not found in py_script
    assert(PyCallable_Check(py_function));
  }
  //Simle tests
  //assert(PyObject_CallFunctionObjArgs(py_function, NULL)); //This worked. Printed from the function.
  //int x_in = 1;
  //int y_in = 1; 
  //assert(PyObject_CallFunctionObjArgs(py_function, x_in, y_in, NULL)); //Segmentation fault - invalid memory reference
  //assert(PyObject_CallFunctionObjArgs(py_function, x_in, NULL)); //Segmentation fault - invalid memory reference
  //assert(PyObject_CallFunctionObjArgs(py_function, &x_in, NULL)); //Works but cannot access value. Ahaa, input needs to be a poniter
  //assert(PyObject_CallFunctionObjArgs(py_function, dims, NULL)); //Works but cannot access value. invalid memory reference
  /* The following worked fine.
  long dims[2];
  dims[0] = 4; dims[1] = 4;
  PyObject *pinValue, *pArgs,*pValue ;
  pArgs = PyTuple_New(2);
  for (int i = 0; i < 2; ++i) {
      pinValue = PyLong_FromLong(dims[i]);
      PyTuple_SetItem(pArgs, i, pinValue);
  }
  pValue = PyObject_CallFunctionObjArgs(py_function, pArgs, NULL);
  if (pValue != NULL) {
       printf("Result of call: %ld\n", PyLong_AsLong(pValue));
       Py_DECREF(pValue);
  }

  double ddims[2];
  ddims[0] = 4.5; ddims[1] = 5.5;
  pArgs = PyTuple_New(2);
  for (int i = 0; i < 2; ++i) {
      pinValue = PyFloat_FromDouble(ddims[i]);
      PyTuple_SetItem(pArgs, i, pinValue);
  }
  pValue = PyObject_CallFunctionObjArgs(py_function, pArgs, NULL);
  if (pValue != NULL) {
       printf("Result of call: %f\n", PyFloat_AsDouble(pValue));
       Py_DECREF(pValue);
  }
  */
  const npy_intp dim_x [] = { nx };
  PyObject *x_py = PyArray_SimpleNewFromData(1, dim_x, NPY_DOUBLE, x);
  const npy_intp dim_y [] = { ny };
  PyObject *y_py = PyArray_SimpleNewFromData(1, dim_y, NPY_DOUBLE, y);
  PyObject *pValue;
  assert(pValue = PyObject_CallFunctionObjArgs(py_function, x_py, NULL));
  if (pValue != NULL) {
       printf("Result of call: %f\n", PyFloat_AsDouble(pValue));
       Py_DECREF(pValue);
  }
  Py_DECREF(x_py); x_py = 0;
  Py_DECREF(y_py); y_py = 0;

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

// Fortran interface: PyTest
extern "C" void FORT(pytest)(const char *py_script, const int &len_py_script,
			     const char *py_function, const int &len_py_function,
			     double *x, const int &nx,
			     double *y, const int &ny)
{
  char *py_script_ = new char [len_py_script+1];
  assert(py_script);
  memcpy(py_script_, py_script, len_py_script*sizeof(char));
  py_script_[len_py_script] = '\0';

  char *py_function_ = new char [len_py_function+1];
  assert(py_function_);
  memcpy(py_function_, py_function, len_py_function*sizeof(char));
  py_function_[len_py_function] = '\0';

  py_test(py_script_, py_function_, x, nx, y, ny);

  if(py_script_) { delete [] py_script_; py_script_ = 0; }
  if(py_function_) { delete [] py_function_; py_function_ = 0; }
}

// Fortran interface: PyFinalize 
extern "C" void FORT(pyfinalize)()
{
  py_finalize();
}

