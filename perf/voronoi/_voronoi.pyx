# SPDX-License-Identifier: GPL-2.0-or-later
# cython: language_level=3, boundscheck=False, wraparound=False
import numpy as np
cimport numpy as cnp
cnp.import_array()
cdef extern from "_voronoi_core.hpp" namespace "mushroom_voronoi":
 void mask(const float*,size_t,float,float,float,float*) except + nogil

def edge_mask(cnp.ndarray[cnp.float32_t,ndim=2,mode='c'] co,float scale,float randomness,float threshold):
 if co.shape[1]!=3 or not np.isfinite(co).all() or not np.isfinite(scale) or scale<=0 or not np.isfinite(randomness) or randomness<0 or randomness>1 or not np.isfinite(threshold) or threshold<0:raise ValueError('Invalid Voronoi mask input')
 if co.size and np.abs(co).max()*scale>1e6:raise ValueError('Coordinate range exceeded')
 cdef cnp.ndarray out=np.empty(co.shape[0],np.float32)
 with nogil:mask(<float*>co.data,co.shape[0],scale,randomness,threshold,<float*>out.data)
 return out

cdef extern from "_texture_core.hpp" namespace "mushroom_fields":
 void texture_values(const float*,size_t,float,int,int,int,float,bint,float*) except + nogil
 void displace(const float*,const float*,size_t,float,int,int,int,float,bint,float,float*) except + nogil
 void extension(const float*,size_t,float,float,const float*,float*) except + nogil

def texture_sample(cnp.ndarray[cnp.float32_t,ndim=2,mode='c'] co,float size,int kind,int subtype,int depth,float turb,bint hard=False):
 if co.shape[1]!=3 or not np.isfinite(co).all() or size<=0 or not np.isfinite(size) or kind not in (0,1) or subtype not in (0,1,2) or depth<0 or depth>10 or not np.isfinite(turb):raise ValueError('Unsupported texture input')
 if co.size and (np.abs(co).max()+1)*(1<<depth)/size>1e7:raise ValueError('Texture coordinate range exceeded')
 cdef cnp.ndarray out=np.empty(co.shape[0],np.float32)
 with nogil:texture_values(<float*>co.data,co.shape[0],size,kind,subtype,depth,turb,hard,<float*>out.data)
 return out

def texture_displace(cnp.ndarray[cnp.float32_t,ndim=2,mode='c'] co,cnp.ndarray[cnp.float32_t,ndim=2,mode='c'] normals,float size,int kind,int subtype,int depth,float turb,bint hard,float strength):
 if co.shape[1]!=3 or normals.shape[1]!=3 or co.shape[0]!=normals.shape[0] or not np.isfinite(co).all() or not np.isfinite(normals).all() or size<=0 or not np.isfinite(size) or kind not in (0,1) or subtype not in (0,1,2) or depth<0 or depth>10 or not np.isfinite(turb) or not np.isfinite(strength):raise ValueError('Unsupported displacement input')
 if co.size and (np.abs(co).max()+1)*(1<<depth)/size>1e7:raise ValueError('Texture coordinate range exceeded')
 cdef cnp.ndarray out=np.empty((co.shape[0],3),np.float32)
 with nogil:displace(<float*>co.data,<float*>normals.data,co.shape[0],size,kind,subtype,depth,turb,hard,strength,<float*>out.data)
 return out

def extension_positions(cnp.ndarray[cnp.float32_t,ndim=2,mode='c'] co,float strength,float scale,cnp.ndarray[cnp.float32_t,ndim=1,mode='c'] offset):
 if co.shape[1]!=3 or offset.shape[0]!=3 or not np.isfinite(co).all() or not np.isfinite(offset).all() or not np.isfinite(strength) or not np.isfinite(scale) or scale<=0 or scale>1000 or np.abs(offset).max()>100:raise ValueError('Unsupported extension input')
 cdef cnp.ndarray out=np.empty((co.shape[0],3),np.float32)
 with nogil:extension(<float*>co.data,co.shape[0],strength,scale,<float*>offset.data,<float*>out.data)
 return out
