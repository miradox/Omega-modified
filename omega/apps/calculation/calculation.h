#ifndef CALCULATION_CALCULATION_H
#define CALCULATION_CALCULATION_H

#include <apps/constant.h>
#include <escher.h>
#include <poincare/context.h>
#include <poincare/expression.h>

namespace Calculation {

class CalculationStore;


/* A calculation is:
 *  |     uint8_t   |KDCoordinate|  KDCoordinate  |  uint8_t  |   ...     |      ...       |         ...          |
 *  |m_displayOutput|  m_height  |m_expandedHeight|m_equalSign|m_inputText|m_exactOuputText|m_approximateOuputText|
 *
 * */

class Calculation {
public:
  enum class EqualSign : uint8_t {
    Unknown,
    Approximation,
    Equal
  };

  enum class DisplayOutput : uint8_t {
    Unknown,
    ExactOnly,
    ApproximateOnly,
    ExactAndApproximate,
    ExactAndApproximateToggle
  };

  /* It is not really the minimal size, but it clears enough space for most
   * calculations instead of clearing less space, then fail to serialize, clear
   * more space, fail to serialize, clear more space, etc., until reaching
   * sufficient free space. */
  static int MinimalSize() { return sizeof(uint8_t) + 2*sizeof(KDCoordinate) + sizeof(uint8_t) + 3*Constant::MaxSerializedExpressionSize; }

  Calculation() :
    m_displayOutput(DisplayOutput::Unknown),
    m_height(-1),
    m_expandedHeight(-1),
    m_equalSign(EqualSign::Unknown)
  {
    assert(sizeof(m_inputText) == 0);
  }
  bool operator==(const Calculation& c);
  Calculation * next() const;

  void tidy();

  // Texts
  const char * inputText() const { return m_inputText; }
  const char * exactOutputText() const { return m_inputText + strlen(m_inputText) + 1; }
  const char * approximateOutputText() const;

  // Expressions
  Poincare::Expression input();
  Poincare::Expression exactOutput();
  Poincare::Expression approximateOutput(Poincare::Context * context);

  // Layouts
  Poincare::Layout createInputLayout();
  Poincare::Layout createExactOutputLayout();
  Poincare::Layout createApproximateOutputLayout(Poincare::Context * context);

  KDCoordinate height(Poincare::Context * context, bool expanded = false);
  DisplayOutput displayOutput(Poincare::Context * context);
  bool shouldOnlyDisplayExactOutput();
  EqualSign exactAndApproximateDisplayedOutputsAreEqual(Poincare::Context * context);
private:
  static constexpr KDCoordinate k_heightComputationFailureHeight = 50;
  /* Buffers holding text expressions have to be longer than the text written
   * by user (of maximum length TextField::maxBufferSize()) because when we
   * print an expression we add omitted signs (multiplications, parenthesis...) */
  DisplayOutput m_displayOutput;
  KDCoordinate m_height __attribute__((packed));
  KDCoordinate m_expandedHeight __attribute__((packed));
  EqualSign m_equalSign;
  char m_inputText[0]; // MUST be the last member variable
};

}

#endif