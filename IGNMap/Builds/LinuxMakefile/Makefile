# Automatically generated makefile, created by the Projucer
# Don't edit this file! Your changes will be overwritten when you re-save the Projucer project!

# build with "V=1" for verbose builds
ifeq ($(V), 1)
V_AT =
else
V_AT = @
endif

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef PKG_CONFIG
  PKG_CONFIG=pkg-config
endif

ifndef STRIP
  STRIP=strip
endif

ifndef AR
  AR=ar
endif

ifndef CONFIG
  CONFIG=Debug
endif

JUCE_ARCH_LABEL := $(shell uname -m)

ifeq ($(CONFIG),Debug)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Debug
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := 
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DDEBUG=1" "-D_DEBUG=1" "-DJUCE_PROJUCER_VERSION=0x8000e" "-DJUCE_MODULE_AVAILABLE_juce_core=1" "-DJUCE_MODULE_AVAILABLE_juce_data_structures=1" "-DJUCE_MODULE_AVAILABLE_juce_events=1" "-DJUCE_MODULE_AVAILABLE_juce_graphics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_extra=1" "-DJUCE_MODULE_AVAILABLE_juce_opengl=1" "-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1" "-DJUCE_STRICT_REFCOUNTEDPOINTER=1" "-DJUCE_STANDALONE_APPLICATION=1" "-DJUCE_MODAL_LOOPS_PERMITTED=1" "-DTINYEXIF_NO_XMP_SUPPORT=1" "-DOPJ_STATIC=1" "-DLASZIPDLL_EXPORTS=1" "-DJUCE_WEB_BROWSER=0" "-DJUCER_LINUX_MAKE_6D53C8B4=1" "-DJUCE_APP_VERSION=1.0.0" "-DJUCE_APP_VERSION_HEX=0x10000" $(shell $(PKG_CONFIG) --cflags $(shell ($(PKG_CONFIG) --exists webkit2gtk-4.1 && echo webkit2gtk-4.1) || echo webkit2gtk-4.0) freetype2 fontconfig gl libcurl gtk+-x11-3.0) -pthread -I../../JuceLibraryCode -I../../../JUCE/modules -I../../../libwebp -I../../../protozero/include -I../../../vtzero/include -I../../../LASzip/include/laszip $(CPPFLAGS)
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0" "-DJucePlugin_Build_LV2=0"
  JUCE_TARGET_APP := IGNMap

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0 $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++17 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) $(shell $(PKG_CONFIG) --libs freetype2 fontconfig gl libcurl) -fvisibility=hidden -lrt -ldl -lpthread $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(JUCE_OBJDIR)
endif

ifeq ($(CONFIG),Release)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Release
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := 
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DNDEBUG=1" "-DJUCE_PROJUCER_VERSION=0x8000e" "-DJUCE_MODULE_AVAILABLE_juce_core=1" "-DJUCE_MODULE_AVAILABLE_juce_data_structures=1" "-DJUCE_MODULE_AVAILABLE_juce_events=1" "-DJUCE_MODULE_AVAILABLE_juce_graphics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_extra=1" "-DJUCE_MODULE_AVAILABLE_juce_opengl=1" "-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1" "-DJUCE_STRICT_REFCOUNTEDPOINTER=1" "-DJUCE_STANDALONE_APPLICATION=1" "-DJUCE_MODAL_LOOPS_PERMITTED=1" "-DTINYEXIF_NO_XMP_SUPPORT=1" "-DOPJ_STATIC=1" "-DLASZIPDLL_EXPORTS=1" "-DJUCE_WEB_BROWSER=0" "-DJUCER_LINUX_MAKE_6D53C8B4=1" "-DJUCE_APP_VERSION=1.0.0" "-DJUCE_APP_VERSION_HEX=0x10000" $(shell $(PKG_CONFIG) --cflags $(shell ($(PKG_CONFIG) --exists webkit2gtk-4.1 && echo webkit2gtk-4.1) || echo webkit2gtk-4.0) freetype2 fontconfig gl libcurl gtk+-x11-3.0) -pthread -I../../JuceLibraryCode -I../../../JUCE/modules -I../../../libwebp -I../../../protozero/include -I../../../vtzero/include -I../../../LASzip/include/laszip $(CPPFLAGS)
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0" "-DJucePlugin_Build_LV2=0"
  JUCE_TARGET_APP := IGNMap

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -O3 $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++17 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) $(shell $(PKG_CONFIG) --libs freetype2 fontconfig gl libcurl) -fvisibility=hidden -lrt -ldl -lpthread $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(JUCE_OBJDIR)
endif

OBJECTS_APP := \
  $(JUCE_OBJDIR)/AffineImage_f63fb19d.o \
  $(JUCE_OBJDIR)/AnnotViewer_a719b399.o \
  $(JUCE_OBJDIR)/AppUtil_5c6d9ca4.o \
  $(JUCE_OBJDIR)/ClassViewer_44f830b.o \
  $(JUCE_OBJDIR)/DtmLayersViewer_98c8b452.o \
  $(JUCE_OBJDIR)/DtmShader_bfa70483.o \
  $(JUCE_OBJDIR)/DtmTmsLayer_da3751b5.o \
  $(JUCE_OBJDIR)/ExportDtmDlg_e5d53c1f.o \
  $(JUCE_OBJDIR)/ExportImageDlg_2dc44e1.o \
  $(JUCE_OBJDIR)/ExportLasDlg_f653953e.o \
  $(JUCE_OBJDIR)/GeoBase_343e9923.o \
  $(JUCE_OBJDIR)/GeoSearch_6564213a.o \
  $(JUCE_OBJDIR)/ImageLayersViewer_843f60d0.o \
  $(JUCE_OBJDIR)/ImageOptionsViewer_66bba7be.o \
  $(JUCE_OBJDIR)/LasLayersViewer_53432e13.o \
  $(JUCE_OBJDIR)/LasShader_1eec2304.o \
  $(JUCE_OBJDIR)/Main_90ebc5c2.o \
  $(JUCE_OBJDIR)/MainComponent_a6ffb4a5.o \
  $(JUCE_OBJDIR)/MainComponentToolbarFactory_861e9a54.o \
  $(JUCE_OBJDIR)/MapThread_564c0ce7.o \
  $(JUCE_OBJDIR)/MapView_229c9e02.o \
  $(JUCE_OBJDIR)/MvtLayer_36152faf.o \
  $(JUCE_OBJDIR)/ObjectViewer_faa9c63a.o \
  $(JUCE_OBJDIR)/OGL3DViewer_c264a858.o \
  $(JUCE_OBJDIR)/OsmLayer_d8c84a51.o \
  $(JUCE_OBJDIR)/PrefDlg_9f17261d.o \
  $(JUCE_OBJDIR)/ProfilViewer_58ce88f7.o \
  $(JUCE_OBJDIR)/SelTreeViewer_bd03fdab.o \
  $(JUCE_OBJDIR)/SentinelViewer_a6f38bdf.o \
  $(JUCE_OBJDIR)/StacViewer_e5b1993e.o \
  $(JUCE_OBJDIR)/StereoViewer_ac9ce7b3.o \
  $(JUCE_OBJDIR)/TmsLayer_3b9cfb60.o \
  $(JUCE_OBJDIR)/VectorLayersViewer_544e20e0.o \
  $(JUCE_OBJDIR)/WmtsLayer_f85ab9dd.o \
  $(JUCE_OBJDIR)/WmtsTmsViewer_a3a54f58.o \
  $(JUCE_OBJDIR)/ZoomViewer_a6d902ce.o \
  $(JUCE_OBJDIR)/jaricom_71e13aae.o \
  $(JUCE_OBJDIR)/jcapimin_89469a80.o \
  $(JUCE_OBJDIR)/jcapistd_89a00211.o \
  $(JUCE_OBJDIR)/jcarith_ce0c02ce.o \
  $(JUCE_OBJDIR)/jccoefct_1cea0e46.o \
  $(JUCE_OBJDIR)/jccolor_32e215a5.o \
  $(JUCE_OBJDIR)/jcdctmgr_23fd93eb.o \
  $(JUCE_OBJDIR)/jchuff_8e9f1e95.o \
  $(JUCE_OBJDIR)/jcinit_8ff2e458.o \
  $(JUCE_OBJDIR)/jcmainct_4e62aff2.o \
  $(JUCE_OBJDIR)/jcmarker_5d94e842.o \
  $(JUCE_OBJDIR)/jcmaster_5fc8946a.o \
  $(JUCE_OBJDIR)/jcomapi_a9a87b7e.o \
  $(JUCE_OBJDIR)/jcparam_cafd5c4f.o \
  $(JUCE_OBJDIR)/jcprepct_298f266.o \
  $(JUCE_OBJDIR)/jcsample_c49d2db2.o \
  $(JUCE_OBJDIR)/jctrans_baaf1baa.o \
  $(JUCE_OBJDIR)/jdapimin_1d8b0981.o \
  $(JUCE_OBJDIR)/jdapistd_1de47112.o \
  $(JUCE_OBJDIR)/jdarith_35ed2fad.o \
  $(JUCE_OBJDIR)/jdatadst_e3030040.o \
  $(JUCE_OBJDIR)/jdatasrc_e3d5ac9f.o \
  $(JUCE_OBJDIR)/jdcoefct_b12e7d47.o \
  $(JUCE_OBJDIR)/jdcolor_9ac34284.o \
  $(JUCE_OBJDIR)/jddctmgr_b84202ec.o \
  $(JUCE_OBJDIR)/jdhuff_c38559d6.o \
  $(JUCE_OBJDIR)/jdinput_d6aeed2b.o \
  $(JUCE_OBJDIR)/jdmainct_e2a71ef3.o \
  $(JUCE_OBJDIR)/jdmarker_f1d95743.o \
  $(JUCE_OBJDIR)/jdmaster_f40d036b.o \
  $(JUCE_OBJDIR)/jdmerge_9b01d619.o \
  $(JUCE_OBJDIR)/jdpostct_1046e65a.o \
  $(JUCE_OBJDIR)/jdsample_58e19cb3.o \
  $(JUCE_OBJDIR)/jdtrans_22904889.o \
  $(JUCE_OBJDIR)/jerror_956092d.o \
  $(JUCE_OBJDIR)/jfdctflt_e06a89c4.o \
  $(JUCE_OBJDIR)/jfdctfst_e06db85d.o \
  $(JUCE_OBJDIR)/jfdctint_e095b905.o \
  $(JUCE_OBJDIR)/jidctflt_9d37d6c7.o \
  $(JUCE_OBJDIR)/jidctfst_9d3b0560.o \
  $(JUCE_OBJDIR)/jidctint_9d630608.o \
  $(JUCE_OBJDIR)/jmemmgr_a81b1028.o \
  $(JUCE_OBJDIR)/jmemnobs_5d66408c.o \
  $(JUCE_OBJDIR)/jquant1_819c5c93.o \
  $(JUCE_OBJDIR)/jquant2_819c6054.o \
  $(JUCE_OBJDIR)/jutils_5aa34196.o \
  $(JUCE_OBJDIR)/arithmeticdecoder_9f9abec7.o \
  $(JUCE_OBJDIR)/arithmeticencoder_cb146b9f.o \
  $(JUCE_OBJDIR)/arithmeticmodel_63af2eac.o \
  $(JUCE_OBJDIR)/integercompressor_23e37670.o \
  $(JUCE_OBJDIR)/lasindex_6ec12af1.o \
  $(JUCE_OBJDIR)/lasinterval_53764450.o \
  $(JUCE_OBJDIR)/lasmessage_7f05ed26.o \
  $(JUCE_OBJDIR)/lasquadtree_f026a8f0.o \
  $(JUCE_OBJDIR)/lasreaditemcompressed_v1_f51600f.o \
  $(JUCE_OBJDIR)/lasreaditemcompressed_v2_f5f7790.o \
  $(JUCE_OBJDIR)/lasreaditemcompressed_v3_f6d8f11.o \
  $(JUCE_OBJDIR)/lasreaditemcompressed_v4_f7ba692.o \
  $(JUCE_OBJDIR)/lasreadpoint_e61892d9.o \
  $(JUCE_OBJDIR)/laswriteitemcompressed_v1_d3846dd2.o \
  $(JUCE_OBJDIR)/laswriteitemcompressed_v2_d3928553.o \
  $(JUCE_OBJDIR)/laswriteitemcompressed_v3_d3a09cd4.o \
  $(JUCE_OBJDIR)/laswriteitemcompressed_v4_d3aeb455.o \
  $(JUCE_OBJDIR)/laswritepoint_6437ed1c.o \
  $(JUCE_OBJDIR)/laszip_f8e72c40.o \
  $(JUCE_OBJDIR)/laszip_dll_ad018865.o \
  $(JUCE_OBJDIR)/mydefs_619244b7.o \
  $(JUCE_OBJDIR)/alpha_dec_65935d01.o \
  $(JUCE_OBJDIR)/buffer_dec_6133586d.o \
  $(JUCE_OBJDIR)/frame_dec_32bfcdd0.o \
  $(JUCE_OBJDIR)/idec_dec_e438bda6.o \
  $(JUCE_OBJDIR)/io_dec_b96d2f53.o \
  $(JUCE_OBJDIR)/quant_dec_f6ce7526.o \
  $(JUCE_OBJDIR)/tree_dec_e8eb228b.o \
  $(JUCE_OBJDIR)/vp8_dec_4bebdb21.o \
  $(JUCE_OBJDIR)/vp8l_dec_d8abe25b.o \
  $(JUCE_OBJDIR)/webp_dec_4c8ed809.o \
  $(JUCE_OBJDIR)/alpha_processing_d6c5115f.o \
  $(JUCE_OBJDIR)/alpha_processing_mips_dsp_r2_c42315be.o \
  $(JUCE_OBJDIR)/alpha_processing_neon_7973f640.o \
  $(JUCE_OBJDIR)/alpha_processing_sse2_82bc0d77.o \
  $(JUCE_OBJDIR)/alpha_processing_sse41_d4c68f62.o \
  $(JUCE_OBJDIR)/cost_b2bd7b58.o \
  $(JUCE_OBJDIR)/cost_mips_dsp_r2_27375237.o \
  $(JUCE_OBJDIR)/cost_mips32_fc78244f.o \
  $(JUCE_OBJDIR)/cost_neon_2e3bdfe7.o \
  $(JUCE_OBJDIR)/cost_sse2_3783f71e.o \
  $(JUCE_OBJDIR)/cpu_68dd4607.o \
  $(JUCE_OBJDIR)/dec_68e619e1.o \
  $(JUCE_OBJDIR)/dec_clip_tables_e90beb16.o \
  $(JUCE_OBJDIR)/dec_mips_dsp_r2_63816b40.o \
  $(JUCE_OBJDIR)/dec_mips32_1a82c9a6.o \
  $(JUCE_OBJDIR)/dec_msa_46f45b9d.o \
  $(JUCE_OBJDIR)/dec_neon_988deefe.o \
  $(JUCE_OBJDIR)/dec_sse2_a1d60635.o \
  $(JUCE_OBJDIR)/dec_sse41_98ebae64.o \
  $(JUCE_OBJDIR)/enc_68f848b9.o \
  $(JUCE_OBJDIR)/enc_mips_dsp_r2_a24c1618.o \
  $(JUCE_OBJDIR)/enc_mips32_5e3eb7ce.o \
  $(JUCE_OBJDIR)/enc_msa_82235e75.o \
  $(JUCE_OBJDIR)/enc_neon_c33f4726.o \
  $(JUCE_OBJDIR)/enc_sse2_cc875e5d.o \
  $(JUCE_OBJDIR)/enc_sse41_c4655b3c.o \
  $(JUCE_OBJDIR)/filters_e7eda9a.o \
  $(JUCE_OBJDIR)/filters_mips_dsp_r2_6a9de79.o \
  $(JUCE_OBJDIR)/filters_msa_fc5c17d6.o \
  $(JUCE_OBJDIR)/filters_neon_901db9e5.o \
  $(JUCE_OBJDIR)/filters_sse2_9965d11c.o \
  $(JUCE_OBJDIR)/lossless_a22d7007.o \
  $(JUCE_OBJDIR)/lossless_enc_42453462.o \
  $(JUCE_OBJDIR)/lossless_enc_mips_dsp_r2_a50fcc41.o \
  $(JUCE_OBJDIR)/lossless_enc_mips32_ff4a0c05.o \
  $(JUCE_OBJDIR)/lossless_enc_msa_ce504d9e.o \
  $(JUCE_OBJDIR)/lossless_enc_neon_fcb03d1d.o \
  $(JUCE_OBJDIR)/lossless_enc_sse2_5f85454.o \
  $(JUCE_OBJDIR)/lossless_enc_sse41_b9132425.o \
  $(JUCE_OBJDIR)/lossless_mips_dsp_r2_55eab866.o \
  $(JUCE_OBJDIR)/lossless_msa_42b82ec3.o \
  $(JUCE_OBJDIR)/lossless_neon_15448098.o \
  $(JUCE_OBJDIR)/lossless_sse2_1e8c97cf.o \
  $(JUCE_OBJDIR)/lossless_sse41_b307500a.o \
  $(JUCE_OBJDIR)/rescaler_306d0166.o \
  $(JUCE_OBJDIR)/rescaler_mips_dsp_r2_36f83345.o \
  $(JUCE_OBJDIR)/rescaler_mips32_17b65281.o \
  $(JUCE_OBJDIR)/rescaler_msa_9a01f8a2.o \
  $(JUCE_OBJDIR)/rescaler_neon_a733f299.o \
  $(JUCE_OBJDIR)/rescaler_sse2_b07c09d0.o \
  $(JUCE_OBJDIR)/ssim_ce3ebd4f.o \
  $(JUCE_OBJDIR)/ssim_sse2_b5205787.o \
  $(JUCE_OBJDIR)/upsampling_db494d4d.o \
  $(JUCE_OBJDIR)/upsampling_mips_dsp_r2_12f6dcac.o \
  $(JUCE_OBJDIR)/upsampling_msa_a477f909.o \
  $(JUCE_OBJDIR)/upsampling_neon_eb7dff12.o \
  $(JUCE_OBJDIR)/upsampling_sse2_f4c61649.o \
  $(JUCE_OBJDIR)/upsampling_sse41_a3fda0d0.o \
  $(JUCE_OBJDIR)/yuv_6a1594b9.o \
  $(JUCE_OBJDIR)/yuv_mips_dsp_r2_c5d76218.o \
  $(JUCE_OBJDIR)/yuv_mips32_2bd3ebce.o \
  $(JUCE_OBJDIR)/yuv_neon_9c917b26.o \
  $(JUCE_OBJDIR)/yuv_sse2_a5d9925d.o \
  $(JUCE_OBJDIR)/yuv_sse41_1559a73c.o \
  $(JUCE_OBJDIR)/bit_reader_utils_cccde142.o \
  $(JUCE_OBJDIR)/bit_writer_utils_dea27cf2.o \
  $(JUCE_OBJDIR)/color_cache_utils_63a946a7.o \
  $(JUCE_OBJDIR)/filters_utils_68f0d6fc.o \
  $(JUCE_OBJDIR)/huffman_encode_utils_3e5fd5f5.o \
  $(JUCE_OBJDIR)/huffman_utils_3932d4ae.o \
  $(JUCE_OBJDIR)/quant_levels_dec_utils_ebd0413b.o \
  $(JUCE_OBJDIR)/quant_levels_utils_aa661178.o \
  $(JUCE_OBJDIR)/random_utils_638e3ad0.o \
  $(JUCE_OBJDIR)/rescaler_utils_9a0258e8.o \
  $(JUCE_OBJDIR)/thread_utils_ce1e2d17.o \
  $(JUCE_OBJDIR)/utils_eae63140.o \
  $(JUCE_OBJDIR)/bio_2615f04d.o \
  $(JUCE_OBJDIR)/cio_262407ce.o \
  $(JUCE_OBJDIR)/dwt_26388f46.o \
  $(JUCE_OBJDIR)/event_accd355f.o \
  $(JUCE_OBJDIR)/function_list_88c0b8aa.o \
  $(JUCE_OBJDIR)/ht_dec_afe07734.o \
  $(JUCE_OBJDIR)/image_70cebfe0.o \
  $(JUCE_OBJDIR)/invert_1aff6fb.o \
  $(JUCE_OBJDIR)/j2k_266d9ce8.o \
  $(JUCE_OBJDIR)/jp2_2688f5f1.o \
  $(JUCE_OBJDIR)/mct_26ae4b63.o \
  $(JUCE_OBJDIR)/mqc_26b468c4.o \
  $(JUCE_OBJDIR)/openjpeg_202db5d7.o \
  $(JUCE_OBJDIR)/opj_clock_3c1a863d.o \
  $(JUCE_OBJDIR)/opj_malloc_b805263.o \
  $(JUCE_OBJDIR)/pi_74ddc05e.o \
  $(JUCE_OBJDIR)/sparse_array_66e19f.o \
  $(JUCE_OBJDIR)/t1_74debfa2.o \
  $(JUCE_OBJDIR)/t2_74dec363.o \
  $(JUCE_OBJDIR)/tcd_2710b3da.o \
  $(JUCE_OBJDIR)/tgt_2712c166.o \
  $(JUCE_OBJDIR)/thread_341c282f.o \
  $(JUCE_OBJDIR)/sqlite3_31f93e88.o \
  $(JUCE_OBJDIR)/TinyEXIF_2b40a0f5.o \
  $(JUCE_OBJDIR)/XEndian_2fd684da.o \
  $(JUCE_OBJDIR)/XFile_699380bb.o \
  $(JUCE_OBJDIR)/XFrame_3a782858.o \
  $(JUCE_OBJDIR)/XGeoBase_f54870ad.o \
  $(JUCE_OBJDIR)/XGeoClass_221ebd66.o \
  $(JUCE_OBJDIR)/XGeoFDtm_98de0273.o \
  $(JUCE_OBJDIR)/XGeoLayer_bf7d54bf.o \
  $(JUCE_OBJDIR)/XGeoLine_a2bdd050.o \
  $(JUCE_OBJDIR)/XGeoMap_8f9c218a.o \
  $(JUCE_OBJDIR)/XGeoPoint_7ff39d7e.o \
  $(JUCE_OBJDIR)/XGeoPoly_7d580c28.o \
  $(JUCE_OBJDIR)/XGeoRaster_81934b99.o \
  $(JUCE_OBJDIR)/XGeoRepres_314321ff.o \
  $(JUCE_OBJDIR)/XGeoVector_36c8745f.o \
  $(JUCE_OBJDIR)/XInterpol_65a0c3b0.o \
  $(JUCE_OBJDIR)/XMat3D_eaaf80bc.o \
  $(JUCE_OBJDIR)/XParserXML_f683e5e3.o \
  $(JUCE_OBJDIR)/XPath_defe72e4.o \
  $(JUCE_OBJDIR)/XPolygone2D_17ca3d3c.o \
  $(JUCE_OBJDIR)/XPt2D_5979b195.o \
  $(JUCE_OBJDIR)/XPt3D_5b2e8a34.o \
  $(JUCE_OBJDIR)/XXml_ba111f82.o \
  $(JUCE_OBJDIR)/XAffinity2D_9345022c.o \
  $(JUCE_OBJDIR)/XInternetMap_ca70d833.o \
  $(JUCE_OBJDIR)/XLasFile_52021412.o \
  $(JUCE_OBJDIR)/XOrthoModel_aa1c1383.o \
  $(JUCE_OBJDIR)/XSentinelScene_b6bda160.o \
  $(JUCE_OBJDIR)/XStereoModel_40b428e9.o \
  $(JUCE_OBJDIR)/XStlWriter_8f030b96.o \
  $(JUCE_OBJDIR)/XTime_636b2fbf.o \
  $(JUCE_OBJDIR)/XToneMapper_61a81e85.o \
  $(JUCE_OBJDIR)/XWmts_7b5ed8c7.o \
  $(JUCE_OBJDIR)/XAlgoGeod_b79ba2f8.o \
  $(JUCE_OBJDIR)/XDomGeod_f9f0910d.o \
  $(JUCE_OBJDIR)/XGeodGrid_bc53526b.o \
  $(JUCE_OBJDIR)/XGeoPref_9bf06b0c.o \
  $(JUCE_OBJDIR)/XGeoProjection_aa910638.o \
  $(JUCE_OBJDIR)/XLambert_3fd1b4d5.o \
  $(JUCE_OBJDIR)/XNCGeod_20a3b77a.o \
  $(JUCE_OBJDIR)/XTransfoGeod_389308bc.o \
  $(JUCE_OBJDIR)/XWebMercator_c15a630b.o \
  $(JUCE_OBJDIR)/XBaseImage_893cd4bc.o \
  $(JUCE_OBJDIR)/XBaseTiffReader_ced0bf21.o \
  $(JUCE_OBJDIR)/XBigTiffReader_64bd9e6a.o \
  $(JUCE_OBJDIR)/XCogImage_c11fa1b8.o \
  $(JUCE_OBJDIR)/XDtmShader_ffa78bd4.o \
  $(JUCE_OBJDIR)/XFileImage_6e953671.o \
  $(JUCE_OBJDIR)/XJpeg2000Image_783c8387.o \
  $(JUCE_OBJDIR)/XJpegCodec_26236640.o \
  $(JUCE_OBJDIR)/XJpegImage_34ced505.o \
  $(JUCE_OBJDIR)/XLzwCodec_a21dc385.o \
  $(JUCE_OBJDIR)/XMemRaster_88ea7764.o \
  $(JUCE_OBJDIR)/XOpenJp2Image_9a97e091.o \
  $(JUCE_OBJDIR)/XPackBitsCodec_476eb49.o \
  $(JUCE_OBJDIR)/XPredictor_739e6f2e.o \
  $(JUCE_OBJDIR)/XStbImage_5f423fd2.o \
  $(JUCE_OBJDIR)/XTiffReader_7e523770.o \
  $(JUCE_OBJDIR)/XTiffStripImage_304717d0.o \
  $(JUCE_OBJDIR)/XTiffTileImage_c8a2bf0a.o \
  $(JUCE_OBJDIR)/XTiffWriter_58a82b20.o \
  $(JUCE_OBJDIR)/XWebPCodec_3ce15dac.o \
  $(JUCE_OBJDIR)/XWebPImage_4b8ccc71.o \
  $(JUCE_OBJDIR)/XZlibCodec_7db0ce3d.o \
  $(JUCE_OBJDIR)/XAnnotation_f349c011.o \
  $(JUCE_OBJDIR)/XCsv_cb1cb8ae.o \
  $(JUCE_OBJDIR)/XDBase_47d5afbd.o \
  $(JUCE_OBJDIR)/XDxf_7a9b6fa.o \
  $(JUCE_OBJDIR)/XGeoJson_9a8e4381.o \
  $(JUCE_OBJDIR)/XGpkgMap_9b3203bf.o \
  $(JUCE_OBJDIR)/XMifMid_16cf4f80.o \
  $(JUCE_OBJDIR)/XShapefile_324c5d85.o \
  $(JUCE_OBJDIR)/XShapefileConverter_24c58b65.o \
  $(JUCE_OBJDIR)/XShapefileRecord_3816b976.o \
  $(JUCE_OBJDIR)/XShapefileVector_d626568.o \
  $(JUCE_OBJDIR)/XTAChantier_c1ce0d83.o \
  $(JUCE_OBJDIR)/XWKBGeom_ed139392.o \
  $(JUCE_OBJDIR)/adler32_b1f51f5.o \
  $(JUCE_OBJDIR)/compress_50f36cec.o \
  $(JUCE_OBJDIR)/crc32_239c1c73.o \
  $(JUCE_OBJDIR)/deflate_fd8910ab.o \
  $(JUCE_OBJDIR)/gzclose_a1e12ee5.o \
  $(JUCE_OBJDIR)/gzlib_573e232.o \
  $(JUCE_OBJDIR)/gzread_b30d6a13.o \
  $(JUCE_OBJDIR)/gzwrite_cdc6cb0c.o \
  $(JUCE_OBJDIR)/infback_78b1b588.o \
  $(JUCE_OBJDIR)/inffast_7f8c7fbd.o \
  $(JUCE_OBJDIR)/inflate_89c9cf87.o \
  $(JUCE_OBJDIR)/inftrees_72d1fa5e.o \
  $(JUCE_OBJDIR)/trees_a719e955.o \
  $(JUCE_OBJDIR)/uncompr_e412550a.o \
  $(JUCE_OBJDIR)/zutil_ea72ee7c.o \
  $(JUCE_OBJDIR)/BinaryData_ce4232d4.o \
  $(JUCE_OBJDIR)/include_juce_core_f26d17db.o \
  $(JUCE_OBJDIR)/include_juce_core_CompilationTime_9257742c.o \
  $(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o \
  $(JUCE_OBJDIR)/include_juce_events_fd7d695.o \
  $(JUCE_OBJDIR)/include_juce_graphics_f817e147.o \
  $(JUCE_OBJDIR)/include_juce_graphics_Harfbuzz_60c52ba2.o \
  $(JUCE_OBJDIR)/include_juce_graphics_Sheenbidi_c310974d.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_2_153c9db8.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_3_154ab539.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_4_1558ccba.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_5_1566e43b.o \
  $(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o \
  $(JUCE_OBJDIR)/include_juce_opengl_a8a032b.o \

.PHONY: clean all strip

all : $(JUCE_OUTDIR)/$(JUCE_TARGET_APP)

$(JUCE_OUTDIR)/$(JUCE_TARGET_APP) : $(OBJECTS_APP) $(JUCE_OBJDIR)/execinfo.cmd $(RESOURCES)
	@command -v $(PKG_CONFIG) >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@$(PKG_CONFIG) --print-errors freetype2 fontconfig gl libcurl
	@echo Linking "IGNMap - App"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	$(V_AT)$(CXX) -o $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(OBJECTS_APP) $(JUCE_LDFLAGS) $(shell cat $(JUCE_OBJDIR)/execinfo.cmd) $(JUCE_LDFLAGS_APP) $(RESOURCES) $(TARGET_ARCH)

$(JUCE_OBJDIR)/AffineImage_f63fb19d.o: ../../Source/AffineImage.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling AffineImage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AnnotViewer_a719b399.o: ../../Source/AnnotViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling AnnotViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AppUtil_5c6d9ca4.o: ../../Source/AppUtil.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling AppUtil.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ClassViewer_44f830b.o: ../../Source/ClassViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ClassViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DtmLayersViewer_98c8b452.o: ../../Source/DtmLayersViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling DtmLayersViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DtmShader_bfa70483.o: ../../Source/DtmShader.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling DtmShader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DtmTmsLayer_da3751b5.o: ../../Source/DtmTmsLayer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling DtmTmsLayer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ExportDtmDlg_e5d53c1f.o: ../../Source/ExportDtmDlg.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ExportDtmDlg.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ExportImageDlg_2dc44e1.o: ../../Source/ExportImageDlg.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ExportImageDlg.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ExportLasDlg_f653953e.o: ../../Source/ExportLasDlg.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ExportLasDlg.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/GeoBase_343e9923.o: ../../Source/GeoBase.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling GeoBase.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/GeoSearch_6564213a.o: ../../Source/GeoSearch.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling GeoSearch.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ImageLayersViewer_843f60d0.o: ../../Source/ImageLayersViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ImageLayersViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ImageOptionsViewer_66bba7be.o: ../../Source/ImageOptionsViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ImageOptionsViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LasLayersViewer_53432e13.o: ../../Source/LasLayersViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling LasLayersViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LasShader_1eec2304.o: ../../Source/LasShader.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling LasShader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Main_90ebc5c2.o: ../../Source/Main.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Main.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MainComponent_a6ffb4a5.o: ../../Source/MainComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MainComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MainComponentToolbarFactory_861e9a54.o: ../../Source/MainComponentToolbarFactory.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MainComponentToolbarFactory.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MapThread_564c0ce7.o: ../../Source/MapThread.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MapThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MapView_229c9e02.o: ../../Source/MapView.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MapView.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MvtLayer_36152faf.o: ../../Source/MvtLayer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MvtLayer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ObjectViewer_faa9c63a.o: ../../Source/ObjectViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ObjectViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OGL3DViewer_c264a858.o: ../../Source/OGL3DViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling OGL3DViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OsmLayer_d8c84a51.o: ../../Source/OsmLayer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling OsmLayer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PrefDlg_9f17261d.o: ../../Source/PrefDlg.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PrefDlg.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProfilViewer_58ce88f7.o: ../../Source/ProfilViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ProfilViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SelTreeViewer_bd03fdab.o: ../../Source/SelTreeViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling SelTreeViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SentinelViewer_a6f38bdf.o: ../../Source/SentinelViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling SentinelViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/StacViewer_e5b1993e.o: ../../Source/StacViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling StacViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/StereoViewer_ac9ce7b3.o: ../../Source/StereoViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling StereoViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TmsLayer_3b9cfb60.o: ../../Source/TmsLayer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling TmsLayer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VectorLayersViewer_544e20e0.o: ../../Source/VectorLayersViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VectorLayersViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WmtsLayer_f85ab9dd.o: ../../Source/WmtsLayer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling WmtsLayer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WmtsTmsViewer_a3a54f58.o: ../../Source/WmtsTmsViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling WmtsTmsViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ZoomViewer_a6d902ce.o: ../../Source/ZoomViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ZoomViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jaricom_71e13aae.o: ../../../jpeg-10/jaricom.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jaricom.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcapimin_89469a80.o: ../../../jpeg-10/jcapimin.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcapimin.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcapistd_89a00211.o: ../../../jpeg-10/jcapistd.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcapistd.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcarith_ce0c02ce.o: ../../../jpeg-10/jcarith.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcarith.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jccoefct_1cea0e46.o: ../../../jpeg-10/jccoefct.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jccoefct.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jccolor_32e215a5.o: ../../../jpeg-10/jccolor.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jccolor.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcdctmgr_23fd93eb.o: ../../../jpeg-10/jcdctmgr.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcdctmgr.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jchuff_8e9f1e95.o: ../../../jpeg-10/jchuff.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jchuff.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcinit_8ff2e458.o: ../../../jpeg-10/jcinit.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcinit.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcmainct_4e62aff2.o: ../../../jpeg-10/jcmainct.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcmainct.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcmarker_5d94e842.o: ../../../jpeg-10/jcmarker.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcmarker.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcmaster_5fc8946a.o: ../../../jpeg-10/jcmaster.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcmaster.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcomapi_a9a87b7e.o: ../../../jpeg-10/jcomapi.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcomapi.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcparam_cafd5c4f.o: ../../../jpeg-10/jcparam.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcparam.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcprepct_298f266.o: ../../../jpeg-10/jcprepct.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcprepct.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcsample_c49d2db2.o: ../../../jpeg-10/jcsample.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcsample.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jctrans_baaf1baa.o: ../../../jpeg-10/jctrans.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jctrans.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdapimin_1d8b0981.o: ../../../jpeg-10/jdapimin.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdapimin.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdapistd_1de47112.o: ../../../jpeg-10/jdapistd.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdapistd.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdarith_35ed2fad.o: ../../../jpeg-10/jdarith.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdarith.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdatadst_e3030040.o: ../../../jpeg-10/jdatadst.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdatadst.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdatasrc_e3d5ac9f.o: ../../../jpeg-10/jdatasrc.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdatasrc.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdcoefct_b12e7d47.o: ../../../jpeg-10/jdcoefct.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdcoefct.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdcolor_9ac34284.o: ../../../jpeg-10/jdcolor.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdcolor.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jddctmgr_b84202ec.o: ../../../jpeg-10/jddctmgr.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jddctmgr.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdhuff_c38559d6.o: ../../../jpeg-10/jdhuff.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdhuff.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdinput_d6aeed2b.o: ../../../jpeg-10/jdinput.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdinput.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdmainct_e2a71ef3.o: ../../../jpeg-10/jdmainct.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdmainct.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdmarker_f1d95743.o: ../../../jpeg-10/jdmarker.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdmarker.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdmaster_f40d036b.o: ../../../jpeg-10/jdmaster.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdmaster.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdmerge_9b01d619.o: ../../../jpeg-10/jdmerge.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdmerge.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdpostct_1046e65a.o: ../../../jpeg-10/jdpostct.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdpostct.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdsample_58e19cb3.o: ../../../jpeg-10/jdsample.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdsample.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdtrans_22904889.o: ../../../jpeg-10/jdtrans.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdtrans.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jerror_956092d.o: ../../../jpeg-10/jerror.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jerror.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jfdctflt_e06a89c4.o: ../../../jpeg-10/jfdctflt.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jfdctflt.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jfdctfst_e06db85d.o: ../../../jpeg-10/jfdctfst.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jfdctfst.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jfdctint_e095b905.o: ../../../jpeg-10/jfdctint.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jfdctint.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jidctflt_9d37d6c7.o: ../../../jpeg-10/jidctflt.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jidctflt.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jidctfst_9d3b0560.o: ../../../jpeg-10/jidctfst.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jidctfst.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jidctint_9d630608.o: ../../../jpeg-10/jidctint.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jidctint.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jmemmgr_a81b1028.o: ../../../jpeg-10/jmemmgr.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jmemmgr.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jmemnobs_5d66408c.o: ../../../jpeg-10/jmemnobs.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jmemnobs.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jquant1_819c5c93.o: ../../../jpeg-10/jquant1.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jquant1.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jquant2_819c6054.o: ../../../jpeg-10/jquant2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jquant2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jutils_5aa34196.o: ../../../jpeg-10/jutils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jutils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/arithmeticdecoder_9f9abec7.o: ../../../LASzip/src/arithmeticdecoder.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling arithmeticdecoder.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/arithmeticencoder_cb146b9f.o: ../../../LASzip/src/arithmeticencoder.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling arithmeticencoder.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/arithmeticmodel_63af2eac.o: ../../../LASzip/src/arithmeticmodel.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling arithmeticmodel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/integercompressor_23e37670.o: ../../../LASzip/src/integercompressor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling integercompressor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lasindex_6ec12af1.o: ../../../LASzip/src/lasindex.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lasindex.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lasinterval_53764450.o: ../../../LASzip/src/lasinterval.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lasinterval.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lasmessage_7f05ed26.o: ../../../LASzip/src/lasmessage.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lasmessage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lasquadtree_f026a8f0.o: ../../../LASzip/src/lasquadtree.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lasquadtree.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lasreaditemcompressed_v1_f51600f.o: ../../../LASzip/src/lasreaditemcompressed_v1.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lasreaditemcompressed_v1.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lasreaditemcompressed_v2_f5f7790.o: ../../../LASzip/src/lasreaditemcompressed_v2.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lasreaditemcompressed_v2.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lasreaditemcompressed_v3_f6d8f11.o: ../../../LASzip/src/lasreaditemcompressed_v3.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lasreaditemcompressed_v3.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lasreaditemcompressed_v4_f7ba692.o: ../../../LASzip/src/lasreaditemcompressed_v4.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lasreaditemcompressed_v4.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lasreadpoint_e61892d9.o: ../../../LASzip/src/lasreadpoint.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lasreadpoint.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/laswriteitemcompressed_v1_d3846dd2.o: ../../../LASzip/src/laswriteitemcompressed_v1.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling laswriteitemcompressed_v1.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/laswriteitemcompressed_v2_d3928553.o: ../../../LASzip/src/laswriteitemcompressed_v2.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling laswriteitemcompressed_v2.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/laswriteitemcompressed_v3_d3a09cd4.o: ../../../LASzip/src/laswriteitemcompressed_v3.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling laswriteitemcompressed_v3.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/laswriteitemcompressed_v4_d3aeb455.o: ../../../LASzip/src/laswriteitemcompressed_v4.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling laswriteitemcompressed_v4.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/laswritepoint_6437ed1c.o: ../../../LASzip/src/laswritepoint.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling laswritepoint.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/laszip_f8e72c40.o: ../../../LASzip/src/laszip.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling laszip.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/laszip_dll_ad018865.o: ../../../LASzip/src/laszip_dll.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling laszip_dll.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/mydefs_619244b7.o: ../../../LASzip/src/mydefs.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling mydefs.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/alpha_dec_65935d01.o: ../../../libwebp/src/dec/alpha_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling alpha_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/buffer_dec_6133586d.o: ../../../libwebp/src/dec/buffer_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling buffer_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/frame_dec_32bfcdd0.o: ../../../libwebp/src/dec/frame_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling frame_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/idec_dec_e438bda6.o: ../../../libwebp/src/dec/idec_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling idec_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/io_dec_b96d2f53.o: ../../../libwebp/src/dec/io_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling io_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/quant_dec_f6ce7526.o: ../../../libwebp/src/dec/quant_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling quant_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/tree_dec_e8eb228b.o: ../../../libwebp/src/dec/tree_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling tree_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/vp8_dec_4bebdb21.o: ../../../libwebp/src/dec/vp8_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling vp8_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/vp8l_dec_d8abe25b.o: ../../../libwebp/src/dec/vp8l_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling vp8l_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/webp_dec_4c8ed809.o: ../../../libwebp/src/dec/webp_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling webp_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/alpha_processing_d6c5115f.o: ../../../libwebp/src/dsp/alpha_processing.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling alpha_processing.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/alpha_processing_mips_dsp_r2_c42315be.o: ../../../libwebp/src/dsp/alpha_processing_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling alpha_processing_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/alpha_processing_neon_7973f640.o: ../../../libwebp/src/dsp/alpha_processing_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling alpha_processing_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/alpha_processing_sse2_82bc0d77.o: ../../../libwebp/src/dsp/alpha_processing_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling alpha_processing_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/alpha_processing_sse41_d4c68f62.o: ../../../libwebp/src/dsp/alpha_processing_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling alpha_processing_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/cost_b2bd7b58.o: ../../../libwebp/src/dsp/cost.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling cost.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/cost_mips_dsp_r2_27375237.o: ../../../libwebp/src/dsp/cost_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling cost_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/cost_mips32_fc78244f.o: ../../../libwebp/src/dsp/cost_mips32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling cost_mips32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/cost_neon_2e3bdfe7.o: ../../../libwebp/src/dsp/cost_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling cost_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/cost_sse2_3783f71e.o: ../../../libwebp/src/dsp/cost_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling cost_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/cpu_68dd4607.o: ../../../libwebp/src/dsp/cpu.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling cpu.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_68e619e1.o: ../../../libwebp/src/dsp/dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_clip_tables_e90beb16.o: ../../../libwebp/src/dsp/dec_clip_tables.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec_clip_tables.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_mips_dsp_r2_63816b40.o: ../../../libwebp/src/dsp/dec_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_mips32_1a82c9a6.o: ../../../libwebp/src/dsp/dec_mips32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec_mips32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_msa_46f45b9d.o: ../../../libwebp/src/dsp/dec_msa.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec_msa.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_neon_988deefe.o: ../../../libwebp/src/dsp/dec_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_sse2_a1d60635.o: ../../../libwebp/src/dsp/dec_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_sse41_98ebae64.o: ../../../libwebp/src/dsp/dec_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/enc_68f848b9.o: ../../../libwebp/src/dsp/enc.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling enc.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/enc_mips_dsp_r2_a24c1618.o: ../../../libwebp/src/dsp/enc_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling enc_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/enc_mips32_5e3eb7ce.o: ../../../libwebp/src/dsp/enc_mips32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling enc_mips32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/enc_msa_82235e75.o: ../../../libwebp/src/dsp/enc_msa.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling enc_msa.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/enc_neon_c33f4726.o: ../../../libwebp/src/dsp/enc_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling enc_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/enc_sse2_cc875e5d.o: ../../../libwebp/src/dsp/enc_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling enc_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/enc_sse41_c4655b3c.o: ../../../libwebp/src/dsp/enc_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling enc_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/filters_e7eda9a.o: ../../../libwebp/src/dsp/filters.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling filters.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/filters_mips_dsp_r2_6a9de79.o: ../../../libwebp/src/dsp/filters_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling filters_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/filters_msa_fc5c17d6.o: ../../../libwebp/src/dsp/filters_msa.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling filters_msa.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/filters_neon_901db9e5.o: ../../../libwebp/src/dsp/filters_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling filters_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/filters_sse2_9965d11c.o: ../../../libwebp/src/dsp/filters_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling filters_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_a22d7007.o: ../../../libwebp/src/dsp/lossless.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_enc_42453462.o: ../../../libwebp/src/dsp/lossless_enc.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_enc.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_enc_mips_dsp_r2_a50fcc41.o: ../../../libwebp/src/dsp/lossless_enc_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_enc_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_enc_mips32_ff4a0c05.o: ../../../libwebp/src/dsp/lossless_enc_mips32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_enc_mips32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_enc_msa_ce504d9e.o: ../../../libwebp/src/dsp/lossless_enc_msa.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_enc_msa.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_enc_neon_fcb03d1d.o: ../../../libwebp/src/dsp/lossless_enc_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_enc_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_enc_sse2_5f85454.o: ../../../libwebp/src/dsp/lossless_enc_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_enc_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_enc_sse41_b9132425.o: ../../../libwebp/src/dsp/lossless_enc_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_enc_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_mips_dsp_r2_55eab866.o: ../../../libwebp/src/dsp/lossless_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_msa_42b82ec3.o: ../../../libwebp/src/dsp/lossless_msa.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_msa.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_neon_15448098.o: ../../../libwebp/src/dsp/lossless_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_sse2_1e8c97cf.o: ../../../libwebp/src/dsp/lossless_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_sse41_b307500a.o: ../../../libwebp/src/dsp/lossless_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rescaler_306d0166.o: ../../../libwebp/src/dsp/rescaler.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling rescaler.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rescaler_mips_dsp_r2_36f83345.o: ../../../libwebp/src/dsp/rescaler_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling rescaler_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rescaler_mips32_17b65281.o: ../../../libwebp/src/dsp/rescaler_mips32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling rescaler_mips32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rescaler_msa_9a01f8a2.o: ../../../libwebp/src/dsp/rescaler_msa.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling rescaler_msa.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rescaler_neon_a733f299.o: ../../../libwebp/src/dsp/rescaler_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling rescaler_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rescaler_sse2_b07c09d0.o: ../../../libwebp/src/dsp/rescaler_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling rescaler_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ssim_ce3ebd4f.o: ../../../libwebp/src/dsp/ssim.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ssim.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ssim_sse2_b5205787.o: ../../../libwebp/src/dsp/ssim_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ssim_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/upsampling_db494d4d.o: ../../../libwebp/src/dsp/upsampling.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling upsampling.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/upsampling_mips_dsp_r2_12f6dcac.o: ../../../libwebp/src/dsp/upsampling_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling upsampling_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/upsampling_msa_a477f909.o: ../../../libwebp/src/dsp/upsampling_msa.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling upsampling_msa.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/upsampling_neon_eb7dff12.o: ../../../libwebp/src/dsp/upsampling_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling upsampling_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/upsampling_sse2_f4c61649.o: ../../../libwebp/src/dsp/upsampling_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling upsampling_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/upsampling_sse41_a3fda0d0.o: ../../../libwebp/src/dsp/upsampling_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling upsampling_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/yuv_6a1594b9.o: ../../../libwebp/src/dsp/yuv.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling yuv.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/yuv_mips_dsp_r2_c5d76218.o: ../../../libwebp/src/dsp/yuv_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling yuv_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/yuv_mips32_2bd3ebce.o: ../../../libwebp/src/dsp/yuv_mips32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling yuv_mips32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/yuv_neon_9c917b26.o: ../../../libwebp/src/dsp/yuv_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling yuv_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/yuv_sse2_a5d9925d.o: ../../../libwebp/src/dsp/yuv_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling yuv_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/yuv_sse41_1559a73c.o: ../../../libwebp/src/dsp/yuv_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling yuv_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/bit_reader_utils_cccde142.o: ../../../libwebp/src/utils/bit_reader_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling bit_reader_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/bit_writer_utils_dea27cf2.o: ../../../libwebp/src/utils/bit_writer_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling bit_writer_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/color_cache_utils_63a946a7.o: ../../../libwebp/src/utils/color_cache_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling color_cache_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/filters_utils_68f0d6fc.o: ../../../libwebp/src/utils/filters_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling filters_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/huffman_encode_utils_3e5fd5f5.o: ../../../libwebp/src/utils/huffman_encode_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling huffman_encode_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/huffman_utils_3932d4ae.o: ../../../libwebp/src/utils/huffman_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling huffman_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/quant_levels_dec_utils_ebd0413b.o: ../../../libwebp/src/utils/quant_levels_dec_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling quant_levels_dec_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/quant_levels_utils_aa661178.o: ../../../libwebp/src/utils/quant_levels_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling quant_levels_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/random_utils_638e3ad0.o: ../../../libwebp/src/utils/random_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling random_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rescaler_utils_9a0258e8.o: ../../../libwebp/src/utils/rescaler_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling rescaler_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/thread_utils_ce1e2d17.o: ../../../libwebp/src/utils/thread_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling thread_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/utils_eae63140.o: ../../../libwebp/src/utils/utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/bio_2615f04d.o: ../../../openjpeg/src/lib/openjp2/bio.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling bio.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/cio_262407ce.o: ../../../openjpeg/src/lib/openjp2/cio.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling cio.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dwt_26388f46.o: ../../../openjpeg/src/lib/openjp2/dwt.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dwt.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/event_accd355f.o: ../../../openjpeg/src/lib/openjp2/event.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling event.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/function_list_88c0b8aa.o: ../../../openjpeg/src/lib/openjp2/function_list.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling function_list.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ht_dec_afe07734.o: ../../../openjpeg/src/lib/openjp2/ht_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ht_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/image_70cebfe0.o: ../../../openjpeg/src/lib/openjp2/image.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling image.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/invert_1aff6fb.o: ../../../openjpeg/src/lib/openjp2/invert.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling invert.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/j2k_266d9ce8.o: ../../../openjpeg/src/lib/openjp2/j2k.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling j2k.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jp2_2688f5f1.o: ../../../openjpeg/src/lib/openjp2/jp2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jp2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/mct_26ae4b63.o: ../../../openjpeg/src/lib/openjp2/mct.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling mct.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/mqc_26b468c4.o: ../../../openjpeg/src/lib/openjp2/mqc.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling mqc.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/openjpeg_202db5d7.o: ../../../openjpeg/src/lib/openjp2/openjpeg.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling openjpeg.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/opj_clock_3c1a863d.o: ../../../openjpeg/src/lib/openjp2/opj_clock.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling opj_clock.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/opj_malloc_b805263.o: ../../../openjpeg/src/lib/openjp2/opj_malloc.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling opj_malloc.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/pi_74ddc05e.o: ../../../openjpeg/src/lib/openjp2/pi.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling pi.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sparse_array_66e19f.o: ../../../openjpeg/src/lib/openjp2/sparse_array.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sparse_array.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/t1_74debfa2.o: ../../../openjpeg/src/lib/openjp2/t1.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling t1.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/t2_74dec363.o: ../../../openjpeg/src/lib/openjp2/t2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling t2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/tcd_2710b3da.o: ../../../openjpeg/src/lib/openjp2/tcd.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling tcd.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/tgt_2712c166.o: ../../../openjpeg/src/lib/openjp2/tgt.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling tgt.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/thread_341c282f.o: ../../../openjpeg/src/lib/openjp2/thread.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling thread.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sqlite3_31f93e88.o: ../../../Sqlite/sqlite3.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sqlite3.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TinyEXIF_2b40a0f5.o: ../../../TinyEXIF/TinyEXIF.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling TinyEXIF.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XEndian_2fd684da.o: ../../../XTool/XEndian.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XEndian.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XFile_699380bb.o: ../../../XTool/XFile.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XFile.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XFrame_3a782858.o: ../../../XTool/XFrame.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XFrame.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGeoBase_f54870ad.o: ../../../XTool/XGeoBase.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGeoBase.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGeoClass_221ebd66.o: ../../../XTool/XGeoClass.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGeoClass.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGeoFDtm_98de0273.o: ../../../XTool/XGeoFDtm.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGeoFDtm.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGeoLayer_bf7d54bf.o: ../../../XTool/XGeoLayer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGeoLayer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGeoLine_a2bdd050.o: ../../../XTool/XGeoLine.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGeoLine.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGeoMap_8f9c218a.o: ../../../XTool/XGeoMap.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGeoMap.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGeoPoint_7ff39d7e.o: ../../../XTool/XGeoPoint.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGeoPoint.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGeoPoly_7d580c28.o: ../../../XTool/XGeoPoly.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGeoPoly.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGeoRaster_81934b99.o: ../../../XTool/XGeoRaster.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGeoRaster.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGeoRepres_314321ff.o: ../../../XTool/XGeoRepres.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGeoRepres.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGeoVector_36c8745f.o: ../../../XTool/XGeoVector.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGeoVector.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XInterpol_65a0c3b0.o: ../../../XTool/XInterpol.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XInterpol.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XMat3D_eaaf80bc.o: ../../../XTool/XMat3D.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XMat3D.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XParserXML_f683e5e3.o: ../../../XTool/XParserXML.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XParserXML.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XPath_defe72e4.o: ../../../XTool/XPath.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XPath.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XPolygone2D_17ca3d3c.o: ../../../XTool/XPolygone2D.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XPolygone2D.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XPt2D_5979b195.o: ../../../XTool/XPt2D.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XPt2D.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XPt3D_5b2e8a34.o: ../../../XTool/XPt3D.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XPt3D.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XXml_ba111f82.o: ../../../XTool/XXml.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XXml.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XAffinity2D_9345022c.o: ../../../XToolAlgo/XAffinity2D.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XAffinity2D.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XInternetMap_ca70d833.o: ../../../XToolAlgo/XInternetMap.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XInternetMap.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XLasFile_52021412.o: ../../../XToolAlgo/XLasFile.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XLasFile.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XOrthoModel_aa1c1383.o: ../../../XToolAlgo/XOrthoModel.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XOrthoModel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XSentinelScene_b6bda160.o: ../../../XToolAlgo/XSentinelScene.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XSentinelScene.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XStereoModel_40b428e9.o: ../../../XToolAlgo/XStereoModel.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XStereoModel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XStlWriter_8f030b96.o: ../../../XToolAlgo/XStlWriter.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XStlWriter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XTime_636b2fbf.o: ../../../XToolAlgo/XTime.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XTime.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XToneMapper_61a81e85.o: ../../../XToolAlgo/XToneMapper.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XToneMapper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XWmts_7b5ed8c7.o: ../../../XToolAlgo/XWmts.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XWmts.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XAlgoGeod_b79ba2f8.o: ../../../XToolGeod/XAlgoGeod.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XAlgoGeod.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XDomGeod_f9f0910d.o: ../../../XToolGeod/XDomGeod.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XDomGeod.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGeodGrid_bc53526b.o: ../../../XToolGeod/XGeodGrid.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGeodGrid.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGeoPref_9bf06b0c.o: ../../../XToolGeod/XGeoPref.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGeoPref.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGeoProjection_aa910638.o: ../../../XToolGeod/XGeoProjection.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGeoProjection.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XLambert_3fd1b4d5.o: ../../../XToolGeod/XLambert.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XLambert.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XNCGeod_20a3b77a.o: ../../../XToolGeod/XNCGeod.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XNCGeod.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XTransfoGeod_389308bc.o: ../../../XToolGeod/XTransfoGeod.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XTransfoGeod.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XWebMercator_c15a630b.o: ../../../XToolGeod/XWebMercator.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XWebMercator.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XBaseImage_893cd4bc.o: ../../../XToolImage/XBaseImage.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XBaseImage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XBaseTiffReader_ced0bf21.o: ../../../XToolImage/XBaseTiffReader.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XBaseTiffReader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XBigTiffReader_64bd9e6a.o: ../../../XToolImage/XBigTiffReader.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XBigTiffReader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XCogImage_c11fa1b8.o: ../../../XToolImage/XCogImage.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XCogImage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XDtmShader_ffa78bd4.o: ../../../XToolImage/XDtmShader.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XDtmShader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XFileImage_6e953671.o: ../../../XToolImage/XFileImage.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XFileImage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XJpeg2000Image_783c8387.o: ../../../XToolImage/XJpeg2000Image.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XJpeg2000Image.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XJpegCodec_26236640.o: ../../../XToolImage/XJpegCodec.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XJpegCodec.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XJpegImage_34ced505.o: ../../../XToolImage/XJpegImage.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XJpegImage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XLzwCodec_a21dc385.o: ../../../XToolImage/XLzwCodec.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XLzwCodec.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XMemRaster_88ea7764.o: ../../../XToolImage/XMemRaster.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XMemRaster.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XOpenJp2Image_9a97e091.o: ../../../XToolImage/XOpenJp2Image.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XOpenJp2Image.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XPackBitsCodec_476eb49.o: ../../../XToolImage/XPackBitsCodec.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XPackBitsCodec.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XPredictor_739e6f2e.o: ../../../XToolImage/XPredictor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XPredictor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XStbImage_5f423fd2.o: ../../../XToolImage/XStbImage.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XStbImage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XTiffReader_7e523770.o: ../../../XToolImage/XTiffReader.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XTiffReader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XTiffStripImage_304717d0.o: ../../../XToolImage/XTiffStripImage.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XTiffStripImage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XTiffTileImage_c8a2bf0a.o: ../../../XToolImage/XTiffTileImage.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XTiffTileImage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XTiffWriter_58a82b20.o: ../../../XToolImage/XTiffWriter.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XTiffWriter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XWebPCodec_3ce15dac.o: ../../../XToolImage/XWebPCodec.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XWebPCodec.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XWebPImage_4b8ccc71.o: ../../../XToolImage/XWebPImage.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XWebPImage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XZlibCodec_7db0ce3d.o: ../../../XToolImage/XZlibCodec.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XZlibCodec.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XAnnotation_f349c011.o: ../../../XToolVector/XAnnotation.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XAnnotation.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XCsv_cb1cb8ae.o: ../../../XToolVector/XCsv.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XCsv.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XDBase_47d5afbd.o: ../../../XToolVector/XDBase.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XDBase.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XDxf_7a9b6fa.o: ../../../XToolVector/XDxf.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XDxf.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGeoJson_9a8e4381.o: ../../../XToolVector/XGeoJson.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGeoJson.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XGpkgMap_9b3203bf.o: ../../../XToolVector/XGpkgMap.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XGpkgMap.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XMifMid_16cf4f80.o: ../../../XToolVector/XMifMid.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XMifMid.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XShapefile_324c5d85.o: ../../../XToolVector/XShapefile.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XShapefile.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XShapefileConverter_24c58b65.o: ../../../XToolVector/XShapefileConverter.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XShapefileConverter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XShapefileRecord_3816b976.o: ../../../XToolVector/XShapefileRecord.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XShapefileRecord.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XShapefileVector_d626568.o: ../../../XToolVector/XShapefileVector.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XShapefileVector.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XTAChantier_c1ce0d83.o: ../../../XToolVector/XTAChantier.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XTAChantier.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XWKBGeom_ed139392.o: ../../../XToolVector/XWKBGeom.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XWKBGeom.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/adler32_b1f51f5.o: ../../../zlib/adler32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling adler32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/compress_50f36cec.o: ../../../zlib/compress.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling compress.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/crc32_239c1c73.o: ../../../zlib/crc32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling crc32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/deflate_fd8910ab.o: ../../../zlib/deflate.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling deflate.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gzclose_a1e12ee5.o: ../../../zlib/gzclose.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling gzclose.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gzlib_573e232.o: ../../../zlib/gzlib.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling gzlib.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gzread_b30d6a13.o: ../../../zlib/gzread.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling gzread.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gzwrite_cdc6cb0c.o: ../../../zlib/gzwrite.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling gzwrite.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/infback_78b1b588.o: ../../../zlib/infback.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling infback.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/inffast_7f8c7fbd.o: ../../../zlib/inffast.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling inffast.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/inflate_89c9cf87.o: ../../../zlib/inflate.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling inflate.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/inftrees_72d1fa5e.o: ../../../zlib/inftrees.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling inftrees.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/trees_a719e955.o: ../../../zlib/trees.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling trees.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/uncompr_e412550a.o: ../../../zlib/uncompr.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling uncompr.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/zutil_ea72ee7c.o: ../../../zlib/zutil.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling zutil.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData_ce4232d4.o: ../../JuceLibraryCode/BinaryData.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling BinaryData.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_core_f26d17db.o: ../../JuceLibraryCode/include_juce_core.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_core.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_core_CompilationTime_9257742c.o: ../../JuceLibraryCode/include_juce_core_CompilationTime.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_core_CompilationTime.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o: ../../JuceLibraryCode/include_juce_data_structures.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_data_structures.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_events_fd7d695.o: ../../JuceLibraryCode/include_juce_events.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_events.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_f817e147.o: ../../JuceLibraryCode/include_juce_graphics.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_Harfbuzz_60c52ba2.o: ../../JuceLibraryCode/include_juce_graphics_Harfbuzz.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_Harfbuzz.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_Sheenbidi_c310974d.o: ../../JuceLibraryCode/include_juce_graphics_Sheenbidi.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_Sheenbidi.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o: ../../JuceLibraryCode/include_juce_gui_basics.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_2_153c9db8.o: ../../JuceLibraryCode/include_juce_gui_basics_2.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_basics_2.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_3_154ab539.o: ../../JuceLibraryCode/include_juce_gui_basics_3.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_basics_3.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_4_1558ccba.o: ../../JuceLibraryCode/include_juce_gui_basics_4.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_basics_4.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_5_1566e43b.o: ../../JuceLibraryCode/include_juce_gui_basics_5.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_basics_5.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o: ../../JuceLibraryCode/include_juce_gui_extra.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_extra.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_opengl_a8a032b.o: ../../JuceLibraryCode/include_juce_opengl.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_opengl.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/execinfo.cmd:
	-$(V_AT)mkdir -p $(@D)
	-@if [ -z "$(V_AT)" ]; then echo "Checking if we need to link libexecinfo"; fi
	$(V_AT)printf "int main() { return 0; }" | $(CXX) -x c++ -o $(@D)/execinfo.x -lexecinfo - >/dev/null 2>&1 && printf -- "-lexecinfo" > "$@" || touch "$@"

$(JUCE_OBJDIR)/cxxfs.cmd:
	-$(V_AT)mkdir -p $(@D)
	-@if [ -z "$(V_AT)" ]; then echo "Checking if we need to link stdc++fs"; fi
	$(V_AT)printf "int main() { return 0; }" | $(CXX) -x c++ -o $(@D)/cxxfs.x -lstdc++fs - >/dev/null 2>&1 && printf -- "-lstdc++fs" > "$@" || touch "$@"

clean:
	@echo Cleaning IGNMap
	$(V_AT)$(CLEANCMD)

strip:
	@echo Stripping IGNMap
	-$(V_AT)$(STRIP) --strip-unneeded $(JUCE_OUTDIR)/$(JUCE_TARGET_APP)

-include $(OBJECTS_APP:%.o=%.d)
