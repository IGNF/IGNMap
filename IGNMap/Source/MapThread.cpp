//-----------------------------------------------------------------------------
//								MapThread.cpp
//								=============
//
// Thread de dessin des objets geographiques
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 06/01/2023
//-----------------------------------------------------------------------------

#include "MapThread.h"
#include "../../XTool/XGeoBase.h"
#include "../../XTool/XGeoClass.h"
#include "../../XTool/XGeoVector.h"
#include "../../XTool/XGeoPoint.h"
#include "../../XTool/XGeoLine.h"
#include "../../XTool/XGeoPoly.h"
#include "DtmShader.h"
#include "LasShader.h"

//==============================================================================
// Constructeur
//==============================================================================
MapThread::MapThread(const juce::String& threadName, size_t threadStackSize) : juce::Thread(threadName, threadStackSize)
{
	m_bFill = false;
	m_nNbPathPt = 0;
	m_GeoBase = nullptr;
	m_nNumObjects = 0;
	m_dX0 = m_dY0 = 0.;
	m_dGsd = 1.0;
	m_bRaster = m_bVector = m_bOverlay = m_bDtm = m_bLas = m_bRasterDone = m_bFirstRaster = false;
	m_bRasterCompleted = m_bVectorCompleted = m_bLasCompleted = false;
}

//==============================================================================
// Allocation des points pour le dessin vectoriel
//==============================================================================
bool MapThread::AllocPoints(int numPt)
{
	if (numPt > m_nNbPathPt) {
		m_Path.preallocateSpace(3 * numPt + 1);
		m_nNbPathPt = numPt;
	}
	return true;
}

//==============================================================================
// Fixe les dimensions des images servant au dessin
//==============================================================================
void MapThread::SetDimension(const int& w, const int& h)
{
	if ((w != m_Vector.getWidth()) || (h != m_Vector.getHeight())) {
		m_Vector = juce::Image(juce::Image::PixelFormat::ARGB, w, h, true);
		m_Raster = juce::Image(juce::Image::PixelFormat::ARGB, w, h, true);
		m_Raster.clear(m_Raster.getBounds(), juce::Colour(0xFFFFFFFF));
		m_Overlay = juce::Image(juce::Image::PixelFormat::ARGB, w, h, true);
		m_Dtm = juce::Image(juce::Image::PixelFormat::ARGB, w, h, true);
		//m_Dtm.clear(m_Dtm.getBounds(), juce::Colour(0xFFFFFFFF));
		m_RawDtm = juce::Image(juce::Image::PixelFormat::ARGB, w, h, true);
		m_Las = juce::Image(juce::Image::PixelFormat::ARGB, w, h, true);
		m_bRasterDone = false;
		m_ClipLas = m_ClipRaster = m_ClipVector = juce::Rectangle<int>();
	}
}

//==============================================================================
// Indique quels elements sont a mettre à jour
//==============================================================================
void MapThread::SetUpdate(bool overlay, bool raster, bool dtm, bool vector, bool las)
{
	m_bRaster = raster;
	m_bVector = vector;
	m_bOverlay = overlay;
	m_bDtm = dtm;
	m_bLas = las;
}

//==============================================================================
// Preparation des images servant a l'affichage
//==============================================================================
void MapThread::PrepareImages(bool totalUpdate, int dX, int dY)
{
	if (m_bRaster) {
		m_ClipRaster = juce::Rectangle<int>();
		if (totalUpdate) {
			m_Raster.clear(m_Raster.getBounds(), juce::Colour(0xFFFFFFFF));
			//m_Raster.clear(m_Raster.getBounds());
			m_bRasterDone = false;
		}
		else {
			//m_Raster.moveImageSection(dX, dY, 0, 0, m_Raster.getWidth() - dX, m_Raster.getHeight() - dY);
			juce::Image tmpImage = juce::Image(juce::Image::PixelFormat::ARGB, m_Raster.getWidth(), m_Raster.getHeight(), true);
			//m_Raster.clear(m_Raster.getBounds(), juce::Colour(0xFFFFFFFF));
			juce::Graphics g(tmpImage);
			g.drawImageAt(m_Raster, dX, dY);
			m_Raster = tmpImage;
			m_ClipRaster = juce::Rectangle<int>(dX, dY, m_Raster.getWidth(), m_Raster.getHeight());
			if (!m_bRasterCompleted) m_ClipRaster = juce::Rectangle<int>();
		}
	}
	if (m_bDtm) {
		m_Dtm.clear(m_Dtm.getBounds());// , juce::Colour(0xFFFFFFFF));
		m_RawDtm.clear(m_RawDtm.getBounds());
		m_bRasterDone = false;
	}
	if (m_bLas) {
		m_ClipLas = juce::Rectangle<int>();
		if (totalUpdate)
			m_Las.clear(m_Las.getBounds());
		else {
			juce::Image tmpImage = juce::Image(juce::Image::PixelFormat::ARGB, m_Las.getWidth(), m_Las.getHeight(), true);
			juce::Graphics g(tmpImage);
			g.drawImageAt(m_Las, dX, dY);
			m_Las = tmpImage;
			m_ClipLas = juce::Rectangle<int>(dX, dY, m_Las.getWidth(), m_Las.getHeight());
			if (!m_bLasCompleted) m_ClipLas = juce::Rectangle<int>();
		}
	}
	if (m_bOverlay)
		m_Overlay.clear(m_Overlay.getBounds());

	if (m_bVector) {
		m_ClipVector = juce::Rectangle<int>();
		if (totalUpdate)
			m_Vector.clear(m_Vector.getBounds());
		else {
			juce::Image tmpImage = juce::Image(juce::Image::PixelFormat::ARGB, m_Vector.getWidth(), m_Vector.getHeight(), true);
			juce::Graphics g(tmpImage);
			g.drawImageAt(m_Vector, dX, dY);
			m_Vector = tmpImage;
			m_ClipVector = juce::Rectangle<int>(dX, dY, m_Vector.getWidth(), m_Vector.getHeight());
			if (!m_bVectorCompleted) m_ClipVector = juce::Rectangle<int>();
		}
	}
}

//==============================================================================
// Fixe l'emprise et la resolution de la vue
//==============================================================================
void MapThread::SetWorld(const double& X0, const double& Y0, const double& gsd, const int& W, const int& H, bool force_vector)
{
	bool totalUpdate = force_vector;
	if (gsd != m_dGsd) totalUpdate = true;
	if ((W != m_Vector.getWidth()) || (H != m_Vector.getHeight())) totalUpdate = true;
	int dX = (int)round((m_dX0 - X0) / m_dGsd), dY = (int)round((Y0 - m_dY0) / m_dGsd);
	PrepareImages(totalUpdate, dX, dY);

	m_dX0 = X0;
	m_dY0 = Y0;
	m_dGsd = gsd;
	SetDimension(W, H);
	m_Frame = XFrame();
	m_Frame += XPt2D(m_dX0, m_dY0);
	m_Frame += XPt2D(m_dX0 + W * m_dGsd, m_dY0 - H * m_dGsd);
	m_Path.clear();
}

//==============================================================================
// Methode run du thread
//==============================================================================
void MapThread::run()
{
	m_nNumObjects = 0;
	if (m_GeoBase == nullptr)
		return;
	// Affichage des couches raster
	if (m_bRaster) {
		m_bRasterDone = m_bRasterCompleted = false;
		m_bFirstRaster = true;
		for (uint32_t i = 0; i < m_GeoBase->NbClass(); i++) {
			XGeoClass* C = m_GeoBase->Class(i);
			if (C == nullptr)
				continue;
			if (!C->IsRaster())
				continue;
			if (C->Visible())
				m_bRasterCompleted |= DrawRasterClass(C);
		}
	}
	// Affichage des couches MNT
	if (m_bDtm) {
		m_bRasterDone = false;
		bool flag = false;
		for (uint32_t i = 0; i < m_GeoBase->NbClass(); i++) {
			XGeoClass* C = m_GeoBase->Class(i);
			if (C == nullptr)
				continue;
			if (!C->IsDTM())
				continue;
			if (C->Visible())
				flag |= DrawDtmClass(C);
		}
		if (flag) {
			DtmShader shader(m_dGsd);
			shader.ConvertImage(&m_RawDtm, &m_Dtm);
		}
	}
	m_bRasterDone = true;
	// Affichage des LAS
	if (m_bLas) {
		m_bLasCompleted = false;
		for (uint32_t i = 0; i < m_GeoBase->NbClass(); i++) {
			XGeoClass* C = m_GeoBase->Class(i);
			if (C == nullptr)
				continue;
			if (!C->IsLAS())
				continue;
			if (C->Visible())
				m_bLasCompleted != DrawLasClass(C);
		}
	}
	// Affichage des couches vectorielles
	if (m_bVector) {
		//m_Vector.clear(m_Vector.getBounds());
		m_bVectorCompleted = false;
		for (uint32_t i = 0; i < m_GeoBase->NbClass(); i++) {
			XGeoClass* C = m_GeoBase->Class(i);
			if (C == nullptr)
				continue;
			if (!C->IsVector())
				continue;
			if (!C->Visible())
				continue;
			m_bVectorCompleted |= DrawVectorClass(C);
		}
	}
	// Affichage de la selection
	if (m_bOverlay)
		DrawSelection();
	m_bRaster = m_bVector = m_bOverlay = false;
}

bool MapThread::Draw(juce::Graphics& g, int x0, int y0, bool overlay)
{
	if (!m_bRasterDone)
		return false;
	// Dessin du fond blanc
	g.setOpacity(1.f);
	g.fillAll(juce::Colours::white);

	g.drawImageAt(m_Raster, x0, y0); // Dessin du raster
	if (DtmShader::m_dOpacity > 0.) {	// Le MNT est affiche sur le raster
		g.setOpacity((float)DtmShader::m_dOpacity * 0.01f);
		g.drawImageAt(m_Dtm, x0, y0);
	}
	if (LasShader::Opacity() > 0.) {	// Dessin des LAS
		g.setOpacity((float)LasShader::Opacity() * 0.01f);
		g.drawImageAt(m_Las, x0, y0);
	}
	g.drawImageAt(m_Vector, x0, y0);
	if (overlay)
		g.drawImageAt(m_Overlay, x0, y0);
	return true;
}

//==============================================================================
// Dessin des classes vectorielles
//==============================================================================
bool MapThread::DrawVectorClass(XGeoClass* C)
{
	if (!m_Frame.Intersect(C->Frame()))
		return true;
	int index = 0;
	do {
		const juce::MessageManagerLock mml(Thread::getCurrentThread());
		if (!mml.lockWasGained())  // if something is trying to kill this job, the lock
			return false;
		juce::Graphics g(m_Vector);
		g.excludeClipRegion(m_ClipVector);

		for (int i = 0; i < 1000; i++) {
			if (threadShouldExit())
				return false;
			XGeoVector* V = C->Vector(index);
			index++;
			if (V == nullptr)
				return false;
			if (!V->Visible())
				continue;
			XFrame F = V->Frame();
			if (!m_Frame.Intersect(F))
				continue;
			XGeoRepres* R = V->Repres();
			if (R == nullptr)
				continue;

			m_bFill = false;
			if (V->IsClosed())
				m_bFill = true;
			
			g.setColour(juce::Colour(R->Color()));


			juce::Rectangle<int> frame = juce::Rectangle<int>((int)round((F.Xmin - m_dX0) / m_dGsd), (int)round((m_dY0 - F.Ymax) / m_dGsd),
				(int)round(F.Width() / m_dGsd), (int)round(F.Height() / m_dGsd));
			if (!m_ClipVector.contains(frame)) {
				if ((frame.getWidth() < 2) && (frame.getHeight() < 2) && (V->NbPt() > 1)) {
					g.drawRect(frame, 2);
				}
				else {
					if (!DrawCentroide(V))
						DrawGeometry(V);
					g.strokePath(m_Path, juce::PathStrokeType(R->Size(), juce::PathStrokeType::beveled));
					if (m_bFill) {
						g.setFillType(juce::FillType(juce::Colour(R->FillColor())));
						g.fillPath(m_Path);
					}
					m_Path.clear();
					DrawText(&g, V);
				}
			}

			m_nNumObjects++;
		}
	} while (!threadShouldExit());
	return true;
}

//==============================================================================
// Dessin des geometries vectorielles
//==============================================================================
bool MapThread::DrawGeometry(XGeoVector* V)
{
	if (!V->LoadGeom2D()) {
		V->Unload();
		return false;
	}
	if (!AllocPoints(V->NbPt())) {
		V->Unload();
		return false;
	}

	bool flag = false;
	switch (V->TypeVector()) {
	case XGeoVector::Point:
		flag = DrawPoint((XGeoPoint2D*)V);
	break;
	case XGeoVector::PointZ:
		flag = DrawPoint((XGeoPoint2D*)V);
	break;
	case XGeoVector::MPoint:
		flag = DrawMultiPoint(V);
	break;
	case XGeoVector::MPointZ:
		flag = DrawMultiPoint(V);
	break;

	case XGeoVector::Line:
		flag = DrawPolyline(V);
	break;
	case XGeoVector::LineZ:
		flag = DrawPolyline(V);
	break;
	case XGeoVector::MLine:
		flag = DrawMultiLine(V);
	break;
	case XGeoVector::MLineZ:
		flag = DrawMultiLine(V);
	break;

	case XGeoVector::Poly:
		flag = DrawPolygon(V);
	break;
	case XGeoVector::PolyZ:
		flag = DrawPolygon(V);
	break;
	case XGeoVector::MPoly:
		flag = DrawMultiPolygon(V);
	break;
	case XGeoVector::MPolyZ:
		flag = DrawMultiPolygon(V);
	break;
		default: flag = false;
	}
	V->Unload();
	return flag;
}

//==============================================================================
// Dessin des textes associes aux geometries
//==============================================================================
bool MapThread::DrawText(juce::Graphics* g, XGeoVector* V)
{
	if (V->Name().empty())
		return false;
	XPt2D P = V->Frame().Center();
	int X = (int)((P.X - m_dX0) / m_dGsd) + 5;
	int Y = (int)((m_dY0 - P.Y) / m_dGsd) - 5;
	g->drawSingleLineText(V->Name(), X, Y);
	return true;
}

//==============================================================================
// Dessin des centroides
//==============================================================================
bool MapThread::DrawCentroide(XGeoVector* G)
{
	if (G->HasCentroide()) {
		XPt2D P = G->Centroide();
		float X = (float)((P.X - m_dX0) / m_dGsd);
		float Y = (float)((m_dY0 - P.Y) / m_dGsd);
		m_Path.startNewSubPath(X, Y);
		m_Path.addStar(juce::Point<float>(X, Y), 4, 5.f, 10.f);
		return true;
	}
	return false;
}

//==============================================================================
// Dessin des ponctuels
//==============================================================================
bool MapThread::DrawPoint(XGeoVector* G)
{
	XPt2D P = G->Pt(0);
	float X = (float)((P.X - m_dX0) / m_dGsd);
	float Y = (float)((m_dY0 - P.Y) / m_dGsd);
	float d = 3.f;
	m_Path.startNewSubPath(X, Y);
	m_Path.addEllipse(X - d, Y - d, 2 * d, 2 * d);
	return true;
}

//-----------------------------------------------------------------------------
// Dessin des polylignes
//-----------------------------------------------------------------------------
bool MapThread::DrawPolyline(XGeoVector* G)
{
	if (G->NbPt() < 2)
		return false;
	XPt* P = G->Pt();
	if (P == nullptr)
		return false;
	float X = (float)((P[0].X - m_dX0) / m_dGsd), Y = (float)((m_dY0 - P[0].Y) / m_dGsd), Xi = 0.f, Yi = 0.f;
	m_Path.startNewSubPath(X , Y);
	for (uint32_t i = 1; i < G->NbPt(); i++) {
		Xi = (float)((P[i].X - m_dX0) / m_dGsd);
		Yi = (float)((m_dY0 - P[i].Y) / m_dGsd);
		if ((fabs(Xi - X) + fabs(Yi - Y)) >= 1.f) {
			m_Path.lineTo(Xi, Yi);
			X = Xi; Y = Yi;
		}
	}
	return true;
}

//==============================================================================
// Dessin des polygones
//==============================================================================
bool MapThread::DrawPolygon(XGeoVector* G)
{
	if (!DrawPolyline(G))
		return false;
	m_Path.closeSubPath();
	return true;
}

//==============================================================================
// Dessin des multi-lignes
//==============================================================================
bool MapThread::DrawMultiLine(XGeoVector* G)
{
	if (G->NbPt() < 2)
		return false;
	XPt* P = G->Pt();
	int* Parts = G->Parts();
	if ((P == nullptr) || (Parts == nullptr))
		return false;

	float X = (float)((P[0].X - m_dX0) / m_dGsd), Y = (float)((m_dY0 - P[0].Y) / m_dGsd), Xi = 0.f, Yi = 0.f;
	m_Path.startNewSubPath(X, Y);
	bool new_ring = false;
	for (uint32_t i = 1; i < G->NbPt(); i++) {
		for (uint32_t j = 1; j < G->NbPart(); j++) {
			if (i == G->Part(j))
				new_ring = true;
		}
		if (new_ring) {
			X = (float)((P[i].X - m_dX0) / m_dGsd);
			Y = (float)((m_dY0 - P[i].Y) / m_dGsd);
			m_Path.startNewSubPath(X, Y);
		} else {
			Xi = (float)((P[i].X - m_dX0) / m_dGsd);
			Yi = (float)((m_dY0 - P[i].Y) / m_dGsd);
			if ((fabs(Xi - X) + fabs(Yi - Y)) >= 1.f) {
				m_Path.lineTo(Xi, Yi);
				X = Xi; Y = Yi;
			}
		}
		new_ring = false;
	}
	return true;
}

//==============================================================================
// Dessin des multi-polygones
//==============================================================================
bool MapThread::DrawMultiPolygon(XGeoVector* G)
{
	if (G->NbPt() < 2)
		return false;
	XPt* P = G->Pt();
	int* Parts = G->Parts();
	if ((P == nullptr)||(Parts == nullptr))
		return false;
	
	float X = (float)((P[0].X - m_dX0) / m_dGsd), Y = (float)((m_dY0 - P[0].Y) / m_dGsd), Xi = 0.f, Yi = 0.f;
	m_Path.startNewSubPath(X, Y);
	bool new_ring = false;
	for (uint32_t i = 1; i < G->NbPt(); i++) {
		for (uint32_t j = 1; j < G->NbPart(); j++) {
			if (i == G->Part(j))
				new_ring = true;
		}
		if (new_ring) {
			m_Path.closeSubPath();
			X = (float)((P[i].X - m_dX0) / m_dGsd);
			Y = (float)((m_dY0 - P[i].Y) / m_dGsd);
			m_Path.startNewSubPath(X, Y);
		} else {
			Xi = (float)((P[i].X - m_dX0) / m_dGsd);
			Yi = (float)((m_dY0 - P[i].Y) / m_dGsd);
			if ((fabs(Xi - X) + fabs(Yi - Y)) >= 1.f) {
				m_Path.lineTo(Xi, Yi);
				X = Xi; Y = Yi;
			}
		}
		new_ring = false;
	}
	m_Path.closeSubPath();
	return true;
}

//==============================================================================
// Dessin des multi-points
//==============================================================================
bool MapThread::DrawMultiPoint(XGeoVector* G)
{
	XPt* P = G->Pt();
	XPt M, N;
	float d = 3.f;
	M.X = (P[0].X - m_dX0) / m_dGsd;
	M.Y = (m_dY0 - P[0].Y) / m_dGsd;
	m_Path.startNewSubPath((float)M.X, (float)M.Y);
	m_Path.addEllipse((float)M.X - d, (float)M.Y - d, 2 * d, 2 * d);
	for (uint32_t i = 1; i < G->NbPt(); i++) {
		N.X = (P[i].X - m_dX0) / m_dGsd;
		N.Y = (m_dY0 - P[i].Y) / m_dGsd;
		if ((fabs(N.X - M.X) < 2.) && (fabs(N.Y - M.Y) < 2.))
			continue;
		m_Path.startNewSubPath((float)N.X, (float)N.Y);
		m_Path.addEllipse((float)N.X - d, (float)N.Y - d, 2 * d, 2 * d);
		M = N;
	}
	return true;
}

//==============================================================================
// Dessin de la selection
//==============================================================================
void MapThread::DrawSelection()
{
	const juce::MessageManagerLock mml(Thread::getCurrentThread());
	if (!mml.lockWasGained())
		return;
	juce::Graphics g(m_Overlay);
	juce::Rectangle<int> clipR = juce::Rectangle<int>(0, 0, m_Overlay.getWidth(), m_Overlay.getHeight());

	for (uint32_t i = 0; i < m_GeoBase->NbSelection(); i++) {
		m_Path.clear();
		XGeoVector* V = m_GeoBase->Selection(i);
		XFrame F = V->Frame();
		int W = (int)round(F.Width() / m_dGsd);
		int H = (int)round(F.Height() / m_dGsd);

		if (V->TypeVector() == XGeoVector::DTM) {
			g.setColour(juce::Colours::darkolivegreen);
			g.drawRect((int)floor((F.Xmin - m_dX0) / m_dGsd), (int)floor((m_dY0 - F.Ymax) / m_dGsd), W, H, 4);
			continue;
		}
		if (V->TypeVector() == XGeoVector::Raster) {
			g.setColour(juce::Colours::cornflowerblue);
			g.drawRect((int)floor((F.Xmin - m_dX0) / m_dGsd), (int)floor((m_dY0 - F.Ymax) / m_dGsd), W, H, 4);
			continue;
		}
		if (V->TypeVector() == XGeoVector::LAS) {
			g.setColour(juce::Colours::mediumvioletred);
			g.drawRect((int)floor((F.Xmin - m_dX0) / m_dGsd), (int)floor((m_dY0 - F.Ymax) / m_dGsd), W, H, 4);
			continue;
		}

		if (m_Frame.Intersect(F)) {
			if ((W < 25) && (H < 25)) {
				g.setColour(juce::Colours::black);
				g.drawRect((int)floor((F.Xmin - m_dX0) / m_dGsd) - 3, (int)floor((m_dY0 - F.Ymax) / m_dGsd) - 3, W + 6, H + 6);
				g.setColour(juce::Colours::white);
				g.drawRect((int)floor((F.Xmin - m_dX0) / m_dGsd) - 2, (int)floor((m_dY0 - F.Ymax) / m_dGsd) - 2, W + 4, H + 4);
			}
			else {
				DrawGeometry(V);
				if (V->HasCentroide()) {
					g.setColour(juce::Colour(V->Repres()->Color()));
					g.strokePath(m_Path, juce::PathStrokeType(V->Repres()->Size(), juce::PathStrokeType::beveled));
				}
				juce::Path::Iterator iter(m_Path);
				int numPoint = -1;
				if (!m_bFill)
					numPoint = 0;
				bool needText = true;
				float dim = std::min<float>(std::max<float>((float)(4.f / m_dGsd), 2.f), 4.f);
				if ((W < 100) && (H < 100))
					needText = false;
				float last_text_X = 0., last_text_Y = 0.;
				while (iter.next()) {
					numPoint++;
					if (!clipR.contains((int)iter.x1, (int)iter.y1))
						continue;
					g.setColour(juce::Colours::black);
					g.drawRect(iter.x1 - dim, iter.y1 - dim, 2.f * dim, 2.f * dim);
					g.setColour(juce::Colours::white);
					g.drawRect(iter.x1 - dim + 1, iter.y1 - dim + 1, 2.f * dim - 2, 2.f * dim - 2);
					if (needText) {
						if ((fabs(last_text_X - iter.x1) < 10) && (fabs(last_text_Y - iter.y1) < 10))
							continue;
						if ((!m_bFill) || (iter.elementType != juce::Path::Iterator::startNewSubPath)) {
							g.drawSingleLineText(juce::String(numPoint), (int)iter.x1 + 4, (int)iter.y1);
							g.drawSingleLineText(juce::String(numPoint), (int)iter.x1 + 6, (int)iter.y1);
							g.drawSingleLineText(juce::String(numPoint), (int)iter.x1 + 5, (int)iter.y1 + 1);
							g.drawSingleLineText(juce::String(numPoint), (int)iter.x1 + 5, (int)iter.y1 - 1);
							g.setColour(juce::Colours::black);
							g.drawSingleLineText(juce::String(numPoint), (int)iter.x1 + 5, (int)iter.y1);
							last_text_X = iter.x1;
							last_text_Y = iter.y1;
						}
					}
				}
			}
		}

		if (threadShouldExit()) 
			return;
	}
}

//==============================================================================
// Dessin des classes raster
//==============================================================================
bool MapThread::DrawRasterClass(XGeoClass* C)
{
	if (!m_Frame.Intersect(C->Frame()))
		return false;
	bool flag = false;
	for (uint32_t i = 0; i < C->NbVector(); i++) {
		if (threadShouldExit())
			return false;
		GeoImage* image = dynamic_cast<GeoImage*>(C->Vector(i));
		if (image == nullptr)
			continue;
		if (!image->Visible())
			continue;
		if (!m_Frame.Intersect(image->Frame()))
			continue;
		const juce::MessageManagerLock mml(Thread::getCurrentThread());
		if (!mml.lockWasGained())  // if something is trying to kill this job, the lock
			continue;
		XFileImage* fileImage = dynamic_cast<XFileImage*>(image);
		if (fileImage != nullptr)
			flag |= DrawFileRaster(fileImage, image->Repres());
		GeoInternetImage* internetImage = dynamic_cast<GeoInternetImage*>(image);
		if (internetImage != nullptr)
			flag |= DrawInternetRaster(internetImage);
	}
	return flag;
}

//==============================================================================
// Dessin d'une image provenant d'un fichier
//==============================================================================
bool MapThread::DrawFileRaster(XFileImage* image, XGeoRepres* repres)
{
	int U0, V0, win, hin, R0, S0, wout, hout, nbBand;
	if (!image->PrepareRasterDraw(&m_Frame, m_Frame.Width() / m_Raster.getWidth(), U0, V0, win, hin, nbBand, R0, S0, wout, hout))
		return false;
	juce::Rectangle<int> destRect(R0, S0, wout, hout);
	if (m_ClipRaster.contains(destRect))
		return true;

	int factor = win / wout;
	if (factor < 1)
		factor = 1;
	int wtmp = win / factor, htmp = hin / factor;
	if ((wtmp == 0) || (htmp == 0))
		return true;
	juce::Image::PixelFormat format = juce::Image::PixelFormat::RGB;
	float opacity = 1.0f;
	uint8_t r = 0, g = 0, b = 0, alpha = 255;
	if (repres != nullptr) {
		opacity = 1.0f - repres->Transparency() / 100.0f;
		repres->FillColor(r, g, b, alpha);
		if (alpha != 255)
			format = juce::Image::PixelFormat::ARGB;
	}
	juce::Image tmpImage(format, wtmp, htmp, true);
	{ // Necessaire pour que bitmap soit detruit avant l'appel a drawImageAt
		juce::Image::BitmapData bitmap(tmpImage, juce::Image::BitmapData::readWrite);
		format = bitmap.pixelFormat;	// Sur Mac, on obtient toujours ARGB meme en demandant RGB !

		if (factor == 1)
			image->GetArea(U0, V0, win, hin, bitmap.data);
		else
			image->GetZoomArea(U0, V0, win, hin, bitmap.data, factor);

		if (format == juce::Image::PixelFormat::RGB) {
			if (nbBand == 1)
				XBaseImage::Gray2RGB(bitmap.data, wtmp * htmp);
			else
				XBaseImage::SwitchRGB2BGR(bitmap.data, wtmp * htmp);
			XBaseImage::OffsetArea(bitmap.data, wtmp * 3, bitmap.height, bitmap.lineStride);
		}
		else {
			if (nbBand == 1)
				XBaseImage::Gray2RGBA(bitmap.data, wtmp * htmp, r, alpha);
			else
				XBaseImage::RGB2BGRA(bitmap.data, wtmp * htmp, r, g, b, alpha);
			XBaseImage::OffsetArea(bitmap.data, wtmp * 4, bitmap.height, bitmap.lineStride);
		}
	}

	if (m_bFirstRaster) {	// Nettoyage pour la premiere couche raster a afficher
		//m_Raster.clear(m_Raster.getBounds(), juce::Colour(0xFFFFFFFF));
		m_bFirstRaster = false;
	}
	juce::Graphics graphic(m_Raster);
	graphic.setOpacity(opacity);
	graphic.drawImage(tmpImage, R0, S0, wout, hout, 0, 0, wtmp, htmp);
	m_nNumObjects++;
	return true;
}

//==============================================================================
// Dessin d'une image provenant d'un flux internet
//==============================================================================
bool MapThread::DrawInternetRaster(GeoInternetImage* image)
{
	juce::Image tmpImage = image->GetAreaImage(m_Frame, m_dGsd);
	if (tmpImage.isNull())
		return false;
	float opacity = 1.0f;
	XGeoRepres* repres = image->Repres();
	if (repres != nullptr)
		opacity = 1.0f - repres->Transparency() / 100.0f;

	if (m_bFirstRaster) { // Nettoyage pour la premiere couche raster a afficher
		m_Raster.clear(m_Raster.getBounds(), juce::Colour(0xFFFFFFFF));
		m_ClipRaster = juce::Rectangle<int>();
		m_bFirstRaster = false;
	}
	juce::Graphics graphic(m_Raster);
	graphic.setOpacity(opacity);
	graphic.drawImageAt(tmpImage, 0, 0);
	m_nNumObjects++;
	return true;
}

//==============================================================================
// Dessin des classes MNT
//==============================================================================
bool MapThread::DrawDtmClass(XGeoClass* C)
{
	if (!m_Frame.Intersect(C->Frame()))
		return true;
	const juce::MessageManagerLock mml(Thread::getCurrentThread());
	if (!mml.lockWasGained())  // if something is trying to kill this job, the lock
		return false;
	bool flag = false;
	for (uint32_t i = 0; i < C->NbVector(); i++) {
		GeoDTM* dtm = (GeoDTM*)C->Vector(i);
		if (!dtm->Visible())
			continue;
		if (m_Frame.Intersect(dtm->Frame()))
			flag |= DrawDtm(dtm);
		if (threadShouldExit())
			return false;
	}
	return flag;
}

//==============================================================================
// Dessin d'un MNT
//==============================================================================
bool MapThread::DrawDtm(GeoDTM* dtm)
{
	XFileImage image;
	if (!image.AnalyzeImage(dtm->ImageName()))
		return false;
	if (image.NbSample() != 1)
		return false;
	double gsd = dtm->Resolution();
	image.SetGeoref(dtm->Frame().Xmin - gsd * 0.5, dtm->Frame().Ymax + gsd * 0.5, gsd);
	int U0, V0, win, hin, R0, S0, wout, hout, nbBand;
	if (!image.PrepareRasterDraw(&m_Frame, m_Frame.Width() / m_Raster.getWidth(), U0, V0, win, hin, nbBand, R0, S0, wout, hout))
		return false;
	
	int factor = win / wout;
	if (factor < 1)
		factor = 1;
	int wtmp = win / factor, htmp = hin / factor;
	if ((wtmp == 0) || (htmp == 0))
		return true;
	
	float* area = new float[wtmp * htmp];
	if (area == nullptr)
		return false;
	uint32_t nb_sample;
	image.GetRawArea(U0, V0, win, hin, area, &nb_sample, factor);
	juce::Image tmpImage(m_RawDtm.getFormat(), wout, hout, true);
	{ // Necessaire pour que bitmap soit detruit avant l'appel a drawImageAt
		juce::Image::BitmapData bitmap(tmpImage, juce::Image::BitmapData::readWrite);
		XBaseImage::FastZoomBil(area, wtmp, htmp, (float*)bitmap.data, wout, hout);
		delete[] area;
		XBaseImage::OffsetArea(bitmap.data, wout * 4, bitmap.height, bitmap.lineStride);
	}

	m_RawDtm.clear(juce::Rectangle<int>(R0, S0, wout, hout));
	juce::Graphics g(m_RawDtm);
	g.setOpacity(1.f);
	g.drawImageAt(tmpImage, R0, S0);

	m_nNumObjects++;
	return true;
}

//==============================================================================
// Recupere le Z a une position image
//==============================================================================
float MapThread::GetZ(int u, int v)
{
	if ((u < 0) || (v < 0))
		return 0.;
	if ((u >= m_RawDtm.getWidth()) || (v >= m_RawDtm.getHeight()))
		return 0.;
	juce::Image::BitmapData bitmap(m_RawDtm, juce::Image::BitmapData::readOnly);
	float* z = (float*)&bitmap.data[v * bitmap.lineStride + u * bitmap.pixelStride];
	if (z == nullptr)
		return 0.;
	return *z;
}

//==============================================================================
// Dessin d'une classe LAS
//==============================================================================
bool MapThread::DrawLasClass(XGeoClass* C)
{
	if (!m_Frame.Intersect(C->Frame()))
		return true;
	const juce::MessageManagerLock mml(Thread::getCurrentThread());
	if (!mml.lockWasGained())  // if something is trying to kill this job, the lock
		return false;
	bool flag = false;
	for (uint32_t i = 0; i < C->NbVector(); i++) {
		GeoLAS* las = (GeoLAS*)C->Vector(i);
		if (!las->Visible())
			continue;
		XFrame F = las->Frame();
		if (!m_Frame.Intersect(F))
			continue;
		juce::Rectangle<int> frame = juce::Rectangle<int>((int)round((F.Xmin - m_dX0) / m_dGsd), (int)round((m_dY0 - F.Ymax) / m_dGsd),
			(int)round(F.Width() / m_dGsd), (int)round(F.Height() / m_dGsd));
		if (!m_ClipLas.contains(frame))
			flag |= DrawLas(las);
		if (threadShouldExit())
			return false;
	}
	return flag;
}

//==============================================================================
// Dessin d'un LAS
//==============================================================================
bool MapThread::DrawLas(GeoLAS* las)
{
	double Z0 = LasShader::Zmin();// m_GeoBase->ZMin();
	double deltaZ = LasShader::Zmax() - Z0; // m_GeoBase->ZMax() - Z0;
	if (deltaZ <= 0) deltaZ = 1.;	// Pour eviter les divisions par 0
	deltaZ = (255. / deltaZ);

	if (m_dGsd > LasShader::MaxGsd()) {
		XFrame F = las->Frame();
		float W = (float)round(F.Width() / m_dGsd);
		float H = (float)round(F.Height() / m_dGsd);
		juce::Graphics g(m_Las);
		/*
		g.setColour(juce::Colours::lightpink);
		g.fillRect((int)floor((F.Xmin - m_dX0) / m_dGsd), (int)floor((m_dY0 - F.Ymax) / m_dGsd), W, H);*/
		float x1 = (float)floor((F.Xmin - m_dX0) / m_dGsd), y1 = (float)floor((m_dY0 - F.Ymax) / m_dGsd);
		juce::ColourGradient gradient(LasShader::AltiColor((uint8_t)((las->Zmax() - Z0) * deltaZ)), x1 + W / 2, y1 + H / 2,
			LasShader::AltiColor((uint8_t)((las->Zmin() - Z0) * deltaZ)), x1 + W, y1 + H, true);
		g.setGradientFill(gradient);
		g.fillRect(x1, y1, W, H);
		g.setColour(juce::Colours::mediumvioletred);
		g.drawRect(x1, y1, W, H);
		m_nNumObjects++;
		return true;
	}

	juce::Image::BitmapData bitmap(m_Las, juce::Image::BitmapData::readWrite);

	if (!las->ReOpen())
		return false;
	if (!las->SetWorld(m_Frame, LasShader::Zmin(), LasShader::Zmax(), m_dGsd))
		return false;
	laszip_point* point = las->GetPoint();

	double X, Y, Z;
	juce::Colour col = juce::Colours::orchid;
	uint8_t data[4] = { 0, 0, 0, 255 };
	uint32_t* data_ptr = (uint32_t*)&data;
	uint8_t* ptr = nullptr;
	uint8_t classification;
	bool classif_newtype = las->IsNewClassification(), end_flag = true;

	while(las->GetNextPoint(&X, &Y, &Z)) {
		if (classif_newtype)
			classification = point->extended_classification;
		else
			classification = point->classification;
		if (!LasShader::ClassificationVisibility(classification)) continue;
		X = (X - m_dX0) / m_dGsd;
		Y = (m_dY0 - Y) / m_dGsd;
		if (m_ClipLas.contains((int)X, (int)Y))
			continue;
		switch (LasShader::Mode()) {
		case LasShader::ShaderMode::Altitude:
			*data_ptr = LasShader::AltiColorARGB((uint8_t)((Z - Z0) * deltaZ));
			break;
		case LasShader::ShaderMode::RGB:
			data[0] = (uint8_t)(point->rgb[2] / 256);
			data[1] = (uint8_t)(point->rgb[1] / 256);
			data[2] = (uint8_t)(point->rgb[0] / 256);
			// data[3] = 255; // deja fixe dans l'initialisation de data
			break;
		case LasShader::ShaderMode::IRC:
			data[0] = (uint8_t)(point->rgb[1] / 256);
			data[1] = (uint8_t)(point->rgb[0] / 256);
			data[2] = (uint8_t)(point->rgb[3] / 256);
			// data[3] = 255; // deja fixe dans l'initialisation de data
			break;
		case LasShader::ShaderMode::Classification:
			col = LasShader::ClassificationColor(classification);
			*data_ptr = (uint32_t)col.getARGB();
			break;
		case LasShader::ShaderMode::Intensity:
			//data[0] = point->intensity;	// L'intensite est normalisee sur 16 bits
			//memcpy(data, &point->intensity, 2 * sizeof(uint8_t));
			*data_ptr = LasShader::IntensityColorARGB(point->intensity);
			break;
		case LasShader::ShaderMode::Angle:
			if (point->extended_scan_angle < 0) {	// Angle en degree = extended_scan_angle * 0.006
				data[2] = (uint8_t)(255 - point->extended_scan_angle * (-0.0085));	 // Normalise sur [0; 255]
				data[1] = 0;
				data[0] = 0; // (uint8_t)(255 - data[2]);
			}
			else {
				data[2] = 0;
				data[1] = (uint8_t)(255 - point->extended_scan_angle * (0.0085));	 // Normalise sur [0; 255]
				data[0] = 0; // (uint8_t)(255 - data[1]);
			}
			break;
		default: ;
		}

		ptr = bitmap.getPixelPointer((int)X, (int)Y);
		memcpy(ptr, &data, sizeof(uint32_t));
		//bitmap.setPixelColour(X, Y, col);
		//g.drawRect((float)X-1.f, (float)Y-1.f, 2.f, 2.f);
		if (threadShouldExit()) {
			end_flag = false;
			break;
		}
	}
	m_nNumObjects++;
	las->CloseIfNeeded(1);

	return end_flag;
}
