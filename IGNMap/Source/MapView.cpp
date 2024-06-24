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
MapView::MapView(juce::String name) : m_MapThread(name)
{
	m_strName = name;
	Clear();
	setOpaque(true);
	startTimerHz(5);
	m_MapThread.addListener(this);
}

MapView::~MapView()
{
	StopThread();
}

void MapView::Clear()
{
	StopThread();
	m_dX0 = m_dY0 = m_dX = m_dY = m_dZ = 0.;
	m_dScale = 1.0;
	m_bDrag = m_bZoom = m_bSelect = m_bDrawing = false;
	m_GeoBase = nullptr;
	m_Frame = m_SelectionFrame = m_3DFrame = XFrame();
	m_nMouseMode = Move;
	m_nFrameCounter = 0;
	m_Annotation.Clear();
}

void MapView::paint(juce::Graphics& g)
{
	if ((m_bZoom) || (m_bSelect)) {
		g.drawImageAt(m_Image, 0, 0);
		g.setColour(juce::Colours::pink);
		if (m_bZoom) g.setColour(juce::Colours::coral);
		g.drawRect(juce::Rectangle<int>(m_StartPt, m_DragPt + m_StartPt), 2);
		return;
	}
	if (m_bDrag) {
		g.fillAll(juce::Colours::white);
		g.drawImageAt(m_Image, m_DragPt.x, m_DragPt.y);
		DrawAnnotation(g, m_DragPt.x, m_DragPt.y);
		DrawTarget(g, m_DragPt.x, m_DragPt.y);
		DrawDecoration(g, m_DragPt.x, m_DragPt.y);
		return;
	}

	if (!m_MapThread.Draw(g)) {
		g.fillAll(juce::Colours::white);
		g.drawImageAt(m_Image, m_DragPt.x, m_DragPt.y);
	}
	if (!m_MapThread.isThreadRunning())
		DrawAnnotation(g);
	DrawFrames(g);
	DrawTarget(g);
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
// Mode de la souris : deplacement, selection, zoom, dessin ...
//==============================================================================
void MapView::SetMouseMode(MouseMode mode)
{ 
	m_nMouseMode = mode;
	if (m_nMouseMode == Move)
		setMouseCursor(juce::MouseCursor(juce::MouseCursor::NormalCursor));
	else
		setMouseCursor(juce::MouseCursor(juce::MouseCursor::CrosshairCursor));
	m_Annotation.Close();
	m_Annotation.Clear();
}

//==============================================================================
// Gestion de la souris
//==============================================================================
void MapView::mouseDown(const juce::MouseEvent& event)
{
	juce::Graphics imaG(m_Image);
	m_MapThread.Draw(imaG);
	m_StartPt = event.getPosition();
	m_DragPt = juce::Point<int>(0, 0);
	//setMouseCursor(juce::MouseCursor(juce::MouseCursor::CrosshairCursor));
	if ((event.mods.isCtrlDown()) || (m_nMouseMode == Zoom)) {
		m_bZoom = true;
		return;
	}
	if ((event.mods.isShiftDown()) || (m_nMouseMode == Select) || (m_nMouseMode == Select3D)) {
		m_bSelect = true;
		return;
	}
	if ((m_nMouseMode == Polyline) || (m_nMouseMode == Polygone) || (m_nMouseMode == Rectangle) || (m_nMouseMode == Text)) {
		m_bDrawing = true;
		AddAnnotationPoint(m_StartPt);
		return;
	}
	m_bZoom = m_bSelect = m_bDrawing = false;
	m_bDrag = true;
	setMouseCursor(juce::MouseCursor(juce::MouseCursor::DraggingHandCursor));
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
	//setMouseCursor(juce::MouseCursor(juce::MouseCursor::NormalCursor));
	double X0 = m_StartPt.x, Y0 = m_StartPt.y, X1 = m_StartPt.x + m_DragPt.x, Y1 = m_StartPt.y + m_DragPt.y;
	Pixel2Ground(X0, Y0);
	Pixel2Ground(X1, Y1);

	if ((abs(m_DragPt.x) > 1) && (abs(m_DragPt.y) > 1)) {	// Drag avec la souris : action zonale
		auto b = getLocalBounds();
		if (m_bZoom) {
			m_dScale /= ((b.getWidth() / m_DragPt.x + b.getHeight() / m_DragPt.y) * 0.5);
			if (m_dScale < 0.05) m_dScale = 0.05;
			m_Image = juce::Image(juce::Image::PixelFormat::ARGB, m_Image.getWidth(), m_Image.getHeight(), true);
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
			setMouseCursor(juce::MouseCursor(juce::MouseCursor::NormalCursor));
			CenterView(m_dX0 + b.getWidth() * m_dScale * 0.5, m_dY0 - b.getHeight() * m_dScale * 0.5);
		}
		EndMouseAction();
		return;
	}

	double x = event.getPosition().x, y = event.getPosition().y;
	Pixel2Ground(x, y);
	if (m_bZoom) {	// Clic pour zoomer
			if (event.mods.isRightButtonDown()) m_dScale *= (2.0);
			if (event.mods.isLeftButtonDown()) m_dScale *= (0.5);
			m_Image = juce::Image(juce::Image::PixelFormat::ARGB, m_Image.getWidth(), m_Image.getHeight(), true);
			CenterView(x, y);
			EndMouseAction();
			return;
	}
	if (event.mods.isShiftDown() || (m_nMouseMode == Select)) { // Clic pour selectionner
		SelectFeatures(event.getPosition());
		EndMouseAction();
		return;
	}
		
	setMouseCursor(juce::MouseCursor(juce::MouseCursor::NormalCursor));
	sendActionMessage("UpdateGroundPos:" + juce::String(X0, 2) + ":" + juce::String(Y0, 2));
	if (event.mods.isAltDown())
		SetTarget(x, y);
	EndMouseAction();
}

void MapView::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
	double X = event.getPosition().x, Y = event.getPosition().y;
	Pixel2Ground(X, Y);
	if (wheel.deltaY < 0.)
		m_dScale *= sqrt(2.0);
	else
		m_dScale *= (1. / sqrt(2.0));
	m_Image = juce::Image(juce::Image::PixelFormat::ARGB, m_Image.getWidth(), m_Image.getHeight(), true);
	CenterView(X, Y);
}

void MapView::mouseDoubleClick(const juce::MouseEvent& event)
{
	double X = event.getPosition().x, Y = event.getPosition().y;
	Pixel2Ground(X, Y);
	CenterView(X, Y);
}

void MapView::EndMouseAction()
{
	m_bDrag = m_bZoom = m_bSelect = false;
	// m_DragPt = juce::Point<int>(0, 0);
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
// Zoom a une resolution donnee
//==============================================================================
void MapView::ZoomGsd(double gsd)
{
	if ((gsd <= 0.) || (gsd > 100000.))
		return;
	if (fabs(gsd - m_dScale) < 0.01)
		return;
	auto b = getLocalBounds();
	double X = b.getWidth() / 2, Y = b.getHeight() / 2;
	Pixel2Ground(X, Y);
	m_dScale = gsd;
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
void MapView::CenterView(const double& X, const double& Y, double scale, bool notification)
{
	auto b = getLocalBounds();
	if (scale > 0)
		m_dScale = scale;
	m_dX0 = X - b.getWidth() * 0.5 * m_dScale;
	m_dY0 = Y + b.getHeight() * 0.5 * m_dScale;
	RenderMap();
	if (notification)
		sendActionMessage("CenterView:" + juce::String(X, 2) + ":" + juce::String(Y, 2) +
																	":" + juce::String(m_dScale, 2));
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
// Cree un cadre terrain a partir d'une position Pixel et de la demi-taille en pixels
//==============================================================================
XFrame MapView::Pixel2Ground(const double& Xcenter, const double& Ycenter, const double& nbpix)
{
	double X0 = Xcenter - nbpix, Y0 = Ycenter - nbpix, X1 = Xcenter + nbpix, Y1 = Ycenter + nbpix;
	Pixel2Ground(X0, Y0);
	Pixel2Ground(X1, Y1);
	XFrame F;
	F += XPt2D(X0, Y0);
	F += XPt2D(X1, Y1);
	return F;
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
	m_dX0 = X - b.getWidth() * 0.5 * m_dScale;
	m_dY0 = Y + b.getHeight() * 0.5 * m_dScale;
	//CenterView(X, Y);
}

//==============================================================================
// Fixe le point cible de la vue
//==============================================================================
void MapView::SetTarget(double x, double y, bool notify)
{ 
	m_Target = XPt2D(x, y);
	if (notify)
		sendActionMessage("UpdateTargetPos:" + juce::String(m_Target.X, 2) + ":" + juce::String(m_Target.Y, 2));
}

//==============================================================================
// Dessin du point cible
//==============================================================================
void MapView::DrawTarget(juce::Graphics& g, int deltaX, int deltaY)
{
	XPt2D P0 = m_Target;
	Ground2Pixel(P0.X, P0.Y);
	P0 += XPt2D(deltaX, deltaY);
	if ((P0.X < 0)||(P0.Y < 0))
		return;
	g.setColour(juce::Colours::fuchsia);
	g.drawEllipse((float)P0.X - 2.f, (float)P0.Y - 2.f, 4.f, 4.f, 1.f);
	g.drawEllipse((float)P0.X - 4.f, (float)P0.Y - 4.f, 8.f, 8.f, 2.f);
}

//==============================================================================
// Dessin des cadres de selection
//==============================================================================
void MapView::DrawFrames(juce::Graphics& g, int deltaX, int deltaY)
{
	XFrame F = m_3DFrame;
	if (!F.IsEmpty()) {
		Ground2Pixel(F.Xmin, F.Ymax);
		Ground2Pixel(F.Xmax, F.Ymin);
		F.Xmin += deltaX; F.Xmax += deltaX;
		F.Ymin += deltaY; F.Ymax += deltaY;
		g.setColour(juce::Colours::deepskyblue);
		g.setOpacity(0.5f + (m_nFrameCounter % 21) * 0.025f);
		g.drawRect((float)F.Xmin, (float)F.Ymax, (float)fabs(F.Width()), (float)fabs(F.Height()), 2.f);
	}

	F = m_SelectionFrame;
	if (!F.IsEmpty()) {
		Ground2Pixel(F.Xmin, F.Ymax);
		Ground2Pixel(F.Xmax, F.Ymin);
		F.Xmin += deltaX; F.Xmax += deltaX;
		F.Ymin += deltaY; F.Ymax += deltaY;
		g.setColour(juce::Colours::deeppink);
		g.setOpacity(0.5f + (m_nFrameCounter % 21) * 0.025f);
		g.drawRect((float)F.Xmin, (float)F.Ymax, (float)fabs(F.Width()), (float)fabs(F.Height()), 2.f);
	}

	m_nFrameCounter++;
}

//==============================================================================
// Selection des vecteurs se trouvant proche du point P (coordoonees pixel)
//==============================================================================
void MapView::SelectFeatures(juce::Point<int> P)
{
	if (m_GeoBase == nullptr)
		return;
	XFrame F = Pixel2Ground(P.x, P.y, 1);
	m_GeoBase->SelectFeatures(&F);
	F = Pixel2Ground(P.x, P.y, 3);
	m_GeoBase->KeepClosestCentroid(&F);
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
	m_SelectionFrame = F;
	sendActionMessage("UpdateSelectFeatures");
}

//==============================================================================
// Selection de l'emprise pour l'affichage 3D
//==============================================================================
void MapView::Update3DView(const double& X0, const double& Y0, const double& X1, const double& Y1)
{
	m_3DFrame = XFrame();
	m_3DFrame += XPt2D(X0, Y0);
	m_3DFrame += XPt2D(X1, Y1);
	StopThread();
	sendActionMessage("Update3DView:" + juce::String(m_3DFrame.Xmin) + ":" + juce::String(m_3DFrame.Xmax) + ":" +
		juce::String(m_3DFrame.Ymin) + ":" + juce::String(m_3DFrame.Ymax));
}

//==============================================================================
// Ajout d'un point a l'annotation en cours d'edition
//==============================================================================
void MapView::AddAnnotationPoint(juce::Point<int>& P)
{
	double X0 = P.x, Y0 = P.y;
	Pixel2Ground(X0, Y0);
	if (m_Annotation.Primitive() == XAnnotation::pNull) {
		if (m_nMouseMode == Polyline) m_Annotation.Primitive(XAnnotation::pPolyline);
		if (m_nMouseMode == Polygone) m_Annotation.Primitive(XAnnotation::pPolygon);
		if (m_nMouseMode == Rectangle) m_Annotation.Primitive(XAnnotation::pRect);
		if (m_nMouseMode == Text) m_Annotation.Primitive(XAnnotation::pText);
	}
	m_Annotation.AddPt(X0, Y0);
}

//==============================================================================
// Dessin de l'annotation en cours
//==============================================================================
void MapView::DrawAnnotation(juce::Graphics& g, int deltaX, int deltaY)
{
	if (m_Annotation.NbPt() < 1)
		return;
	XPt2D P0 = m_Annotation.Pt(0);
	Ground2Pixel(P0.X, P0.Y);
	P0 += XPt2D(deltaX, deltaY);
	g.setColour(juce::Colours::chartreuse);
	g.drawEllipse((float)P0.X - 3.f, (float)P0.Y - 3.f, 6.f, 6.f, 2.f);

	if (m_Annotation.Primitive() == XAnnotation::pText) {
		g.drawSingleLineText(juce::String("Text"), (int)P0.X + 5, (int)P0.Y);
		return;
	}
	if (m_Annotation.Primitive() == XAnnotation::pRect) {
		XPt2D P1 = m_Annotation.Pt(2);	// XAnnotation renvoit systematiquement 4 points
		Ground2Pixel(P1.X, P1.Y);
		P1 += XPt2D(deltaX, deltaY);
		g.drawEllipse((float)P1.X - 3.f, (float)P1.Y - 3.f, 6.f, 6.f, 2.f);
		g.drawRect(juce::Rectangle<float>(juce::Point<float>((float)P0.X, (float)P0.Y), juce::Point<float>((float)P1.X, (float)P1.Y)));
		return;
	}
	if (m_Annotation.NbPt() < 2)
		return;
	juce::Path path;
	path.startNewSubPath((float)P0.X, (float)P0.Y);
	for (uint32_t i = 0; i < m_Annotation.NbPt(); i++) {
		XPt2D Pi = m_Annotation.Pt(i);
		Ground2Pixel(Pi.X, Pi.Y);
		Pi += XPt2D(deltaX, deltaY);
		path.lineTo((float)Pi.X, (float)Pi.Y);
	}
	g.strokePath(path, juce::PathStrokeType(2, juce::PathStrokeType::beveled));
}