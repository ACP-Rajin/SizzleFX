/*
 * Copyright 2025 The G3DL Project Developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#pragma once

#include <array>
#include <cmath>
#include <cassert>

namespace g3dl_math{

// Forward declarations
struct Vector2f;
struct Vector3f;
struct Vector4f;
struct Matrix2f;
struct Matrix3f;
struct Matrix4f;

struct Vector2i;
struct Vector3i;
struct Vector4i;
struct Matrix2i;
struct Matrix3i;
struct Matrix4i;

// Vector2f
struct Vector2f{
  static const Vector2f zero;
  float x=0,y=0;

  constexpr Vector2f()=default;
  constexpr Vector2f(float xy):x(xy),y(xy){}
  constexpr Vector2f(float x,float y){this->x=x;this->y=y;}

  constexpr std::array<float,2>toArray()const{return{x,y};}

  void set(float x,float y){this->x=x;this->y=y;}
  void set(const Vector2f& v){x=v.x;y=v.y;}
  void set(float xy){x=xy;y=xy;}
};
inline constexpr Vector2f Vector2f::zero{0,0};

// Vector3f
struct Vector3f{
  static const Vector3f zero;
  float x=0,y=0,z=0;

  constexpr Vector3f()=default;
  constexpr Vector3f(float xyz):x(xyz),y(xyz),z(xyz){}
  constexpr Vector3f(float x,float y,float z){this->x=x;this->y=y;this->z=z;}

  constexpr std::array<float,3>toArray()const{return{x,y,z};}

  void set(float x,float y,float z){this->x=x;this->y=y;this->z=z;}
  void set(const Vector3f& vec3){x=vec3.x;y=vec3.y;z=vec3.z;}
  void set(float xyz){x=xyz;y=xyz;z=xyz;}
};
inline constexpr Vector3f Vector3f::zero{0,0,0};

// Vector4f
struct Vector4f{
  static const Vector4f zero;
  float x=0,y=0,z=0,w=0;

  constexpr Vector4f()=default;
  constexpr Vector4f(float xyzw):x(xyzw),y(xyzw),z(xyzw),w(xyzw){}
  constexpr Vector4f(float x,float y,float z,float w){this->x=x;this->y=y;this->z=z;this->w=w;}

  constexpr std::array<float,4>toArray()const{return{x,y,z,w};}

  void set(float x,float y,float z,float w){this->x=x;this->y=y;this->z=z;this->w=w;}
  void set(const Vector4f& vec4){x=vec4.x;y=vec4.y;z=vec4.z;w=vec4.w;}
  void set(float xyzw){x=xyzw;y=xyzw;z=xyzw;w=xyzw;}
};
inline constexpr Vector4f Vector4f::zero{0,0,0,0};

// Matrix2f
struct Matrix2f{
  std::array<float,4>val{};

  Matrix2f(){identity();}
  Matrix2f(std::initializer_list<float>values){
    assert(values.size()==4);
    std::copy(values.begin(),values.end(),val.begin());
  }

  void identity(){val={1,0,0,1};}

  void setValues(std::initializer_list<float>values){
    assert(values.size()==4);
    std::copy(values.begin(),values.end(),val.begin());
  }

  void setValue(int x,int y,float v){val[x*2+y]=v;}
  float getValue(int x,int y)const{return val[x*2+y];}

  const float* data()const{return val.data();}
};

// Matrix3f
struct Matrix3f{
  std::array<float,9>val{};

  Matrix3f(){identity();}
  Matrix3f(std::initializer_list<float>values){
    assert(values.size()==9);
    std::copy(values.begin(),values.end(),val.begin());
  }

  void identity(){val={1,0,0,0,1,0,0,0,1};}

  void setValues(std::initializer_list<float>values){
    assert(values.size()==9);
    std::copy(values.begin(),values.end(),val.begin());
  }

  void setValue(int x,int y,float v){val[x*3+y]=v;}
  float getValue(int x,int y)const{return val[x*3+y];}

  const float* data()const{return val.data();}
};

// Matrix4f
struct Matrix4f{
  std::array<float,16>val{};

  Matrix4f(){identity();}
  Matrix4f(std::initializer_list<float>values){
    assert(values.size()==16);
    std::copy(values.begin(),values.end(),val.begin());
  }

  void identity(){val={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};}

  void setValues(std::initializer_list<float>values){
    assert(values.size()==16);
    std::copy(values.begin(),values.end(),val.begin());
  }

  void setValue(int x,int y,float v){val[x*4+y]=v;}
  float getValue(int x,int y)const{return val[x*4+y];}

  const float* data()const{return val.data();}
};

// Vector2i
struct Vector2i{
  static const Vector2i zero;
  int x=0,y=0;

  constexpr Vector2i()=default;
  constexpr Vector2i(int xy):x(xy),y(xy){}
  constexpr Vector2i(int x,int y){this->x=x;this->y=y;}

  constexpr std::array<int,2>toArray()const{return{x,y};}

  void set(int x,int y){this->x=x;this->y=y;}
  void set(const Vector2i& v){x=v.x;y=v.y;}
  void set(int xy){x=xy;y=xy;}
};
inline constexpr Vector2i Vector2i::zero{0,0};

// Vector3i
struct Vector3i{
  static const Vector3i zero;
  int x=0,y=0,z=0;

  constexpr Vector3i()=default;
  constexpr Vector3i(int xyz):x(xyz),y(xyz),z(xyz){}
  constexpr Vector3i(int x,int y,int z){this->x=x;this->y=y;this->z=z;}

  constexpr std::array<int,3>toArray()const{return{x,y,z};}

  void set(int x,int y,int z){this->x=x;this->y=y;this->z=z;}
  void set(const Vector3i& vec3){x=vec3.x;y=vec3.y;z=vec3.z;}
  void set(int xyz){x=xyz;y=xyz;z=xyz;}
};
inline constexpr Vector3i Vector3i::zero{0,0,0};

// Vector4i
struct Vector4i{
  static const Vector4i zero;
  int x=0,y=0,z=0,w=0;

  constexpr Vector4i()=default;
  constexpr Vector4i(int xyzw):x(xyzw),y(xyzw),z(xyzw),w(xyzw){}
  constexpr Vector4i(int x,int y,int z,int w){this->x=x;this->y=y;this->z=z;this->w=w;}

  constexpr std::array<int,4>toArray()const{return{x,y,z,w};}

  void set(int x,int y,int z,int w){this->x=x;this->y=y;this->z=z;this->w=w;}
  void set(const Vector4i& vec4){x=vec4.x;y=vec4.y;z=vec4.z;w=vec4.w;}
  void set(int xyzw){x=xyzw;y=xyzw;z=xyzw;w=xyzw;}
};
inline constexpr Vector4i Vector4i::zero{0,0,0,0};

// Matrix2i
struct Matrix2i{
  std::array<int,4>val{};

  Matrix2i(){identity();}
  Matrix2i(std::initializer_list<int>values){
    assert(values.size()==4);
    std::copy(values.begin(),values.end(),val.begin());
  }

  void identity(){val={1,0,0,1};}

  void setValues(std::initializer_list<int>values){
    assert(values.size()==4);
    std::copy(values.begin(),values.end(),val.begin());
  }

  void setValue(int x,int y,int v){val[x*2+y]=v;}
  int getValue(int x,int y)const{return val[x*2+y];}

  const int* data()const{return val.data();}
};

// Matrix3i
struct Matrix3i{
  std::array<int,9>val{};

  Matrix3i(){identity();}
  Matrix3i(std::initializer_list<int>values){
    assert(values.size()==9);
    std::copy(values.begin(),values.end(),val.begin());
  }

  void identity(){val={1,0,0,0,1,0,0,0,1};}

  void setValues(std::initializer_list<int>values){
    assert(values.size()==9);
    std::copy(values.begin(),values.end(),val.begin());
  }

  void setValue(int x,int y,int v){val[x*3+y]=v;}
  int getValue(int x,int y)const{return val[x*3+y];}

  const int* data()const{return val.data();}
};

// Matrix4i
struct Matrix4i{
  std::array<int,16>val{};

  Matrix4i(){identity();}
  Matrix4i(std::initializer_list<int>values){
    assert(values.size()==16);
    std::copy(values.begin(),values.end(),val.begin());
  }

  void identity(){val={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};}

  void setValues(std::initializer_list<int>values){
    assert(values.size()==16);
    std::copy(values.begin(),values.end(),val.begin());
  }

  void setValue(int x,int y,int v){val[x*4+y]=v;}
  int getValue(int x,int y)const{return val[x*4+y];}

  const int* data()const{return val.data();}
};

// Math2 utilities (free functions)

inline double sigmoid(double x){
  if(x<=-6.9067547786485534722)return 0.0;
  return 1.0/(1.0+std::exp(-x));
}

inline double tanh(double x){return std::tanh(x);}

inline double tanhDerivative(double x){
  double t=std::tanh(x);
  return 1.0-t*t;
}

inline double sigmoidDerivative(double x){
  double sx=sigmoid(x);
  return sx*(1.0-sx);
}

inline double random(double min,double max){return((double)rand()/RAND_MAX)*(max-min)+min;}

// Equality for vectors

inline bool equals(const Vector2f& a,const Vector2f& b){return a.x==b.x&&a.y==b.y;}
inline bool equals(const Vector3f& a,const Vector3f& b){return a.x==b.x&&a.y==b.y&&a.z==b.z;}
inline bool equals(const Vector4f& a,const Vector4f& b){return a.x==b.x&&a.y==b.y&&a.z==b.z&&a.w==b.w;}

inline bool equals(const Vector2i& a,const Vector2i& b){return a.x==b.x&&a.y==b.y;}
inline bool equals(const Vector3i& a,const Vector3i& b){return a.x==b.x&&a.y==b.y&&a.z==b.z;}
inline bool equals(const Vector4i& a,const Vector4i& b){return a.x==b.x&&a.y==b.y&&a.z==b.z&&a.w==b.w;}

// Distance functions
inline double distance(const Vector2f& a,const Vector2f& b){
  double dx=b.x-a.x,dy=b.y-a.y;
  return std::sqrt(dx*dx+dy*dy);
}
inline double distance(const Vector3f& a,const Vector3f& b){
  double dx=b.x-a.x,dy=b.y-a.y,dz=b.z-a.z;
  return std::sqrt(dx*dx+dy*dy+dz*dz);
}
inline double distance(const Vector4f& a,const Vector4f& b){
  double dx=b.x-a.x,dy=b.y-a.y,dz=b.z-a.z,dw=b.w-a.w;
  return std::sqrt(dx*dx+dy*dy+dz*dz+dw*dw);
}

inline int distance(const Vector2i& a,const Vector2i& b){
  int dx=b.x-a.x,dy=b.y-a.y;
  return std::sqrt(dx*dx+dy*dy);
}
inline int distance(const Vector3i& a,const Vector3i& b){
  int dx=b.x-a.x,dy=b.y-a.y,dz=b.z-a.z;
  return std::sqrt(dx*dx+dy*dy+dz*dz);
}
inline int distance(const Vector4i& a,const Vector4i& b){
  int dx=b.x-a.x,dy=b.y-a.y,dz=b.z-a.z,dw=b.w-a.w;
  return std::sqrt(dx*dx+dy*dy+dz*dz+dw*dw);
}

// Normalize vectors in place
inline void normalize(Vector2f& vec2){
  double len=std::sqrt(vec2.x*vec2.x+vec2.y*vec2.y);
  if(len>0.0){
    float inv=static_cast<float>(1.0/len);
    vec2.x*=inv;
    vec2.y*=inv;
  }
}
inline void normalize(Vector3f& vec3){
  double len=std::sqrt(vec3.x*vec3.x+vec3.y*vec3.y+vec3.z*vec3.z);
  if(len>0.0){
    float inv=static_cast<float>(1.0/len);
    vec3.x*=inv;
    vec3.y*=inv;
    vec3.z*=inv;
  }
}
inline void normalize(Vector4f& vec4){
  double len=std::sqrt(vec4.x*vec4.x+vec4.y*vec4.y+vec4.z*vec4.z+vec4.w*vec4.w);
  if(len>0.0){
    float inv=static_cast<float>(1.0/len);
    vec4.x*=inv;
    vec4.y*=inv;
    vec4.z*=inv;
    vec4.w*=inv;
  }
}

inline void normalize(Vector2i& vec2){
  int len=std::sqrt(vec2.x*vec2.x+vec2.y*vec2.y);
  if(len>0.0){
    int inv=static_cast<int>(1.0/len);
    vec2.x*=inv;
    vec2.y*=inv;
  }
}
inline void normalize(Vector3i& vec3){
  int len=std::sqrt(vec3.x*vec3.x+vec3.y*vec3.y+vec3.z*vec3.z);
  if(len>0.0){
    int inv=static_cast<int>(1.0/len);
    vec3.x*=inv;
    vec3.y*=inv;
    vec3.z*=inv;
  }
}
inline void normalize(Vector4i& vec4){
  int len=std::sqrt(vec4.x*vec4.x+vec4.y*vec4.y+vec4.z*vec4.z+vec4.w*vec4.w);
  if(len>0.0){
    int inv=static_cast<int>(1.0/len);
    vec4.x*=inv;
    vec4.y*=inv;
    vec4.z*=inv;
    vec4.w*=inv;
  }
}

// Vector addition
inline Vector2f add(const Vector2f& a,const Vector2f& b){return Vector2f(a.x+b.x,a.y+b.y);}
inline Vector3f add(const Vector3f& a,const Vector3f& b){return Vector3f(a.x+b.x,a.y+b.y,a.z+b.z);}
inline Vector4f add(const Vector4f& a,const Vector4f& b){return Vector4f(a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w);}

inline Vector2i add(const Vector2i& a,const Vector2i& b){return Vector2i(a.x+b.x,a.y+b.y);}
inline Vector3i add(const Vector3i& a,const Vector3i& b){return Vector3i(a.x+b.x,a.y+b.y,a.z+b.z);}
inline Vector4i add(const Vector4i& a,const Vector4i& b){return Vector4i(a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w);}

// Vector subtraction

inline Vector2f subtract(const Vector2f& a,const Vector2f& b){return Vector2f(a.x-b.x,a.y-b.y);}
inline Vector3f subtract(const Vector3f& a,const Vector3f& b){return Vector3f(a.x-b.x,a.y-b.y,a.z-b.z);}
inline Vector4f subtract(const Vector4f& a,const Vector4f& b){return Vector4f(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w);}

inline Vector2i subtract(const Vector2i& a,const Vector2i& b){return Vector2i(a.x-b.x,a.y-b.y);}
inline Vector3i subtract(const Vector3i& a,const Vector3i& b){return Vector3i(a.x-b.x,a.y-b.y,a.z-b.z);}
inline Vector4i subtract(const Vector4i& a,const Vector4i& b){return Vector4i(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w);}

// Matrix multiplication
inline Matrix2f multiply(const Matrix2f& a,const Matrix2f& b){
  Matrix2f r;
  for(int i=0;i<2;++i){
    float r0=0,r1=0;
    for(int j=0;j<2;++j){
      float v=a.getValue(i,j);
      r0+=b.getValue(j,0)*v;
      r1+=b.getValue(j,1)*v;
    }
    r.setValue(i,0,r0);
    r.setValue(i,1,r1);
  }
  return r;
}
inline Matrix3f multiply(const Matrix3f& a,const Matrix3f& b){
  Matrix3f r;
  for(int i=0;i<3;++i){
    float r0=0,r1=0,r2=0;
    for(int j=0;j<3;++j){
      float v=a.getValue(i,j);
      r0+=b.getValue(j,0)*v;
      r1+=b.getValue(j,1)*v;
      r2+=b.getValue(j,2)*v;
    }
    r.setValue(i,0,r0);
    r.setValue(i,1,r1);
    r.setValue(i,2,r2);
  }
  return r;
}
inline Matrix4f multiply(const Matrix4f& a,const Matrix4f& b){
  Matrix4f r;
  for(int i=0;i<4;++i){
    float r0=0,r1=0,r2=0,r3=0;
    for(int j=0;j<4;++j){
      float v=a.getValue(i,j);
      r0+=b.getValue(j,0)*v;
      r1+=b.getValue(j,1)*v;
      r2+=b.getValue(j,2)*v;
      r3+=b.getValue(j,3)*v;
    }
    r.setValue(i,0,r0);
    r.setValue(i,1,r1);
    r.setValue(i,2,r2);
    r.setValue(i,3,r3);
  }
  return r;
}

inline Matrix2i multiply(const Matrix2i& a,const Matrix2i& b){
  Matrix2i r;
  for(int i=0;i<2;++i){
    int r0=0,r1=0;
    for(int j=0;j<2;++j){
      int v=a.getValue(i,j);
      r0+=b.getValue(j,0)*v;
      r1+=b.getValue(j,1)*v;
    }
    r.setValue(i,0,r0);
    r.setValue(i,1,r1);
  }
  return r;
}
inline Matrix3i multiply(const Matrix3i& a,const Matrix3i& b){
  Matrix3i r;
  for(int i=0;i<3;++i){
    int r0=0,r1=0,r2=0;
    for(int j=0;j<3;++j){
      int v=a.getValue(i,j);
      r0+=b.getValue(j,0)*v;
      r1+=b.getValue(j,1)*v;
      r2+=b.getValue(j,2)*v;
    }
    r.setValue(i,0,r0);
    r.setValue(i,1,r1);
    r.setValue(i,2,r2);
  }
  return r;
}
inline Matrix4i multiply(const Matrix4i& a,const Matrix4i& b){
  Matrix4i r;
  for(int i=0;i<4;++i){
    int r0=0,r1=0,r2=0,r3=0;
    for(int j=0;j<4;++j){
      int v=a.getValue(i,j);
      r0+=b.getValue(j,0)*v;
      r1+=b.getValue(j,1)*v;
      r2+=b.getValue(j,2)*v;
      r3+=b.getValue(j,3)*v;
    }
    r.setValue(i,0,r0);
    r.setValue(i,1,r1);
    r.setValue(i,2,r2);
    r.setValue(i,3,r3);
  }
  return r;
}

// Vector multiply
inline Vector2f multiply(const Vector2f& vec2,float scalar){return Vector2f(vec2.x*scalar,vec2.y*scalar);}
inline Vector3f multiply(const Vector3f& vec3,float scalar){return Vector3f(vec3.x*scalar,vec3.y*scalar,vec3.z*scalar);}
inline Vector4f multiply(const Vector4f& vec4,float scalar){return Vector4f(vec4.x*scalar,vec4.y*scalar,vec4.z*scalar,vec4.w *scalar);}

inline Vector2i multiply(const Vector2i& vec2,int scalar){return Vector2i(vec2.x*scalar,vec2.y*scalar);}
inline Vector3i multiply(const Vector3i& vec3,int scalar){return Vector3i(vec3.x*scalar,vec3.y*scalar,vec3.z*scalar);}
inline Vector4i multiply(const Vector4i& vec4,int scalar){return Vector4i(vec4.x*scalar,vec4.y*scalar,vec4.z*scalar,vec4.w *scalar);}

// Vector divide by scalar (throws assert if zero)

inline Vector2f divide(const Vector2f& vec2,float scalar){
  assert(scalar!=0.0f&&"Division by zero");
  float inv=1.0f/scalar;
  return Vector2f(vec2.x*inv,vec2.y*inv);
}
inline Vector3f divide(const Vector3f& vec3,float scalar){
  assert(scalar!=0.0f&&"Division by zero");
  float inv=1.0f/scalar;
  return Vector3f(vec3.x*inv,vec3.y*inv,vec3.z*inv);
}
inline Vector4f divide(const Vector4f& vec4,float scalar){
  assert(scalar!=0.0f&&"Division by zero");
  float inv=1.0f/scalar;
  return Vector4f(vec4.x*inv,vec4.y*inv,vec4.z*inv,vec4.w*inv);
}

inline Vector2i divide(const Vector2i& vec2,int scalar){
  assert(scalar!=0.0f&&"Division by zero");
  int inv=1.0f/scalar;
  return Vector2i(vec2.x*inv,vec2.y*inv);
}
inline Vector3i divide(const Vector3i& vec3,int scalar){
  assert(scalar!=0.0f&&"Division by zero");
  int inv=1.0f/scalar;
  return Vector3i(vec3.x*inv,vec3.y*inv,vec3.z*inv);
}
inline Vector4i divide(const Vector4i& vec4,int scalar){
  assert(scalar!=0.0f&&"Division by zero");
  int inv=1.0f/scalar;
  return Vector4i(vec4.x*inv,vec4.y*inv,vec4.z*inv,vec4.w*inv);
}

} // namespace g3dl_math
