/*
 * Python bindings module for libesedb (pyesedb)
 *
 * Copyright (c) 2009-2014, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <types.h>

#if defined( HAVE_STDLIB_H ) || defined( HAVE_WINAPI )
#include <stdlib.h>
#endif

#include "pyesedb.h"
#include "pyesedb_column.h"
#include "pyesedb_column_types.h"
#include "pyesedb_columns.h"
#include "pyesedb_error.h"
#include "pyesedb_file.h"
#include "pyesedb_file_object_io_handle.h"
#include "pyesedb_libcerror.h"
#include "pyesedb_libcstring.h"
#include "pyesedb_libesedb.h"
#include "pyesedb_python.h"
#include "pyesedb_record.h"
#include "pyesedb_records.h"
#include "pyesedb_table.h"
#include "pyesedb_tables.h"
#include "pyesedb_unused.h"
#include "pyesedb_value_flags.h"

#if !defined( LIBESEDB_HAVE_BFIO )
LIBESEDB_EXTERN \
int libesedb_check_file_signature_file_io_handle(
     libbfio_handle_t *file_io_handle,
     libesedb_error_t **error );
#endif

/* The pyesedb module methods
 */
PyMethodDef pyesedb_module_methods[] = {
	{ "get_version",
	  (PyCFunction) pyesedb_get_version,
	  METH_NOARGS,
	  "get_version() -> String\n"
	  "\n"
	  "Retrieves the version." },

	{ "check_file_signature",
	  (PyCFunction) pyesedb_check_file_signature,
	  METH_VARARGS | METH_KEYWORDS,
	  "check_file_signature(filename) -> Boolean\n"
	  "\n"
	  "Checks if a file has a Personal Folder Format (ESEDB) signature." },

	{ "check_file_signature_file_object",
	  (PyCFunction) pyesedb_check_file_signature_file_object,
	  METH_VARARGS | METH_KEYWORDS,
	  "check_file_signature_file_object(file_object) -> Boolean\n"
	  "\n"
	  "Checks if a file has a Personal Folder Format (ESEDB) signature using a file-like object." },

	{ "open",
	  (PyCFunction) pyesedb_file_new_open,
	  METH_VARARGS | METH_KEYWORDS,
	  "open(filename, mode='r') -> Object\n"
	  "\n"
	  "Opens a file." },

	{ "open_file_object",
	  (PyCFunction) pyesedb_file_new_open_file_object,
	  METH_VARARGS | METH_KEYWORDS,
	  "open_file_object(file_object, mode='r') -> Object\n"
	  "\n"
	  "Opens a file using a file-like object." },

	/* Sentinel */
	{ NULL,
	  NULL,
	  0,
	  NULL}
};

/* Retrieves the pyesedb/libesedb version
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyesedb_get_version(
           PyObject *self PYESEDB_ATTRIBUTE_UNUSED,
           PyObject *arguments PYESEDB_ATTRIBUTE_UNUSED )
{
	const char *errors           = NULL;
	const char *version_string   = NULL;
	size_t version_string_length = 0;

	PYESEDB_UNREFERENCED_PARAMETER( self )
	PYESEDB_UNREFERENCED_PARAMETER( arguments )

	Py_BEGIN_ALLOW_THREADS

	version_string = libesedb_get_version();

	Py_END_ALLOW_THREADS

	version_string_length = libcstring_narrow_string_length(
	                         version_string );

	/* Pass the string length to PyUnicode_DecodeUTF8
	 * otherwise it makes the end of string character is part
	 * of the string
	 */
	return( PyUnicode_DecodeUTF8(
	         version_string,
	         (Py_ssize_t) version_string_length,
	         errors ) );
}

/* Checks if the file has a Personal Folder File (ESEDB) signature
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyesedb_check_file_signature(
           PyObject *self PYESEDB_ATTRIBUTE_UNUSED,
           PyObject *arguments,
           PyObject *keywords )
{
	libcerror_error_t *error    = NULL;
	static char *function       = "pyesedb_check_file_signature";
	static char *keyword_list[] = { "filename", NULL };
	const char *filename        = NULL;
	int result                  = 0;

	PYESEDB_UNREFERENCED_PARAMETER( self )

	if( PyArg_ParseTupleAndKeywords(
	     arguments,
	     keywords,
	     "|s",
	     keyword_list,
	     &filename ) == 0 )
	{
		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libesedb_check_file_signature(
	          filename,
	          &error );

	Py_END_ALLOW_THREADS

	if( result == -1 )
	{
		pyesedb_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to check file signature.",
		 function );

		libcerror_error_free(
		 &error );

		return( NULL );
	}
	if( result != 0 )
	{
		return( Py_True );
	}
	return( Py_False );
}

/* Checks if the file has a Personal Folder File (ESEDB) file signature using a file-like object
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyesedb_check_file_signature_file_object(
           PyObject *self PYESEDB_ATTRIBUTE_UNUSED,
           PyObject *arguments,
           PyObject *keywords )
{
	libcerror_error_t *error         = NULL;
	libbfio_handle_t *file_io_handle = NULL;
	PyObject *file_object            = NULL;
	static char *function            = "pyesedb_check_file_signature_file_object";
	static char *keyword_list[]      = { "file_object", NULL };
	int result                       = 0;

	PYESEDB_UNREFERENCED_PARAMETER( self )

	if( PyArg_ParseTupleAndKeywords(
	     arguments,
	     keywords,
	     "|O",
	     keyword_list,
	     &file_object ) == 0 )
	{
		return( NULL );
	}
	if( pyesedb_file_object_initialize(
	     &file_io_handle,
	     file_object,
	     &error ) != 1 )
	{
		pyesedb_error_raise(
		 error,
		 PyExc_MemoryError,
		 "%s: unable to initialize file IO handle.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	Py_BEGIN_ALLOW_THREADS

	result = libesedb_check_file_signature_file_io_handle(
	          file_io_handle,
	          &error );

	Py_END_ALLOW_THREADS

	if( result == -1 )
	{
		pyesedb_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to check file signature.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	if( libbfio_handle_free(
	     &file_io_handle,
	     &error ) != 1 )
	{
		pyesedb_error_raise(
		 error,
		 PyExc_MemoryError,
		 "%s: unable to free file IO handle.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	if( result != 0 )
	{
		return( Py_True );
	}
	return( Py_False );

on_error:
	if( file_io_handle != NULL )
	{
		libbfio_handle_free(
		 &file_io_handle,
		 NULL );
	}
	return( NULL );
}

/* Declarations for DLL import/export
 */
#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

/* Initializes the pyesedb module
 */
PyMODINIT_FUNC initpyesedb(
                void )
{
	PyObject *module                       = NULL;
	PyTypeObject *column_type_object       = NULL;
	PyTypeObject *column_types_type_object = NULL;
	PyTypeObject *columns_type_object      = NULL;
	PyTypeObject *file_type_object         = NULL;
	PyTypeObject *record_type_object       = NULL;
	PyTypeObject *records_type_object      = NULL;
	PyTypeObject *table_type_object        = NULL;
	PyTypeObject *tables_type_object       = NULL;
	PyTypeObject *value_flags_type_object  = NULL;
	PyGILState_STATE gil_state             = 0;

	/* Create the module
	 * This function must be called before grabbing the GIL
	 * otherwise the module will segfault on a version mismatch
	 */
	module = Py_InitModule3(
	          "pyesedb",
	          pyesedb_module_methods,
	          "Python libesedb module (pyesedb)." );

	PyEval_InitThreads();

	gil_state = PyGILState_Ensure();

	/* Setup the file type object
	 */
	pyesedb_file_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pyesedb_file_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyesedb_file_type_object );

	file_type_object = &pyesedb_file_type_object;

	PyModule_AddObject(
	 module,
	 "file",
	 (PyObject *) file_type_object );

	/* Setup the tables type object
	 */
	pyesedb_tables_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pyesedb_tables_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyesedb_tables_type_object );

	tables_type_object = &pyesedb_tables_type_object;

	PyModule_AddObject(
	 module,
	 "_tables",
	 (PyObject *) tables_type_object );

	/* Setup the table type object
	 */
	pyesedb_table_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pyesedb_table_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyesedb_table_type_object );

	table_type_object = &pyesedb_table_type_object;

	PyModule_AddObject(
	 module,
	 "table",
	 (PyObject *) table_type_object );

	/* Setup the columns type object
	 */
	pyesedb_columns_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pyesedb_columns_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyesedb_columns_type_object );

	columns_type_object = &pyesedb_columns_type_object;

	PyModule_AddObject(
	 module,
	 "_columns",
	 (PyObject *) columns_type_object );

	/* Setup the column type object
	 */
	pyesedb_column_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pyesedb_column_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyesedb_column_type_object );

	column_type_object = &pyesedb_column_type_object;

	PyModule_AddObject(
	 module,
	 "column",
	 (PyObject *) column_type_object );

	/* Setup the records type object
	 */
	pyesedb_records_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pyesedb_records_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyesedb_records_type_object );

	records_type_object = &pyesedb_records_type_object;

	PyModule_AddObject(
	 module,
	 "_records",
	 (PyObject *) records_type_object );

	/* Setup the record type object
	 */
	pyesedb_record_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pyesedb_record_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyesedb_record_type_object );

	record_type_object = &pyesedb_record_type_object;

	PyModule_AddObject(
	 module,
	 "record",
	 (PyObject *) record_type_object );

	/* Setup the column types type object
	 */
	pyesedb_column_types_type_object.tp_new = PyType_GenericNew;

	if( pyesedb_column_types_init_type(
             &pyesedb_column_types_type_object ) != 1 )
	{
		goto on_error;
	}
	if( PyType_Ready(
	     &pyesedb_column_types_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyesedb_column_types_type_object );

	column_types_type_object = &pyesedb_column_types_type_object;

	PyModule_AddObject(
	 module,
	 "column_types",
	 (PyObject *) column_types_type_object );

	/* Setup the value flags type object
	 */
	pyesedb_value_flags_type_object.tp_new = PyType_GenericNew;

	if( pyesedb_value_flags_init_type(
             &pyesedb_value_flags_type_object ) != 1 )
	{
		goto on_error;
	}
	if( PyType_Ready(
	     &pyesedb_value_flags_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyesedb_value_flags_type_object );

	value_flags_type_object = &pyesedb_value_flags_type_object;

	PyModule_AddObject(
	 module,
	 "value_flags",
	 (PyObject *) value_flags_type_object );

on_error:
	PyGILState_Release(
	 gil_state );
}

