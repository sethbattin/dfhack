/*
www.sourceforge.net/projects/dfhack
Copyright (c) 2009 Petr Mrázek (peterix), Kenneth Ferland (Impaler[WrG]), dorf, doomchild

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#ifndef __DFAPI__
#define __DFAPI__

#include "Python.h"
#include <string>
#include "DFTypes.h"
#include "DFHackAPI.h"
#include "DF_MemInfo.cpp"
#include "DF_Position.cpp"
#include "DF_Material.cpp"
#include "DF_CreatureManager.cpp"

using namespace std;
using namespace DFHack;

struct DF_API
{
	PyObject_HEAD
	PyObject* mem_info;
	PyObject* position;
	PyObject* material;
	PyObject* creature;
	DFHack::API* api_Ptr;
};

// API type Allocation, Deallocation, and Initialization

static PyObject* DF_API_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	DF_API* self;
	
	self = (DF_API*)type->tp_alloc(type, 0);
	
	if(self != NULL)
		self->api_Ptr = NULL;
	
	return (PyObject*)self;
}

static int DF_API_init(DF_API* self, PyObject* args, PyObject* kwds)
{
	const char* memFileString = NULL;
	
	if(self->api_Ptr == NULL)
	{
		self->mem_info = NULL;
		self->position = NULL;
		self->material = NULL;
		self->creature = NULL;
		
		if(!PyArg_ParseTuple(args, "s", &memFileString))
			return -1;
		
		if(memFileString)
			self->api_Ptr = new DFHack::API(std::string(memFileString));
		else
			return -1;
	}
	
	return 0;
}

static void DF_API_dealloc(DF_API* self)
{
	if(self != NULL)
	{
		Py_CLEAR(self->mem_info);
		Py_CLEAR(self->position);
		Py_CLEAR(self->material);
		Py_CLEAR(self->creature);
		
		if(self->api_Ptr != NULL)
		{
			delete self->api_Ptr;
			
			self->api_Ptr = NULL;
		}
		
		self->ob_type->tp_free((PyObject*)self);
	}
}

// Accessors

static PyObject* DF_API_getIsAttached(DF_API* self, void* closure)
{
    try
    {
        if(self->api_Ptr != NULL)
            if(self->api_Ptr->isAttached())
                Py_RETURN_TRUE;
    }
    catch(...)
    {
        PyErr_SetString(PyExc_ValueError, "Error trying to read attached flag");
		Py_RETURN_FALSE;
    }
    
    Py_RETURN_FALSE;
}

static PyObject* DF_API_getIsSuspended(DF_API* self, void* closure)
{
    try
    {
        if(self->api_Ptr != NULL)
            if(self->api_Ptr->isSuspended())
                Py_RETURN_TRUE;
    }
    catch(...)
    {
        PyErr_SetString(PyExc_ValueError, "Error trying to read suspension flag");
		return NULL;
    }
    
    Py_RETURN_FALSE;
}

static PyObject* DF_API_getMemoryInfo(DF_API* self, void* closure)
{
	if(self->mem_info != NULL)
		return self->mem_info;
	
	try
	{
		if(self->api_Ptr != NULL)
		{
			self->mem_info = PyObject_Call((PyObject*)&DF_MemInfo_type, NULL, NULL);
			
			if(self->mem_info != NULL)
			{
				((DF_MemInfo*)(self->mem_info))->mem_Ptr = self->api_Ptr->getMemoryInfo();
				
				if(((DF_MemInfo*)(self->mem_info))->mem_Ptr != NULL)
					return self->mem_info;
			}
		}
	}
	catch(...)
	{
		PyErr_SetString(PyExc_ValueError, "Error trying to read memory info");
		return NULL;
	}
	
	Py_RETURN_NONE;
}

static PyObject* DF_API_getPosition(DF_API* self, void* closure)
{
	if(self->position != NULL)
		return self->position;
	
	try
	{
		if(self->api_Ptr != NULL)
		{
			self->position = PyObject_Call((PyObject*)&DF_Position_type, NULL, NULL);
			
			if(self->position != NULL)
			{
				((DF_Position*)(self->position))->pos_Ptr = self->api_Ptr->getPosition();
				
				if(((DF_Position*)(self->position))->pos_Ptr != NULL)
					return self->position;
			}
		}
	}
	catch(...)
	{
		PyErr_SetString(PyExc_ValueError, "Error trying to read position");
		return NULL;
	}
	
	Py_RETURN_NONE;
}

static PyObject* DF_API_getMaterial(DF_API* self, void* closure)
{
	if(self->material != NULL)
		return self->material;
	
	try
	{
		if(self->api_Ptr != NULL)
		{
			self->material = PyObject_Call((PyObject*)&DF_Material_type, NULL, NULL);
			
			if(self->material != NULL)
			{
				((DF_Material*)(self->material))->mat_Ptr = self->api_Ptr->getMaterials();
				
				if(((DF_Material*)(self->material))->mat_Ptr != NULL)
					return self->material;
			}
		}
	}
	catch(...)
	{
		PyErr_SetString(PyExc_ValueError, "Error trying to read material");
		return NULL;
	}
	
	Py_RETURN_NONE;
}

static PyObject* DF_API_getCreature(DF_API* self, void* closure)
{
	if(self->creature != NULL)
		return self->creature;
	
	try
	{
		if(self->api_Ptr != NULL)
		{
			self->creature = PyObject_Call((PyObject*)&DF_CreatureManager_type, NULL, NULL);
			
			if(self->creature != NULL)
			{
				((DF_CreatureManager*)(self->creature))->creature_Ptr = self->api_Ptr->getCreatures();
				
				if(((DF_CreatureManager*)(self->creature))->creature_Ptr != NULL)
					return self->creature;
			}
		}
	}
	catch(...)
	{
		PyErr_SetString(PyExc_ValueError, "Error trying to read creature");
		return NULL;
	}
	
	Py_RETURN_NONE;
}

static PyGetSetDef DF_API_getterSetters[] =
{
    {"is_attached", (getter)DF_API_getIsAttached, NULL, "is_attached", NULL},
    {"is_suspended", (getter)DF_API_getIsSuspended, NULL, "is_suspended", NULL},
	{"memory_info", (getter)DF_API_getMemoryInfo, NULL, "memory_info", NULL},
	{"position", (getter)DF_API_getPosition, NULL, "position", NULL},
	{"materials", (getter)DF_API_getMaterial, NULL, "materials", NULL},
	{"creatures", (getter)DF_API_getCreature, NULL, "creatures", NULL},
    {NULL}  // Sentinel
};

// API type methods

static PyObject* DF_API_Attach(DF_API* self)
{
    if(self->api_Ptr != NULL)
        if(self->api_Ptr->Attach())
            Py_RETURN_TRUE;
    
    Py_RETURN_FALSE;
}

static PyObject* DF_API_Detach(DF_API* self)
{
    if(self->api_Ptr != NULL)
        if(self->api_Ptr->Detach())
            Py_RETURN_TRUE;
    
    Py_RETURN_FALSE;
}

static PyObject* DF_API_Suspend(DF_API* self)
{
    try
    {
        if(self->api_Ptr != NULL)
            if(self->api_Ptr->Suspend())
                Py_RETURN_TRUE;
    }
    catch(...)
    {
        PyErr_SetString(PyExc_ValueError, "Error trying to suspend");
		return NULL;
    }
    
    Py_RETURN_FALSE;
}

static PyObject* DF_API_Resume(DF_API* self)
{
    try
    {
        if(self->api_Ptr != NULL)
            if(self->api_Ptr->Resume())
                Py_RETURN_TRUE;
    }
    catch(...)
    {
        PyErr_SetString(PyExc_ValueError, "Error trying to resume");
		return NULL;
    }
    
    Py_RETURN_FALSE;
}

static PyObject* DF_API_AsyncSuspend(DF_API* self)
{
    try
    {
        if(self->api_Ptr != NULL)
            if(self->api_Ptr->AsyncSuspend())
                Py_RETURN_TRUE;
    }
    catch(...)
    {
        PyErr_SetString(PyExc_ValueError, "Error trying to asynchronously suspend");
		return NULL;
    }
    
    Py_RETURN_FALSE;
}

static PyObject* DF_API_ForceResume(DF_API* self)
{
    try
    {
        if(self->api_Ptr != NULL)
            if(self->api_Ptr->ForceResume())
                Py_RETURN_TRUE;
    }
    catch(...)
    {
        PyErr_SetString(PyExc_ValueError, "Error trying to force resume");
		return NULL;
    }
    
    Py_RETURN_FALSE;
}

static PyMethodDef DF_API_methods[] =
{
    {"Attach", (PyCFunction)DF_API_Attach, METH_NOARGS, "Attach to the DF process"},
    {"Detach", (PyCFunction)DF_API_Detach, METH_NOARGS, "Detach from the DF process"},
    {"Suspend", (PyCFunction)DF_API_Suspend, METH_NOARGS, "Suspend the DF process"},
    {"Resume", (PyCFunction)DF_API_Resume, METH_NOARGS, "Resume the DF process"},
    {"Async_Suspend", (PyCFunction)DF_API_AsyncSuspend, METH_NOARGS, "Asynchronously suspend the DF process"},
    {"Force_Resume", (PyCFunction)DF_API_ForceResume, METH_NOARGS, "Force the DF process to resume"},
    {NULL}  // Sentinel
};

static PyTypeObject DF_API_type =
{
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pydfhack.API",             /*tp_name*/
    sizeof(DF_API), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DF_API_dealloc,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "pydfhack API objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    DF_API_methods,             /* tp_methods */
    0,                      /* tp_members */
    DF_API_getterSetters,      /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)DF_API_init,      /* tp_init */
    0,                         /* tp_alloc */
    DF_API_new,                 /* tp_new */
};

#endif