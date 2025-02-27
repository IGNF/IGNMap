//*----------------------------------------------------------------------------
//								XToneMapper.h
//								=============
//
// Auteur : F.Becirspahic - MODSP
//
// Adaptation des travaux de Nasca Octavian Paul
// https://zynaddsubfx.sourceforge.io/other/tonemapping/
// Licence : GPLv3
//
// 04/02/2010
//*----------------------------------------------------------------------------

#ifndef XTONEMAPPER_H
#define XTONEMAPPER_H

#include <cstdlib>
#include <cmath>

//-----------------------------------------------------------------------------
// Espace de noms XToneMapper
//-----------------------------------------------------------------------------
namespace XToneMapper
{

//-----------------------------------------------------------------------------
// Classe ToneMappingParameters
//-----------------------------------------------------------------------------
class ToneMappingParameters{
  public:
    static const int TONEMAPPING_MAX_STAGES = 4;

    ToneMappingParameters();
    ~ToneMappingParameters();

    float get_power(int nstage);
    float get_blur(int nstage);

    float get_unsharp_mask_power();
    float get_unsharp_mask_blur();

    bool info_fast_mode;

    //parameters
    int low_saturation;
    int high_saturation;
    bool stretch_contrast;
    int function_id;
    struct{
      bool enabled;
      float power;
      float blur;
    }stage[TONEMAPPING_MAX_STAGES];

    struct{
      bool enabled;
      float power;
      float blur;
      int threshold;
    } unsharp_mask;
};

//-----------------------------------------------------------------------------
// Classe ToneMappingBase
//-----------------------------------------------------------------------------
class ToneMappingBase{
  public:
    ToneMappingBase();
    virtual ~ToneMappingBase() { ; }

      float func(float x1, float x2);

    virtual void set_blur(int nstage, float value);//1..5000
    virtual void set_power(int nstage, float value);//0..100.0
    virtual void set_low_saturation(int value);//0..100
    virtual void set_high_saturation(int value);//0..100
    virtual void set_stretch_contrast(bool value);
    virtual void set_function_id (int value);//0..1
    void set_enabled(int nstage,bool enabled){
      par.stage[nstage].enabled=enabled;
    };
    void set_info_fast_mode(bool value){
      par.info_fast_mode=value;
    };

    void set_unsharp_mask_enabled(bool value){
      par.unsharp_mask.enabled=value;
    };
    void set_unsharp_mask_power(float value){
      if (value<0.0) value=0.0;
      if (value>100.0) value=100.0;
      par.unsharp_mask.power=value;
    };
    void set_unsharp_mask_blur(float value){
      if (value<0.0) value=0.0;
      if (value>5000.0) value=5000.0;
      par.unsharp_mask.blur=value;
    };
    void set_unsharp_mask_threshold(int value){
      if (value<0) value=0;
      if (value>100) value=100;
      par.unsharp_mask.threshold=value;
    };


    virtual void process_8bit_rgb_image(unsigned char *img,int sizex,int sizey)=0;

    virtual void update_preprocessed_values()=0;

    void apply_parameters(ToneMappingParameters inpar);

    ToneMappingParameters get_parameters(){
      return par;
    };

    float get_enabled(int nstage) const { return par.stage[nstage].enabled;}
    float get_blur(int nstage) const { return par.stage[nstage].blur;}
    float get_power(int nstage) const { return par.stage[nstage].power;}
    int get_low_saturation() const { return par.low_saturation;}
    int get_high_saturation() const { return par.high_saturation;}
    bool get_stretch_contrast() const { return par.stretch_contrast;}
    int get_function_id() const { return par.function_id;}
    bool get_info_fast_mode() const { return par.info_fast_mode;}

    bool get_unsharp_mask_enabled() const { return par.unsharp_mask.enabled;}
    float get_unsharp_mask_power() const { return par.unsharp_mask.power;}
    float get_unsharp_mask_() const { return par.unsharp_mask.blur;}
    int get_unsharp_mask_threshold() const { return par.unsharp_mask.threshold;}

    void set_current_stage(int nstage){
      current_process_power_value=par.get_power(nstage);
    };

    void set_preview_zoom(float val){
      if ((val>0.001)&&(val<1000.0)) preview_zoom=val;
    };
  protected:
    float preview_zoom;//used for zoom on previews
    ToneMappingParameters par;

    //preprocessed values
    float current_process_power_value;
};

//-----------------------------------------------------------------------------
// Classe ToneMappingInt
//-----------------------------------------------------------------------------
class ToneMappingInt:public ToneMappingBase{
  public:
    ToneMappingInt();
    ~ToneMappingInt();
    void set_power(int nstage, float value);
    void set_function_id(int value);

    void process_8bit_rgb_image(unsigned char *img,int sizex,int sizey);
    void recompute_func_table(int nstage);
    void update_preprocessed_values();

    void get_min_max_data(unsigned char *img,int size,int &min,int &max);
    void stretch_contrast_8bit_rgb_image(unsigned char *img,int sizex,int sizey,int min,int max,unsigned char *stretch_contrast_table=NULL);
  private:
    void inplace_blur_8bit_process(unsigned char *data,int sizex, int sizey,float blur);
    inline unsigned char fast_func(unsigned char x1, unsigned char x2){
      return current_func_lookup_table[(((int)x1)<<8)+x2];
    };
    inline unsigned int max3(unsigned int x1,unsigned int x2,unsigned int x3){
      unsigned int max=x1;
      if (x2>max) max=x2;
      if (x3>max) max=x3;
      return max;
    };
    inline unsigned int min3(unsigned int x1,unsigned int x2,unsigned int x3){
      unsigned int min=x1;
      if (x2<min) min=x2;
      if (x3<min) min=x3;
      return min;
    };
    inline void rgb2hsv(unsigned char r,unsigned char g,unsigned char b,
              unsigned int &h,unsigned int &s,unsigned int &v){
      unsigned char min=min3(r,g,b);
      unsigned char max=max3(r,g,b);
      int diff=max-min;
      if (max==0) {
        h=v=s=0;
        return;
      };
      //value
      v=max;

      //saturation
      s=(255*diff)/v;

      if (diff==0){
        h=0;
        return;
      };
      //hue
      if (max==r) {
        h=(4096*((int)g-(int)b)/diff+24576)%24576;
        return;
      };
      if (max==g) {
        h=8192+4096*((int)b-(int)r)/diff;
        return;
      };
      if (max==b) {
        h=16384+4096*((int)r-(int)g)/diff;
        return;
      };
    };
    inline void hsv2rgb(unsigned int h,unsigned int s,unsigned int v,
              unsigned char &r,unsigned char &g,unsigned char &b){
      unsigned int hi=(h>>12)%6;
      unsigned int f=(h&4095)>>4;

      unsigned char p=(v*(255^s))>>8;
      unsigned char q=( v*(65535^(f*s)))>>16;
      unsigned char t=(v*(65535^(255^f)*s))>>16;

      switch(hi){
        case 0:
          r=v;g=t;b=p;
          break;
        case 1:
          r=q;g=v;b=p;
          break;
        case 2:
          r=p;g=v;b=t;
          break;
        case 3:
          r=p;g=q;b=v;
          break;
        case 4:
          r=t;g=p;b=v;
          break;
        case 5:
          r=v;g=p;b=q;
          break;
      };
    };
    struct {
      bool changed;
      unsigned char *func_lookup_table;
    }precomputed[ToneMappingParameters::TONEMAPPING_MAX_STAGES];
    unsigned char *current_func_lookup_table;
};

//-----------------------------------------------------------------------------
// Classe ToneMappingFloat
//-----------------------------------------------------------------------------
class ToneMappingFloat:public ToneMappingBase{
  public:
    ToneMappingFloat();
    ~ToneMappingFloat() { ; }

    void process_8bit_rgb_image(unsigned char *img,int sizex,int sizey);
    void process_rgb_image(float *img,int sizex,int sizey);
    void update_preprocessed_values();

  private:
    void inplace_blur(float*data,int sizex, int sizey, float blur);
    void stretch_contrast(float*data, int datasize);
    inline void rgb2hsv(const float&r, const float&g, const float&b,
      float&h, float&s, float&v){

      float maxrg=(r>g)?r:g;
      float max=(maxrg>b)?maxrg:b;

      float minrg=(r<g)?r:g;
      float min=(minrg<b)?minrg:b;

      float delta=max-min;

      //hue
      if (min==max){
        h=0.0;
      }else{
        if (max==r){
          h=fmod(60.0*(g-b)/delta+360.0,360.0);
        }else{
          if (max==g){
            h=60.0*(b-r)/delta+120.0;
          }else{//max==b
            h=60.0*(r-g)/delta+240.0;
          };
        };
      };

      //saturation
      if (max<1e-6){
        s=0;
      }else{
        s=1.0-min/max;
      };

      //value
      v=max;

    };

    inline void hsv2rgb(const float&h,const float&s,const float&v,
      float&r, float&g, float&b){
      float hfi = (float)floor(h/60.0);
      float f = (float)(h/60.0)-hfi;
      int hi=((int)hfi)%6;

      float p = (float)(v*(1.0-s));
      float q = (float)(v*(1.0-f*s));
      float t = (float)(v*(1.0-(1.0-f)*s));
      switch (hi){
        case 0:
          r=v;g=t;b=p;
          break;
        case 1:
          r=q;g=v;b=p;
          break;
        case 2:
          r=p;g=v;b=t;
          break;
        case 3:
          r=p;g=q;b=v;
          break;
        case 4:
          r=t;g=p;b=v;
          break;
        case 5:
          r=v;g=p;b=q;
          break;
      };
    };

};

} // Fin de l'espace de noms XToneMapper

#endif // XTONEMAPPER_H
