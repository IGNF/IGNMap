//-----------------------------------------------------------------------------
//								MapView.cpp
//								===========
//
// Composant pour la visualisation d'objets geographiques
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 06/01/2023
//-----------------------------------------------------------------------------

#include "MapView.h"
#include "../../XTool/XGeoBase.h"

//==============================================================================
MapView::MapView() : m_MapThread("MapThread")
{
	Clear();
	setOpaque(true);
	startTimerHz(60);
}

MapView::~MapView()
{
	m_MapThread.stopThread(5000);
}

void MapView::Clear()
{
	m_dX0 = m_dY0 = m_dX = m_dY = m_dZ = 0.;
	m_dScale = 1.0;
	m_bDrag = m_bZoom = m_bSelect = false;
	m_GeoBase = nullptr;
	m_Frame = XFrame();
	m_nMouseMode = Move;
}

void MapView::paint(juce::Graphics& g)
{
	if ((m_bZoom) || (m_bSelect)) {
		g.drawImageAt(m_Image, 0, 0);
		g.setColour(juce::Colours::pink);
		g.drawRect(juce::Rectangle<int>(m_StartPt, m_DragPt + m_StartPt), 1);
		return;
	}
	if (m_bDrag) {
		//g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId
		g.fillAll(juce::Colours::white);
		g.drawImageAt(m_Image, m_DragPt.x, m_DragPt.y);
		DrawDecoration(g, m_DragPt.x, m_DragPt.y);
		return;
	}

	m_MapThread.Draw(g);
	DrawDecoration(g);
}

void MapView::resized()
{
	auto b = getLocalBounds();
	if (b.isEmpty())
		return;
	m_Image = juce::Image(juce::Image::PixelFormat::ARGB, b.getWidth(), b.getHeight(), true);
	RenderMap();
}

//==============================================================================
// Affichage des coordonnees, de l'emprise, de l'echelle ...
//==============================================================================
void MapView::DrawDecoration(juce::Graphics& g, int deltaX, int deltaY)
{
	// Affichage des coordonnees, de l'emprise, de l'echelle ...
	XFrame F = m_MapThread.Frame();
	g.setFont(10.0f);
	auto b = getLocalBounds();
	juce::Rectangle<int> R(0, b.getHeight() - 15, b.getWidth(), 15);
	g.setColour(juce::Colours::darkgrey);
	g.setOpacity(0.5);
	g.fillRect(R);
	R.reduce(5, 0);
	g.setColour(juce::Colours::white);
	g.setOpacity(1.);
	g.drawText(juce::String(F.Xmin - deltaX * m_dScale, 2) + " ; " + juce::String(F.Ymin + deltaY * m_dScale, 2), R, juce::Justification::centredLeft);
	g.drawText(juce::String(m_dX, 2) + " ; " + juce::String(m_dY, 2) + " ; " + juce::String(m_dZ, 2), R, juce::Justification::centred);
	g.drawText(juce::String("1/") + juce::String(ComputeCartoScale(), 1) + juce::String(" (gsd = ") + juce::String(m_dScale, 2) + juce::String(")"),
						 R, juce::Justification::centredRight);

	R = juce::Rectangle<int>(0, 0, b.getWidth(), 15);
	if (m_MapThread.isThreadRunning())
		g.setColour(juce::Colours::lightpink);
	else
		g.setColour(juce::Colours::darkgrey);
	g.setOpacity(0.5);
	g.fillRect(R);
	R.reduce(5, 0);
	g.setColour(juce::Colours::white);
	g.setOpacity(1.);
	g.drawText(juce::String(F.Xmax - deltaX * m_dScale, 2) + " ; " + juce::String(F.Ymax + deltaY * m_dScale, 2), R, juce::Justification::centredRight);
	g.drawText(juce::String(m_MapThread.NumObjects()), R, juce::Justification::centred);
}

//==============================================================================
// Lancement du thread de dessin des donnees
//==============================================================================
void MapView::RenderMap(bool overlay, bool raster, bool dtm, bool vector, bool las, bool totalUpdate)
{
	bool updateMode = totalUpdate;
	m_MapThread.signalThreadShouldExit();
	if (m_MapThread.isThreadRunning()) {
		//m_MapThread.waitForThreadToExit(1000);
		updateMode = true;	// Le thread n'est pas termine, la carte est peut etre sale
		if (!m_MapThread.stopThread(-1))
			return;
	}

	auto b = getLocalBounds();
	m_MapThread.SetGeoBase(m_GeoBase);
	m_MapThread.SetUpdate(overlay, raster, dtm, vector, las);
	m_MapThread.SetWorld(m_dX0, m_dY0, m_dScale, b.getWidth(), b.getHeight(), updateMode);

	m_MapThread.startThread();
}

//==============================================================================
// Gestion de la souris
//==============================================================================
void MapView::mouseDown(const juce::MouseEvent& event)
{
	juce::Graphics imaG(m_Image);
	m_MapThread.Draw(imaG);
	m_StartPt = event.getPosition();
	setMouseCursor(juce::MouseCursor(juce::MouseCursor::CrosshairCursor));
	if ((event.mods.isCtrlDown()) || (m_nMouseMode == Zoom)) {
		m_bZoom = true;
		return;
	}
	if ((event.mods.isShiftDown()) || (m_nMouseMode == Select) || (m_nMouseMode == Select3D)) {
		m_bSelect = true;
		return;
	}
	m_bZoom = m_bSelect = false;
	m_bDrag = true;
	setMouseCursor(juce::MouseCursor(juce::MouseCursor::DraggingHandCursor));
	double x0 = m_StartPt.getX(), y0 = m_StartPt.getY();
	Pixel2Ground(x0, y0);
	sendActionMessage("UpdateGroundPos:"+ juce::String(x0, 2) + ":" + juce::String(y0, 2));
}

void MapView::mouseMove(const juce::MouseEvent& event)
{
	m_dX = event.x;
	m_dY = event.y;
	m_dZ = m_MapThread.GetZ(event.x, event.y);
	Pixel2Ground(m_dX, m_dY);
}

void MapView::mouseDrag(const juce::MouseEvent& event)
{
	m_DragPt.x = event.getDistanceFromDragStartX();
	m_DragPt.y = event.getDistanceFromDragStartY();
	repaint();
}

void MapView::mouseUp(const juce::MouseEvent& event)
{
	setMouseCursor(juce::MouseCursor(juce::MouseCursor::NormalCursor));
	double X0 = m_StartPt.x, Y0 = m_StartPt.y, X1 = m_StartPt.x + m_DragPt.x, Y1 = m_StartPt.y + m_DragPt.y;
	Pixel2Ground(X0, Y0);
	Pixel2Ground(X1, Y1);

	if ((abs(m_DragPt.x) > 1) || (abs(m_DragPt.y) > 1)) {
		auto b = getLocalBounds();
		if (m_bZoom) {
			m_dScale /= ((b.getWidth() / m_DragPt.x + b.getHeight() / m_DragPt.y) * 0.5);
			CenterView((X0 + X1) * 0.5, (Y0 + Y1) * 0.5);
		}
		if (m_bSelect) {
			if ((m_nMouseMode == Select)|| (event.mods.isShiftDown()))
				SelectFeatures(X0, Y0, X1, Y1);
			if (m_nMouseMode == Select3D)
				Update3DView(X0, Y0, X1, Y1);
		}
		if ((!m_bZoom) && (!m_bSelect)) {
			m_dX0 -= m_DragPt.x * m_dScale;
			m_dY0 += m_DragPt.y * m_dScale;
			RenderMap();
		}
	}
	else {
		if (event.mods.isShiftDown() || (m_nMouseMode == Select))
			SelectFeatures(event.getPosition());
	}
	m_bDrag = m_bZoom = m_bSelect = false;
	m_DragPt = juce::Point<int>(0, 0);
}

void MapView::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
	double X = event.getPosition().x, Y = event.getPosition().y;
	Pixel2Ground(X, Y);
	if (wheel.deltaY < 0.)
		m_dScale *= sqrt(2.0);
	else
		m_dScale *= (1. / sqrt(2.0));
	CenterView(X, Y);
}

void MapView::mouseDoubleClick(const juce::MouseEvent& event)
{
	double X = event.getPosition().x, Y = event.getPosition().y;
	Pixel2Ground(X, Y);
	CenterView(X, Y);
}

//==============================================================================
// Zoom sur l'emprise des donnees
//==============================================================================
void MapView::ZoomWorld()
{
	if (m_Frame.IsEmpty())
		return;
	auto b = getLocalBounds();
	double scaleX = (m_Frame.Xmax - m_Frame.Xmin) / b.getWidth();
	double scaleY = (m_Frame.Ymax - m_Frame.Ymin) / b.getHeight();
	m_dScale = (scaleX > scaleY) ? scaleX : scaleY;
	CenterView((m_Frame.Xmax + m_Frame.Xmin) * 0.5, (m_Frame.Ymax + m_Frame.Ymin) * 0.5);
}

//==============================================================================
// Zoom au niveau le plus proche de la pyramide WMTS
//==============================================================================
void MapView::ZoomLevel()
{
	if (m_GeoBase == nullptr)
		return;
	auto b = getLocalBounds();
	double X = b.getWidth() / 2, Y = b.getHeight() / 2;
	Pixel2Ground(X, Y);
	
	double gsd = std::numeric_limits<double>::max();
	for (uint32_t i = 0; i < m_GeoBase->NbClass(); i++) {
		XGeoClass* C = m_GeoBase->Class(i);
		if (C->IsVector())
			continue;
		for(uint32_t j = 0; j < C->NbVector(); j++)
			gsd = std::min<double>(gsd, C->Vector(j)->Resolution());
	}
	
	if (gsd == std::numeric_limits<double>::max())
		return; // Rien a faire : que des donnees vectorielles
	m_dScale = gsd;
	CenterView(X, Y);
}

//==============================================================================
// Zoom a une echelle cartographique
//==============================================================================
void MapView::ZoomScale(double scale)
{
	auto b = getLocalBounds();
	double X = b.getWidth() / 2, Y = b.getHeight() / 2;
	Pixel2Ground(X, Y);
	ComputeCartoScale(scale);
	CenterView(X, Y);
}

//==============================================================================
// Zoom sur une emprise
//==============================================================================
void MapView::ZoomFrame(const XFrame& F, double buffer)
{
	auto b = getLocalBounds();
	double scaleX = (F.Xmax - F.Xmin + 2 * buffer) / b.getWidth();
	double scaleY = (F.Ymax - F.Ymin + 2 * buffer) / b.getHeight();
	m_dScale = (scaleX > scaleY) ? scaleX : scaleY;
	CenterView((F.Xmax + F.Xmin) * 0.5, (F.Ymax + F.Ymin) * 0.5);
}

//==============================================================================
// Centre la vue sur un point terrain
//==============================================================================
void MapView::CenterView(const double& X, const double& Y)
{
	auto b = getLocalBounds();
	m_dX0 = X - b.getWidth() * 0.5 * m_dScale;
	m_dY0 = Y + b.getHeight() * 0.5 * m_dScale;
	RenderMap();
}

//==============================================================================
// Conversion Pixel -> Terrain
//==============================================================================
void MapView::Pixel2Ground(double& X, double& Y)
{
	X = m_dX0 + X * m_dScale;
	Y = m_dY0 - Y * m_dScale;
}

//==============================================================================
// Conversion Terrain -> Pixel
//==============================================================================
void MapView::Ground2Pixel(double& X, double& Y)
{
	X = (X - m_dX0) / m_dScale;
	Y = (m_dY0 - Y) / m_dScale;
}

//==============================================================================
// Echelle cartograpique 1 : cartoscale
//==============================================================================
double MapView::ComputeCartoScale(double cartoscale)
{
	juce::Desktop* desktop = &juce::Desktop::getInstance();
	const juce::Displays* displays = &(desktop->getDisplays());
	if (displays == nullptr)
		return 0.;
	const juce::Displays::Display* display = displays->getPrimaryDisplay();
	if (display == nullptr)
		return 0;
	double factor = (0.0254 / (display->dpi / display->scale));
	if (cartoscale <= 0.)	// On cherche le denominateur de l'echelle cartographique
		return m_dScale / factor;
	// Sinon on fixe l'echelle de la vue a partir de l'echelle cartographique
	m_dScale = cartoscale * factor;
	return 0.;
}

//==============================================================================
// Fixe l'envelope totale de la vue
//==============================================================================
void MapView::SetFrame(XFrame F)
{
	auto b = getLocalBounds();
	double X = b.getCentreX(), Y = b.getCentreY();
	if (!m_Frame.IsEmpty())
		Pixel2Ground(X, Y);
	else {
		X = (F.Xmin + F.Xmax) * 0.5;
		Y = (F.Ymin + F.Ymax) * 0.5;
		ComputeCartoScale(10000);
	}
	m_Frame = F;
	m_dX0 = F.Xmin;
	m_dY0 = F.Ymax;
	CenterView(X, Y);
}

//==============================================================================
// Selection des vecteurs se trouvant proche du point P (coordoonees pixel)
//==============================================================================
void MapView::SelectFeatures(juce::Point<int> P)
{
	if (m_GeoBase == nullptr)
		return;
	XFrame F;
	double X = P.x - 1, Y = P.y - 1;
	Pixel2Ground(X, Y);
	F += XPt2D(X, Y);
	X = P.x + 1; Y = P.y + 1;
	Pixel2Ground(X, Y);
	F += XPt2D(X, Y);
	m_GeoBase->SelectFeatures(&F);
	sendActionMessage("UpdateSelectFeatures");
}

//==============================================================================
// Selection des vecteurs se trouvant dans une enveloppe (coordonnees terrain)
//==============================================================================
void MapView::SelectFeatures(const double& X0, const double& Y0, const double& X1, const double& Y1)
{
	if (m_GeoBase == nullptr)
		return;
	XFrame F;
	F += XPt2D(X0, Y0);
	F += XPt2D(X1, Y1);
	m_GeoBase->SelectFeatures(&F);
	sendActionMessage("UpdateSelectFeatures");
}

//==============================================================================
// Selection de l'emprise pour l'affichage 3D
//==============================================================================
void MapView::Update3DView(const double& X0, const double& Y0, const double& X1, const double& Y1)
{
	XFrame F;
	F += XPt2D(X0, Y0);
	F += XPt2D(X1, Y1);
	sendActionMessage("Update3DView:" + juce::String(F.Xmin) + ":" + juce::String(F.Xmax) + ":" +
		juce::String(F.Ymin) + ":" + juce::String(F.Ymax));
}

void MapView::DrawFrame(const XFrame& env)
{

	repaint();
}