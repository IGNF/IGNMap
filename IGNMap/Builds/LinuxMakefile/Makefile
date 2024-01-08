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

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DDEBUG=1" "-D_DEBUG=1" "-DJUCE_DISPLAY_SPLASH_SCREEN=1" "-DJUCE_USE_DARK_SPLASH_SCREEN=1" "-DJUCE_PROJUCER_VERSION=0x70009" "-DJUCE_MODULE_AVAILABLE_juce_core=1" "-DJUCE_MODULE_AVAILABLE_juce_data_structures=1" "-DJUCE_MODULE_AVAILABLE_juce_events=1" "-DJUCE_MODULE_AVAILABLE_juce_graphics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_extra=1" "-DJUCE_MODULE_AVAILABLE_juce_opengl=1" "-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1" "-DJUCE_STRICT_REFCOUNTEDPOINTER=1" "-DJUCE_STANDALONE_APPLICATION=1" "-DJUCE_MODAL_LOOPS_PERMITTED=1" "-DOPJ_STATIC" "-DLASZIPDLL_EXPORTS" "-DJUCER_LINUX_MAKE_6D53C8B4=1" "-DJUCE_APP_VERSION=1.0.0" "-DJUCE_APP_VERSION_HEX=0x10000" $(shell $(PKG_CONFIG) --cflags freetype2 gl libcurl webkit2gtk-4.0 gtk+-x11-3.0) -pthread -I../../JuceLibraryCode -I../../../JUCE/modules -I./../../../libwebp-1.2.4 $(CPPFLAGS)
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0" "-DJucePlugin_Build_LV2=0"
  JUCE_TARGET_APP := IGNMap

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0 $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++17 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) $(shell $(PKG_CONFIG) --libs freetype2 gl libcurl) -fvisibility=hidden -lrt -ldl -lpthread $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

ifeq ($(CONFIG),Release)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Release
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := 
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DNDEBUG=1" "-DJUCE_DISPLAY_SPLASH_SCREEN=1" "-DJUCE_USE_DARK_SPLASH_SCREEN=1" "-DJUCE_PROJUCER_VERSION=0x70009" "-DJUCE_MODULE_AVAILABLE_juce_core=1" "-DJUCE_MODULE_AVAILABLE_juce_data_structures=1" "-DJUCE_MODULE_AVAILABLE_juce_events=1" "-DJUCE_MODULE_AVAILABLE_juce_graphics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_extra=1" "-DJUCE_MODULE_AVAILABLE_juce_opengl=1" "-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1" "-DJUCE_STRICT_REFCOUNTEDPOINTER=1" "-DJUCE_STANDALONE_APPLICATION=1" "-DJUCE_MODAL_LOOPS_PERMITTED=1" "-DOPJ_STATIC" "-DLASZIPDLL_EXPORTS" "-DJUCER_LINUX_MAKE_6D53C8B4=1" "-DJUCE_APP_VERSION=1.0.0" "-DJUCE_APP_VERSION_HEX=0x10000" $(shell $(PKG_CONFIG) --cflags freetype2 gl libcurl webkit2gtk-4.0 gtk+-x11-3.0) -pthread -I../../JuceLibraryCode -I../../../JUCE/modules -I./../../../libwebp-1.2.4 $(CPPFLAGS)
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0" "-DJucePlugin_Build_LV2=0"
  JUCE_TARGET_APP := IGNMap

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -O3 $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++17 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) $(shell $(PKG_CONFIG) --libs freetype2 gl libcurl) -fvisibility=hidden -lrt -ldl -lpthread $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

OBJECTS_APP := \
  $(JUCE_OBJDIR)/jaricom_ede2b4da.o \
  $(JUCE_OBJDIR)/jcapimin_8d7465d4.o \
  $(JUCE_OBJDIR)/jcapistd_8dcdcd65.o \
  $(JUCE_OBJDIR)/jcarith_4a0d7cfa.o \
  $(JUCE_OBJDIR)/jccoefct_2117d99a.o \
  $(JUCE_OBJDIR)/jccolor_aee38fd1.o \
  $(JUCE_OBJDIR)/jcdctmgr_282b5f3f.o \
  $(JUCE_OBJDIR)/jchuff_d4afaee9.o \
  $(JUCE_OBJDIR)/jcinit_d60374ac.o \
  $(JUCE_OBJDIR)/jcmainct_52907b46.o \
  $(JUCE_OBJDIR)/jcmarker_61c2b396.o \
  $(JUCE_OBJDIR)/jcmaster_63f65fbe.o \
  $(JUCE_OBJDIR)/jcomapi_25a9f5aa.o \
  $(JUCE_OBJDIR)/jcparam_46fed67b.o \
  $(JUCE_OBJDIR)/jcprepct_6c6bdba.o \
  $(JUCE_OBJDIR)/jcsample_c8caf906.o \
  $(JUCE_OBJDIR)/jctrans_36b095d6.o \
  $(JUCE_OBJDIR)/jdapimin_21b8d4d5.o \
  $(JUCE_OBJDIR)/jdapistd_22123c66.o \
  $(JUCE_OBJDIR)/jdarith_b1eea9d9.o \
  $(JUCE_OBJDIR)/jdatadst_e730cb94.o \
  $(JUCE_OBJDIR)/jdatasrc_e80377f3.o \
  $(JUCE_OBJDIR)/jdcoefct_b55c489b.o \
  $(JUCE_OBJDIR)/jdcolor_16c4bcb0.o \
  $(JUCE_OBJDIR)/jddctmgr_bc6fce40.o \
  $(JUCE_OBJDIR)/jdhuff_995ea2a.o \
  $(JUCE_OBJDIR)/jdinput_52b06757.o \
  $(JUCE_OBJDIR)/jdmainct_e6d4ea47.o \
  $(JUCE_OBJDIR)/jdmarker_f6072297.o \
  $(JUCE_OBJDIR)/jdmaster_f83acebf.o \
  $(JUCE_OBJDIR)/jdmerge_17035045.o \
  $(JUCE_OBJDIR)/jdpostct_1474b1ae.o \
  $(JUCE_OBJDIR)/jdsample_5d0f6807.o \
  $(JUCE_OBJDIR)/jdtrans_9e91c2b5.o \
  $(JUCE_OBJDIR)/jerror_4f669981.o \
  $(JUCE_OBJDIR)/jfdctflt_e4985518.o \
  $(JUCE_OBJDIR)/jfdctfst_e49b83b1.o \
  $(JUCE_OBJDIR)/jfdctint_e4c38459.o \
  $(JUCE_OBJDIR)/jidctflt_a165a21b.o \
  $(JUCE_OBJDIR)/jidctfst_a168d0b4.o \
  $(JUCE_OBJDIR)/jidctint_a190d15c.o \
  $(JUCE_OBJDIR)/jmemmgr_241c8a54.o \
  $(JUCE_OBJDIR)/jmemnobs_61940be0.o \
  $(JUCE_OBJDIR)/jquant1_fd9dd6bf.o \
  $(JUCE_OBJDIR)/jquant2_fd9dda80.o \
  $(JUCE_OBJDIR)/jutils_a0b3d1ea.o \
  $(JUCE_OBJDIR)/arithmeticdecoder_9f9abec7.o \
  $(JUCE_OBJDIR)/arithmeticencoder_cb146b9f.o \
  $(JUCE_OBJDIR)/arithmeticmodel_63af2eac.o \
  $(JUCE_OBJDIR)/integercompressor_23e37670.o \
  $(JUCE_OBJDIR)/lasindex_6ec12af1.o \
  $(JUCE_OBJDIR)/lasinterval_53764450.o \
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
  $(JUCE_OBJDIR)/alpha_dec_84673aaf.o \
  $(JUCE_OBJDIR)/buffer_dec_1cdb307f.o \
  $(JUCE_OBJDIR)/frame_dec_5193ab7e.o \
  $(JUCE_OBJDIR)/idec_dec_f5bb7238.o \
  $(JUCE_OBJDIR)/io_dec_7ad7e065.o \
  $(JUCE_OBJDIR)/quant_dec_15a252d4.o \
  $(JUCE_OBJDIR)/tree_dec_fa6dd71d.o \
  $(JUCE_OBJDIR)/vp8_dec_b7d74c4f.o \
  $(JUCE_OBJDIR)/vp8l_dec_ea2e96ed.o \
  $(JUCE_OBJDIR)/webp_dec_5e118c9b.o \
  $(JUCE_OBJDIR)/alpha_processing_f35b13f1.o \
  $(JUCE_OBJDIR)/alpha_processing_mips_dsp_r2_bc7fcd50.o \
  $(JUCE_OBJDIR)/alpha_processing_neon_2d72beee.o \
  $(JUCE_OBJDIR)/alpha_processing_sse2_36bad625.o \
  $(JUCE_OBJDIR)/alpha_processing_sse41_a0a0dc74.o \
  $(JUCE_OBJDIR)/cost_dee548ea.o \
  $(JUCE_OBJDIR)/cost_mips_dsp_r2_43cd54c9.o \
  $(JUCE_OBJDIR)/cost_mips32_b5cb4e7d.o \
  $(JUCE_OBJDIR)/cost_neon_4d0fbd95.o \
  $(JUCE_OBJDIR)/cost_sse2_5657d4cc.o \
  $(JUCE_OBJDIR)/cpu_b49c7e35.o \
  $(JUCE_OBJDIR)/dec_b4a5520f.o \
  $(JUCE_OBJDIR)/dec_clip_tables_13424e44.o \
  $(JUCE_OBJDIR)/dec_mips_dsp_r2_8db7ce6e.o \
  $(JUCE_OBJDIR)/dec_mips32_d62aa1b8.o \
  $(JUCE_OBJDIR)/dec_msa_b2dfcccb.o \
  $(JUCE_OBJDIR)/dec_neon_aa10a390.o \
  $(JUCE_OBJDIR)/dec_sse2_b358bac7.o \
  $(JUCE_OBJDIR)/dec_sse41_b7bf8c12.o \
  $(JUCE_OBJDIR)/enc_b4b780e7.o \
  $(JUCE_OBJDIR)/enc_mips_dsp_r2_cc827946.o \
  $(JUCE_OBJDIR)/enc_mips32_19e68fe0.o \
  $(JUCE_OBJDIR)/enc_msa_ee0ecfa3.o \
  $(JUCE_OBJDIR)/enc_neon_d4c1fbb8.o \
  $(JUCE_OBJDIR)/enc_sse2_de0a12ef.o \
  $(JUCE_OBJDIR)/enc_sse41_e33938ea.o \
  $(JUCE_OBJDIR)/filters_7a6a4bc8.o \
  $(JUCE_OBJDIR)/filters_mips_dsp_r2_9b7efaa7.o \
  $(JUCE_OBJDIR)/filters_msa_b5af4204.o \
  $(JUCE_OBJDIR)/filters_neon_12fd577.o \
  $(JUCE_OBJDIR)/filters_sse2_a77ecae.o \
  $(JUCE_OBJDIR)/lossless_b3b02499.o \
  $(JUCE_OBJDIR)/lossless_enc_b3574ff4.o \
  $(JUCE_OBJDIR)/lossless_enc_mips_dsp_r2_e38b1cd3.o \
  $(JUCE_OBJDIR)/lossless_enc_mips32_941f2833.o \
  $(JUCE_OBJDIR)/lossless_enc_msa_eae65030.o \
  $(JUCE_OBJDIR)/lossless_enc_neon_72da8ccb.o \
  $(JUCE_OBJDIR)/lossless_enc_sse2_7c22a402.o \
  $(JUCE_OBJDIR)/lossless_enc_sse41_832ca37.o \
  $(JUCE_OBJDIR)/lossless_mips_dsp_r2_5bb921f8.o \
  $(JUCE_OBJDIR)/lossless_msa_b3ca4a55.o \
  $(JUCE_OBJDIR)/lossless_neon_c675d746.o \
  $(JUCE_OBJDIR)/lossless_sse2_cfbdee7d.o \
  $(JUCE_OBJDIR)/lossless_sse41_2800cf1c.o \
  $(JUCE_OBJDIR)/rescaler_41efb5f8.o \
  $(JUCE_OBJDIR)/rescaler_mips_dsp_r2_3cc69cd7.o \
  $(JUCE_OBJDIR)/rescaler_mips32_41ecb5af.o \
  $(JUCE_OBJDIR)/rescaler_msa_b141434.o \
  $(JUCE_OBJDIR)/rescaler_neon_58654947.o \
  $(JUCE_OBJDIR)/rescaler_sse2_61ad607e.o \
  $(JUCE_OBJDIR)/ssim_fa668ae1.o \
  $(JUCE_OBJDIR)/ssim_sse2_d3f43535.o \
  $(JUCE_OBJDIR)/upsampling_96f1255f.o \
  $(JUCE_OBJDIR)/upsampling_mips_dsp_r2_ded129be.o \
  $(JUCE_OBJDIR)/upsampling_msa_1971781b.o \
  $(JUCE_OBJDIR)/upsampling_neon_15b46240.o \
  $(JUCE_OBJDIR)/upsampling_sse2_1efc7977.o \
  $(JUCE_OBJDIR)/upsampling_sse41_c093a362.o \
  $(JUCE_OBJDIR)/yuv_b5d4cce7.o \
  $(JUCE_OBJDIR)/yuv_mips_dsp_r2_f00dc546.o \
  $(JUCE_OBJDIR)/yuv_mips32_e77bc3e0.o \
  $(JUCE_OBJDIR)/yuv_neon_ae142fb8.o \
  $(JUCE_OBJDIR)/yuv_sse2_b75c46ef.o \
  $(JUCE_OBJDIR)/yuv_sse41_342d84ea.o \
  $(JUCE_OBJDIR)/bit_reader_utils_1bed8754.o \
  $(JUCE_OBJDIR)/bit_writer_utils_2dc22304.o \
  $(JUCE_OBJDIR)/color_cache_utils_f87e62d5.o \
  $(JUCE_OBJDIR)/filters_utils_93273a2a.o \
  $(JUCE_OBJDIR)/huffman_encode_utils_a3a2307.o \
  $(JUCE_OBJDIR)/huffman_utils_636937dc.o \
  $(JUCE_OBJDIR)/quant_levels_dec_utils_2a4b91cd.o \
  $(JUCE_OBJDIR)/quant_levels_utils_b0347b0a.o \
  $(JUCE_OBJDIR)/random_utils_d887b9e2.o \
  $(JUCE_OBJDIR)/rescaler_utils_b6985b7a.o \
  $(JUCE_OBJDIR)/thread_utils_4317ac29.o \
  $(JUCE_OBJDIR)/utils_56d1a26e.o \
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
  $(JUCE_OBJDIR)/DtmLayersViewer_98c8b452.o \
  $(JUCE_OBJDIR)/DtmShader_bfa70483.o \
  $(JUCE_OBJDIR)/GeoBase_343e9923.o \
  $(JUCE_OBJDIR)/ImageLayersViewer_843f60d0.o \
  $(JUCE_OBJDIR)/ImageOptionsViewer_66bba7be.o \
  $(JUCE_OBJDIR)/LasLayersViewer_53432e13.o \
  $(JUCE_OBJDIR)/LasShader_1eec2304.o \
  $(JUCE_OBJDIR)/Main_90ebc5c2.o \
  $(JUCE_OBJDIR)/MainComponent_a6ffb4a5.o \
  $(JUCE_OBJDIR)/MainComponentToolbarFactory_861e9a54.o \
  $(JUCE_OBJDIR)/MapThread_564c0ce7.o \
  $(JUCE_OBJDIR)/MapView_229c9e02.o \
  $(JUCE_OBJDIR)/OGL3DViewer_c264a858.o \
  $(JUCE_OBJDIR)/OsmLayer_d8c84a51.o \
  $(JUCE_OBJDIR)/SelTreeViewer_bd03fdab.o \
  $(JUCE_OBJDIR)/VectorLayersViewer_544e20e0.o \
  $(JUCE_OBJDIR)/WmtsLayer_f85ab9dd.o \
  $(JUCE_OBJDIR)/sqlite3_31f93e88.o \
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
  $(JUCE_OBJDIR)/XParserXML_f683e5e3.o \
  $(JUCE_OBJDIR)/XPath_defe72e4.o \
  $(JUCE_OBJDIR)/XPolygone2D_17ca3d3c.o \
  $(JUCE_OBJDIR)/XPt2D_5979b195.o \
  $(JUCE_OBJDIR)/XPt3D_5b2e8a34.o \
  $(JUCE_OBJDIR)/XLasFile_52021412.o \
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
  $(JUCE_OBJDIR)/XLzwCodec_a21dc385.o \
  $(JUCE_OBJDIR)/XOpenJp2Image_9a97e091.o \
  $(JUCE_OBJDIR)/XPackBitsCodec_476eb49.o \
  $(JUCE_OBJDIR)/XPredictor_739e6f2e.o \
  $(JUCE_OBJDIR)/XTiffReader_7e523770.o \
  $(JUCE_OBJDIR)/XTiffStripImage_304717d0.o \
  $(JUCE_OBJDIR)/XTiffTileImage_c8a2bf0a.o \
  $(JUCE_OBJDIR)/XTiffWriter_58a82b20.o \
  $(JUCE_OBJDIR)/XWebPCodec_3ce15dac.o \
  $(JUCE_OBJDIR)/XWebPImage_4b8ccc71.o \
  $(JUCE_OBJDIR)/XZlibCodec_7db0ce3d.o \
  $(JUCE_OBJDIR)/XDBase_47d5afbd.o \
  $(JUCE_OBJDIR)/XGpkgMap_9b3203bf.o \
  $(JUCE_OBJDIR)/XMifMid_16cf4f80.o \
  $(JUCE_OBJDIR)/XShapefile_324c5d85.o \
  $(JUCE_OBJDIR)/XShapefileConverter_24c58b65.o \
  $(JUCE_OBJDIR)/XShapefileRecord_3816b976.o \
  $(JUCE_OBJDIR)/XShapefileVector_d626568.o \
  $(JUCE_OBJDIR)/XWKBGeom_ed139392.o \
  $(JUCE_OBJDIR)/adler32_5cf31f67.o \
  $(JUCE_OBJDIR)/compress_39994dba.o \
  $(JUCE_OBJDIR)/crc32_7e889e65.o \
  $(JUCE_OBJDIR)/deflate_4f5cde1d.o \
  $(JUCE_OBJDIR)/gzclose_f3b4fc57.o \
  $(JUCE_OBJDIR)/gzlib_60606424.o \
  $(JUCE_OBJDIR)/gzread_b5b12661.o \
  $(JUCE_OBJDIR)/gzwrite_1f9a987e.o \
  $(JUCE_OBJDIR)/infback_ca8582fa.o \
  $(JUCE_OBJDIR)/inffast_d1604d2f.o \
  $(JUCE_OBJDIR)/inflate_db9d9cf9.o \
  $(JUCE_OBJDIR)/inftrees_5b77db2c.o \
  $(JUCE_OBJDIR)/trees_2066b47.o \
  $(JUCE_OBJDIR)/uncompr_35e6227c.o \
  $(JUCE_OBJDIR)/zutil_455f706e.o \
  $(JUCE_OBJDIR)/BinaryData_ce4232d4.o \
  $(JUCE_OBJDIR)/include_juce_core_f26d17db.o \
  $(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o \
  $(JUCE_OBJDIR)/include_juce_events_fd7d695.o \
  $(JUCE_OBJDIR)/include_juce_graphics_f817e147.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o \
  $(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o \
  $(JUCE_OBJDIR)/include_juce_opengl_a8a032b.o \

.PHONY: clean all strip

all : $(JUCE_OUTDIR)/$(JUCE_TARGET_APP)

$(JUCE_OUTDIR)/$(JUCE_TARGET_APP) : $(OBJECTS_APP) $(JUCE_OBJDIR)/execinfo.cmd $(RESOURCES)
	@command -v $(PKG_CONFIG) >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@$(PKG_CONFIG) --print-errors freetype2 gl libcurl
	@echo Linking "IGNMap - App"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	$(V_AT)$(CXX) -o $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(OBJECTS_APP) $(JUCE_LDFLAGS) $(shell cat $(JUCE_OBJDIR)/execinfo.cmd) $(JUCE_LDFLAGS_APP) $(RESOURCES) $(TARGET_ARCH)

$(JUCE_OBJDIR)/jaricom_ede2b4da.o: ../../../jpeg-9d/jaricom.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jaricom.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcapimin_8d7465d4.o: ../../../jpeg-9d/jcapimin.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcapimin.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcapistd_8dcdcd65.o: ../../../jpeg-9d/jcapistd.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcapistd.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcarith_4a0d7cfa.o: ../../../jpeg-9d/jcarith.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcarith.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jccoefct_2117d99a.o: ../../../jpeg-9d/jccoefct.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jccoefct.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jccolor_aee38fd1.o: ../../../jpeg-9d/jccolor.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jccolor.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcdctmgr_282b5f3f.o: ../../../jpeg-9d/jcdctmgr.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcdctmgr.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jchuff_d4afaee9.o: ../../../jpeg-9d/jchuff.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jchuff.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcinit_d60374ac.o: ../../../jpeg-9d/jcinit.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcinit.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcmainct_52907b46.o: ../../../jpeg-9d/jcmainct.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcmainct.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcmarker_61c2b396.o: ../../../jpeg-9d/jcmarker.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcmarker.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcmaster_63f65fbe.o: ../../../jpeg-9d/jcmaster.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcmaster.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcomapi_25a9f5aa.o: ../../../jpeg-9d/jcomapi.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcomapi.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcparam_46fed67b.o: ../../../jpeg-9d/jcparam.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcparam.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcprepct_6c6bdba.o: ../../../jpeg-9d/jcprepct.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcprepct.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcsample_c8caf906.o: ../../../jpeg-9d/jcsample.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jcsample.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jctrans_36b095d6.o: ../../../jpeg-9d/jctrans.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jctrans.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdapimin_21b8d4d5.o: ../../../jpeg-9d/jdapimin.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdapimin.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdapistd_22123c66.o: ../../../jpeg-9d/jdapistd.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdapistd.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdarith_b1eea9d9.o: ../../../jpeg-9d/jdarith.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdarith.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdatadst_e730cb94.o: ../../../jpeg-9d/jdatadst.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdatadst.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdatasrc_e80377f3.o: ../../../jpeg-9d/jdatasrc.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdatasrc.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdcoefct_b55c489b.o: ../../../jpeg-9d/jdcoefct.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdcoefct.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdcolor_16c4bcb0.o: ../../../jpeg-9d/jdcolor.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdcolor.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jddctmgr_bc6fce40.o: ../../../jpeg-9d/jddctmgr.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jddctmgr.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdhuff_995ea2a.o: ../../../jpeg-9d/jdhuff.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdhuff.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdinput_52b06757.o: ../../../jpeg-9d/jdinput.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdinput.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdmainct_e6d4ea47.o: ../../../jpeg-9d/jdmainct.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdmainct.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdmarker_f6072297.o: ../../../jpeg-9d/jdmarker.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdmarker.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdmaster_f83acebf.o: ../../../jpeg-9d/jdmaster.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdmaster.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdmerge_17035045.o: ../../../jpeg-9d/jdmerge.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdmerge.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdpostct_1474b1ae.o: ../../../jpeg-9d/jdpostct.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdpostct.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdsample_5d0f6807.o: ../../../jpeg-9d/jdsample.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdsample.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jdtrans_9e91c2b5.o: ../../../jpeg-9d/jdtrans.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jdtrans.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jerror_4f669981.o: ../../../jpeg-9d/jerror.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jerror.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jfdctflt_e4985518.o: ../../../jpeg-9d/jfdctflt.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jfdctflt.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jfdctfst_e49b83b1.o: ../../../jpeg-9d/jfdctfst.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jfdctfst.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jfdctint_e4c38459.o: ../../../jpeg-9d/jfdctint.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jfdctint.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jidctflt_a165a21b.o: ../../../jpeg-9d/jidctflt.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jidctflt.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jidctfst_a168d0b4.o: ../../../jpeg-9d/jidctfst.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jidctfst.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jidctint_a190d15c.o: ../../../jpeg-9d/jidctint.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jidctint.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jmemmgr_241c8a54.o: ../../../jpeg-9d/jmemmgr.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jmemmgr.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jmemnobs_61940be0.o: ../../../jpeg-9d/jmemnobs.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jmemnobs.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jquant1_fd9dd6bf.o: ../../../jpeg-9d/jquant1.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jquant1.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jquant2_fd9dda80.o: ../../../jpeg-9d/jquant2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling jquant2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jutils_a0b3d1ea.o: ../../../jpeg-9d/jutils.c
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

$(JUCE_OBJDIR)/alpha_dec_84673aaf.o: ../../../libwebp-1.2.4/src/dec/alpha_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling alpha_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/buffer_dec_1cdb307f.o: ../../../libwebp-1.2.4/src/dec/buffer_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling buffer_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/frame_dec_5193ab7e.o: ../../../libwebp-1.2.4/src/dec/frame_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling frame_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/idec_dec_f5bb7238.o: ../../../libwebp-1.2.4/src/dec/idec_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling idec_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/io_dec_7ad7e065.o: ../../../libwebp-1.2.4/src/dec/io_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling io_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/quant_dec_15a252d4.o: ../../../libwebp-1.2.4/src/dec/quant_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling quant_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/tree_dec_fa6dd71d.o: ../../../libwebp-1.2.4/src/dec/tree_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling tree_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/vp8_dec_b7d74c4f.o: ../../../libwebp-1.2.4/src/dec/vp8_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling vp8_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/vp8l_dec_ea2e96ed.o: ../../../libwebp-1.2.4/src/dec/vp8l_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling vp8l_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/webp_dec_5e118c9b.o: ../../../libwebp-1.2.4/src/dec/webp_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling webp_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/alpha_processing_f35b13f1.o: ../../../libwebp-1.2.4/src/dsp/alpha_processing.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling alpha_processing.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/alpha_processing_mips_dsp_r2_bc7fcd50.o: ../../../libwebp-1.2.4/src/dsp/alpha_processing_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling alpha_processing_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/alpha_processing_neon_2d72beee.o: ../../../libwebp-1.2.4/src/dsp/alpha_processing_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling alpha_processing_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/alpha_processing_sse2_36bad625.o: ../../../libwebp-1.2.4/src/dsp/alpha_processing_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling alpha_processing_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/alpha_processing_sse41_a0a0dc74.o: ../../../libwebp-1.2.4/src/dsp/alpha_processing_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling alpha_processing_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/cost_dee548ea.o: ../../../libwebp-1.2.4/src/dsp/cost.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling cost.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/cost_mips_dsp_r2_43cd54c9.o: ../../../libwebp-1.2.4/src/dsp/cost_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling cost_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/cost_mips32_b5cb4e7d.o: ../../../libwebp-1.2.4/src/dsp/cost_mips32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling cost_mips32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/cost_neon_4d0fbd95.o: ../../../libwebp-1.2.4/src/dsp/cost_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling cost_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/cost_sse2_5657d4cc.o: ../../../libwebp-1.2.4/src/dsp/cost_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling cost_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/cpu_b49c7e35.o: ../../../libwebp-1.2.4/src/dsp/cpu.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling cpu.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_b4a5520f.o: ../../../libwebp-1.2.4/src/dsp/dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_clip_tables_13424e44.o: ../../../libwebp-1.2.4/src/dsp/dec_clip_tables.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec_clip_tables.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_mips_dsp_r2_8db7ce6e.o: ../../../libwebp-1.2.4/src/dsp/dec_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_mips32_d62aa1b8.o: ../../../libwebp-1.2.4/src/dsp/dec_mips32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec_mips32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_msa_b2dfcccb.o: ../../../libwebp-1.2.4/src/dsp/dec_msa.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec_msa.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_neon_aa10a390.o: ../../../libwebp-1.2.4/src/dsp/dec_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_sse2_b358bac7.o: ../../../libwebp-1.2.4/src/dsp/dec_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dec_sse41_b7bf8c12.o: ../../../libwebp-1.2.4/src/dsp/dec_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dec_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/enc_b4b780e7.o: ../../../libwebp-1.2.4/src/dsp/enc.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling enc.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/enc_mips_dsp_r2_cc827946.o: ../../../libwebp-1.2.4/src/dsp/enc_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling enc_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/enc_mips32_19e68fe0.o: ../../../libwebp-1.2.4/src/dsp/enc_mips32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling enc_mips32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/enc_msa_ee0ecfa3.o: ../../../libwebp-1.2.4/src/dsp/enc_msa.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling enc_msa.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/enc_neon_d4c1fbb8.o: ../../../libwebp-1.2.4/src/dsp/enc_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling enc_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/enc_sse2_de0a12ef.o: ../../../libwebp-1.2.4/src/dsp/enc_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling enc_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/enc_sse41_e33938ea.o: ../../../libwebp-1.2.4/src/dsp/enc_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling enc_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/filters_7a6a4bc8.o: ../../../libwebp-1.2.4/src/dsp/filters.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling filters.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/filters_mips_dsp_r2_9b7efaa7.o: ../../../libwebp-1.2.4/src/dsp/filters_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling filters_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/filters_msa_b5af4204.o: ../../../libwebp-1.2.4/src/dsp/filters_msa.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling filters_msa.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/filters_neon_12fd577.o: ../../../libwebp-1.2.4/src/dsp/filters_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling filters_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/filters_sse2_a77ecae.o: ../../../libwebp-1.2.4/src/dsp/filters_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling filters_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_b3b02499.o: ../../../libwebp-1.2.4/src/dsp/lossless.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_enc_b3574ff4.o: ../../../libwebp-1.2.4/src/dsp/lossless_enc.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_enc.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_enc_mips_dsp_r2_e38b1cd3.o: ../../../libwebp-1.2.4/src/dsp/lossless_enc_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_enc_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_enc_mips32_941f2833.o: ../../../libwebp-1.2.4/src/dsp/lossless_enc_mips32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_enc_mips32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_enc_msa_eae65030.o: ../../../libwebp-1.2.4/src/dsp/lossless_enc_msa.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_enc_msa.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_enc_neon_72da8ccb.o: ../../../libwebp-1.2.4/src/dsp/lossless_enc_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_enc_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_enc_sse2_7c22a402.o: ../../../libwebp-1.2.4/src/dsp/lossless_enc_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_enc_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_enc_sse41_832ca37.o: ../../../libwebp-1.2.4/src/dsp/lossless_enc_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_enc_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_mips_dsp_r2_5bb921f8.o: ../../../libwebp-1.2.4/src/dsp/lossless_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_msa_b3ca4a55.o: ../../../libwebp-1.2.4/src/dsp/lossless_msa.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_msa.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_neon_c675d746.o: ../../../libwebp-1.2.4/src/dsp/lossless_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_sse2_cfbdee7d.o: ../../../libwebp-1.2.4/src/dsp/lossless_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lossless_sse41_2800cf1c.o: ../../../libwebp-1.2.4/src/dsp/lossless_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling lossless_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rescaler_41efb5f8.o: ../../../libwebp-1.2.4/src/dsp/rescaler.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling rescaler.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rescaler_mips_dsp_r2_3cc69cd7.o: ../../../libwebp-1.2.4/src/dsp/rescaler_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling rescaler_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rescaler_mips32_41ecb5af.o: ../../../libwebp-1.2.4/src/dsp/rescaler_mips32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling rescaler_mips32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rescaler_msa_b141434.o: ../../../libwebp-1.2.4/src/dsp/rescaler_msa.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling rescaler_msa.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rescaler_neon_58654947.o: ../../../libwebp-1.2.4/src/dsp/rescaler_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling rescaler_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rescaler_sse2_61ad607e.o: ../../../libwebp-1.2.4/src/dsp/rescaler_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling rescaler_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ssim_fa668ae1.o: ../../../libwebp-1.2.4/src/dsp/ssim.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ssim.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ssim_sse2_d3f43535.o: ../../../libwebp-1.2.4/src/dsp/ssim_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ssim_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/upsampling_96f1255f.o: ../../../libwebp-1.2.4/src/dsp/upsampling.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling upsampling.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/upsampling_mips_dsp_r2_ded129be.o: ../../../libwebp-1.2.4/src/dsp/upsampling_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling upsampling_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/upsampling_msa_1971781b.o: ../../../libwebp-1.2.4/src/dsp/upsampling_msa.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling upsampling_msa.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/upsampling_neon_15b46240.o: ../../../libwebp-1.2.4/src/dsp/upsampling_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling upsampling_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/upsampling_sse2_1efc7977.o: ../../../libwebp-1.2.4/src/dsp/upsampling_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling upsampling_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/upsampling_sse41_c093a362.o: ../../../libwebp-1.2.4/src/dsp/upsampling_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling upsampling_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/yuv_b5d4cce7.o: ../../../libwebp-1.2.4/src/dsp/yuv.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling yuv.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/yuv_mips_dsp_r2_f00dc546.o: ../../../libwebp-1.2.4/src/dsp/yuv_mips_dsp_r2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling yuv_mips_dsp_r2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/yuv_mips32_e77bc3e0.o: ../../../libwebp-1.2.4/src/dsp/yuv_mips32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling yuv_mips32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/yuv_neon_ae142fb8.o: ../../../libwebp-1.2.4/src/dsp/yuv_neon.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling yuv_neon.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/yuv_sse2_b75c46ef.o: ../../../libwebp-1.2.4/src/dsp/yuv_sse2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling yuv_sse2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/yuv_sse41_342d84ea.o: ../../../libwebp-1.2.4/src/dsp/yuv_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling yuv_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/bit_reader_utils_1bed8754.o: ../../../libwebp-1.2.4/src/utils/bit_reader_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling bit_reader_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/bit_writer_utils_2dc22304.o: ../../../libwebp-1.2.4/src/utils/bit_writer_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling bit_writer_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/color_cache_utils_f87e62d5.o: ../../../libwebp-1.2.4/src/utils/color_cache_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling color_cache_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/filters_utils_93273a2a.o: ../../../libwebp-1.2.4/src/utils/filters_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling filters_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/huffman_encode_utils_a3a2307.o: ../../../libwebp-1.2.4/src/utils/huffman_encode_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling huffman_encode_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/huffman_utils_636937dc.o: ../../../libwebp-1.2.4/src/utils/huffman_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling huffman_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/quant_levels_dec_utils_2a4b91cd.o: ../../../libwebp-1.2.4/src/utils/quant_levels_dec_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling quant_levels_dec_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/quant_levels_utils_b0347b0a.o: ../../../libwebp-1.2.4/src/utils/quant_levels_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling quant_levels_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/random_utils_d887b9e2.o: ../../../libwebp-1.2.4/src/utils/random_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling random_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rescaler_utils_b6985b7a.o: ../../../libwebp-1.2.4/src/utils/rescaler_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling rescaler_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/thread_utils_4317ac29.o: ../../../libwebp-1.2.4/src/utils/thread_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling thread_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/utils_56d1a26e.o: ../../../libwebp-1.2.4/src/utils/utils.c
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

$(JUCE_OBJDIR)/DtmLayersViewer_98c8b452.o: ../../Source/DtmLayersViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling DtmLayersViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DtmShader_bfa70483.o: ../../Source/DtmShader.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling DtmShader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/GeoBase_343e9923.o: ../../Source/GeoBase.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling GeoBase.cpp"
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

$(JUCE_OBJDIR)/OGL3DViewer_c264a858.o: ../../Source/OGL3DViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling OGL3DViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OsmLayer_d8c84a51.o: ../../Source/OsmLayer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling OsmLayer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SelTreeViewer_bd03fdab.o: ../../Source/SelTreeViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling SelTreeViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VectorLayersViewer_544e20e0.o: ../../Source/VectorLayersViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VectorLayersViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WmtsLayer_f85ab9dd.o: ../../Source/WmtsLayer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling WmtsLayer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sqlite3_31f93e88.o: ../../../Sqlite/sqlite3.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sqlite3.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

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

$(JUCE_OBJDIR)/XLasFile_52021412.o: ../../../XToolAlgo/XLasFile.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XLasFile.cpp"
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

$(JUCE_OBJDIR)/XLzwCodec_a21dc385.o: ../../../XToolImage/XLzwCodec.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XLzwCodec.cpp"
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

$(JUCE_OBJDIR)/XDBase_47d5afbd.o: ../../../XToolVector/XDBase.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XDBase.cpp"
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

$(JUCE_OBJDIR)/XWKBGeom_ed139392.o: ../../../XToolVector/XWKBGeom.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling XWKBGeom.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/adler32_5cf31f67.o: ../../../zlib-1.2.11/adler32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling adler32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/compress_39994dba.o: ../../../zlib-1.2.11/compress.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling compress.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/crc32_7e889e65.o: ../../../zlib-1.2.11/crc32.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling crc32.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/deflate_4f5cde1d.o: ../../../zlib-1.2.11/deflate.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling deflate.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gzclose_f3b4fc57.o: ../../../zlib-1.2.11/gzclose.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling gzclose.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gzlib_60606424.o: ../../../zlib-1.2.11/gzlib.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling gzlib.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gzread_b5b12661.o: ../../../zlib-1.2.11/gzread.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling gzread.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gzwrite_1f9a987e.o: ../../../zlib-1.2.11/gzwrite.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling gzwrite.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/infback_ca8582fa.o: ../../../zlib-1.2.11/infback.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling infback.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/inffast_d1604d2f.o: ../../../zlib-1.2.11/inffast.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling inffast.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/inflate_db9d9cf9.o: ../../../zlib-1.2.11/inflate.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling inflate.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/inftrees_5b77db2c.o: ../../../zlib-1.2.11/inftrees.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling inftrees.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/trees_2066b47.o: ../../../zlib-1.2.11/trees.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling trees.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/uncompr_35e6227c.o: ../../../zlib-1.2.11/uncompr.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling uncompr.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/zutil_455f706e.o: ../../../zlib-1.2.11/zutil.c
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

$(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o: ../../JuceLibraryCode/include_juce_gui_basics.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_basics.cpp"
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
	-$(V_AT)$(STRIP) --strip-unneeded $(JUCE_OUTDIR)/$(TARGET)

-include $(OBJECTS_APP:%.o=%.d)
