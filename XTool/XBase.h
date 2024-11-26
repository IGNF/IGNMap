//-----------------------------------------------------------------------------
//								XBase.h
//								=======
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 10/08/00
//
// XBase.h contient les declarations de base utilisees par la plupart des
// classes de la bibliotheque
//-----------------------------------------------------------------------------

#ifndef _XBASE_H
#define _XBASE_H

#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <string>
#include <fstream>
#include <cassert>

// Definitions des types standards C++ 11
#ifdef XTOOL_C11_TYPES_NEEDED
#ifndef uint64_t
typedef unsigned long long uint64_t;
#endif

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

#ifndef uint16_t
typedef unsigned short uint16_t;
#endif

#ifndef uint8_t
typedef unsigned char uint8_t;
#endif
#endif

#ifndef XPt
typedef struct {
  double X;
  double Y;
} XPt;
#endif

#ifndef XPt3
typedef struct {
  double X;
  double Y;
  double Z;
} XPt3;
#endif

// Definition des constantes standards
#ifndef XPI
#define XPI		3.1415926535897932384626433832795
#endif
#ifndef XPI2
#define XPI2	1.5707963267948966192313216916398
#endif
#ifndef XPI4
#define XPI4	0.78539816339744830961566084581988
#endif

// Definitions de XMin et XMax (en general min et max causent des problemes de
//								definitions multiples)
template<class T> const T& XMin(const T& x, const T& y)
								{ if (x < y) return x; else return y;}
template<class T> const T& XMax(const T& x, const T& y)
								{ if (x > y) return x; else return y;}

// Fonction XRint : arrondi a l'entier le plus proche
inline int XRint( double x ) { return (x - floor(x)<0.5)? (int)floor(x):(int)ceil(x); }

// Fonction XRound : arrondi a la precision souhaitee
inline double XRound(double x, double prec)
						{ double t = x / prec; if (t - floor(t) < 0.5) return floor(t) * prec; return ceil(t) * prec;}

// Fonction XAbs : valeur absolue
template<class T> inline const T& XAbs(const T& x) { return (x >=0)? x : -x;}

// Calcul de la moyenne / ecart-type / variance avec l'algorithme de B. P. Welford / Donald Knuth
class InlineStat {
private:
	int m_nCount;
	double m_dOldM, m_dNewM, m_dOldS, m_dNewS;
public:
	InlineStat() : m_nCount(0) { m_dOldM = m_dNewM = m_dOldS = m_dNewS = 0.; }
	void Clear() { m_nCount = 0;}
	int NumValues() const { return m_nCount;}
	double Mean() const { return (m_nCount > 0) ? m_dNewM : 0.0; }
	double Variance() const { return ((m_nCount > 1) ? m_dNewS / (m_nCount - 1) : 0.0);}
	double StandardDeviation() const { return sqrt(Variance()); }

	void AddValue(double x) // See Knuth TAOCP vol 2, 3rd edition, page 232
	{
		m_nCount++;	
		if (m_nCount == 1) {
			m_dOldM = m_dNewM = x;
			m_dOldS = 0.0;
		} else {
			m_dNewM = m_dOldM + (x - m_dOldM) / m_nCount;
			m_dNewS = m_dOldS + (x - m_dOldM) * (x - m_dNewM);
			m_dOldM = m_dNewM;
			m_dOldS = m_dNewS;
		}
	}
};

// Macro pour les assertions
#ifndef _DEBUG
#define XAssert(expr)	(void(0))
#else
#define XAssert(expr)	assert(expr)
#endif

//-----------------------------------------------------------------------------
// Classe XError
//				-> La classe XError permet de gerer le routage des messages
//				   d'erreur
//-----------------------------------------------------------------------------
class XError {
protected:
  uint32_t m_nError;	// Compteur d'erreurs
  uint32_t m_nAlert;	// Compteur d'alertes
public:
	enum Type { eNull, eAllocation, eRange, eIOOpen, eIOSeek, eIORead, eIOWrite,
							eBadFormat, eUnsupported, eIllegal, eBadData};

	XError() {m_nError = 0; m_nAlert = 0;}
	virtual ~XError() {;}

	virtual void Error(const char* , Type = eNull) {m_nError++;}	// Message d'erreur
	virtual void Alert(const char* , Type = eNull) {m_nAlert++;}	// Message d'alerte
	virtual void Info(const char* , Type = eNull) {;}	// Message d'information

	virtual void Reset() { m_nError = 0; m_nAlert = 0;}			// Remise a 0 des compteurs
  inline uint32_t NbError() const { return m_nError;}
  inline uint32_t NbAlert() const { return m_nAlert;}

	virtual void Output(std::ostream*) {;}				// Sortie d'un flux
	virtual std::ostream* Output() { return NULL;}
	virtual void BeginOutput() {;}
	virtual void EndOutput() {;}

	friend bool XErrorError(XError* error, const char* mes, XError::Type t = XError::eNull)
								{ if (error != NULL) error->Error(mes, t); return false; }
	friend bool XErrorInfo(XError* error, const char* mes, XError::Type t = XError::eNull)
								{ if (error != NULL) error->Info(mes, t); return true; }
	friend bool XErrorAlert(XError* error, const char* mes, XError::Type t = XError::eNull)
								{ if (error != NULL) error->Alert(mes, t); return true; }

	friend bool XErrorError(XError* error, std::string& mes, XError::Type t = XError::eNull)
								{ return XErrorError(error, mes.c_str(), t); }
	friend bool XErrorInfo(XError* error, std::string& mes, XError::Type t = XError::eNull)
								{ return XErrorInfo(error, mes.c_str(), t); }
	friend bool XErrorAlert(XError* error, std::string& mes, XError::Type t = XError::eNull)
								{ return XErrorAlert(error, mes.c_str(), t); }

	friend void XErrorReset(XError* error) { if (error != NULL) error->Reset();}
  friend uint32_t XErrorNbError(XError* error) { if (error != NULL) return error->NbError(); return 0;}
  friend uint32_t XErrorNbAlert(XError* error) { if (error != NULL) return error->NbAlert(); return 0;}

	friend void XErrorOutput(XError* error, std::ostream* out)
								{ if (error != NULL) error->Output(out);}
	friend std::ostream* XErrorOutput(XError* error)
								{ if (error == NULL) return NULL; else return error->Output();}
	friend void XErrorBeginOutput(XError* error)
								{ if (error != NULL) error->BeginOutput();}
	friend void XErrorEndOutput(XError* error)
								{ if (error != NULL) error->EndOutput();}
};

//-----------------------------------------------------------------------------
// Classe XWait
//				-> La classe XWait permet de gerer les temps d'attente lors
//				   des traitements longs
//-----------------------------------------------------------------------------
class XWait {
protected:
	int		m_nMin;
	int		m_nMax;
	int		m_nStep;
	bool	m_bCancel;
public:
	XWait(int min=0, int max=100, int step=1) : m_nMin(min), m_nMax(max), m_nStep(step) {m_bCancel = false;}
	virtual ~XWait() {;}

	virtual void SetRange(int min, int max) { m_nMin = min ; m_nMax = max;}
	virtual void SetStep(int step) { m_nStep = step;}
	virtual void StepIt() { m_nStep++;}
	virtual void SetStatus(const char* ) {;}
	virtual void Cancel() { m_bCancel = true;}
	virtual bool CheckCancel() { return m_bCancel;}

	friend void XWaitRange(XWait* wait, int min, int max)
											{ if (wait != NULL) wait->SetRange(min, max);}
	friend void XWaitStep(XWait* wait, int step) { if (wait != NULL) wait->SetStep(step);}
	friend void XWaitStepIt(XWait* wait) { if (wait != NULL) wait->StepIt();}
	friend void XWaitStatus(XWait* wait, const char* s) { if (wait != NULL) wait->SetStatus(s);}
	friend void XWaitStatus(XWait* wait, std::string& s) { XWaitStatus(wait, s.c_str());}
	friend void XWaitCancel(XWait* wait) { if (wait != NULL) wait->Cancel();}
	friend bool XWaitCheckCancel(XWait* wait) { if (wait != NULL) return wait->CheckCancel(); return false;}
};

#endif //_XBASE_H
