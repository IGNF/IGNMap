//-----------------------------------------------------------------------------
//								SetereoViewer.cpp
//								=================
//
// Visualisation en stereoscopie par ananglyphes
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 03/09/2025
//-----------------------------------------------------------------------------

#include "StereoViewer.h"
#include "../../XTool/XPath.h"
#include "../../XTool/XParserXML.h"
#include "../../XToolGeod/XGeoPref.h"
#include "../../XToolImage/XTiffWriter.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
StereoView::StereoView()
{
	Clear();
	setOpaque(true);
	setWantsKeyboardFocus(true);
}

//-----------------------------------------------------------------------------
// Reinitialisation
//-----------------------------------------------------------------------------
void StereoView::Clear()
{
	m_nFactor = 1;
	m_ViewX = m_ViewY = 0;
	m_nGamma = 0;
	m_bRedLeft = m_bStereoOnly = m_bDirty = true;
	m_bDrag = false;
	m_Restit = false;
	m_OrientationType = NoOrientation;
	m_ViewMode = ViewMode::Anaglyph;
	m_dPara = m_dGSD = 1.0;
	m_nBalColor = 0;
	m_BalColorL = juce::Colours::yellow;
	m_BalColorR = juce::Colours::yellow;
	m_BalShape = Dot;
	m_ImageL.Close();
	m_ImageR.Close();
	m_strOrientationFile = m_strCameraFile = "";
	m_Model = XStereoModel();
	m_BalL = m_BalR = XPt2D();
	m_Bal = XPt3D();
	m_Image.clear(juce::Rectangle<int>(m_Image.getWidth(), m_Image.getHeight()));
}

//-----------------------------------------------------------------------------
// Ouverture d'un fichier projet
//-----------------------------------------------------------------------------
bool StereoView::OpenProject(std::string filename)
{
	XParserXML parser;
	if (!parser.Parse(filename))
		return false;
	XPath path;
	std::string folder = path.Path(filename.c_str());

	std::string left_image = path.Absolute(folder.c_str(), parser.ReadNode("/stereopair_project/left_image").c_str());
	std::string right_image = path.Absolute(folder.c_str(), parser.ReadNode("/stereopair_project/right_image").c_str());
	std::string opk_file = path.Absolute(folder.c_str(), parser.ReadNode("/stereopair_project/opk_file").c_str());
	std::string camera_file = path.Absolute(folder.c_str(), parser.ReadNode("/stereopair_project/camera_file").c_str());
	uint32_t rot_left = parser.ReadNodeAsUInt32("/stereopair_project/left_image_rotation");
	uint32_t rot_right = parser.ReadNodeAsUInt32("/stereopair_project/right_image_rotation");
	OpenImage(left_image, true);
	m_ImageL.SetRotation(rot_left);
	OpenImage(right_image, false);
	m_ImageR.SetRotation(rot_right);
	OpenOrientation(opk_file);
	OpenCamera(camera_file);
	m_ViewX = parser.ReadNodeAsInt("/stereopair_project/viewX");
	m_ViewY = parser.ReadNodeAsInt("/stereopair_project/viewX");
	m_nFactor = parser.ReadNodeAsInt("/stereopair_project/factor");
	if (m_nFactor < 1) m_nFactor = 1;
	int x = parser.ReadNodeAsInt("/stereopair_project/right_ori_X");
	int y = parser.ReadNodeAsInt("/stereopair_project/right_ori_Y");
	m_ImageR.SetOrigin(x, y);

	return true;
}

//-----------------------------------------------------------------------------
// Sauvegarde du projet dans un fichier
//-----------------------------------------------------------------------------
bool StereoView::SaveProject(std::string filename)
{
	XPath path;
	std::string folder = path.Path(filename.c_str());
	std::ofstream out(filename);
	if (!out.good())
		return false;
	out << "<stereopair_project>" << std::endl;
	out << "<left_image> " << path.Relative(folder.c_str(), m_ImageL.Filename().c_str()) << " </left_image>" << std::endl;
	out << "<left_image_rotation> " << m_ImageL.GetRotation() << " </left_image_rotation>" << std::endl;
	out << "<right_image> " << path.Relative(folder.c_str(), m_ImageR.Filename().c_str()) << " </right_image>" << std::endl;
	out << "<right_image_rotation> " << m_ImageL.GetRotation() << " </right_image_rotation>" << std::endl;
	out << "<opk_file> " << path.Relative(folder.c_str(), m_strOrientationFile.c_str()) << " </opk_file>" << std::endl;
	out << "<camera_file> " << path.Relative(folder.c_str(), m_strCameraFile.c_str()) << " </camera_file>" << std::endl;
	out << "<viewX> " << m_ViewX << " </viewX>" << std::endl;
	out << "<viewY> " << m_ViewY << " </viewY>" << std::endl;
	out << "<factor> " << m_nFactor << " </factor>" << std::endl;
	out << "<right_ori_X> " << m_ImageR.OriX() << " </right_ori_X>" << std::endl;
	out << "<right_ori_Y> " << m_ImageR.OriY() << " </right_ori_Y>" << std::endl;

	out << "</stereopair_project>" << std::endl;
	return true;
}

//-----------------------------------------------------------------------------
// Redimmensionnement
//-----------------------------------------------------------------------------
void StereoView::resized()
{
	auto bounds = getLocalBounds();
	m_Image = juce::Image(juce::Image::ARGB, bounds.getWidth(), bounds.getHeight(), true);
	m_bDirty = true;
	grabKeyboardFocus();
}

//-----------------------------------------------------------------------------
// Mise a jour de la vue
//-----------------------------------------------------------------------------
void StereoView::Update()
{
	m_bDirty = true;
	SetBallonnet();
	repaint();
}

//==============================================================================
// Gestion du clavier
//==============================================================================
bool StereoView::keyPressed(const juce::KeyPress& key)
{
	if (key.getModifiers() == juce::ModifierKeys::ctrlModifier) {
		if (juce::String(key.getTextCharacter()).compareIgnoreCase("o")) {
			juce::String filename = AppUtil::OpenFile("ProjectPath", juce::translate("Open Project"), "*.xml");
			if (!filename.isEmpty())
				OpenProject(AppUtil::GetStringFilename(filename));
			return true;
		}
	}

	if ((key.getTextCharacter() == 'W') || (key.getTextCharacter() == 'w')) {
		juce::String filename = AppUtil::OpenFile("RasterPath", juce::translate("Open Left Image"), "*.tif;*.jp2;*.cog");
		if (!filename.isEmpty())
			OpenImage(AppUtil::GetStringFilename(filename), true);
		return true;
	}
	if ((key.getTextCharacter() == 'X') || (key.getTextCharacter() == 'x')) {
		juce::String filename = AppUtil::OpenFile("RasterPath", juce::translate("Open Right Image"), "*.tif;*.jp2;*.cog");
		if (!filename.isEmpty())
			OpenImage(AppUtil::GetStringFilename(filename), false);
		return true;
	}
	if ((key.getTextCharacter() == 'C') || (key.getTextCharacter() == 'c')) {
		juce::String filename = AppUtil::OpenFile("CameraPath", juce::translate("Open Camera File"), "*.xml");
		if (!filename.isEmpty())
			OpenCamera(AppUtil::GetStringFilename(filename));
		return true;
	}
	if ((key.getTextCharacter() == 'V') || (key.getTextCharacter() == 'v')) {
		juce::String filename = AppUtil::OpenFile("CameraPath", juce::translate("Open Orientation File"), "*.opk");
		if (!filename.isEmpty())
			OpenOrientation(AppUtil::GetStringFilename(filename));
		return true;
	}

	int Tx = 0, Ty = 0;
	// Deplacement du ViewPort
	if (key.getKeyCode() == juce::KeyPress::leftKey)
		Tx = -100;
	if (key.getKeyCode() == juce::KeyPress::rightKey)
		Tx = 100;
	if (key.getKeyCode() == juce::KeyPress::upKey)
		Ty = -100;
	if (key.getKeyCode() == juce::KeyPress::downKey)
		Ty = 100;
	if ((Tx != 0) || (Ty != 0)) {
		m_ViewX += Tx;
		m_ViewY += Ty;
		Update();
		return true;
	}

	// Deplacement de l'image droite
	if ((key.getTextCharacter() == 'Q') || (key.getTextCharacter() == 'q'))
		Tx = -1 * m_nFactor;
	if ((key.getTextCharacter() == 'D') || (key.getTextCharacter() == 'd'))
		Tx = 1 * m_nFactor;
	if ((key.getTextCharacter() == 'Z') || (key.getTextCharacter() == 'z'))
		Ty = -1 * m_nFactor;
	if ((key.getTextCharacter() == 'S') || (key.getTextCharacter() == 's'))
		Ty = 1 * m_nFactor;
	if ((Tx != 0) || (Ty != 0)) {
		if (key.getModifiers().isShiftDown()) {
			Tx *= 5;
			Ty *= 5;
		}
		m_ImageR.MoveOrigin(Tx, Ty);
		Update();
		return true;
	}

	// Facteur d'echelle
	if (key.getKeyCode() == juce::KeyPress::pageDownKey) {
		m_nFactor--;
		if (m_nFactor < 1) m_nFactor = 1;
		if (key.getModifiers().isShiftDown()) m_nFactor = 1;
		Update();
		return true;
	}
	if (key.getKeyCode() == juce::KeyPress::pageUpKey) {
		m_nFactor++;
		if (key.getModifiers().isShiftDown()) m_nFactor = 24;
		Update();
		return true;
	}

	// Z du ballonnet
	double dZ = 0.;
	if ((key.getTextCharacter() == 'F') || (key.getTextCharacter() == 'f'))
		dZ = -0.5 * m_dGSD * m_nFactor;
	if ((key.getTextCharacter() == 'R') || (key.getTextCharacter() == 'r'))
		dZ = 0.5 * m_dGSD * m_nFactor;
	if (dZ != 0.) {
		m_Bal.Z += dZ;
		SetBallonnet();
		repaint();
		return true;
	}

	// Mise au niveau du ballonnet
	if ((key.getTextCharacter() == 'E') || (key.getTextCharacter() == 'e')) {
		SetZBallonnet();
		return true;
	}

	// Recherche de l'homologue par correlation
	if ((key.getTextCharacter() == 'A') || (key.getTextCharacter() == 'a')) {
		Correlation();
		return true;
	}

	// Recherche du Z du ballonnet sur la Geoplateforme et mise a niveau
	if ((key.getTextCharacter() == 'T') || (key.getTextCharacter() == 't')) {
		XGeoPref pref;
		pref.Projection(XGeoProjection::Lambert93);
		double lon, lat;
		pref.ConvertDeg(pref.Projection(), XGeoProjection::RGF93, m_Bal.X, m_Bal.Y, lon, lat);
		m_Bal.Z = m_GeoSearch.GetAltitude(lon, lat);
		SetBallonnet();
		SetZBallonnet();
		repaint();
		return true;
	}

	// Choix du mode d'affichage
	if ((key.getTextCharacter() == 'I') || (key.getTextCharacter() == 'i')) {
		m_ViewMode = (m_ViewMode + 1) % 4;
		m_bDirty = true;
		repaint();
		return true;
	}
	if ((key.getTextCharacter() == 'O') || (key.getTextCharacter() == 'o')) {
		m_bStereoOnly = (!m_bStereoOnly);
		m_bDirty = true;
		repaint();
		return true;
	}
	if ((key.getTextCharacter() == 'P') || (key.getTextCharacter() == 'p')) {
		m_bRedLeft = (!m_bRedLeft);
		m_bDirty = true;
		repaint();
		return true;
	}
	if ((key.getTextCharacter() == 'G') || (key.getTextCharacter() == 'g')) {	// Gamma
		m_nGamma = (m_nGamma + 1) % 10;
		m_bDirty = true;
		repaint();
		return true;
	}

	// Forme du ballonnet
	if ((key.getTextCharacter() == 'B') || (key.getTextCharacter() == 'b')) {
		m_BalShape = (BallonnetShape)((m_BalShape + 1) % 6);
		repaint();
		return true;
	}

	// Couleur du ballonnet
	if ((key.getTextCharacter() == 'N') || (key.getTextCharacter() == 'n')) {
		m_nBalColor = (m_nBalColor + 1) % 4;
		switch (m_nBalColor) {
		case 0: m_BalColorL = m_BalColorR = juce::Colours::yellow; break;
		case 1: m_BalColorL = m_BalColorR = juce::Colours::purple; break;
		case 2: m_BalColorL = m_BalColorR = juce::Colours::black; break;
		case 3: m_BalColorL = m_BalColorR = juce::Colours::lightpink; break;
		default:;
		}
		repaint();
		return true;
	}

	// Zoom x 2
	if ((key.getTextCharacter() == 'M') || (key.getTextCharacter() == 'm')) {
		//m_nZoom = m_nZoom % 2 + 1;
		repaint();
		return true;
	}

	// Mode restitution
	if (key.getKeyCode() == juce::KeyPress::F2Key) {
		m_Restit = (!m_Restit);
		if (m_Restit)
			setMouseCursor(juce::MouseCursor(juce::MouseCursor::NoCursor));
		else
			setMouseCursor(juce::MouseCursor(juce::MouseCursor::NormalCursor));
		return true;
	}

	return false;	// On transmet l'evenement sans le traiter
}

//==============================================================================
// Gestion de la molette de la souris
//==============================================================================
void StereoView::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
	// Changement du Z du ballonnet
	if (m_Restit) {
		m_Bal.Z -= (wheel.deltaY * m_dGSD);
		SetBallonnet();
		repaint();
		return;
	}

	// Zoom / dezoom dans l'image
	double X = m_ViewX + event.getPosition().x * m_nFactor, Y = m_ViewY + event.getPosition().y * m_nFactor;
	bool accelerator = juce::ComponentPeer::getCurrentModifiersRealtime().isShiftDown();
	if (wheel.deltaY < 0.) {
		m_nFactor++;
		if (accelerator) m_nFactor = 24;
	}
	else {
		m_nFactor--;
		if (accelerator) m_nFactor = 1;
	}
	if (m_nFactor < 1) {
		m_nFactor = 1;
		return;
	}

	auto bounds = getBounds();
	m_ViewX = X - (bounds.getWidth() / 2) * m_nFactor;
	m_ViewY = Y - (bounds.getHeight() / 2) * m_nFactor;
	repaint();
}

//==============================================================================
// Deplacement de la souris
//==============================================================================
void StereoView::mouseMove(const juce::MouseEvent& event)
{
	if (m_Restit)
		GoToPix(event.x, event.y);
}

void StereoView::mouseDrag(const juce::MouseEvent& event)
{
	m_DragPt.x = (float)event.getDistanceFromDragStartX();
	m_DragPt.y = (float)event.getDistanceFromDragStartY();
	if ((abs(m_DragPt.x) > 1) && (abs(m_DragPt.y) > 1))
		repaint();
}

//==============================================================================
// Clic de la souris
//==============================================================================
void StereoView::mouseDown(const juce::MouseEvent& event)
{
	m_DragPt = juce::Point<float>(0, 0);
	if (event.mods.isLeftButtonDown()) {
		m_StartPt = event.position;
		setMouseCursor(juce::MouseCursor(juce::MouseCursor::DraggingHandCursor));
		m_bDrag = true;
	}
	if ((event.mods.isMiddleButtonDown()) && (m_Restit)) {
		Correlation();
		SetZBallonnet();
	}
}

void StereoView::mouseUp(const juce::MouseEvent& event)
{
	m_bDrag = false;
	if (m_Restit)
		setMouseCursor(juce::MouseCursor(juce::MouseCursor::NoCursor));
	else
		setMouseCursor(juce::MouseCursor(juce::MouseCursor::NormalCursor));

	if ((abs(m_DragPt.x) > 1) && (abs(m_DragPt.y) > 1)) {	// Drag avec la souris : action zonale
		m_ViewX -= (int)(m_DragPt.x * m_nFactor);
		m_ViewY -= (int)(m_DragPt.y * m_nFactor);
		m_bDirty = true;
	}
	repaint();
}

//-----------------------------------------------------------------------------
// Affichage du composant
//-----------------------------------------------------------------------------
void StereoView::paint(juce::Graphics& graphics)
{
	if (m_bDrag) {
		graphics.fillAll(juce::Colours::white);
		graphics.drawImageAt(m_Image, (int)m_DragPt.x, (int)m_DragPt.y);
		return;
	}

	if (m_bDirty)
		UpdateImage();
	graphics.drawImageAt(m_Image, 0, 0);

	if (m_ViewMode == ViewMode::Anaglyph)
		graphics.drawImageAt(m_BalImage, m_BalPos.getX(), m_BalPos.getY());

	if (m_ViewMode == ViewMode::Split) {
		auto b = getBounds();

		int x = m_BalL.X, y = m_BalL.Y;
		Image2Component(true, x, y);
		if (x < b.getWidth() / 2)
			DrawBallonnet(graphics, x, y, m_BalColorL);

		x = m_BalR.X;
		y = m_BalR.Y;
		Image2Component(false, x, y);
		x += (b.getWidth() / 2);
		DrawBallonnet(graphics, x, y, m_BalColorR);
	}

	DrawDecoration(graphics);
}

//-----------------------------------------------------------------------------
// Affichage des decorations
//-----------------------------------------------------------------------------
void StereoView::DrawDecoration(juce::Graphics& g)
{
	g.setFont(10.0f);
	auto b = getLocalBounds();
	juce::Rectangle<int> R(0, b.getHeight() - 15, b.getWidth(), 15);
	g.setColour(juce::Colours::darkgrey);
	g.setOpacity(0.5);
	g.fillRect(R);
	R.reduce(5, 0);
	g.setColour(juce::Colours::white);
	g.setOpacity(1.);
	g.drawText(juce::String("Parallaxe : ") + juce::String(m_dPara), R, juce::Justification::centredLeft);
	g.drawText(juce::String(m_Bal.X, 2) + " ; " + juce::String(m_Bal.Y, 2) + " ; " + juce::String(m_Bal.Z, 2), R, juce::Justification::centred);
	g.drawText(juce::String("Gamma : ") + juce::String(m_nGamma) + juce::String(" | Factor : ") + juce::String(m_nFactor),
		R, juce::Justification::centredRight);
}

//-----------------------------------------------------------------------------
// Mise a jour de l'image de fond
//-----------------------------------------------------------------------------
void StereoView::UpdateImage()
{
	juce::Graphics graphics(m_Image);

	if ((m_ImageL.IsValid()) && (!m_ImageR.IsValid())) {
		DrawMono(graphics, true);
		return;
	}
	if ((!m_ImageL.IsValid()) && (m_ImageR.IsValid())) {
		DrawMono(graphics, false);
		return;
	}

	switch (m_ViewMode) {
	case ViewMode::Anaglyph: DrawStereo(graphics);
		break;
	case ViewMode::Split: DrawSplit(graphics);
		break;
	case ViewMode::Left: DrawMono(graphics, true);
		break;
	case ViewMode::Right: DrawMono(graphics, false);
		break;
	}
}

//-----------------------------------------------------------------------------
// Affichage Stereo
//-----------------------------------------------------------------------------
bool StereoView::DrawStereo(juce::Graphics& graphics)
{
	if ((!m_ImageL.IsValid()) || (!m_ImageR.IsValid())) {
		graphics.fillAll(juce::Colours::white);
		return false;
	}
	bool rgbImage = true;
	int nb_byte = 3;
	if (m_ImageL.NbByte() != 3) { // Cas des images monochromes
		rgbImage = false;
		nb_byte = 1;
	}

	auto bounds = getLocalBounds();

	int win = bounds.getWidth(), hin = bounds.getHeight();

	m_ImageL.SetViewport(m_ViewX, m_ViewY, m_ViewX + win * m_nFactor, m_ViewY + hin * m_nFactor, m_nFactor);
	m_ImageR.SetViewport(m_ViewX, m_ViewY, m_ViewX + win * m_nFactor, m_ViewY + hin * m_nFactor, m_nFactor);

	juce::Image tmpImage(juce::Image::RGB, bounds.getWidth(), bounds.getHeight(), false, juce::SoftwareImageType());

	{ // Necessaire pour que bitmap soit detruit avant l'appel a drawImageAt
		juce::Image::BitmapData bitmap(tmpImage, juce::Image::BitmapData::readWrite);
		juce::Image::PixelFormat format = bitmap.pixelFormat;	// Sur Mac, on obtient toujours ARGB meme en demandant RGB !

		int rL, gL, bL, rR, gR, bR, r, g, b, pos;

		for (int i = 0; i < bitmap.height; i++) {
			int startX_L = 0, startX_R = 0;
			uint32_t w_L = 0, w_R = 0;

			uint8_t* pixL = m_ImageL.GetLinePointer(i * m_nFactor + m_ViewY, &startX_L, &w_L);
			if (pixL == nullptr)
				continue;
			uint8_t* pixR = m_ImageR.GetLinePointer(i * m_nFactor + m_ViewY, &startX_R, &w_R);
			if (pixR == nullptr)
				continue;

			uint8_t* line = bitmap.getLinePointer(i);

			for (int j = 0; j < bitmap.width; j++) {
				int x = m_ViewX + j * m_nFactor;
				if ((x < startX_L) || (x > startX_L + w_L)) {
					if (m_bStereoOnly) {
						line += bitmap.pixelStride;
						continue;
					}
					else
						rL = gL = bL = 0;
				}
				else {
					pos = ((x - startX_L) / m_nFactor) * nb_byte;
					if (rgbImage) {
						rL = pixL[pos + 0];
						gL = pixL[pos + 1];
						bL = pixL[pos + 2];
					}
					else
						rL = gL = bL = pixL[pos];
				}

				if ((x < startX_R) || (x > startX_R + w_R)) {
					if (m_bStereoOnly) {
						line += bitmap.pixelStride;
						continue;
					}
					else
						rR = gR = bR = 0;
				}
				else {
					pos = ((x - startX_R) / m_nFactor) * nb_byte;
					if (rgbImage) {
						rR = pixR[pos + 0];
						gR = pixR[pos + 1];
						bR = pixR[pos + 2];
					}
					else
						rR = gR = bR = pixR[pos];
				}

				if (m_nGamma != 0) {
					rR = (int)(pow((double)rR / 255., 1. - (double)m_nGamma * 0.1) * 255.);
					gR = (int)(pow((double)gR / 255., 1. - (double)m_nGamma * 0.1) * 255.);
					bR = (int)(pow((double)bR / 255., 1. - (double)m_nGamma * 0.1) * 255.);
				}

				if (m_bRedLeft) { // Rouge à gauche
					r = 0.4154 * rL + 0.4710 * gL + 0.1669 * bL - 0.0109 * rR - 0.0364 * gR - 0.0060 * bR;
					g = -0.0458 * rL - 0.0484 * gL - 0.0257 * bL + 0.3756 * rR + 0.7333 * gR + 0.0111 * bR;
					b = -0.0547 * rL - 0.0615 * gL + 0.0128 * bL - 0.0651 * rR - 0.1287 * gR + 1.2971 * bR;
				}
				else {
					r = 0.4154 * rR + 0.4710 * gR + 0.1669 * bR - 0.0109 * rL - 0.0364 * gL - 0.0060 * bL;
					g = -0.0458 * rR - 0.0484 * gR - 0.0257 * bR + 0.3756 * rL + 0.7333 * gL + 0.0111 * bL;
					b = -0.0547 * rR - 0.0615 * gR + 0.0128 * bR - 0.0651 * rL - 0.1287 * gL + 1.2971 * bL;
				}

				if (r < 0) r = 0; if (r > 255) r = 255;
				if (g < 0) g = 0; if (g > 255) g = 255;
				if (b < 0) b = 0; if (b > 255) b = 255;

				line[indexB] = b;
				line[indexG] = g;
				line[indexR] = r;

				line += bitmap.pixelStride;
			}
		}
	}

	graphics.setOpacity(1.f);
	graphics.drawImageAt(tmpImage, 0, 0);
	return true;
}

//-----------------------------------------------------------------------------
// Affichage Split
//-----------------------------------------------------------------------------
bool StereoView::DrawSplit(juce::Graphics& graphics)
{
	if ((!m_ImageL.IsValid()) || (!m_ImageR.IsValid())) {
		graphics.fillAll(juce::Colours::white);
		return false;
	}
	bool rgbImage = true;
	int nb_byte = 3;
	if (m_ImageL.NbByte() != 3) { // Cas des images monochromes
		rgbImage = false;
		nb_byte = 1;
	}
	auto bounds = getBounds();
	int win = bounds.getWidth(), hin = bounds.getHeight();

	m_ImageL.SetViewport(m_ViewX, m_ViewY, m_ViewX + (win / 2) * m_nFactor, m_ViewY + hin * m_nFactor, m_nFactor);
	m_ImageR.SetViewport(m_ViewX, m_ViewY, m_ViewX + (win / 2) * m_nFactor, m_ViewY + hin * m_nFactor, m_nFactor);

	juce::Image tmpImage(juce::Image::RGB, win, hin, false, juce::SoftwareImageType());

	{ // Necessaire pour que bitmap soit detruit avant l'appel a drawImageAt
		juce::Image::BitmapData bitmap(tmpImage, juce::Image::BitmapData::readWrite);
		juce::Image::PixelFormat format = bitmap.pixelFormat;	// Sur Mac, on obtient toujours ARGB meme en demandant RGB !

		int r, g, b;

		for (int i = 0; i < bitmap.height; i++) {
			int startX_L = 0, startX_R = 0;
			uint32_t w_L = 0, w_R = 0;
			uint8_t* pixL = m_ImageL.GetLinePointer(i * m_nFactor + m_ViewY, &startX_L, &w_L);
			uint8_t* pixR = m_ImageR.GetLinePointer(i * m_nFactor + m_ViewY, &startX_R, &w_R);

			uint8_t* line = bitmap.getLinePointer(i);

			if (pixL != nullptr) {	// Image gauche
				for (int j = 0; j < bitmap.width / 2; j++) {
					int x = m_ViewX + j * m_nFactor;
					if ((x < startX_L) || (x > startX_L + w_L)) {
						line += bitmap.pixelStride;
						continue;
					}
					int pos = ((x - startX_L) / m_nFactor) * nb_byte;

					if (rgbImage) {
						r = pixL[pos + 0];
						g = pixL[pos + 1];
						b = pixL[pos + 2];
					}
					else
						r = g = b = pixL[pos];

					line[indexB] = b;
					line[indexG] = g;
					line[indexR] = r;

					line += bitmap.pixelStride;
				}
			}

			if (pixR != nullptr) {	// Image droite
				for (int j = bitmap.width / 2; j < bitmap.width; j++) {
					int x = m_ViewX + (j - bitmap.width / 2) * m_nFactor;
					if ((x < startX_R) || (x > startX_R + w_R)) {
						line += bitmap.pixelStride;
						continue;
					}
					int pos = ((x - startX_R) / m_nFactor) * nb_byte;

					if (rgbImage) {
						r = pixR[pos + 0];
						g = pixR[pos + 1];
						b = pixR[pos + 2];
					}
					else
						r = g = b = pixR[pos];

					line[indexB] = b;
					line[indexG] = g;
					line[indexR] = r;

					line += bitmap.pixelStride;
				}
			}

		}
	}
	graphics.setOpacity(1.f);
	graphics.drawImageAt(tmpImage, 0, 0);
	return true;
}

//-----------------------------------------------------------------------------
// Affichage Mono
//-----------------------------------------------------------------------------
bool StereoView::DrawMono(juce::Graphics& graphics, bool left)
{
	XMemRaster* image = &m_ImageL;
	if (!left)
		image = &m_ImageR;
	if (!image->IsValid()) {
		graphics.fillAll(juce::Colours::white);
		return false;
	}
	bool rgbImage = true;
	int nb_byte = 3;
	if (image->NbByte() != 3) { // Cas des images monochromes
		rgbImage = false;
		nb_byte = 1;
	}

	auto bounds = getLocalBounds();
	int win = bounds.getWidth(), hin = bounds.getHeight();
	image->SetViewport(m_ViewX, m_ViewY, m_ViewX + win * m_nFactor, m_ViewY + hin * m_nFactor, m_nFactor);

	juce::Image tmpImage(juce::Image::RGB, win, hin, false, juce::SoftwareImageType());

	{ // Necessaire pour que bitmap soit detruit avant l'appel a drawImageAt
		juce::Image::BitmapData bitmap(tmpImage, juce::Image::BitmapData::readWrite);
		juce::Image::PixelFormat format = bitmap.pixelFormat;	// Sur Mac, on obtient toujours ARGB meme en demandant RGB !

		int r, g, b;

		for (int i = 0; i < bitmap.height; i++) {
			int startX = 0;
			uint32_t w = 0;
			uint8_t* pix = image->GetLinePointer(i * m_nFactor + m_ViewY, &startX, &w);
			if (pix == nullptr)
				continue;

			uint8_t* line = bitmap.getLinePointer(i);
			for (int j = 0; j < bitmap.width; j++) {
				int x = m_ViewX + j * m_nFactor;
				if ((x < startX) || (x > startX + w)) {
					line += bitmap.pixelStride;
					continue;
				}
				int pos = ((x - startX) / m_nFactor) * nb_byte;

				if (rgbImage) {
					r = pix[pos + 0];
					g = pix[pos + 1];
					b = pix[pos + 2];
				}
				else
					r = g = b = pix[pos];

				line[indexB] = b;
				line[indexG] = g;
				line[indexR] = r;

				line += bitmap.pixelStride;
			}
		}
	}
	graphics.setOpacity(1.f);
	graphics.drawImageAt(tmpImage, 0, 0);
	return true;
}

//-----------------------------------------------------------------------------
// Ouverture d'une image du couple
//-----------------------------------------------------------------------------
bool StereoView::OpenImage(std::string filename, bool left, int rot)
{
	XMemRaster* image = &m_ImageL;
	if (!left)
		image = &m_ImageR;
	image->Close();
	if (!image->AnalyzeImage(filename))
		return false;
	image->SetRotation(rot);

	SetApproximatePosition();
	repaint();
	return true;
}

//-----------------------------------------------------------------------------
// Ouverture du fichier d'orientation
//-----------------------------------------------------------------------------
bool StereoView::OpenOrientation(std::string filename)
{
	std::ifstream in;
	in.open(filename);
	if (!in.good())
		return false;
	m_strOrientationFile = filename;
	std::string field, sep, value, name, camera;
	in >> field >> sep >> value; // CHANTIER : ori
	in >> field >> sep >> value; // PROJECTION : LAMBERT93
	in >> field >> field >> sep >> value; // REFERENTIEL ALTIMETRIQUE : IGN69
	in >> field >> field >> sep >> value; // UNITE ANGLE  : degre
	for (int i = 0; i < 8; i++)
		in >> field; // NOM	X	Y	Z	O	P	K	CAMERA
	std::string nameL, nameR;
	XPath P;
	nameL = P.Name(m_ImageL.Filename().c_str(), false);
	nameR = P.Name(m_ImageR.Filename().c_str(), false);
	double x, y, z, omega, phi, kappa;
	bool left_good = false, right_good = false;
	while (!in.eof()) {
		in >> name >> x >> y >> z >> omega >> phi >> kappa >> camera;
		if (name.compare(nameL) == 0) {
			m_Model.SetCli(true, XPt3D(x, y, z), omega, phi, kappa);
			left_good = true;
		}
		if (name.compare(nameR) == 0) {
			m_Model.SetCli(false, XPt3D(x, y, z), omega, phi, kappa);
			right_good = true;
		}
		if (left_good && right_good)
			break;
	}
	if (!(left_good && right_good))
		return false;
	m_OrientationType = StereoModel;
	return true;
}

//-----------------------------------------------------------------------------
// Ouverture du fichier camera
//-----------------------------------------------------------------------------
bool StereoView::OpenCamera(std::string filename)
{
	XParserXML parser;
	if (!parser.Parse(filename))
		return false;
	m_strCameraFile = filename;
	XParserXML sensor = parser.FindSubParser("/sensor");
	if (sensor.IsEmpty())
		return false;
	XParserXML focal = sensor.FindSubParser("/sensor/focal/pt3d");
	if (focal.IsEmpty())
		return false;
	XPt3D F;
	F.XmlRead(&focal);
	double pixelSize = sensor.ReadNodeAsDouble("/sensor/pixel_size");
	m_Model.SetFocal(F, pixelSize);

	SetCenterPosition();
	return true;
}

//-----------------------------------------------------------------------------
// Ouverture d'une pseudo orientation
//-----------------------------------------------------------------------------
bool StereoView::OpenPseudoOrientation(std::string filename)
{
	std::ifstream in;
	in.open(filename);
	if (!in.good())
		return false;
	double xmin, ymax, z0, gsd;
	in >> xmin >> ymax >> z0 >> gsd;
	std::string name;
	double xS, yS, zS;
	in >> name >> xS >> yS >> zS;
	m_Ortho.SetCli(true, XPt3D(xS, yS, zS), XPt3D(xmin, ymax, z0), gsd);
	in >> name >> xS >> yS >> zS;
	m_Ortho.SetCli(false, XPt3D(xS, yS, zS), XPt3D(xmin, ymax, z0), gsd);
	m_OrientationType = OrthoModel;

	SetOrthoPosition();
	return true;
}

//-----------------------------------------------------------------------------
// Fixe une pseudo orientation
//-----------------------------------------------------------------------------
void StereoView::SetPseudoOrientation(const XPt3D& SL, const XPt3D& SR, const XPt3D& Ori, const double& gsd)
{
	m_OrientationType = OrthoModel;
	m_Ortho.SetCli(true, SL, Ori, gsd);
	m_Ortho.SetCli(false, SR, Ori, gsd);
	SetOrthoPosition();
}


//-----------------------------------------------------------------------------
// Fixe la position approchee du couple stereo
//-----------------------------------------------------------------------------
void StereoView::SetApproximatePosition(double overlap)
{
	if ((!m_ImageL.IsValid()) || (!m_ImageR.IsValid()))
		return;
	auto bounds = getBounds();
	m_ViewX = m_ImageL.NbPixelX() / 2 - bounds.getWidth() / 2;
	m_ViewY = m_ImageL.NbPixelY() / 2 - bounds.getHeight() / 2;

	m_ImageR.SetOrigin(m_ImageL.NbPixelX() * (1. - overlap), 0);
}

//-----------------------------------------------------------------------------
// Fixe la position au centre du couple stereo
//-----------------------------------------------------------------------------
void StereoView::SetCenterPosition()
{
	// Position du centre du couple
	XPt3D Som = (m_Model.SomL() + m_Model.SomR()) * 0.5;
	XGeoPref pref;
	pref.Projection(XGeoProjection::Lambert93);
	double lon, lat;
	pref.ConvertDeg(pref.Projection(), XGeoProjection::RGF93, Som.X, Som.Y, lon, lat);
	Som.Z = m_GeoSearch.GetAltitude(lon, lat);
	m_Bal = Som;
	m_dGSD = (m_Model.SomL().Z - Som.Z) / m_Model.Focal().Z;	// La focale est exprimee en pixel
	XPt2D pixL, pixR;
	//m_Model.Ground2Image(Som, pixL, pixR);
	Ground2Image(Som, pixL, pixR);

	// Viewport
	auto bounds = getBounds();
	int uL = pixL.X, vL = pixL.Y, uR = pixR.X, vR = pixR.Y;
	m_ImageL.Image2Viewport(uL, vL);
	m_ImageR.SetOrigin(0, 0);
	m_ImageR.Image2Viewport(uR, vR);
	m_ImageR.SetOrigin(uL - uR, vL - vR);

	m_ViewX = uL - bounds.getWidth() / 2;
	m_ViewY = vL - bounds.getHeight() / 2;

	SetBallonnet(Som.X, Som.Y, Som.Z);
	repaint();
}

//-----------------------------------------------------------------------------
// Fixe la position au centre du couple ortho
//-----------------------------------------------------------------------------
void StereoView::SetOrthoPosition()
{
	XPt3D oriL = m_Ortho.OriL(), oriR = m_Ortho.OriR();
	XPt3D T = (oriR - oriL) / m_Ortho.GsdR();
	m_ImageR.SetOrigin(XRint(T.X), XRint(T.Y));
	XPt3D Som = (m_Ortho.SomL() + m_Ortho.SomR()) * 0.5;
	Som.Z = oriL.Z;

	SetBallonnet(Som.X, Som.Y, Som.Z);
	repaint();
}


//-----------------------------------------------------------------------------
// Met le plan de visualisation au Z du ballonnet
//-----------------------------------------------------------------------------
void StereoView::SetZBallonnet()
{
	int uL = m_BalL.X, vL = m_BalL.Y;
	int uR = m_BalR.X, vR = m_BalR.Y;
	m_ImageL.Image2Viewport(uL, vL);
	m_ImageR.SetOrigin(0, 0);
	m_ImageR.Image2Viewport(uR, vR);
	m_ImageR.SetOrigin(uL - uR, vL - vR);

	SetBallonnet();
	repaint();
}

//-----------------------------------------------------------------------------
// Conversion coordonnees Terrain <-> coordonnees Image
//-----------------------------------------------------------------------------
bool StereoView::Ground2Image(XPt3D P, XPt2D& uL, XPt2D& uR)
{
	if (m_OrientationType == StereoModel) {
		m_Model.Ground2Image(P, uL, uR);
		return true;
	}
	if (m_OrientationType == OrthoModel) {
		m_Ortho.Ground2Image(P, uL, uR);
		return true;
	}
	int xL = P.X, yL = P.Y, xR = P.X, yR = P.Y;
	m_ImageL.ViewPort2Image(xL, yL);
	m_ImageR.ViewPort2Image(xR, yR);
	uL = XPt2D(xL, yL);
	uR = XPt2D(xR, yR);
	return false;
}

double StereoView::Image2Ground(XPt2D uL, XPt2D uR, XPt3D& PL, XPt3D& PR)
{
	if (m_OrientationType == StereoModel)
		return m_Model.Image2Ground(uL, uR, PL, PR);
	if (m_OrientationType == OrthoModel)
		return m_Ortho.Image2Ground(uL, uR, PL, PR);
	int xL = uL.X, yL = uL.Y, xR = uR.X, yR = uR.Y;
	m_ImageL.Image2Viewport(xL, yL);
	m_ImageR.Image2Viewport(xR, yR);
	PL = XPt3D(xL, yL, 0.);
	PR = XPt3D(xR, yR, 0.);
	return 0;
}

//-----------------------------------------------------------------------------
// Conversion coordonnees Component <-> coordonnees Viewport
//-----------------------------------------------------------------------------
void StereoView::Component2Viewport(int& x, int& y)
{
	x = m_ViewX + x * m_nFactor;
	y = m_ViewY + y * m_nFactor;
}

void StereoView::Viewport2Component(int& x, int& y)
{
	x = (x - m_ViewX) / m_nFactor;
	y = (y - m_ViewY) / m_nFactor;
}

//-----------------------------------------------------------------------------
// Conversion coordonnees Component <-> coordonnees image
//-----------------------------------------------------------------------------
void StereoView::Component2Image(bool left, int& x, int& y)
{
	Component2Viewport(x, y);
	if (left)
		m_ImageL.ViewPort2Image(x, y);
	else
		m_ImageR.ViewPort2Image(x, y);
}

void StereoView::Image2Component(bool left, int& x, int& y)
{
	if (left)
		m_ImageL.Image2Viewport(x, y);
	else
		m_ImageR.Image2Viewport(x, y);
	Viewport2Component(x, y);
}

//-----------------------------------------------------------------------------
// Conversion coordonnees Component <-> coordonnees image
//-----------------------------------------------------------------------------
void StereoView::GoToPix(int x, int y)
{
	int xL = x, yL = y, xR = x, yR = y;
	Component2Image(true, xL, yL);
	Component2Image(false, xR, yR);
	XPt3D U1, U2;
	m_dPara = Image2Ground(XPt2D(xL, yL), XPt2D(xR, yR), U1, U2);
	double oldZ = m_Bal.Z;
	m_Bal = (U1 + U2) * 0.5;
	m_Bal.Z = oldZ;
	SetBallonnet();
	repaint();
}

//-----------------------------------------------------------------------------
// Fixe le ballonnet
//-----------------------------------------------------------------------------
void StereoView::SetBallonnet(double x, double y, double z)
{
	XPt2D pL, pR;
	Ground2Image(XPt3D(x, y, z), m_BalL, m_BalR);
	int uL = m_BalL.X, vL = m_BalL.Y, uR = m_BalR.X, vR = m_BalR.Y;
	Image2Component(true, uL, vL);
	Image2Component(false, uR, vR);

	int u0 = XRint(XMin(uL, uR)) - 10;
	int v0 = XRint(XMin(vL, vR)) - 10;
	int u1 = XRint(XMax(uL + 1, uR + 1)) + 10;
	int v1 = XRint(XMax(vL + 1, vR + 1)) + 10;
	juce::Image imaL(juce::Image::ARGB, u1 - u0, v1 - v0, true);
	juce::Image imaR(juce::Image::ARGB, u1 - u0, v1 - v0, true);
	m_BalImage = juce::Image(juce::Image::ARGB, u1 - u0, v1 - v0, true);

	{
		juce::Graphics graphicsL(imaL);
		DrawBallonnet(graphicsL, uL - u0, vL - v0, m_BalColorL);
		juce::Graphics graphicsR(imaR);
		DrawBallonnet(graphicsR, uR - u0, vR - v0, m_BalColorR);
	}
	DrawAnaglyph(imaL, imaR, m_BalImage);
	m_BalPos = juce::Point<int>(u0, v0);
}

//-----------------------------------------------------------------------------
// Dessin du ballonnet
//-----------------------------------------------------------------------------
void StereoView::DrawBallonnet(juce::Graphics& graphics, int u, int v, juce::Colour& color)
{
	graphics.setColour(color);
	switch (m_BalShape) {
	case Dot:
		graphics.drawEllipse(u - 2.f, v - 2.f, 4.f, 4.f, 1.f);
		break;
	case BigDot:
		graphics.drawEllipse(u - 4.f, v - 4.f, 8.f, 8.f, 1.f);
		break;
	case Cross:
		graphics.drawLine(u - 4.f, v, u + 4.f, v, 1.f);
		graphics.drawLine(u, v - 4.f, u, v + 4.f, 1.f);
		break;
	case BigCross:
		graphics.drawLine(u - 8.f, v, u + 8.f, v, 2.f);
		graphics.drawLine(u, v - 8.f, u, v + 8.f, 2.f);
		break;
	case XCross:
		graphics.drawLine(u - 4.f, v - 4.f, u + 4.f, v + 4.f, 1.f);
		graphics.drawLine(u - 4.f, v + 4.f, u + 4.f, v - 4.f, 1.f);
		break;
	case BigXCross:
		graphics.drawLine(u - 8.f, v - 8.f, u + 8.f, v + 8.f, 2.f);
		graphics.drawLine(u - 8.f, v + 8.f, u + 8.f, v - 8.f, 2.f);
		break;
	default:;
	}
}

//-----------------------------------------------------------------------------
// Calcul de l'image anaglyphe a partir de deux images de meme dimension
//-----------------------------------------------------------------------------
bool StereoView::DrawAnaglyph(juce::Image& left, juce::Image& right, juce::Image& stereo, bool redLeft)
{
	if ((left.getWidth() != right.getWidth()) || (left.getWidth() != stereo.getWidth()))
		return false;
	if ((left.getHeight() != right.getHeight()) || (left.getHeight() != stereo.getHeight()))
		return false;
	if ((left.getFormat() != juce::Image::ARGB) || (right.getFormat() != juce::Image::ARGB) || (stereo.getFormat() != juce::Image::ARGB))
		return false;

	juce::Image::BitmapData bitmapL(left, juce::Image::BitmapData::readWrite);
	juce::Image::BitmapData bitmapR(right, juce::Image::BitmapData::readWrite);
	juce::Image::BitmapData bitmapS(stereo, juce::Image::BitmapData::readWrite);
	int rL, gL, bL, aL, rR, gR, bR, aR, r, g, b, a;
	juce::Colour color;

	for (int i = 0; i < bitmapS.height; i++) {
		uint8_t* lineL = bitmapL.getLinePointer(i);
		uint8_t* lineR = bitmapR.getLinePointer(i);
		uint8_t* lineS = bitmapS.getLinePointer(i);

		for (int j = 0; j < bitmapS.width; j++) {

			aL = lineL[0];
			bL = lineL[1];
			gL = lineL[2];
			rL = lineL[3];
			lineL += bitmapL.pixelStride;

			aR = lineR[0];
			bR = lineR[1];
			gR = lineR[2];
			rR = lineR[3];
			lineR += bitmapR.pixelStride;

			if (redLeft) { // Rouge à gauche
				r = 0.4154 * rL + 0.4710 * gL + 0.1669 * bL - 0.0109 * rR - 0.0364 * gR - 0.0060 * bR;
				g = -0.0458 * rL - 0.0484 * gL - 0.0257 * bL + 0.3756 * rR + 0.7333 * gR + 0.0111 * bR;
				b = -0.0547 * rL - 0.0615 * gL + 0.0128 * bL - 0.0651 * rR - 0.1287 * gR + 1.2971 * bR;
			}
			else {
				r = 0.4154 * rR + 0.4710 * gR + 0.1669 * bR - 0.0109 * rL - 0.0364 * gL - 0.0060 * bL;
				g = -0.0458 * rR - 0.0484 * gR - 0.0257 * bR + 0.3756 * rL + 0.7333 * gL + 0.0111 * bL;
				b = -0.0547 * rR - 0.0615 * gR + 0.0128 * bR - 0.0651 * rL - 0.1287 * gL + 1.2971 * bL;
			}

			if (r < 0) r = 0; if (r > 255) r = 255;
			if (g < 0) g = 0; if (g > 255) g = 255;
			if (b < 0) b = 0; if (b > 255) b = 255;

			a = 255;
			if ((aL == 0) && (aR == 0))
				a = 0;
			color = juce::Colour::fromRGBA(r, g, b, a);
			*((uint32_t*)lineS) = color.getARGB();

			lineS += bitmapS.pixelStride;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Recherche de l'homologue par correlation
//-----------------------------------------------------------------------------
void StereoView::Correlation()
{
	uint32_t half_win = 7, half_search = 100;
	uint32_t win_size = half_win * 2 + 1, win_search = 2 * half_search + 1;

	if (((m_BalL.X - half_win) < 0.) || ((m_BalL.Y - half_win) < 0.))
		return;
	if (((m_BalR.X - half_search) < 0.) || ((m_BalR.Y - half_search) < 0.))
		return;
	if (((m_BalL.X + half_win) >= m_ImageL.Width()) || ((m_BalL.Y + half_win) >= m_ImageL.Height()))
		return;
	if (((m_BalR.X + half_search) >= m_ImageR.Width()) || ((m_BalR.Y + half_search) >= m_ImageR.Height()))
		return;

	uint8_t* pixL = m_ImageL.AllocArea(win_size, win_size);
	uint8_t* pixR = m_ImageR.AllocArea(win_search, win_search);
	if ((pixL == nullptr) || (pixR == nullptr)) {
		delete[] pixL; delete[] pixR;
		return;
	}

	// Chargement des zones de pixels
	bool flagL = m_ImageL.GetArea(m_BalL.X - half_win, m_BalL.Y - half_win, win_size, win_size, pixL);
	bool flagR = m_ImageR.GetArea(m_BalR.X - half_search, m_BalR.Y - half_search, win_search, win_search, pixR);
	if ((flagL == false) || (flagR == false)) {
		delete[] pixL; delete[] pixR;
		return;
	}

	// Fenetres de correlation
	uint8_t* corL = new (std::nothrow) uint8_t[win_size * win_size];
	uint8_t* corR = new (std::nothrow) uint8_t[win_search * win_search];
	if ((corL == nullptr) || (corR == nullptr)) {
		delete[] pixL; delete[] pixR; delete[] corL; delete[] corR;
		return;
	}

	if (m_ImageL.NbByte() == 3) {	// Passage en niveau de gris des images RGB
		for (uint32_t i = 0; i < win_size * win_size; i++)
			corL[i] = (uint8_t)(((int)pixL[3 * i] + (int)pixL[3 * i + 1] + (int)pixL[3 * i + 2]) / 3);
		for (uint32_t i = 0; i < win_search * win_search; i++)
			corR[i] = (uint8_t)(((int)pixR[3 * i] + (int)pixR[3 * i + 1] + (int)pixR[3 * i + 2]) / 3);
	}
	else {	// Images monochromes
		memcpy(corL, pixL, win_size * win_size);
		memcpy(corR, pixR, win_search * win_search);
	}

	// Correlation
	double u = 0, v = 0, pic = 0;
	XBaseImage::Correlation(corL, win_size, win_size, corR, win_search, win_search, &u, &v, &pic);
	delete[] pixL; delete[] pixR;
	delete[] corL; delete[] corR;
	if (pic < 0.75)
		return;

	m_BalR.X -= (half_search - u);
	m_BalR.Y -= (half_search - v);
	if ((m_OrientationType == StereoModel) || (m_OrientationType == OrthoModel)) {
		XPt3D U1, U2;
		m_dPara = Image2Ground(m_BalL, m_BalR, U1, U2);
		m_Bal = (U1 + U2) * 0.5;
		SetBallonnet(m_Bal.X, m_Bal.Y, m_Bal.Z);
	}
	repaint();
}