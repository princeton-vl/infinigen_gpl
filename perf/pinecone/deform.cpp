// SPDX-License-Identifier: GPL-2.0-or-later
// Equations from Blender 4.2 source/blender/modifiers/intern/MOD_simpledeform.cc.
#include <cmath>
#include <algorithm>
#include <cfloat>
extern "C" void pinecone_deform(float* co,int n,float angle,int mode,int axis){
 int limit=mode==0?2:(axis==2?0:2);float lo=FLT_MAX,hi=-FLT_MAX;
 for(int i=0;i<n;++i){lo=std::min(lo,co[3*i+limit]);hi=std::max(hi,co[3*i+limit]);}
 float upper=lo+(hi-lo),factor=angle/std::max(FLT_EPSILON,upper-lo);
 if(mode&&std::abs(factor)<1e-6f)return;
 for(int i=0;i<n;++i){float*p=co+3*i;float x=p[0],y=p[1],z=p[2],value=p[limit],clipped=std::clamp(value,lo,upper),cut=value-clipped;if(limit==0)x=clipped;else z=clipped;float theta=(mode&&axis==2?x:z)*factor,s=sinf(theta),c=cosf(theta);
  if(!mode){p[0]=x*c-y*s;p[1]=x*s+y*c;p[2]=z+cut;}
  else if(axis==0){p[0]=x;p[1]=y*c+(1-c)/factor+s*cut;p[2]=-(y-1/factor)*s+c*cut;}
  else if(axis==1){p[0]=x*c+(1-c)/factor+s*cut;p[1]=y;p[2]=-(x-1/factor)*s+c*cut;}
  else {p[0]=-(y-1/factor)*s+c*cut;p[1]=y*c+(1-c)/factor+s*cut;p[2]=z;}
 }
}
