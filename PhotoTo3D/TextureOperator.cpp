#include "TextureOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

TextureOperator::TextureOperator(const std::string& texture) {
	this->name = "texture";
	this->texture = texture;
}

boost::shared_ptr<Shape> TextureOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	shape->texture(grammar.evalString(texture, shape));
	return shape;
}

}
