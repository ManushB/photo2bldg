#pragma once

#include <map>
#include "Grammar.h"
#include <QFile>
#include <QDomDocument>
#include <QDomNode>

namespace cga {

void parseGrammar(const char* filename, Grammar& grammar);
boost::shared_ptr<Operator> parseCenterOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseColorOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseCompOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseCopyOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseCornerCutOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseExtrudeOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseHemisphereOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseInnerCircleOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseInnerSemiCircleOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseInsertOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseOffsetOperator(const QDomNode& node);
boost::shared_ptr<Operator> parsePyramidOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseRoofGableOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseRoofHipOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseRotateOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseSetupProjectionOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseShapeLOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseShapeUOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseSizeOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseSplitOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseTaperOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseTextureOperator(const QDomNode& node);
boost::shared_ptr<Operator> parseTranslateOperator(const QDomNode& node);

}