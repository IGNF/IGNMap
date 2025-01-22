//*----------------------------------------------------------------------------
//								XToneMapper.cpp
//								===============
//
// Auteur : F.Becirspahic - MODSP
//
// Adaptation des travaux de Nasca Octavian Paul
// https://zynaddsubfx.sourceforge.io/other/tonemapping/
// Licence : GPLv3
// 
// 04/02/2010
//*----------------------------------------------------------------------------

#include "XToneMapper.h"

using namespace XToneMapper;

//-----------------------------------------------------------------------------
// Classe ToneMappingParameters
//-----------------------------------------------------------------------------
ToneMappingParameters::ToneMappingParameters()
{
  info_fast_mode=true;
  high_saturation=100;
  low_saturation=100;
  stretch_contrast=true;
  function_id=0;
  for (int i=0;i<TONEMAPPING_MAX_STAGES;i++){
    stage[i].enabled=(i==0);
    stage[i].power=30.0;
    stage[i].blur=80;
  };
  unsharp_mask.enabled=false;
  unsharp_mask.power=30;
  unsharp_mask.blur=4.0;
  unsharp_mask.threshold=0;
}

ToneMappingParameters::~ToneMappingParameters()
{
}

float ToneMappingParameters::get_power(int nstage)
{
  float power=stage[nstage].power;
  power=pow(power/100.0,1.5)*100.0;
  return power;
}

float ToneMappingParameters::get_blur(int nstage)
{
  return stage[nstage].blur;
}

float ToneMappingParameters::get_unsharp_mask_power()
{
  float power=unsharp_mask.power;
  power=pow(power/100.0,3.0)*10.0;
  return power;
}

float ToneMappingParameters::get_unsharp_mask_blur()
{
  return unsharp_mask.blur;
}

//-----------------------------------------------------------------------------
// Classe ToneMappingBase
//-----------------------------------------------------------------------------
ToneMappingBase::ToneMappingBase()
{
  current_process_power_value=20.0;
  preview_zoom=1.0;
}

void ToneMappingBase::set_blur(int nstage, float value)
{
  if (value<0) value=0;
  if (value>10000.0) value=10000.0;
  par.stage[nstage].blur=value;
}

void ToneMappingBase::set_power(int nstage, float value)
{
  if (value<0) value=0;
  if (value>100.0) value=100.0;
  par.stage[nstage].power=value;
}

void ToneMappingBase::set_low_saturation(int value)
{
  if (value<0) value=0;
  if (value>100) value=100;
  par.low_saturation=value;
}

void ToneMappingBase::set_high_saturation(int value)
{
  if (value<0) value=0;
  if (value>100) value=100;
  par.high_saturation=value;
}

void ToneMappingBase::set_stretch_contrast(bool value)
{
  par.stretch_contrast=value;
}

void ToneMappingBase::set_function_id (int value)
{
  if (value<0) value=0;
  if (value>1) value=1;
  par.function_id=value;
}


float ToneMappingBase::func(float x1,float x2)
{
  float result=0.5;
  float p;

  switch (par.function_id){
      case 0://power function
        p=pow(10.0,fabs((x2*2.0-1.0))*current_process_power_value*0.02);
        if (x2>=0.5) result=pow(x1,p);
          else result=1.0-pow(1.0-x1,p);
        break;
      case 1://linear function
        p=1.0/(1+exp(-(x2*2.0-1.0)*current_process_power_value*0.04));
        result=(x1<p)?(x1*(1.0-p)/p):((1.0-p)+(x1-p)*p/(1.0-p));
        break;
  };


  return result;
}

void ToneMappingBase::apply_parameters(ToneMappingParameters inpar)
{
  par=inpar;
  set_low_saturation(par.low_saturation);
  set_high_saturation(par.high_saturation);
  set_stretch_contrast(par.stretch_contrast);
  set_function_id(par.function_id);

  for (int i=0;i< ToneMappingParameters::TONEMAPPING_MAX_STAGES;i++){
    set_power(i,par.stage[i].power);
    set_blur(i,par.stage[i].blur);
  };
  update_preprocessed_values();
}

//-----------------------------------------------------------------------------
// Classe ToneMappingInt
//-----------------------------------------------------------------------------
ToneMappingInt::ToneMappingInt():ToneMappingBase()
{
  int table_size = 65536;
  par.info_fast_mode=true;
  for (int nstage=0;nstage< ToneMappingParameters::TONEMAPPING_MAX_STAGES;nstage++){
    precomputed[nstage].func_lookup_table=new unsigned char [table_size];
    for (int i=0;i< table_size;i++) precomputed[nstage].func_lookup_table[i]=0;
    precomputed[nstage].changed=true;
  };
  current_func_lookup_table=precomputed[0].func_lookup_table;
  // Initialisation
  set_unsharp_mask_enabled(false);
  set_unsharp_mask_power(30.f);
  set_unsharp_mask_blur(4.f);
  set_unsharp_mask_threshold(0.f);
  set_enabled(0, true);
  set_power(0, 30.f);
  set_blur(0, 80.f);
  set_stretch_contrast(true);
  set_low_saturation(100);
  set_high_saturation(100);
}

ToneMappingInt::~ToneMappingInt()
{
  for (int nstage=0;nstage< ToneMappingParameters::TONEMAPPING_MAX_STAGES;nstage++){
    delete [] precomputed[nstage].func_lookup_table;
  };
}

void ToneMappingInt::set_power(int nstage,float value)
{
  ToneMappingBase::set_power(nstage,value);
  precomputed[nstage].changed=true;
}

void ToneMappingInt::set_function_id(int value)
{
  ToneMappingBase::set_function_id(value);
  for (int nstage=0;nstage< ToneMappingParameters::TONEMAPPING_MAX_STAGES;nstage++) precomputed[nstage].changed=true;
}

void ToneMappingInt::recompute_func_table(int nstage)
{
  int pos=0;
  unsigned char *func_lookup_table=precomputed[nstage].func_lookup_table;
  current_process_power_value=par.get_power(nstage);
  for (int x1=0;x1<256;x1++){
    for (int x2=0;x2<256;x2++){
      float f=func(x1/255.0,x2/255.0);
      func_lookup_table[pos]=(int) (f*255.0);
      pos++;
    };
  };
  precomputed[nstage].changed=false;
}

void ToneMappingInt::update_preprocessed_values()
{
  for (int nstage=0;nstage< ToneMappingParameters::TONEMAPPING_MAX_STAGES;nstage++){
    if (precomputed[nstage].changed&&par.stage[nstage].enabled) recompute_func_table(nstage);
  }
}

void ToneMappingInt::get_min_max_data(unsigned char *img,int size,int &min,int &max)
{
  const int ucsize=256; //size of a unsigned char

  //first, we compute the histogram
  unsigned int histogram[ucsize];
  for (int i=0;i<ucsize;i++) histogram[i]=0;
  for (int i=0;i<size;i++){
    histogram[img[i]]++;
  }
  //I want to strip the lowest and upper 0.1 procents (in the histogram) of the pixels
  unsigned int desired_sum=size/1000;
  unsigned int sum_min=0;
  unsigned int sum_max=0;
  for (int i=0;i<ucsize;i++){
    sum_min+=histogram[i];
    if (sum_min>desired_sum){
      min=i;
      break;
    }
  }
  for (int i=ucsize-1;i>=0;i--){
    sum_max+=histogram[i];
    if (sum_max>desired_sum){
      max=i;
      break;
    }
  }
  if (min>=max){
    min=0;
    max=255;
  }
}

void ToneMappingInt::stretch_contrast_8bit_rgb_image(unsigned char *img,int sizex,int sizey,int min,int max,unsigned char *stretch_contrast_table)
{
  const int ucsize=256; //size of a unsigned char
  bool delete_table=false;
  if (stretch_contrast_table==NULL) {
    stretch_contrast_table=new unsigned char[ucsize];
    delete_table=true;
  };
  //compute the lookup table for stretching the contrast
  int diff=max-min;
  for (int i=0;i<ucsize;i++){
    int f=((i-min)<<8)/diff;
    if (f<0) f=0;
    if (f>255) f=255;
    stretch_contrast_table[i]=f;
  };
  int size=sizex*sizey;
  //apply the lookup table
  for (int i=0;i<(size*3);i++){
    img[i]=stretch_contrast_table[img[i]];
  };
  if (delete_table) delete []stretch_contrast_table;
};

void ToneMappingInt::process_8bit_rgb_image(unsigned char *img,int sizex,int sizey){
  update_preprocessed_values();
  int size=sizex*sizey;
  const int ucsize=256; //size of a unsigned char
  unsigned char stretch_contrast_table[ucsize];
  unsigned char *srcimg=new unsigned char[size*3];
  unsigned char *blurimage=new unsigned char[size];

  for (int i=0;i<size*3;i++) srcimg[i]=img[i];

  if (par.stretch_contrast){
    //stretch the contrast
    int min,max;
    get_min_max_data(img,size*3,min,max);
    stretch_contrast_8bit_rgb_image(img,sizex,sizey,min,max,stretch_contrast_table);
  }else{
    for (int i=0;i<ucsize;i++) stretch_contrast_table[i]=i;//no contrast stretch
  };
  for (int nstage=0;nstage< ToneMappingParameters::TONEMAPPING_MAX_STAGES;nstage++){
    if (!par.stage[nstage].enabled) continue;

    int pos;
    //compute the desatured image
    pos=0;
    for (int i=0;i<size;i++){
      blurimage[i]=((int)img[pos]+(int)img[pos+1]+(int)img[pos+2])/3;
      pos+=3;
    };

    current_func_lookup_table=precomputed[nstage].func_lookup_table;

    //blur
    inplace_blur_8bit_process(blurimage,sizex,sizey,par.get_blur(nstage));

    //modify saturation values
    pos=0;
    for (int i=0;i<size;i++){
      unsigned char src_r=img[pos];
      unsigned char src_g=img[pos+1];
      unsigned char src_b=img[pos+2];

      unsigned char blur=blurimage[i];

      unsigned char dest_r=fast_func(src_r,blur);
      unsigned char dest_g=fast_func(src_g,blur);
      unsigned char dest_b=fast_func(src_b,blur);

      img[pos]=dest_r;
      img[pos+1]=dest_g;
      img[pos+2]=dest_b;
      pos+=3;
    };
  };
  int high_saturation_value=100-par.high_saturation;
  int low_saturation_value=100-par.low_saturation;
  if ((par.high_saturation!=100)||(par.low_saturation!=100)){
    int pos=0;
    for (int i=0;i<size;i++){
      unsigned int src_h,src_s,src_v;
      unsigned int dest_h,dest_s,dest_v;
      rgb2hsv(srcimg[pos],srcimg[pos+1],srcimg[pos+2],src_h,src_s,src_v);
      rgb2hsv(img[pos],img[pos+1],img[pos+2],dest_h,dest_s,dest_v);

      unsigned int dest_saturation=(src_s*high_saturation_value+dest_s*(100-high_saturation_value))/100;
      if (dest_v>src_v){
        int s1=dest_saturation*src_v/(dest_v+1);
        dest_saturation=(low_saturation_value*s1+par.low_saturation*dest_saturation)/100;
      };

      hsv2rgb(dest_h,dest_saturation,dest_v,img[pos],img[pos+1],img[pos+2]);

      pos+=3;
    };
  };

  //Unsharp Mask filter
  if (par.unsharp_mask.enabled){
    unsigned char *val=new unsigned char[size];
    //compute the desatured image
    int pos=0;
    for (int i=0;i<size;i++){
      val[i]=blurimage[i]=((int)img[pos]+(int)img[pos+1]+(int)img[pos+2])/3;
      //val[i]=blurimage[i]=max3(img[pos],img[pos+1],img[pos+2]);
      pos+=3;
    };

    float blur_value=par.get_unsharp_mask_blur();
    inplace_blur_8bit_process(blurimage,sizex,sizey,blur_value);

    pos=0;
    int pow=(int)(250*par.get_unsharp_mask_power());
    int threshold=(par.unsharp_mask.threshold*pow)/100;
    int threshold2=threshold/2;

    for (int i=0;i<size;i++){
      int dval=((val[i]-blurimage[i])*pow)/100;

      int abs_dval=abs(dval);
      if (abs_dval<threshold){
        if (abs_dval>threshold2){
          bool sign=(dval<0);
          dval=(abs_dval-threshold2)*2;
          if (sign) dval=-dval;
        }else {
          dval=0;
        };
      };
      int r=img[pos]+dval;
      int g=img[pos+1]+dval;
      int b=img[pos+2]+dval;

      if (r<0) r=0;
      if (r>255) r=255;
      if (g<0) g=0;
      if (g>255) g=255;
      if (b<0) b=0;
      if (b>255) b=255;

      img[pos]=r;
      img[pos+1]=g;
      img[pos+2]=b;
      pos+=3;
    };


    delete[]val;
  };


  delete[]blurimage;
  delete[]srcimg;
};

void ToneMappingInt::inplace_blur_8bit_process(unsigned char *data,int sizex, int sizey, float blur){
  blur/=preview_zoom;
  float af=exp(log(0.5)/blur*sqrt(2));
  if ((af<=0.0)||(af>=1.0)) return;
  unsigned int a=(int)(65536*af*af);
  if (a==0) return;
  for (int y=0;y<sizey;y++){
    int pos=y*sizex;
    unsigned int old=data[pos]<<8;
    pos++;
    for (int x=1;x<sizex;x++){
      old=((data[pos]<<8)*(65535^a)+old*a)>>16;
      data[pos]=old>>8;
      pos++;
    };
    pos=y*sizex+sizex-1;;
    for (int x=1;x<sizex;x++){
      old=((data[pos]<<8)*(65535^a)+old*a)>>16;
      data[pos]=old>>8;
      pos--;
    };

  };

  for (int x=0;x<sizex;x++){
    int pos=x;
    unsigned int old=data[pos]<<8;
    for (int y=1;y<sizey;y++){
      old=((data[pos]<<8)*(65535^a)+old*a)>>16;
      data[pos]=old>>8;
      pos+=sizex;
    };
    pos=x+sizex*(sizey-1);
    for (int y=1;y<sizey;y++){
      old=((data[pos]<<8)*(65535^a)+old*a)>>16;
      data[pos]=old>>8;
      pos-=sizex;
    };

  };

};

//-----------------------------------------------------------------------------
// Classe ToneMappingFloat
//-----------------------------------------------------------------------------
ToneMappingFloat::ToneMappingFloat():ToneMappingBase()
{
  par.info_fast_mode=false;
};

void ToneMappingFloat::process_rgb_image(float *img,int sizex,int sizey){
  update_preprocessed_values();
  int size=sizex*sizey;
  float *blurimage=new float[size];
  float *srcimg=new float[size*3];

  for (int i=0;i<(size*3);i++) srcimg[i]=img[i];
  if (par.stretch_contrast){
    stretch_contrast(img,size*3);
  };

  int pos=0;
  for (int nstage=0;nstage< ToneMappingParameters::TONEMAPPING_MAX_STAGES;nstage++){
    if (!par.stage[nstage].enabled) continue;
    //compute the desatured image
    pos=0;
    for (int i=0;i<size;i++){
      blurimage[i]=(img[pos]+img[pos+1]+img[pos+2])/3.0;
      pos+=3;
    };

    current_process_power_value=par.get_power(nstage);
    //blur
    inplace_blur(blurimage,sizex,sizey,par.get_blur(nstage));

    //
    pos=0;
    for (int i=0;i<size;i++){
      float src_r=img[pos];
      float src_g=img[pos+1];
      float src_b=img[pos+2];

      float blur=blurimage[i];


      float dest_r=func(src_r,blur);
      float dest_g=func(src_g,blur);
      float dest_b=func(src_b,blur);

      img[pos]=dest_r;
      img[pos+1]=dest_g;
      img[pos+2]=dest_b;
      pos+=3;
    };
  };

  int high_saturation_value=100-par.high_saturation;
  int low_saturation_value=100-par.low_saturation;
  if ((par.high_saturation!=100)||(par.low_saturation!=100)){
    int pos=0;
    for (int i=0;i<size;i++){
      float src_h,src_s,src_v;
      float dest_h,dest_s,dest_v;
      rgb2hsv(srcimg[pos],srcimg[pos+1],srcimg[pos+2],src_h,src_s,src_v);
      rgb2hsv(img[pos],img[pos+1],img[pos+2],dest_h,dest_s,dest_v);

      float dest_saturation=(src_s*high_saturation_value+dest_s*(100.0-high_saturation_value))*0.01;
      if (dest_v>src_v){
        float s1=dest_saturation*src_v/(dest_v+1.0/255.0);
        dest_saturation=(low_saturation_value*s1+par.low_saturation*dest_saturation)*0.01;
      };

      hsv2rgb(dest_h,dest_saturation,dest_v,img[pos],img[pos+1],img[pos+2]);

      pos+=3;
    };
  };

  //Unsharp Mask filter
  if (par.unsharp_mask.enabled){
    float *val=new float[size];
    //compute the desatured image
    int pos=0;
    for (int i=0;i<size;i++){
      val[i]=blurimage[i]=(img[pos]+img[pos+1]+img[pos+2])/3.0;
      //val[i]=blurimage[i]=max3(img[pos],img[pos+1],img[pos+2]);
      pos+=3;
    };

    float blur_value=par.get_unsharp_mask_blur();
    inplace_blur(blurimage,sizex,sizey,blur_value);

    pos=0;
    float pow=2.5*par.get_unsharp_mask_power();
    float threshold=par.unsharp_mask.threshold*pow/250.0;
    float threshold2=threshold/2;

    for (int i=0;i<size;i++){
      float dval=(val[i]-blurimage[i])*pow;
      float abs_dval=fabs(dval);
      if (abs_dval<threshold){
        if (abs_dval>threshold2){
          bool sign=(dval<0.0);
          dval=(abs_dval-threshold2)*2.0;
          if (sign) dval=-dval;
        }else {
          dval=0;
        };
      };

      float r=img[pos]+dval;
      float g=img[pos+1]+dval;
      float b=img[pos+2]+dval;

      if (r<0.0) r=0.0;
      if (r>1.0) r=1.0;
      if (g<0.0) g=0.0;
      if (g>1.0) g=1.0;
      if (b<0.0) b=0.0;
      if (b>1.0) b=1.0;

      img[pos]=r;
      img[pos+1]=g;
      img[pos+2]=b;
      pos+=3;
    };

    delete[]val;
  };


  delete[]srcimg;
  delete[]blurimage;
};
void ToneMappingFloat::update_preprocessed_values(){
};

void ToneMappingFloat::process_8bit_rgb_image(unsigned char *img,int sizex,int sizey){
  int size=sizex*sizey;
  float *tmpimage=new float[size*3];

  const float inv_256=1.0/256.0;

  for (int i=0;i<size*3;i++) {//convert to floating point
    tmpimage[i]=img[i]/255.0;
  };

  process_rgb_image(tmpimage,sizex,sizey);

  //convert back to 8 bits (with dithering)
  int pos=0;
  for (int i=0;i<size;i++){
    float dither=((rand()/256)%256)*inv_256;
    img[pos]=(int)(tmpimage[pos]*255.0+dither);
    img[pos+1]=(int)(tmpimage[pos+1]*255.0+dither);
    img[pos+2]=(int)(tmpimage[pos+2]*255.0+dither);
    pos+=3;
  };


  delete[]tmpimage;
};

void ToneMappingFloat::inplace_blur(float *data,int sizex, int sizey, float blur){
  blur/=preview_zoom;
  if (blur<0.3) return;
  float a=exp(log(0.25)/blur);
  if ((a<=0.0)||(a>=1.0)) return;
  a*=a;
  float denormal_remove=1e-15;
  for (int stage=0;stage<2;stage++){
    for (int y=0;y<sizey;y++){
      int pos=y*sizex;
      float old=data[pos];
      pos++;
      for (int x=1;x<sizex;x++){
        old=(data[pos]*(1-a)+old*a)+denormal_remove;
        data[pos]=old;
        pos++;
      };
      pos=y*sizex+sizex-1;;
      for (int x=1;x<sizex;x++){
        old=(data[pos]*(1-a)+old*a)+denormal_remove;
        data[pos]=old;
        pos--;
      };

    };

    for (int x=0;x<sizex;x++){
      int pos=x;
      float old=data[pos];
      for (int y=1;y<sizey;y++){
        old=(data[pos]*(1-a)+old*a)+denormal_remove;
        data[pos]=old;
        pos+=sizex;
      };
      pos=x+sizex*(sizey-1);
      for (int y=1;y<sizey;y++){
        old=(data[pos]*(1-a)+old*a)+denormal_remove;
        data[pos]=old;
        pos-=sizex;
      };

    };
  };
};

void ToneMappingFloat::stretch_contrast(float *data, int datasize){
  //stretch the contrast
  const unsigned int histogram_size=256;
  //first, we compute the histogram
  unsigned int histogram[histogram_size];
  for (int i=0;i<histogram_size;i++) histogram[i]=0;
  for (int i=0;i<datasize;i++){
    int m=(int)(data[i]*(histogram_size-1));
    if (m<0) m=0;
    if (m>(histogram_size-1)) m=histogram_size-1;
    histogram[m]++;
  };

  //I want to strip the lowest and upper 0.1 procents (in the histogram) of the pixels
  int min=0,max=255;
  unsigned int desired_sum=datasize/1000;
  unsigned int sum_min=0;
  unsigned int sum_max=0;
  for (int i=0;i<histogram_size;i++){
    sum_min+=histogram[i];
    if (sum_min>desired_sum){
      min=i;
      break;
    };
  };
  for (int i=histogram_size-1;i>=0;i--){
    sum_max+=histogram[i];
    if (sum_max>desired_sum){
      max=i;
      break;
    };
  };
  if (min>=max){
    min=0;
    max=255;
  };

  float min_src_val=min/255.0;
  float max_src_val=max/255.0;

  for (int i=0;i<datasize;i++) {//stretch the contrast
    float x=data[i];
    x=(x-min_src_val)/(max_src_val-min_src_val);
    if (x<0.0) x=0.0;
    if (x>1.0) x=1.0;
    data[i]=x;
  };
};
