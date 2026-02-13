//-----------------------------------------------------------------------------
//								ZoomViewer.cpp
//								==============
//
// Zoom rapide
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 07/04/2025
//-----------------------------------------------------------------------------

#include "ZoomViewer.h"
#include "../XToolImage/XBaseImage.h"
#include "../XTool/XTransfo.h"
#include "../XTool/XInterpol.h"
#include "../../XToolAlgo/XToneMapper.h"

class XTransfoZoom : public XTransfo {
protected:
  double	m_dZoom;
  double	m_dFactor;

public:
  XTransfoZoom(double zoom) { m_dZoom = zoom; m_dFactor = 1. / zoom; }

  virtual void Direct(double x, double y, double* u, double* v) {
    *u = x * m_dFactor;
    *v = y * m_dFactor;
  }
  virtual void Dimension(int w, int h, int* wout, int* hout) { *wout = (int)(w * m_dZoom); *hout = (int)(h * m_dZoom); }
};


//-----------------------------------------------------------------------------
// Fonction utilitaire
//-----------------------------------------------------------------------------
#ifdef  IGNMAP_ONNX
template <typename T>
Ort::Value vec_to_tensor(std::vector<T>& data, const std::vector<std::int64_t>& shape) {
  Ort::MemoryInfo mem_info =
    Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
  auto tensor = Ort::Value::CreateTensor<T>(mem_info, data.data(), data.size(), shape.data(), shape.size());
  return tensor;
}
#endif

//-----------------------------------------------------------------------------
// Nettoyage du modele
//-----------------------------------------------------------------------------
void ZoomViewer::Clear()
{
#ifdef  IGNMAP_ONNX
  if (m_Session != nullptr)
    delete m_Session;
  if (m_Env != nullptr)
    delete m_Env;
  m_Session = nullptr;
  m_Env = nullptr;
#endif
}

//-----------------------------------------------------------------------------
// Chargement d'un modele
//-----------------------------------------------------------------------------
#ifdef  IGNMAP_ONNX
bool ZoomViewer::LoadModel()
{
  juce::String filename = AppUtil::OpenFile("Model", "Modele de super resolution", "*.onnx");
  if (filename.isEmpty())
    return false;

  if (m_Env == nullptr) {
    m_Env = new  Ort::Env(ORT_LOGGING_LEVEL_WARNING, "IGNMap");
  }
  if (m_Session != nullptr)   // On detruit la session si un modele a deja ete charge
    delete m_Session;

  juce::File modelFile(filename);

#if defined JUCE_WINDOWS
  std::basic_string<ORTCHAR_T> model_file = filename.toWideCharPointer();
#else
  std::basic_string<ORTCHAR_T> model_file = filename.toStdString();
#endif

  // Creation d'une session ONNXRuntime
  Ort::SessionOptions session_options;
  m_Session = new Ort::Session(*m_Env, model_file.c_str(), session_options);

  return true;
}
#endif

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void ZoomViewer::mouseDown(const juce::MouseEvent& event)
{
	if (event.y <= getTitleBarHeight()) {
		juce::DocumentWindow::mouseDown(event);
		return;
	}

  juce::PopupMenu menu;
  bool tick = false;
#ifdef  IGNMAP_ONNX
  std::function< void() > Interpolation = [=]() {	Clear(); };
  menu.addItem(juce::translate("Interpolation"), Interpolation);
  std::function< void() > ZoomIA = [=]() {	LoadModel(); };
  menu.addItem(juce::translate("IA"), ZoomIA);
#endif
  std::function< void() > Zoom2X = [=]() {	m_nZoom = 2; ComputeZoom(); };
  if (m_nZoom == 2) tick = true; else tick = false;
  menu.addItem(juce::translate("Zoom x 2"), true, tick, Zoom2X);
  std::function< void() > Zoom4X = [=]() {	m_nZoom = 4; ComputeZoom(); };
  if (m_nZoom == 4) tick = true; else tick = false;
  menu.addItem(juce::translate("Zoom x 4"), true, tick, Zoom4X);
  if (m_nZoom == 6) tick = true; else tick = false;
  std::function< void() > Zoom6X = [=]() {	m_nZoom = 6; ComputeZoom(); };
  menu.addItem(juce::translate("Zoom x 6"), true, tick, Zoom6X);

  if (m_nTone == 1) tick = true; else tick = false;
  std::function< void() > Magic = [=]() {	m_nTone = (m_nTone + 1) % 2; ComputeZoom(); };
  menu.addItem(juce::translate("Tone Mapping"), true, tick, Magic);

  menu.showMenuAsync(juce::PopupMenu::Options());

}

//-----------------------------------------------------------------------------
// Fixe l'image source
//-----------------------------------------------------------------------------
#ifdef  IGNMAP_ONNX
void ZoomViewer::SetTargetImage(const juce::Image& image)
{
  if (m_Session == nullptr) {
    juce::Image resized_image = image.rescaled(image.getWidth() * 4, image.getHeight() * 4, juce::Graphics::highResamplingQuality);
    m_ImageComponent.setImage(resized_image);
  }
	else
		RunModel(image);
}
#else
void ZoomViewer::SetTargetImage(const juce::Image& image) { m_TargetImage = image; ComputeZoom(); }

void ZoomViewer::ComputeZoom()
{
  /*
  juce::Image resized_image = image.rescaled(image.getWidth() * 4, image.getHeight() * 4, juce::Graphics::highResamplingQuality);
  m_ImageComponent.setImage(resized_image);
  */
  //juce::Image resized_image = image.rescaled(image.getWidth() * 4, image.getHeight() * 4, juce::Graphics::highResamplingQuality);

  juce::Image imageRGB = m_TargetImage.convertedToFormat(juce::Image::RGB);

  juce::Image resized_image(imageRGB.getFormat(), imageRGB.getWidth() * m_nZoom, imageRGB.getHeight() * m_nZoom, true);

  juce::Image::BitmapData bitmap(imageRGB, juce::Image::BitmapData::readOnly);
  juce::Image::BitmapData resized(resized_image, juce::Image::BitmapData::readWrite);
  {
    int w = bitmap.width, h = bitmap.height, nbSample = bitmap.pixelStride;
    int offset = bitmap.lineStride - (w * nbSample);
    XTransfoZoom transfo(m_nZoom);
    XInterCubCatmull interpol;
    uint8_t* out = resized.data;
    XBaseImage::Resample(bitmap.data, out, w, h, (uint16_t)nbSample, (uint16_t)offset, &transfo, &interpol, true);

    if (m_nTone != 0) {
      XToneMapper::ToneMappingInt mapper;
      mapper.set_enabled(0, true);
      mapper.set_power(0, (float)5);
      mapper.set_unsharp_mask_enabled(true);
      mapper.set_unsharp_mask_power((float)20);
      mapper.process_8bit_rgb_image(out, w * m_nZoom, h * m_nZoom);
    }
  }
  /*
  juce::ImageConvolutionKernel kernel(3);
  kernel.setKernelValue(0, 0, 0.f);
  kernel.setKernelValue(0, 1, -1.f);
  kernel.setKernelValue(0, 2, 0.f);
  kernel.setKernelValue(1, 0, -1.f);
  kernel.setKernelValue(1, 1, 5.f);
  kernel.setKernelValue(1, 2, -1.f);
  kernel.setKernelValue(2, 0, 0.f);
  kernel.setKernelValue(2, 1, -1.f);
  kernel.setKernelValue(2, 2, 0.f);
  kernel.applyToImage(resized_image, resized_image, juce::Rectangle<int>(resized_image.getWidth(), resized_image.getHeight()));
  */
  m_ImageComponent.setImage(resized_image, juce::RectanglePlacement(juce::RectanglePlacement::doNotResize));
}
#endif

//-----------------------------------------------------------------------------
// Applique le modele IA
//-----------------------------------------------------------------------------
#ifdef  IGNMAP_ONNX
bool ZoomViewer::RunModel(const juce::Image& image)
{
  if (m_Session == nullptr) // Pas de modele charge
    return false;
  Ort::TypeInfo info = m_Session->GetInputTypeInfo(0);
  Ort::ConstTensorTypeAndShapeInfo tensorInfo = info.GetTensorTypeAndShapeInfo();
  if (tensorInfo.GetElementType() == ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT)
    return RunModel32(image);
  if (tensorInfo.GetElementType() == ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16)
    return RunModel16(image);
  return false;
}
#endif

//-----------------------------------------------------------------------------
// Applique le modele IA en float 32 bits
//-----------------------------------------------------------------------------
#ifdef  IGNMAP_ONNX
bool ZoomViewer::RunModel32(const juce::Image& image)
{
  if (m_Session == nullptr) // Pas de modele charge
    return false;

  if ((m_Session->GetInputCount() < 1) || (m_Session->GetOutputCount() < 1))
    return false;

  if (!image.isValid())
    return false;

  Ort::AllocatorWithDefaultOptions allocator;

  // Entree du modele
  std::vector<IOInfo> InputT;
  for (std::size_t i = 0; i < m_Session->GetInputCount(); i++) {
    IOInfo inputInfo;
    inputInfo.Type = m_Session->GetInputNameAllocated(i, allocator).get();
    inputInfo.Shape = m_Session->GetInputTypeInfo(i).GetTensorTypeAndShapeInfo().GetShape();
    InputT.push_back(inputInfo);
  }

  // Sortie du modele
  std::vector<IOInfo> OutputT;
  for (std::size_t i = 0; i < m_Session->GetOutputCount(); i++) {
    IOInfo outputInfo;
    outputInfo.Type = m_Session->GetOutputNameAllocated(i, allocator).get();
    outputInfo.Shape = m_Session->GetOutputTypeInfo(i).GetTensorTypeAndShapeInfo().GetShape();
    OutputT.push_back(outputInfo);
  }
  if (OutputT.size() < 1)
    return false;
  //if (OutputT[0].Type != "upscaled_image")
  //  return false;

  // Preparation de l'entree
  int defaultSize = 64;
  auto input_shape = InputT[0].Shape;
  input_shape[0] = 1;	// Une seule image
  input_shape[1] = 3; // On impose le RGB
  if ((input_shape[2] == -1) && (input_shape[3] == -1))
    input_shape[2] = input_shape[3] = defaultSize;

  // Recuperation de l'image en entree
  int total_number_elements = (int)(input_shape[3] * input_shape[2] * input_shape[1]);
  std::vector<float> input_tensor_values(total_number_elements);

  //image = image.getClippedImage(juce::Rectangle<int>(X, Y, input_shape[2], input_shape[3]));
  juce::Image resized_image = image.rescaled(input_shape[2], input_shape[3], juce::Graphics::highResamplingQuality);
  {
    juce::Image::BitmapData bitmap(resized_image, juce::Image::BitmapData::readOnly);

    if ((bitmap.height != input_shape[3]) || (bitmap.width != input_shape[2]) || (bitmap.pixelStride < input_shape[1]))
      return false;

    // Recuperation des valeurs des pixels
    int cmpt = 0, channel_size = bitmap.height * bitmap.width;
    for (int i = 0; i < bitmap.height; i++) {
      juce::uint8* line = bitmap.getLinePointer(i);
      for (int j = 0; j < bitmap.width; j++) {
        // Ordre BGR dans les images JUCE
        input_tensor_values[cmpt + 2 * channel_size] = (float)(line[0] / 255.);
        input_tensor_values[cmpt + channel_size] = (float)(line[1] / 255.);
        input_tensor_values[cmpt] = (float)(line[2] / 255.);
        cmpt++;
        line += bitmap.pixelStride;
      }
    }
  }

  // Creation du tenseur d'entree pour l'image
  std::vector<Ort::Value> input_tensors;
  input_tensors.emplace_back(vec_to_tensor<float>(input_tensor_values, input_shape));

  // Creation des tableaux de chaines de caracteres
  std::vector<std::string> input_names;
  input_names.push_back(InputT[0].Type);
  std::vector<const char*> input_names_char(input_names.size(), nullptr);
  std::transform(std::begin(input_names), std::end(input_names), std::begin(input_names_char),
    [&](const std::string& str) { return str.c_str(); });

  std::vector<std::string> output_names;
  output_names.push_back(OutputT[0].Type);
  std::vector<const char*> output_names_char(output_names.size(), nullptr);
  std::transform(std::begin(output_names), std::end(output_names), std::begin(output_names_char),
    [&](const std::string& str) { return str.c_str(); });


  try {
    int64_t t0 = juce::Time::currentTimeMillis();
    auto output_tensors = m_Session->Run(Ort::RunOptions{ nullptr }, input_names_char.data(), input_tensors.data(),
      input_names_char.size(), output_names_char.data(), output_names_char.size());

    jassert(output_tensors.size() == output_names.size() && output_tensors[0].IsTensor());

    // Creation de l'image
    float* rec = output_tensors[0].GetTensorMutableData<float>();
    std::vector< int64_t > shape = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();
    float* R = rec;
    float* G = &rec[shape[2] * shape[3]];
    float* B = &rec[2 * shape[2] * shape[3]];

    // Creation de l'image de sortie en ARGB
    juce::Image outImage(juce::Image::ARGB, (int)shape[2], (int)shape[3], true);
    juce::Image::BitmapData outData(outImage, juce::Image::BitmapData::readWrite);

    for (int i = 0; i < outData.height; i++) {
      uint8_t* line = outData.getLinePointer(i);
      for (int j = 0; j < outData.width; j++) {
        line[3] = 255;
        line[2] = (uint8_t)std::clamp((*R * 255.f), 0.f, 255.f); R++;
        line[1] = (uint8_t)std::clamp((*G * 255.f), 0.f, 255.f); G++;
        line[0] = (uint8_t)std::clamp((*B * 255.f), 0.f, 255.f); B++;

        line += outData.pixelStride;
      }
    }
    m_ImageComponent.setImage(outImage);
  }
  catch (const Ort::Exception& exception) {
    juce::String message = "ERROR running model inference: ";
    message += exception.what();
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon, juce::translate("IGNMap"), message, "OK");
    return false;
  }

  return true;
}
#endif

//-----------------------------------------------------------------------------
// Applique le modele IA en float 16 bits
//-----------------------------------------------------------------------------
#ifdef  IGNMAP_ONNX
bool ZoomViewer::RunModel16(const juce::Image& image)
{
  if (m_Session == nullptr) // Pas de modele charge
    return false;

  if ((m_Session->GetInputCount() < 1) || (m_Session->GetOutputCount() < 1))
    return false;

  if (!image.isValid())
    return false;

  Ort::AllocatorWithDefaultOptions allocator;

  // Entree du modele
  std::vector<IOInfo> InputT;
  for (std::size_t i = 0; i < m_Session->GetInputCount(); i++) {
    IOInfo inputInfo;
    inputInfo.Type = m_Session->GetInputNameAllocated(i, allocator).get();
    inputInfo.Shape = m_Session->GetInputTypeInfo(i).GetTensorTypeAndShapeInfo().GetShape();
    InputT.push_back(inputInfo);
  }

  // Sortie du modele
  std::vector<IOInfo> OutputT;
  for (std::size_t i = 0; i < m_Session->GetOutputCount(); i++) {
    IOInfo outputInfo;
    outputInfo.Type = m_Session->GetOutputNameAllocated(i, allocator).get();
    outputInfo.Shape = m_Session->GetOutputTypeInfo(i).GetTensorTypeAndShapeInfo().GetShape();
    OutputT.push_back(outputInfo);
  }
  if (OutputT.size() != 1)
    return false;
  //if (OutputT[0].Type != "upscaled_image")
  //  return false;

  // Preparation de l'entree
  int defaultSize = 64;
  auto input_shape = InputT[0].Shape;
  input_shape[0] = 1;	// Une seule image
  input_shape[1] = 3; // On impose le RGB
  if ((input_shape[2] == -1) && (input_shape[3] == -1))
    input_shape[2] = input_shape[3] = defaultSize;

  // Recuperation de l'image en entree
  int total_number_elements = (int)(input_shape[3] * input_shape[2] * input_shape[1]);
  std::vector<Ort::Float16_t> input_tensor_values(total_number_elements);

  //image = image.getClippedImage(juce::Rectangle<int>(X, Y, input_shape[2], input_shape[3]));
  {
    juce::Image::BitmapData bitmap(image, juce::Image::BitmapData::readOnly);

    if ((bitmap.height != input_shape[3]) || (bitmap.width != input_shape[2]) || (bitmap.pixelStride < input_shape[1]))
      return false;

    // Recuperation des valeurs des pixels
    int cmpt = 0, channel_size = bitmap.height * bitmap.width;
    for (int i = 0; i < bitmap.height; i++) {
      juce::uint8* line = bitmap.getLinePointer(i);
      for (int j = 0; j < bitmap.width; j++) {
        // Ordre BGR dans les images JUCE
        input_tensor_values[cmpt + 2 * channel_size] = (Ort::Float16_t)(float)(line[0] / 255.);
        input_tensor_values[cmpt + channel_size] = (Ort::Float16_t)(float)(line[1] / 255.);
        input_tensor_values[cmpt] = (Ort::Float16_t)(float)(line[2] / 255.);
        cmpt++;
        line += bitmap.pixelStride;
      }
    }
  }

  // Creation du tenseur d'entree pour l'image
  std::vector<Ort::Value> input_tensors;
  input_tensors.emplace_back(vec_to_tensor<Ort::Float16_t>(input_tensor_values, input_shape));

  // Creation des tableaux de chaines de caracteres
  std::vector<std::string> input_names;
  input_names.push_back(InputT[0].Type);
  std::vector<const char*> input_names_char(input_names.size(), nullptr);
  std::transform(std::begin(input_names), std::end(input_names), std::begin(input_names_char),
    [&](const std::string& str) { return str.c_str(); });

  std::vector<std::string> output_names;
  output_names.push_back(OutputT[0].Type);
  std::vector<const char*> output_names_char(output_names.size(), nullptr);
  std::transform(std::begin(output_names), std::end(output_names), std::begin(output_names_char),
    [&](const std::string& str) { return str.c_str(); });


  try {
    auto output_tensors = m_Session->Run(Ort::RunOptions{ nullptr }, input_names_char.data(), input_tensors.data(),
      input_names_char.size(), output_names_char.data(), output_names_char.size());

    jassert(output_tensors.size() == output_names.size() && output_tensors[0].IsTensor());

    // Creation de l'image
    Ort::Float16_t* rec = output_tensors[0].GetTensorMutableData<Ort::Float16_t>();
    std::vector< int64_t > shape = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();
    Ort::Float16_t* R = rec;
    Ort::Float16_t* G = &rec[shape[2] * shape[3]];
    Ort::Float16_t* B = &rec[2 * shape[2] * shape[3]];
    float red, green, blue;

    // Creation de l'image de sortie en ARGB
    juce::Image outImage(juce::Image::ARGB, (int)shape[2], (int)shape[3], true);
    juce::Image::BitmapData outData(outImage, juce::Image::BitmapData::readWrite);

    for (int i = 0; i < outData.height; i++) {
      uint8_t* line = outData.getLinePointer(i);
      for (int j = 0; j < outData.width; j++) {

        red = (float)*R;
        green = (float)*G;
        blue = (float)*B;

        line[3] = 255;
        line[2] = (uint8_t)std::clamp((red * 255.f), 0.f, 255.f); R++;
        line[1] = (uint8_t)std::clamp((green * 255.f), 0.f, 255.f); G++;
        line[0] = (uint8_t)std::clamp((blue * 255.f), 0.f, 255.f); B++;

        line += outData.pixelStride;
      }
    }
    m_ImageComponent.setImage(outImage);
  }
  catch (const Ort::Exception& exception) {
    juce::String message = "ERROR running model inference: ";
    message += exception.what();
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon, juce::translate("IGNMap"), message, "OK");
    return false;
  }

  return true;
}
#endif
